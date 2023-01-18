/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test networking
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2019, Even Rouault <even dot rouault at spatialys dot com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "gtest_include.h"

#include <memory>
#include <stdio.h>
#include <stdlib.h>

#include "proj_internal.h"
#include <proj.h>

#include <sqlite3.h>
#include <time.h>

#ifdef CURL_ENABLED
#include <curl/curl.h>
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace {

static const int byte_order_test = 1;
#define IS_LSB                                                                 \
    (1 == (reinterpret_cast<const unsigned char *>(&byte_order_test))[0])

static void swap_words(void *dataIn, size_t word_size, size_t word_count)

{
    unsigned char *data = static_cast<unsigned char *>(dataIn);
    for (size_t word = 0; word < word_count; word++) {
        for (size_t i = 0; i < word_size / 2; i++) {
            unsigned char t;

            t = data[i];
            data[i] = data[word_size - i - 1];
            data[word_size - i - 1] = t;
        }

        data += word_size;
    }
}

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

static bool networkAccessOK = false;

static size_t noop_curl_write_func(void *, size_t, size_t nmemb, void *) {
    return nmemb;
}

TEST(networking, initial_check) {
    CURL *hCurlHandle = curl_easy_init();
    if (!hCurlHandle)
        return;
    curl_easy_setopt(hCurlHandle, CURLOPT_URL,
                     "https://cdn.proj.org/fr_ign_ntf_r93.tif");

    curl_easy_setopt(hCurlHandle, CURLOPT_RANGE, "0-1");
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, noop_curl_write_func);

    curl_easy_perform(hCurlHandle);

    long response_code = 0;
    curl_easy_getinfo(hCurlHandle, CURLINFO_HTTP_CODE, &response_code);

    curl_easy_cleanup(hCurlHandle);

    networkAccessOK = (response_code == 206);
    if (!networkAccessOK) {
        fprintf(stderr, "network access not working");
    }
}

#endif

// ---------------------------------------------------------------------------

static void silent_logger(void *, int, const char *) {}

// ---------------------------------------------------------------------------

TEST(networking, basic) {
    const char *pipeline =
        "+proj=pipeline "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=hgridshift +grids=https://cdn.proj.org/fr_ign_ntf_r93.tif "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg";

    // network access disabled by default
    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_log_func(ctx, nullptr, silent_logger);
    auto P = proj_create(ctx, pipeline);
    ASSERT_EQ(P, nullptr);
    proj_context_destroy(ctx);

    proj_cleanup();

#ifdef CURL_ENABLED
    // enable through env variable
    putenv(const_cast<char *>("PROJ_NETWORK=ON"));
    ctx = proj_context_create();
    P = proj_create(ctx, pipeline);
    if (networkAccessOK) {
        ASSERT_NE(P, nullptr);
    }
    proj_destroy(P);
    proj_context_destroy(ctx);
    putenv(const_cast<char *>("PROJ_NETWORK="));
#endif

    proj_cleanup();

    // still disabled
    ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_log_func(ctx, nullptr, silent_logger);
    P = proj_create(ctx, pipeline);
    ASSERT_EQ(P, nullptr);
    proj_context_destroy(ctx);

    proj_cleanup();

    // enable through API
    ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);
    P = proj_create(ctx, pipeline);
#ifdef CURL_ENABLED
    if (networkAccessOK) {
        ASSERT_NE(P, nullptr);
    } else {
        ASSERT_EQ(P, nullptr);
        proj_context_destroy(ctx);
        return;
    }
    double longitude = 2;
    double lat = 49;
    proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1, &lat,
                       sizeof(double), 1, nullptr, 0, 0, nullptr, 0, 0);
    EXPECT_NEAR(longitude, 1.9992776848, 1e-10);
    EXPECT_NEAR(lat, 48.9999322600, 1e-10);

    proj_destroy(P);
#else
    ASSERT_EQ(P, nullptr);
#endif
    proj_context_destroy(ctx);
    proj_cleanup();
}

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

TEST(networking, curl_invalid_resource) {
    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);
    proj_log_func(ctx, nullptr, silent_logger);
    auto P = proj_create(
        ctx, "+proj=hgridshift +grids=https://i_do_not.exist/my.tif");
    proj_context_destroy(ctx);
    ASSERT_EQ(P, nullptr);
}
#endif

// ---------------------------------------------------------------------------

struct Event {
    virtual ~Event();
    std::string type{};
    PJ_CONTEXT *ctx = nullptr;
};

Event::~Event() = default;

struct OpenEvent : public Event {
    OpenEvent() { type = "OpenEvent"; }

    std::string url{};
    unsigned long long offset = 0;
    size_t size_to_read = 0;
    std::vector<unsigned char> response{};
    std::string errorMsg{};
    int file_id = 0;
};

struct CloseEvent : public Event {
    CloseEvent() { type = "CloseEvent"; }

    int file_id = 0;
};

struct GetHeaderValueEvent : public Event {
    GetHeaderValueEvent() { type = "GetHeaderValueEvent"; }

    int file_id = 0;
    std::string key{};
    std::string value{};
};

struct ReadRangeEvent : public Event {
    ReadRangeEvent() { type = "ReadRangeEvent"; }

    unsigned long long offset = 0;
    size_t size_to_read = 0;
    std::vector<unsigned char> response{};
    std::string errorMsg{};
    int file_id = 0;
};

struct File {};

struct ExchangeWithCallback {
    std::vector<std::unique_ptr<Event>> events{};
    size_t nextEvent = 0;
    bool error = false;
    std::map<int, PROJ_NETWORK_HANDLE *> mapIdToHandle{};

    bool allConsumedAndNoError() const {
        return nextEvent == events.size() && !error;
    }
};

static PROJ_NETWORK_HANDLE *open_cbk(PJ_CONTEXT *ctx, const char *url,
                                     unsigned long long offset,
                                     size_t size_to_read, void *buffer,
                                     size_t *out_size_read,
                                     size_t error_string_max_size,
                                     char *out_error_string, void *user_data) {
    auto exchange = static_cast<ExchangeWithCallback *>(user_data);
    if (exchange->error)
        return nullptr;
    if (exchange->nextEvent >= exchange->events.size()) {
        fprintf(stderr, "unexpected call to open(%s, %ld, %ld)\n", url,
                (long)offset, (long)size_to_read);
        exchange->error = true;
        return nullptr;
    }
    auto openEvent =
        dynamic_cast<OpenEvent *>(exchange->events[exchange->nextEvent].get());
    if (!openEvent) {
        fprintf(stderr,
                "unexpected call to open(%s, %ld, %ld). "
                "Was expecting a %s event\n",
                url, (long)offset, (long)size_to_read,
                exchange->events[exchange->nextEvent]->type.c_str());
        exchange->error = true;
        return nullptr;
    }
    exchange->nextEvent++;
    if (openEvent->ctx != ctx || openEvent->url != url ||
        openEvent->offset != offset ||
        openEvent->size_to_read != size_to_read) {
        fprintf(stderr,
                "wrong call to open(%s, %ld, %ld). Was expecting "
                "open(%s, %ld, %ld)\n",
                url, (long)offset, (long)size_to_read, openEvent->url.c_str(),
                (long)openEvent->offset, (long)openEvent->size_to_read);
        exchange->error = true;
        return nullptr;
    }
    if (!openEvent->errorMsg.empty()) {
        snprintf(out_error_string, error_string_max_size, "%s",
                 openEvent->errorMsg.c_str());
        return nullptr;
    }

    memcpy(buffer, openEvent->response.data(), openEvent->response.size());
    *out_size_read = openEvent->response.size();
    auto handle = reinterpret_cast<PROJ_NETWORK_HANDLE *>(new File());
    exchange->mapIdToHandle[openEvent->file_id] = handle;
    return handle;
}

static void close_cbk(PJ_CONTEXT *ctx, PROJ_NETWORK_HANDLE *handle,
                      void *user_data) {
    auto exchange = static_cast<ExchangeWithCallback *>(user_data);
    if (exchange->error)
        return;
    if (exchange->nextEvent >= exchange->events.size()) {
        fprintf(stderr, "unexpected call to close()\n");
        exchange->error = true;
        return;
    }
    auto closeEvent =
        dynamic_cast<CloseEvent *>(exchange->events[exchange->nextEvent].get());
    if (!closeEvent) {
        fprintf(stderr,
                "unexpected call to close(). "
                "Was expecting a %s event\n",
                exchange->events[exchange->nextEvent]->type.c_str());
        exchange->error = true;
        return;
    }
    if (closeEvent->ctx != ctx) {
        fprintf(stderr, "close() called with bad context\n");
        exchange->error = true;
        return;
    }
    if (exchange->mapIdToHandle[closeEvent->file_id] != handle) {
        fprintf(stderr, "close() called with bad handle\n");
        exchange->error = true;
        return;
    }
    exchange->nextEvent++;
    delete reinterpret_cast<File *>(handle);
}

static const char *get_header_value_cbk(PJ_CONTEXT *ctx,
                                        PROJ_NETWORK_HANDLE *handle,
                                        const char *header_name,
                                        void *user_data) {
    auto exchange = static_cast<ExchangeWithCallback *>(user_data);
    if (exchange->error)
        return nullptr;
    if (exchange->nextEvent >= exchange->events.size()) {
        fprintf(stderr, "unexpected call to get_header_value()\n");
        exchange->error = true;
        return nullptr;
    }
    auto getHeaderValueEvent = dynamic_cast<GetHeaderValueEvent *>(
        exchange->events[exchange->nextEvent].get());
    if (!getHeaderValueEvent) {
        fprintf(stderr,
                "unexpected call to get_header_value(). "
                "Was expecting a %s event\n",
                exchange->events[exchange->nextEvent]->type.c_str());
        exchange->error = true;
        return nullptr;
    }
    if (getHeaderValueEvent->ctx != ctx) {
        fprintf(stderr, "get_header_value() called with bad context\n");
        exchange->error = true;
        return nullptr;
    }
    if (getHeaderValueEvent->key != header_name) {
        fprintf(stderr,
                "wrong call to get_header_value(%s). Was expecting "
                "get_header_value(%s)\n",
                header_name, getHeaderValueEvent->key.c_str());
        exchange->error = true;
        return nullptr;
    }
    if (exchange->mapIdToHandle[getHeaderValueEvent->file_id] != handle) {
        fprintf(stderr, "get_header_value() called with bad handle\n");
        exchange->error = true;
        return nullptr;
    }
    exchange->nextEvent++;
    return getHeaderValueEvent->value.c_str();
}

static size_t read_range_cbk(PJ_CONTEXT *ctx, PROJ_NETWORK_HANDLE *handle,
                             unsigned long long offset, size_t size_to_read,
                             void *buffer, size_t error_string_max_size,
                             char *out_error_string, void *user_data) {
    auto exchange = static_cast<ExchangeWithCallback *>(user_data);
    if (exchange->error)
        return 0;
    if (exchange->nextEvent >= exchange->events.size()) {
        fprintf(stderr, "unexpected call to read_range(%ld, %ld)\n",
                (long)offset, (long)size_to_read);
        exchange->error = true;
        return 0;
    }
    auto readRangeEvent = dynamic_cast<ReadRangeEvent *>(
        exchange->events[exchange->nextEvent].get());
    if (!readRangeEvent) {
        fprintf(stderr,
                "unexpected call to read_range(). "
                "Was expecting a %s event\n",
                exchange->events[exchange->nextEvent]->type.c_str());
        exchange->error = true;
        return 0;
    }
    if (exchange->mapIdToHandle[readRangeEvent->file_id] != handle) {
        fprintf(stderr, "read_range() called with bad handle\n");
        exchange->error = true;
        return 0;
    }
    if (readRangeEvent->ctx != ctx || readRangeEvent->offset != offset ||
        readRangeEvent->size_to_read != size_to_read) {
        fprintf(stderr,
                "wrong call to read_range(%ld, %ld). Was expecting "
                "read_range(%ld, %ld)\n",
                (long)offset, (long)size_to_read, (long)readRangeEvent->offset,
                (long)readRangeEvent->size_to_read);
        exchange->error = true;
        return 0;
    }
    exchange->nextEvent++;
    if (!readRangeEvent->errorMsg.empty()) {
        snprintf(out_error_string, error_string_max_size, "%s",
                 readRangeEvent->errorMsg.c_str());
        return 0;
    }
    memcpy(buffer, readRangeEvent->response.data(),
           readRangeEvent->response.size());
    return readRangeEvent->response.size();
}

TEST(networking, custom) {
    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);
    ExchangeWithCallback exchange;
    ASSERT_TRUE(proj_context_set_network_callbacks(ctx, open_cbk, close_cbk,
                                                   get_header_value_cbk,
                                                   read_range_cbk, &exchange));

    {
        std::unique_ptr<OpenEvent> event(new OpenEvent());
        event->ctx = ctx;
        event->url = "https://foo/my.tif";
        event->offset = 0;
        event->size_to_read = 16384;
        event->response.resize(16384);
        event->file_id = 1;

        const char *proj_source_data = getenv("PROJ_SOURCE_DATA");
        ASSERT_TRUE(proj_source_data != nullptr);
        std::string filename(proj_source_data);
        filename += "/tests/egm96_15_uncompressed_truncated.tif";
        FILE *f = fopen(filename.c_str(), "rb");
        ASSERT_TRUE(f != nullptr);
        ASSERT_EQ(fread(&event->response[0], 1, 956, f), 956U);
        fclose(f);
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Content-Range";
        event->value = "bytes=0-16383/10000000";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Last-Modified";
        event->value = "some_date";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "ETag";
        event->value = "some_etag";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<CloseEvent> event(new CloseEvent());
        event->ctx = ctx;
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }

    auto P = proj_create(
        ctx, "+proj=vgridshift +grids=https://foo/my.tif +multiplier=1");

    ASSERT_NE(P, nullptr);
    ASSERT_TRUE(exchange.allConsumedAndNoError());

    {
        std::unique_ptr<OpenEvent> event(new OpenEvent());
        event->ctx = ctx;
        event->url = "https://foo/my.tif";
        event->offset = 524288;
        event->size_to_read = 278528;
        event->response.resize(278528);
        event->file_id = 2;
        float f = 1.25;
        if (!IS_LSB) {
            swap_words(&f, sizeof(f), 1);
        }
        for (size_t i = 0; i < 278528 / sizeof(float); i++) {
            memcpy(&event->response[i * sizeof(float)], &f, sizeof(float));
        }
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Content-Range";
        event->value = "bytes=0-16383/10000000";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Last-Modified";
        event->value = "some_date";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "ETag";
        event->value = "some_etag";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        double longitude = 2 / 180. * M_PI;
        double lat = 49 / 180. * M_PI;
        double z = 0;
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1,
                                     &lat, sizeof(double), 1, &z,
                                     sizeof(double), 1, nullptr, 0, 0),
                  1U);
        EXPECT_EQ(z, 1.25);
    }

    ASSERT_TRUE(exchange.allConsumedAndNoError());

    {
        std::unique_ptr<ReadRangeEvent> event(new ReadRangeEvent());
        event->ctx = ctx;
        event->offset = 3670016;
        event->size_to_read = 278528;
        event->response.resize(278528);
        event->file_id = 2;
        float f = 2.25;
        if (!IS_LSB) {
            swap_words(&f, sizeof(f), 1);
        }
        for (size_t i = 0; i < 278528 / sizeof(float); i++) {
            memcpy(&event->response[i * sizeof(float)], &f, sizeof(float));
        }
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Content-Range";
        event->value = "bytes=0-16383/10000000";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Last-Modified";
        event->value = "some_date";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "ETag";
        event->value = "some_etag";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }

    {
        double longitude = 2 / 180. * M_PI;
        double lat = -49 / 180. * M_PI;
        double z = 0;
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1,
                                     &lat, sizeof(double), 1, &z,
                                     sizeof(double), 1, nullptr, 0, 0),
                  1U);
        EXPECT_EQ(z, 2.25);
    }
    {
        std::unique_ptr<CloseEvent> event(new CloseEvent());
        event->ctx = ctx;
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    proj_destroy(P);

    ASSERT_TRUE(exchange.allConsumedAndNoError());

    // Once again ! No network access

    P = proj_create(ctx,
                    "+proj=vgridshift +grids=https://foo/my.tif +multiplier=1");
    ASSERT_NE(P, nullptr);

    {
        double longitude = 2 / 180. * M_PI;
        double lat = 49 / 180. * M_PI;
        double z = 0;
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1,
                                     &lat, sizeof(double), 1, &z,
                                     sizeof(double), 1, nullptr, 0, 0),
                  1U);
        EXPECT_EQ(z, 1.25);
    }

    proj_destroy(P);

    ASSERT_TRUE(exchange.allConsumedAndNoError());

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

TEST(networking, getfilesize) {
    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);
    ExchangeWithCallback exchange;
    ASSERT_TRUE(proj_context_set_network_callbacks(ctx, open_cbk, close_cbk,
                                                   get_header_value_cbk,
                                                   read_range_cbk, &exchange));

    {
        std::unique_ptr<OpenEvent> event(new OpenEvent());
        event->ctx = ctx;
        event->url = "https://foo/getfilesize.tif";
        event->offset = 0;
        event->size_to_read = 16384;
        event->response.resize(16384);
        event->file_id = 1;

        const char *proj_source_data = getenv("PROJ_SOURCE_DATA");
        ASSERT_TRUE(proj_source_data != nullptr);
        std::string filename(proj_source_data);
        filename += "/tests/test_vgrid_single_strip_truncated.tif";
        FILE *f = fopen(filename.c_str(), "rb");
        ASSERT_TRUE(f != nullptr);
        ASSERT_EQ(fread(&event->response[0], 1, 550, f), 550U);
        fclose(f);
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Content-Range";
        event->value = "bytes 0-16383/4153510";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Last-Modified";
        event->value = "some_date";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "ETag";
        event->value = "some_etag";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<CloseEvent> event(new CloseEvent());
        event->ctx = ctx;
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }

    auto P = proj_create(
        ctx,
        "+proj=vgridshift +grids=https://foo/getfilesize.tif +multiplier=1");

    ASSERT_NE(P, nullptr);
    ASSERT_TRUE(exchange.allConsumedAndNoError());

    proj_destroy(P);

    P = proj_create(
        ctx,
        "+proj=vgridshift +grids=https://foo/getfilesize.tif +multiplier=1");

    ASSERT_NE(P, nullptr);
    ASSERT_TRUE(exchange.allConsumedAndNoError());

    proj_destroy(P);

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

TEST(networking, simul_open_error) {
    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_log_func(ctx, nullptr, silent_logger);
    proj_context_set_enable_network(ctx, true);
    ExchangeWithCallback exchange;
    ASSERT_TRUE(proj_context_set_network_callbacks(ctx, open_cbk, close_cbk,
                                                   get_header_value_cbk,
                                                   read_range_cbk, &exchange));

    {
        std::unique_ptr<OpenEvent> event(new OpenEvent());
        event->ctx = ctx;
        event->url = "https://foo/open_error.tif";
        event->offset = 0;
        event->size_to_read = 16384;
        event->errorMsg = "Cannot open file";
        event->file_id = 1;

        exchange.events.emplace_back(std::move(event));
    }

    auto P = proj_create(
        ctx,
        "+proj=vgridshift +grids=https://foo/open_error.tif +multiplier=1");

    ASSERT_EQ(P, nullptr);
    ASSERT_TRUE(exchange.allConsumedAndNoError());

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

TEST(networking, simul_read_range_error) {
    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);
    ExchangeWithCallback exchange;
    ASSERT_TRUE(proj_context_set_network_callbacks(ctx, open_cbk, close_cbk,
                                                   get_header_value_cbk,
                                                   read_range_cbk, &exchange));

    {
        std::unique_ptr<OpenEvent> event(new OpenEvent());
        event->ctx = ctx;
        event->url = "https://foo/read_range_error.tif";
        event->offset = 0;
        event->size_to_read = 16384;
        event->response.resize(16384);
        event->file_id = 1;

        const char *proj_source_data = getenv("PROJ_SOURCE_DATA");
        ASSERT_TRUE(proj_source_data != nullptr);
        std::string filename(proj_source_data);
        filename += "/tests/egm96_15_uncompressed_truncated.tif";
        FILE *f = fopen(filename.c_str(), "rb");
        ASSERT_TRUE(f != nullptr);
        ASSERT_EQ(fread(&event->response[0], 1, 956, f), 956U);
        fclose(f);
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Content-Range";
        event->value = "bytes=0-16383/10000000";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Last-Modified";
        event->value = "some_date";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "ETag";
        event->value = "some_etag";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<CloseEvent> event(new CloseEvent());
        event->ctx = ctx;
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }

    auto P = proj_create(ctx, "+proj=vgridshift "
                              "+grids=https://foo/read_range_error.tif "
                              "+multiplier=1");

    ASSERT_NE(P, nullptr);
    ASSERT_TRUE(exchange.allConsumedAndNoError());

    {
        std::unique_ptr<OpenEvent> event(new OpenEvent());
        event->ctx = ctx;
        event->url = "https://foo/read_range_error.tif";
        event->offset = 524288;
        event->size_to_read = 278528;
        event->response.resize(278528);
        event->file_id = 2;
        float f = 1.25;
        if (!IS_LSB) {
            swap_words(&f, sizeof(f), 1);
        }
        for (size_t i = 0; i < 278528 / sizeof(float); i++) {
            memcpy(&event->response[i * sizeof(float)], &f, sizeof(float));
        }
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Content-Range";
        event->value = "bytes=0-16383/10000000";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Last-Modified";
        event->value = "some_date";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "ETag";
        event->value = "some_etag";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }

    {
        double longitude = 2 / 180. * M_PI;
        double lat = 49 / 180. * M_PI;
        double z = 0;
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1,
                                     &lat, sizeof(double), 1, &z,
                                     sizeof(double), 1, nullptr, 0, 0),
                  1U);
        EXPECT_EQ(z, 1.25);
    }

    ASSERT_TRUE(exchange.allConsumedAndNoError());

    {
        std::unique_ptr<ReadRangeEvent> event(new ReadRangeEvent());
        event->ctx = ctx;
        event->offset = 3670016;
        event->size_to_read = 278528;
        event->errorMsg = "read range error";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }

    {
        double longitude = 2 / 180. * M_PI;
        double lat = -49 / 180. * M_PI;
        double z = 0;
        proj_log_func(ctx, nullptr, silent_logger);
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1,
                                     &lat, sizeof(double), 1, &z,
                                     sizeof(double), 1, nullptr, 0, 0),
                  1U);
        EXPECT_EQ(z, HUGE_VAL);
    }
    {
        std::unique_ptr<CloseEvent> event(new CloseEvent());
        event->ctx = ctx;
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    proj_destroy(P);

    ASSERT_TRUE(exchange.allConsumedAndNoError());

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

TEST(networking, simul_file_change_while_opened) {
    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);
    ExchangeWithCallback exchange;
    ASSERT_TRUE(proj_context_set_network_callbacks(ctx, open_cbk, close_cbk,
                                                   get_header_value_cbk,
                                                   read_range_cbk, &exchange));

    {
        std::unique_ptr<OpenEvent> event(new OpenEvent());
        event->ctx = ctx;
        event->url = "https://foo/file_change_while_opened.tif";
        event->offset = 0;
        event->size_to_read = 16384;
        event->response.resize(16384);
        event->file_id = 1;

        const char *proj_source_data = getenv("PROJ_SOURCE_DATA");
        ASSERT_TRUE(proj_source_data != nullptr);
        std::string filename(proj_source_data);
        filename += "/tests/egm96_15_uncompressed_truncated.tif";
        FILE *f = fopen(filename.c_str(), "rb");
        ASSERT_TRUE(f != nullptr);
        ASSERT_EQ(fread(&event->response[0], 1, 956, f), 956U);
        fclose(f);
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Content-Range";
        event->value = "bytes=0-16383/10000000";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Last-Modified";
        event->value = "some_date";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "ETag";
        event->value = "some_etag";
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<CloseEvent> event(new CloseEvent());
        event->ctx = ctx;
        event->file_id = 1;
        exchange.events.emplace_back(std::move(event));
    }

    auto P = proj_create(ctx, "+proj=vgridshift "
                              "+grids=https://foo/file_change_while_opened.tif "
                              "+multiplier=1");

    ASSERT_NE(P, nullptr);
    ASSERT_TRUE(exchange.allConsumedAndNoError());

    {
        std::unique_ptr<OpenEvent> event(new OpenEvent());
        event->ctx = ctx;
        event->url = "https://foo/file_change_while_opened.tif";
        event->offset = 524288;
        event->size_to_read = 278528;
        event->response.resize(278528);
        event->file_id = 2;
        float f = 1.25;
        if (!IS_LSB) {
            swap_words(&f, sizeof(f), 1);
        }
        for (size_t i = 0; i < 278528 / sizeof(float); i++) {
            memcpy(&event->response[i * sizeof(float)], &f, sizeof(float));
        }
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Content-Range";
        event->value = "bytes=0-16383/10000000";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Last-Modified";
        event->value = "some_date CHANGED!!!!";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "ETag";
        event->value = "some_etag";
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<CloseEvent> event(new CloseEvent());
        event->ctx = ctx;
        event->file_id = 2;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<OpenEvent> event(new OpenEvent());
        event->ctx = ctx;
        event->url = "https://foo/file_change_while_opened.tif";
        event->offset = 0;
        event->size_to_read = 16384;
        event->response.resize(16384);
        event->file_id = 3;

        const char *proj_source_data = getenv("PROJ_SOURCE_DATA");
        ASSERT_TRUE(proj_source_data != nullptr);
        std::string filename(proj_source_data);
        filename += "/tests/egm96_15_uncompressed_truncated.tif";
        FILE *f = fopen(filename.c_str(), "rb");
        ASSERT_TRUE(f != nullptr);
        ASSERT_EQ(fread(&event->response[0], 1, 956, f), 956U);
        fclose(f);
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Content-Range";
        event->value = "bytes=0-16383/10000000";
        event->file_id = 3;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "Last-Modified";
        event->value = "some_date CHANGED!!!!";
        event->file_id = 3;
        exchange.events.emplace_back(std::move(event));
    }
    {
        std::unique_ptr<GetHeaderValueEvent> event(new GetHeaderValueEvent());
        event->ctx = ctx;
        event->key = "ETag";
        event->value = "some_etag";
        event->file_id = 3;
        exchange.events.emplace_back(std::move(event));
    }

    {
        double longitude = 2 / 180. * M_PI;
        double lat = 49 / 180. * M_PI;
        double z = 0;
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1,
                                     &lat, sizeof(double), 1, &z,
                                     sizeof(double), 1, nullptr, 0, 0),
                  1U);
        EXPECT_EQ(z, 1.25);
    }

    ASSERT_TRUE(exchange.allConsumedAndNoError());

    {
        std::unique_ptr<CloseEvent> event(new CloseEvent());
        event->ctx = ctx;
        event->file_id = 3;
        exchange.events.emplace_back(std::move(event));
    }
    proj_destroy(P);

    ASSERT_TRUE(exchange.allConsumedAndNoError());

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

TEST(networking, curl_hgridshift) {
    if (!networkAccessOK) {
        return;
    }

    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);

    // NTF to RGF93 v1. Using fr_ign_gr3df97a.tif
    auto P = proj_create_crs_to_crs(ctx, "EPSG:4275", "EPSG:4171", nullptr);
    ASSERT_NE(P, nullptr);

    PJ_COORD c;
    c.xyz.x = 49; // lat
    c.xyz.y = 2;  // long
    c.xyz.z = 0;
    c = proj_trans(P, PJ_FWD, c);

    proj_assign_context(P, ctx); // (dummy) test context reassignment

    proj_destroy(P);
    proj_context_destroy(ctx);

    EXPECT_NEAR(c.xyz.x, 48.9999322600, 1e-8);
    EXPECT_NEAR(c.xyz.y, 1.9992776848, 1e-8);
    EXPECT_NEAR(c.xyz.z, 0, 1e-2);
}

#endif

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

TEST(networking, curl_vgridshift) {
    if (!networkAccessOK) {
        return;
    }

    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);

    // WGS84 to EGM2008 height. Using egm08_25.tif
    auto P =
        proj_create_crs_to_crs(ctx, "EPSG:4979", "EPSG:4326+3855", nullptr);
    ASSERT_NE(P, nullptr);

    PJ_COORD c;
    c.xyz.x = -30; // lat
    c.xyz.y = 150; // long
    c.xyz.z = 0;
    c = proj_trans(P, PJ_FWD, c);

    proj_assign_context(P, ctx); // (dummy) test context reassignment

    proj_destroy(P);
    proj_context_destroy(ctx);

    EXPECT_NEAR(c.xyz.x, -30, 1e-8);
    EXPECT_NEAR(c.xyz.y, 150, 1e-8);
    EXPECT_NEAR(c.xyz.z, -31.89, 1e-2);
}

#endif

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

TEST(networking, curl_vgridshift_vertcon) {
    if (!networkAccessOK) {
        return;
    }

    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);

    // NGVD29 to NAVD88 height. Using vertcone.tif
    auto P = proj_create_crs_to_crs(ctx, "EPSG:4269+7968", "EPSG:4269+5703",
                                    nullptr);
    ASSERT_NE(P, nullptr);

    PJ_COORD c;
    c.xyz.x = 40;  // lat
    c.xyz.y = -80; // long
    c.xyz.z = 0;
    c = proj_trans(P, PJ_FWD, c);

    proj_destroy(P);
    proj_context_destroy(ctx);

    EXPECT_NEAR(c.xyz.x, 40, 1e-8);
    EXPECT_NEAR(c.xyz.y, -80, 1e-8);
    EXPECT_NEAR(c.xyz.z, -0.15, 1e-2);
}

#endif

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

TEST(networking, network_endpoint_env_variable) {
    putenv(const_cast<char *>("PROJ_NETWORK_ENDPOINT=http://0.0.0.0/"));
    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);

    // NAD83 to NAD83(HARN) in West-Virginia. Using wvhpgn.tif
    auto P = proj_create_crs_to_crs(ctx, "EPSG:4269", "EPSG:4152", nullptr);
    ASSERT_NE(P, nullptr);

    PJ_COORD c;
    c.xyz.x = 40;  // lat
    c.xyz.y = -80; // long
    c.xyz.z = 0;
    c = proj_trans(P, PJ_FWD, c);
    putenv(const_cast<char *>("PROJ_NETWORK_ENDPOINT="));

    proj_destroy(P);
    proj_context_destroy(ctx);

    EXPECT_EQ(c.xyz.x, HUGE_VAL);
}

#endif

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

TEST(networking, network_endpoint_api) {
    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);
    proj_context_set_url_endpoint(ctx, "http://0.0.0.0");

    // NAD83 to NAD83(HARN) in West-Virginia. Using wvhpgn.tif
    auto P = proj_create_crs_to_crs(ctx, "EPSG:4269", "EPSG:4152", nullptr);
    ASSERT_NE(P, nullptr);

    PJ_COORD c;
    c.xyz.x = 40;  // lat
    c.xyz.y = -80; // long
    c.xyz.z = 0;
    c = proj_trans(P, PJ_FWD, c);

    proj_destroy(P);
    proj_context_destroy(ctx);

    EXPECT_EQ(c.xyz.x, HUGE_VAL);
}

#endif

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

static PROJ_NETWORK_HANDLE *dummy_open_cbk(PJ_CONTEXT *, const char *,
                                           unsigned long long, size_t, void *,
                                           size_t *, size_t, char *, void *) {
    assert(false);
    return nullptr;
}

static void dummy_close_cbk(PJ_CONTEXT *, PROJ_NETWORK_HANDLE *, void *) {
    assert(false);
}

static const char *dummy_get_header_value_cbk(PJ_CONTEXT *,
                                              PROJ_NETWORK_HANDLE *,
                                              const char *, void *) {
    assert(false);
    return nullptr;
}

static size_t dummy_read_range_cbk(PJ_CONTEXT *, PROJ_NETWORK_HANDLE *,
                                   unsigned long long, size_t, void *, size_t,
                                   char *, void *) {
    assert(false);
    return 0;
}

TEST(networking, cache_basic) {
    if (!networkAccessOK) {
        return;
    }

    proj_cleanup();

    const char *pipeline =
        "+proj=pipeline "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=hgridshift +grids=https://cdn.proj.org/fr_ign_ntf_r93.tif "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg";

    auto ctx = proj_context_create();
    proj_context_set_enable_network(ctx, true);

    auto P = proj_create(ctx, pipeline);
    ASSERT_NE(P, nullptr);
    proj_destroy(P);

    EXPECT_TRUE(!pj_context_get_grid_cache_filename(ctx).empty());

    sqlite3 *hDB = nullptr;
    sqlite3_open_v2(pj_context_get_grid_cache_filename(ctx).c_str(), &hDB,
                    SQLITE_OPEN_READONLY, nullptr);
    ASSERT_NE(hDB, nullptr);
    sqlite3_stmt *hStmt = nullptr;
    sqlite3_prepare_v2(hDB,
                       "SELECT url, offset FROM chunks WHERE id = ("
                       "SELECT chunk_id FROM linked_chunks WHERE id = ("
                       "SELECT head FROM linked_chunks_head_tail))",
                       -1, &hStmt, nullptr);
    ASSERT_NE(hStmt, nullptr);
    ASSERT_EQ(sqlite3_step(hStmt), SQLITE_ROW);
    const char *url =
        reinterpret_cast<const char *>(sqlite3_column_text(hStmt, 0));
    ASSERT_NE(url, nullptr);
    ASSERT_EQ(std::string(url), "https://cdn.proj.org/fr_ign_ntf_r93.tif");
    ASSERT_EQ(sqlite3_column_int64(hStmt, 1), 0);
    sqlite3_finalize(hStmt);
    sqlite3_close(hDB);

    proj_cleanup();

    // Check that a second access doesn't trigger any network activity
    ASSERT_TRUE(proj_context_set_network_callbacks(
        ctx, dummy_open_cbk, dummy_close_cbk, dummy_get_header_value_cbk,
        dummy_read_range_cbk, nullptr));
    P = proj_create(ctx, pipeline);
    ASSERT_NE(P, nullptr);
    proj_destroy(P);

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

TEST(networking, proj_grid_cache_clear) {
    if (!networkAccessOK) {
        return;
    }
    const char *pipeline =
        "+proj=pipeline "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=hgridshift +grids=https://cdn.proj.org/fr_ign_ntf_r93.tif "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg";

    proj_cleanup();

    auto ctx = proj_context_create();
    proj_context_set_enable_network(ctx, true);
    proj_grid_cache_set_filename(ctx, "tmp_proj_db_cache.db");
    EXPECT_EQ(pj_context_get_grid_cache_filename(ctx),
              std::string("tmp_proj_db_cache.db"));

    proj_grid_cache_clear(ctx);

    auto P = proj_create(ctx, pipeline);
    ASSERT_NE(P, nullptr);
    proj_destroy(P);

    // Check that the file exists
    {
        sqlite3 *hDB = nullptr;
        ASSERT_EQ(
            sqlite3_open_v2(pj_context_get_grid_cache_filename(ctx).c_str(),
                            &hDB, SQLITE_OPEN_READONLY, nullptr),
            SQLITE_OK);
        sqlite3_close(hDB);
    }

    proj_grid_cache_clear(ctx);

    // Check that the file no longer exists
    {
        sqlite3 *hDB = nullptr;
        ASSERT_NE(
            sqlite3_open_v2(pj_context_get_grid_cache_filename(ctx).c_str(),
                            &hDB, SQLITE_OPEN_READONLY, nullptr),
            SQLITE_OK);
        sqlite3_close(hDB);
    }

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

TEST(networking, cache_saturation) {
    if (!networkAccessOK) {
        return;
    }
    const char *pipeline =
        "+proj=pipeline "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=hgridshift +grids=https://cdn.proj.org/fr_ign_ntf_r93.tif "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg";

    proj_cleanup();

    auto ctx = proj_context_create();
    proj_context_set_enable_network(ctx, true);
    proj_grid_cache_set_filename(ctx, "tmp_proj_db_cache.db");

    proj_grid_cache_clear(ctx);

    // Limit to two chunks
    putenv(const_cast<char *>("PROJ_GRID_CACHE_MAX_SIZE_BYTES=32768"));
    proj_grid_cache_set_max_size(ctx, 0);
    putenv(const_cast<char *>("PROJ_GRID_CACHE_MAX_SIZE_BYTES="));

    auto P = proj_create(ctx, pipeline);
    ASSERT_NE(P, nullptr);

    double longitude = 2;
    double lat = 49;
    proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1, &lat,
                       sizeof(double), 1, nullptr, 0, 0, nullptr, 0, 0);
    EXPECT_NEAR(longitude, 1.9992776848, 1e-10);
    EXPECT_NEAR(lat, 48.9999322600, 1e-10);

    proj_destroy(P);

    sqlite3 *hDB = nullptr;
    sqlite3_open_v2(pj_context_get_grid_cache_filename(ctx).c_str(), &hDB,
                    SQLITE_OPEN_READONLY, nullptr);
    ASSERT_NE(hDB, nullptr);

    sqlite3_stmt *hStmt = nullptr;
    sqlite3_prepare_v2(hDB,
                       "SELECT COUNT(*) FROM chunk_data UNION ALL "
                       "SELECT COUNT(*) FROM chunks UNION ALL "
                       "SELECT COUNT(*) FROM linked_chunks",
                       -1, &hStmt, nullptr);
    ASSERT_NE(hStmt, nullptr);
    ASSERT_EQ(sqlite3_step(hStmt), SQLITE_ROW);
    ASSERT_EQ(sqlite3_column_int64(hStmt, 0), 2);
    ASSERT_EQ(sqlite3_step(hStmt), SQLITE_ROW);
    ASSERT_EQ(sqlite3_column_int64(hStmt, 0), 2);
    ASSERT_EQ(sqlite3_step(hStmt), SQLITE_ROW);
    ASSERT_EQ(sqlite3_column_int64(hStmt, 0), 2);
    sqlite3_finalize(hStmt);
    sqlite3_close(hDB);

    proj_grid_cache_clear(ctx);

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

TEST(networking, cache_ttl) {
    if (!networkAccessOK) {
        return;
    }
    const char *pipeline =
        "+proj=pipeline "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=hgridshift +grids=https://cdn.proj.org/fr_ign_ntf_r93.tif "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg";

    proj_cleanup();

    auto ctx = proj_context_create();
    proj_context_set_enable_network(ctx, true);
    proj_grid_cache_set_filename(ctx, "tmp_proj_db_cache.db");

    proj_grid_cache_clear(ctx);

    auto P = proj_create(ctx, pipeline);
    ASSERT_NE(P, nullptr);

    double longitude = 2;
    double lat = 49;
    proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1, &lat,
                       sizeof(double), 1, nullptr, 0, 0, nullptr, 0, 0);
    EXPECT_NEAR(longitude, 1.9992776848, 1e-10);
    EXPECT_NEAR(lat, 48.9999322600, 1e-10);

    proj_destroy(P);

    sqlite3 *hDB = nullptr;
    sqlite3_open_v2(pj_context_get_grid_cache_filename(ctx).c_str(), &hDB,
                    SQLITE_OPEN_READWRITE, nullptr);
    ASSERT_NE(hDB, nullptr);

    // Force lastChecked to the Epoch so that data is expired.
    sqlite3_stmt *hStmt = nullptr;
    sqlite3_prepare_v2(hDB,
                       "UPDATE properties SET lastChecked = 0, "
                       "lastModified = 'foo', etag = 'bar'",
                       -1, &hStmt, nullptr);
    ASSERT_NE(hStmt, nullptr);
    ASSERT_EQ(sqlite3_step(hStmt), SQLITE_DONE);
    sqlite3_finalize(hStmt);

    // Put junk in already cached data to check that we will refresh it.
    hStmt = nullptr;
    sqlite3_prepare_v2(hDB, "UPDATE chunk_data SET data = zeroblob(16384)", -1,
                       &hStmt, nullptr);
    ASSERT_NE(hStmt, nullptr);
    ASSERT_EQ(sqlite3_step(hStmt), SQLITE_DONE);
    sqlite3_finalize(hStmt);
    sqlite3_close(hDB);

    proj_cleanup();

    // Set a never expire ttl
    proj_grid_cache_set_ttl(ctx, -1);

    // We'll get junk data, hence the pipeline initialization fails
    proj_log_func(ctx, nullptr, silent_logger);
    P = proj_create(ctx, pipeline);
    ASSERT_EQ(P, nullptr);
    proj_destroy(P);

    proj_cleanup();

    // Set a normal ttl
    proj_grid_cache_set_ttl(ctx, 86400);

    // Pipeline creation succeeds
    P = proj_create(ctx, pipeline);
    ASSERT_NE(P, nullptr);
    proj_destroy(P);

    hDB = nullptr;
    sqlite3_open_v2(pj_context_get_grid_cache_filename(ctx).c_str(), &hDB,
                    SQLITE_OPEN_READWRITE, nullptr);
    ASSERT_NE(hDB, nullptr);
    hStmt = nullptr;
    sqlite3_prepare_v2(hDB,
                       "SELECT lastChecked, lastModified, etag FROM properties",
                       -1, &hStmt, nullptr);
    ASSERT_NE(hStmt, nullptr);
    ASSERT_EQ(sqlite3_step(hStmt), SQLITE_ROW);
    ASSERT_NE(sqlite3_column_int64(hStmt, 0), 0);
    ASSERT_NE(sqlite3_column_text(hStmt, 1), nullptr);
    ASSERT_NE(std::string(reinterpret_cast<const char *>(
                  sqlite3_column_text(hStmt, 1))),
              "foo");
    ASSERT_NE(sqlite3_column_text(hStmt, 2), nullptr);
    ASSERT_NE(std::string(reinterpret_cast<const char *>(
                  sqlite3_column_text(hStmt, 2))),
              "bar");
    sqlite3_finalize(hStmt);
    sqlite3_close(hDB);

    proj_grid_cache_clear(ctx);

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

TEST(networking, cache_lock) {
    if (!networkAccessOK) {
        return;
    }
    const char *pipeline =
        "+proj=pipeline "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=hgridshift +grids=https://cdn.proj.org/fr_ign_ntf_r93.tif "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg";

    proj_cleanup();

    auto ctx = proj_context_create();
    proj_context_set_enable_network(ctx, true);
    proj_grid_cache_set_filename(ctx, "tmp_proj_db_cache.db");

    proj_grid_cache_clear(ctx);

    auto P = proj_create(ctx, pipeline);
    ASSERT_NE(P, nullptr);

    double longitude = 2;
    double lat = 49;
    proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1, &lat,
                       sizeof(double), 1, nullptr, 0, 0, nullptr, 0, 0);
    EXPECT_NEAR(longitude, 1.9992776848, 1e-10);
    EXPECT_NEAR(lat, 48.9999322600, 1e-10);

    proj_destroy(P);

    // Take a lock
    sqlite3 *hDB = nullptr;
    sqlite3_open_v2(pj_context_get_grid_cache_filename(ctx).c_str(), &hDB,
                    SQLITE_OPEN_READWRITE, nullptr);
    ASSERT_NE(hDB, nullptr);
    sqlite3_stmt *hStmt = nullptr;
    sqlite3_prepare_v2(hDB, "BEGIN EXCLUSIVE", -1, &hStmt, nullptr);
    ASSERT_NE(hStmt, nullptr);
    ASSERT_EQ(sqlite3_step(hStmt), SQLITE_DONE);
    sqlite3_finalize(hStmt);

    proj_cleanup();

    time_t start;
    time(&start);
    // 2 lock attempts, so we must sleep for each at least 0.5 ms
    putenv(const_cast<char *>("PROJ_LOCK_MAX_ITERS=25"));
    P = proj_create(ctx, pipeline);
    putenv(const_cast<char *>("PROJ_LOCK_MAX_ITERS="));
    ASSERT_NE(P, nullptr);
    proj_destroy(P);

    // Check that we have spend more than 1 sec
    time_t end;
    time(&end);
    ASSERT_GE(end - start, 1U);

    sqlite3_close(hDB);

    proj_grid_cache_clear(ctx);

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

TEST(networking, download_whole_files) {
    if (!networkAccessOK) {
        return;
    }

    proj_cleanup();
    unlink("proj_test_tmp/cache.db");
    unlink("proj_test_tmp/dk_sdfe_dvr90.tif");
    rmdir("proj_test_tmp");

    putenv(const_cast<char *>("PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY="));
    putenv(const_cast<char *>("PROJ_USER_WRITABLE_DIRECTORY=./proj_test_tmp"));
    putenv(const_cast<char *>("PROJ_FULL_FILE_CHUNK_SIZE=100000"));

    proj_context_set_enable_network(nullptr, true);
    const auto grid_info = proj_grid_info("dk_sdfe_dvr90.tif");
    EXPECT_EQ(std::string(grid_info.filename), "");
    EXPECT_EQ(std::string(grid_info.gridname), "dk_sdfe_dvr90.tif");
    EXPECT_EQ(std::string(grid_info.format), "gtiff");
    proj_context_set_enable_network(nullptr, false);

    auto ctx = proj_context_create();
    proj_context_set_enable_network(ctx, true);

    ASSERT_TRUE(proj_is_download_needed(ctx, "dk_sdfe_dvr90.tif", false));

    ASSERT_TRUE(
        proj_download_file(ctx, "dk_sdfe_dvr90.tif", false, nullptr, nullptr));

    FILE *f = fopen("proj_test_tmp/dk_sdfe_dvr90.tif", "rb");
    ASSERT_NE(f, nullptr);
    fclose(f);

    proj_context_set_enable_network(ctx, false);

    const char *pipeline =
        "+proj=pipeline "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=vgridshift +grids=dk_sdfe_dvr90.tif +multiplier=1 "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg";

    auto P = proj_create(ctx, pipeline);
    ASSERT_NE(P, nullptr);

    double longitude = 12;
    double lat = 56;
    double z = 0;
    proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1, &lat,
                       sizeof(double), 1, &z, sizeof(double), 1, nullptr, 0, 0);
    EXPECT_NEAR(z, 36.5909996032715, 1e-10);
    proj_destroy(P);

    proj_context_set_enable_network(ctx, true);

    ASSERT_FALSE(proj_is_download_needed(ctx, "dk_sdfe_dvr90.tif", false));

    {
        sqlite3 *hDB = nullptr;
        sqlite3_open_v2("proj_test_tmp/cache.db", &hDB, SQLITE_OPEN_READWRITE,
                        nullptr);
        ASSERT_NE(hDB, nullptr);
        // Force lastChecked to the Epoch so that data is expired.
        sqlite3_stmt *hStmt = nullptr;
        sqlite3_prepare_v2(
            hDB, "UPDATE downloaded_file_properties SET lastChecked = 0", -1,
            &hStmt, nullptr);
        ASSERT_NE(hStmt, nullptr);
        ASSERT_EQ(sqlite3_step(hStmt), SQLITE_DONE);
        sqlite3_finalize(hStmt);
        sqlite3_close(hDB);
    }

    // If we ignore TTL settings, then no network access will be done
    ASSERT_FALSE(proj_is_download_needed(ctx, "dk_sdfe_dvr90.tif", true));

    {
        sqlite3 *hDB = nullptr;
        sqlite3_open_v2("proj_test_tmp/cache.db", &hDB, SQLITE_OPEN_READWRITE,
                        nullptr);
        ASSERT_NE(hDB, nullptr);
        // Check that the lastChecked timestamp is still 0
        sqlite3_stmt *hStmt = nullptr;
        sqlite3_prepare_v2(hDB,
                           "SELECT lastChecked FROM downloaded_file_properties",
                           -1, &hStmt, nullptr);
        ASSERT_NE(hStmt, nullptr);
        ASSERT_EQ(sqlite3_step(hStmt), SQLITE_ROW);
        ASSERT_EQ(sqlite3_column_int64(hStmt, 0), 0);
        sqlite3_finalize(hStmt);
        sqlite3_close(hDB);
    }

    // Should recheck from the CDN, update last_checked and do nothing
    ASSERT_FALSE(proj_is_download_needed(ctx, "dk_sdfe_dvr90.tif", false));

    {
        sqlite3 *hDB = nullptr;
        sqlite3_open_v2("proj_test_tmp/cache.db", &hDB, SQLITE_OPEN_READWRITE,
                        nullptr);
        ASSERT_NE(hDB, nullptr);
        sqlite3_stmt *hStmt = nullptr;
        // Check that the lastChecked timestamp has been updated
        sqlite3_prepare_v2(hDB,
                           "SELECT lastChecked FROM downloaded_file_properties",
                           -1, &hStmt, nullptr);
        ASSERT_NE(hStmt, nullptr);
        ASSERT_EQ(sqlite3_step(hStmt), SQLITE_ROW);
        ASSERT_NE(sqlite3_column_int64(hStmt, 0), 0);
        sqlite3_finalize(hStmt);
        hStmt = nullptr;

        // Now invalid lastModified. This should trigger a new download
        sqlite3_prepare_v2(
            hDB,
            "UPDATE downloaded_file_properties SET lastChecked = 0, "
            "lastModified = 'foo'",
            -1, &hStmt, nullptr);
        ASSERT_NE(hStmt, nullptr);
        ASSERT_EQ(sqlite3_step(hStmt), SQLITE_DONE);
        sqlite3_finalize(hStmt);
        sqlite3_close(hDB);
    }

    ASSERT_TRUE(proj_is_download_needed(ctx, "dk_sdfe_dvr90.tif", false));

    // Redo download with a progress callback this time.
    unlink("proj_test_tmp/dk_sdfe_dvr90.tif");

    const auto cbk = [](PJ_CONTEXT *l_ctx, double pct, void *user_data) -> int {
        auto vect = static_cast<std::vector<std::pair<PJ_CONTEXT *, double>> *>(
            user_data);
        vect->push_back(std::pair<PJ_CONTEXT *, double>(l_ctx, pct));
        return true;
    };

    std::vector<std::pair<PJ_CONTEXT *, double>> vectPct;
    ASSERT_TRUE(
        proj_download_file(ctx, "dk_sdfe_dvr90.tif", false, cbk, &vectPct));
    ASSERT_EQ(vectPct.size(), 3U);
    ASSERT_EQ(vectPct.back().first, ctx);
    ASSERT_EQ(vectPct.back().second, 1.0);

    proj_context_destroy(ctx);
    putenv(const_cast<char *>("PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES"));
    putenv(const_cast<char *>("PROJ_USER_WRITABLE_DIRECTORY="));
    putenv(const_cast<char *>("PROJ_FULL_FILE_CHUNK_SIZE="));
    unlink("proj_test_tmp/cache.db");
    unlink("proj_test_tmp/dk_sdfe_dvr90.tif");
    rmdir("proj_test_tmp");
}

// ---------------------------------------------------------------------------

TEST(networking, file_api) {
    if (!networkAccessOK) {
        return;
    }

    proj_cleanup();
    unlink("proj_test_tmp/cache.db");
    unlink("proj_test_tmp/dk_sdfe_dvr90.tif");
    rmdir("proj_test_tmp");

    putenv(const_cast<char *>("PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY="));
    putenv(const_cast<char *>("PROJ_USER_WRITABLE_DIRECTORY=./proj_test_tmp"));
    putenv(const_cast<char *>("PROJ_FULL_FILE_CHUNK_SIZE=30000"));
    auto ctx = proj_context_create();
    proj_context_set_enable_network(ctx, true);

    struct UserData {
        bool in_open = false;
        bool in_read = false;
        bool in_write = false;
        bool in_seek = false;
        bool in_tell = false;
        bool in_close = false;
        bool in_exists = false;
        bool in_mkdir = false;
        bool in_unlink = false;
        bool in_rename = false;
    };

    struct PROJ_FILE_API api;
    api.version = 1;
    api.open_cbk = [](PJ_CONTEXT *, const char *filename,
                      PROJ_OPEN_ACCESS access,
                      void *user_data) -> PROJ_FILE_HANDLE * {
        static_cast<UserData *>(user_data)->in_open = true;
        return reinterpret_cast<PROJ_FILE_HANDLE *>(fopen(
            filename,
            access == PROJ_OPEN_ACCESS_READ_ONLY
                ? "rb"
                : access == PROJ_OPEN_ACCESS_READ_UPDATE ? "r+b" : "w+b"));
    };
    api.read_cbk = [](PJ_CONTEXT *, PROJ_FILE_HANDLE *handle, void *buffer,
                      size_t sizeBytes, void *user_data) -> size_t {
        static_cast<UserData *>(user_data)->in_read = true;
        return fread(buffer, 1, sizeBytes, reinterpret_cast<FILE *>(handle));
    };
    api.write_cbk = [](PJ_CONTEXT *, PROJ_FILE_HANDLE *handle,
                       const void *buffer, size_t sizeBytes,
                       void *user_data) -> size_t {
        static_cast<UserData *>(user_data)->in_write = true;
        return fwrite(buffer, 1, sizeBytes, reinterpret_cast<FILE *>(handle));
    };
    api.seek_cbk = [](PJ_CONTEXT *, PROJ_FILE_HANDLE *handle, long long offset,
                      int whence, void *user_data) -> int {
        static_cast<UserData *>(user_data)->in_seek = true;
        return fseek(reinterpret_cast<FILE *>(handle),
                     static_cast<long>(offset), whence) == 0;
    };
    api.tell_cbk = [](PJ_CONTEXT *, PROJ_FILE_HANDLE *handle,
                      void *user_data) -> unsigned long long {
        static_cast<UserData *>(user_data)->in_tell = true;
        return ftell(reinterpret_cast<FILE *>(handle));
    };
    api.close_cbk = [](PJ_CONTEXT *, PROJ_FILE_HANDLE *handle,
                       void *user_data) -> void {
        static_cast<UserData *>(user_data)->in_close = true;
        fclose(reinterpret_cast<FILE *>(handle));
    };
    api.exists_cbk = [](PJ_CONTEXT *, const char *filename,
                        void *user_data) -> int {
        static_cast<UserData *>(user_data)->in_exists = true;
        struct stat buf;
        return stat(filename, &buf) == 0;
    };
    api.mkdir_cbk = [](PJ_CONTEXT *, const char *filename,
                       void *user_data) -> int {
        static_cast<UserData *>(user_data)->in_mkdir = true;
#ifdef _WIN32
        return mkdir(filename) == 0;
#else
        return mkdir(filename, 0755) == 0;
#endif
    };
    api.unlink_cbk = [](PJ_CONTEXT *, const char *filename,
                        void *user_data) -> int {
        static_cast<UserData *>(user_data)->in_unlink = true;
        return unlink(filename) == 0;
    };
    api.rename_cbk = [](PJ_CONTEXT *, const char *oldPath, const char *newPath,
                        void *user_data) -> int {
        static_cast<UserData *>(user_data)->in_rename = true;
        return rename(oldPath, newPath) == 0;
    };

    UserData userData;
    ASSERT_TRUE(proj_context_set_fileapi(ctx, &api, &userData));

    ASSERT_TRUE(proj_is_download_needed(ctx, "dk_sdfe_dvr90.tif", false));

    ASSERT_TRUE(
        proj_download_file(ctx, "dk_sdfe_dvr90.tif", false, nullptr, nullptr));

    ASSERT_TRUE(userData.in_open);
    ASSERT_FALSE(userData.in_read);
    ASSERT_TRUE(userData.in_write);
    ASSERT_TRUE(userData.in_close);
    ASSERT_TRUE(userData.in_exists);
    ASSERT_TRUE(userData.in_mkdir);
    ASSERT_TRUE(userData.in_unlink);
    ASSERT_TRUE(userData.in_rename);

    proj_context_set_enable_network(ctx, false);

    const char *pipeline =
        "+proj=pipeline "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=vgridshift +grids=dk_sdfe_dvr90.tif +multiplier=1 "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg";

    auto P = proj_create(ctx, pipeline);
    ASSERT_NE(P, nullptr);

    double longitude = 12;
    double lat = 56;
    double z = 0;
    proj_trans_generic(P, PJ_FWD, &longitude, sizeof(double), 1, &lat,
                       sizeof(double), 1, &z, sizeof(double), 1, nullptr, 0, 0);
    EXPECT_NEAR(z, 36.5909996032715, 1e-10);

    proj_destroy(P);

    ASSERT_TRUE(userData.in_read);
    ASSERT_TRUE(userData.in_seek);

    proj_context_destroy(ctx);
    putenv(const_cast<char *>("PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES"));
    putenv(const_cast<char *>("PROJ_USER_WRITABLE_DIRECTORY="));
    putenv(const_cast<char *>("PROJ_FULL_FILE_CHUNK_SIZE="));
    unlink("proj_test_tmp/cache.db");
    unlink("proj_test_tmp/dk_sdfe_dvr90.tif");
    rmdir("proj_test_tmp");
}

#endif

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

TEST(networking, proj_coordoperation_get_grid_used) {
    if (!networkAccessOK) {
        return;
    }

    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);

    // Test bugfix for
    // https://github.com/OSGeo/PROJ/issues/3444#issuecomment-1309499342
    for (int i = 0; i < 2; ++i) {
        // This file is not in grid_alternatives, but in the CDN
        const char *proj_string =
            "proj=vgridshift grids=nz_linz_nzgd2000-c120100904-grid01.tif";
        PJ *P = proj_create(ctx, proj_string);

        const char *shortName = nullptr;
        const char *fullName = nullptr;
        const char *packageName = nullptr;
        const char *url = nullptr;
        int directDownload = 0;
        int openLicense = 0;
        int available = 0;

        proj_coordoperation_get_grid_used(ctx, P, 0, &shortName, &fullName,
                                          &packageName, &url, &directDownload,
                                          &openLicense, &available);

        EXPECT_EQ(std::string(shortName),
                  "nz_linz_nzgd2000-c120100904-grid01.tif");
        EXPECT_EQ(std::string(fullName), "");
        EXPECT_EQ(
            std::string(url),
            "https://cdn.proj.org/nz_linz_nzgd2000-c120100904-grid01.tif");

        proj_destroy(P);
    }

    proj_context_destroy(ctx);
}

#endif

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

TEST(networking, pyproj_issue_1192) {
    if (!networkAccessOK) {
        return;
    }

    const auto doTest = [](PJ_CONTEXT *ctxt) {
        auto factory_context =
            proj_create_operation_factory_context(ctxt, nullptr);
        proj_operation_factory_context_set_grid_availability_use(
            ctxt, factory_context, PROJ_GRID_AVAILABILITY_IGNORED);
        proj_operation_factory_context_set_spatial_criterion(
            ctxt, factory_context, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);
        auto from = proj_create(ctxt, "EPSG:4326");
        auto to = proj_create(ctxt, "EPSG:2964");
        auto pj_operations =
            proj_create_operations(ctxt, from, to, factory_context);
        proj_destroy(from);
        proj_destroy(to);
        auto num_operations = proj_list_get_count(pj_operations);
        for (int i = 0; i < num_operations; ++i) {
            PJ *P = proj_list_get(ctxt, pj_operations, i);
            int is_instantiable = proj_coordoperation_is_instantiable(ctxt, P);
            if (is_instantiable) {
                EXPECT_TRUE(proj_pj_info(P).id != nullptr);
            }
            proj_destroy(P);
        }
        proj_operation_factory_context_destroy(factory_context);
        proj_list_destroy(pj_operations);
    };

    auto ctx = proj_context_create();
    proj_grid_cache_set_enable(ctx, false);
    proj_context_set_enable_network(ctx, true);
    doTest(ctx);
    proj_context_set_enable_network(ctx, false);
    doTest(ctx);

    proj_context_destroy(ctx);
}

#endif

} // namespace
