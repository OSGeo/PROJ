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

#ifdef CURL_ENABLED
#include <curl/curl.h>
#endif

namespace {

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
                     "https://cdn.proj.org/ntf_r93.tif");

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
        "+step +proj=hgridshift +grids=https://cdn.proj.org/ntf_r93.tif "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg";

    // network access disabled by default
    auto ctx = proj_context_create();
    proj_log_func(ctx, nullptr, silent_logger);
    auto P = proj_create(ctx, pipeline);
    ASSERT_EQ(P, nullptr);
    proj_context_destroy(ctx);

#ifdef CURL_ENABLED
    // enable through env variable
    ctx = proj_context_create();
    putenv(const_cast<char *>("PROJ_NETWORK=ON"));
    P = proj_create(ctx, pipeline);
    if (networkAccessOK) {
        ASSERT_NE(P, nullptr);
    }
    proj_destroy(P);
    proj_context_destroy(ctx);
    putenv(const_cast<char *>("PROJ_NETWORK="));
#endif

    // still disabled
    ctx = proj_context_create();
    proj_log_func(ctx, nullptr, silent_logger);
    P = proj_create(ctx, pipeline);
    ASSERT_EQ(P, nullptr);
    proj_context_destroy(ctx);

    // enable through API
    ctx = proj_context_create();
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
    double lon = 2;
    double lat = 49;
    proj_trans_generic(P, PJ_FWD, &lon, sizeof(double), 1, &lat, sizeof(double),
                       1, nullptr, 0, 0, nullptr, 0, 0);
    EXPECT_NEAR(lon, 1.9992776848, 1e-10);
    EXPECT_NEAR(lat, 48.9999322600, 1e-10);

    proj_destroy(P);
#else
    ASSERT_EQ(P, nullptr);
#endif
    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

TEST(networking, curl_invalid_resource) {
    auto ctx = proj_context_create();
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
        fprintf(stderr, "unexpected call to open(%s, %ld, %ld). "
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
        fprintf(stderr, "wrong call to open(%s, %ld, %ld). Was expecting "
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
        fprintf(stderr, "unexpected call to close(). "
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
        fprintf(stderr, "unexpected call to get_header_value(). "
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
        fprintf(stderr, "wrong call to get_header_value(%s). Was expecting "
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
        fprintf(stderr, "unexpected call to read_range(). "
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
        fprintf(stderr, "wrong call to read_range(%ld, %ld). Was expecting "
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
        event->value = "dummy"; // dummy value: not used
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
        for (size_t i = 0; i < 278528 / sizeof(float); i++) {
            memcpy(&event->response[i * sizeof(float)], &f, sizeof(float));
        }
        exchange.events.emplace_back(std::move(event));
    }

    {
        double lon = 2 / 180. * M_PI;
        double lat = 49 / 180. * M_PI;
        double z = 0;
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &lon, sizeof(double), 1, &lat,
                                     sizeof(double), 1, &z, sizeof(double), 1,
                                     nullptr, 0, 0),
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
        for (size_t i = 0; i < 278528 / sizeof(float); i++) {
            memcpy(&event->response[i * sizeof(float)], &f, sizeof(float));
        }
        exchange.events.emplace_back(std::move(event));
    }

    {
        double lon = 2 / 180. * M_PI;
        double lat = -49 / 180. * M_PI;
        double z = 0;
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &lon, sizeof(double), 1, &lat,
                                     sizeof(double), 1, &z, sizeof(double), 1,
                                     nullptr, 0, 0),
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
        double lon = 2 / 180. * M_PI;
        double lat = 49 / 180. * M_PI;
        double z = 0;
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &lon, sizeof(double), 1, &lat,
                                     sizeof(double), 1, &z, sizeof(double), 1,
                                     nullptr, 0, 0),
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
        event->value = "dummy"; // dummy value: not used
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
        for (size_t i = 0; i < 278528 / sizeof(float); i++) {
            memcpy(&event->response[i * sizeof(float)], &f, sizeof(float));
        }
        exchange.events.emplace_back(std::move(event));
    }

    {
        double lon = 2 / 180. * M_PI;
        double lat = 49 / 180. * M_PI;
        double z = 0;
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &lon, sizeof(double), 1, &lat,
                                     sizeof(double), 1, &z, sizeof(double), 1,
                                     nullptr, 0, 0),
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
        double lon = 2 / 180. * M_PI;
        double lat = -49 / 180. * M_PI;
        double z = 0;
        proj_log_func(ctx, nullptr, silent_logger);
        ASSERT_EQ(proj_trans_generic(P, PJ_FWD, &lon, sizeof(double), 1, &lat,
                                     sizeof(double), 1, &z, sizeof(double), 1,
                                     nullptr, 0, 0),
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

#ifdef CURL_ENABLED

TEST(networking, curl_hgridshift) {
    auto ctx = proj_context_create();
    proj_context_set_enable_network(ctx, true);

    // NAD83 to NAD83(HARN) in West-Virginia. Using wvhpgn.tif
    auto P = proj_create_crs_to_crs(ctx, "EPSG:4269", "EPSG:4152", nullptr);
    ASSERT_NE(P, nullptr);

    PJ_COORD c;
    c.xyz.x = 40;  // lat
    c.xyz.y = -80; // lon
    c.xyz.z = 0;
    c = proj_trans(P, PJ_FWD, c);

    proj_assign_context(P, ctx); // (dummy) test context reassignment

    proj_destroy(P);
    proj_context_destroy(ctx);

    EXPECT_NEAR(c.xyz.x, 39.99999839, 1e-8);
    EXPECT_NEAR(c.xyz.y, -79.99999807, 1e-8);
    EXPECT_NEAR(c.xyz.z, 0, 1e-2);
}

#endif

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

TEST(networking, curl_vgridshift) {
    auto ctx = proj_context_create();
    proj_context_set_enable_network(ctx, true);

    // WGS84 to EGM2008 height. Using egm08_25.tif
    auto P =
        proj_create_crs_to_crs(ctx, "EPSG:4326", "EPSG:4326+3855", nullptr);
    ASSERT_NE(P, nullptr);

    PJ_COORD c;
    c.xyz.x = -30; // lat
    c.xyz.y = 150; // lon
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
    auto ctx = proj_context_create();
    proj_context_set_enable_network(ctx, true);

    // NGVD29 to NAVD88 height. Using vertcone.tif
    auto P = proj_create_crs_to_crs(ctx, "EPSG:4269+7968", "EPSG:4269+5703",
                                    nullptr);
    ASSERT_NE(P, nullptr);

    PJ_COORD c;
    c.xyz.x = 40;  // lat
    c.xyz.y = -80; // lon
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
    proj_context_set_enable_network(ctx, true);

    // NAD83 to NAD83(HARN) in West-Virginia. Using wvhpgn.tif
    auto P = proj_create_crs_to_crs(ctx, "EPSG:4269", "EPSG:4152", nullptr);
    ASSERT_NE(P, nullptr);

    PJ_COORD c;
    c.xyz.x = 40;  // lat
    c.xyz.y = -80; // lon
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
    proj_context_set_enable_network(ctx, true);
    proj_context_set_url_endpoint(ctx, "http://0.0.0.0");

    // NAD83 to NAD83(HARN) in West-Virginia. Using wvhpgn.tif
    auto P = proj_create_crs_to_crs(ctx, "EPSG:4269", "EPSG:4152", nullptr);
    ASSERT_NE(P, nullptr);

    PJ_COORD c;
    c.xyz.x = 40;  // lat
    c.xyz.y = -80; // lon
    c.xyz.z = 0;
    c = proj_trans(P, PJ_FWD, c);

    proj_destroy(P);
    proj_context_destroy(ctx);

    EXPECT_EQ(c.xyz.x, HUGE_VAL);
}

#endif

} // namespace
