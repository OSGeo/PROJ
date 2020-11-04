/******************************************************************************
 * Project:  PROJ
 * Purpose:  Functionality related to triangulation transformation
 * Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2020, Even Rouault, <even.rouault at spatialys.com>
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
 *****************************************************************************/

#define PJ_LIB__
#define PROJ_COMPILATION

#include "tinshift.hpp"
#include "filemanager.hpp"
#include "proj_internal.h"

PROJ_HEAD(tinshift, "Triangulation based transformation");

using namespace TINSHIFT_NAMESPACE;

namespace {

struct tinshiftData {
    std::unique_ptr<Evaluator> evaluator{};

    tinshiftData() = default;

    tinshiftData(const tinshiftData &) = delete;
    tinshiftData &operator=(const tinshiftData &) = delete;
};

} // namespace

static PJ *destructor(PJ *P, int errlev) {
    if (nullptr == P)
        return nullptr;

    auto Q = static_cast<struct tinshiftData *>(P->opaque);
    delete Q;
    P->opaque = nullptr;

    return pj_default_destructor(P, errlev);
}

static PJ_COORD tinshift_forward_4d(PJ_COORD in, PJ *P) {
    auto *Q = (struct tinshiftData *)P->opaque;

    PJ_COORD out = in;
    if (!Q->evaluator->forward(in.xyz.x, in.xyz.y, in.xyz.z, out.xyz.x,
                               out.xyz.y, out.xyz.z)) {
        return proj_coord_error();
    }
    return out;
}

static PJ_COORD tinshift_reverse_4d(PJ_COORD in, PJ *P) {
    auto *Q = (struct tinshiftData *)P->opaque;

    PJ_COORD out = in;
    if (!Q->evaluator->inverse(in.xyz.x, in.xyz.y, in.xyz.z, out.xyz.x,
                               out.xyz.y, out.xyz.z)) {
        return proj_coord_error();
    }
    return out;
}

PJ *TRANSFORMATION(tinshift, 1) {

    const char *filename = pj_param(P->ctx, P->params, "sfile").s;
    if (!filename) {
        proj_log_error(P, "tinshift: +file= should be specified.");
        return destructor(P, PJD_ERR_NO_ARGS);
    }

    auto file = NS_PROJ::FileManager::open_resource_file(P->ctx, filename);
    if (nullptr == file) {
        proj_log_error(P, "tinshift: Cannot open %s", filename);
        return destructor(P, PJD_ERR_INVALID_ARG);
    }
    file->seek(0, SEEK_END);
    unsigned long long size = file->tell();
    // Arbitrary threshold to avoid ingesting an arbitrarily large JSON file,
    // that could be a denial of service risk. 10 MB should be sufficiently
    // large for any valid use !
    if (size > 10 * 1024 * 1024) {
        proj_log_error(P, "tinshift: File %s too large", filename);
        return destructor(P, PJD_ERR_INVALID_ARG);
    }
    file->seek(0);
    std::string jsonStr;
    jsonStr.resize(static_cast<size_t>(size));
    if (file->read(&jsonStr[0], jsonStr.size()) != jsonStr.size()) {
        proj_log_error(P, "tinshift: Cannot read %s", filename);
        return destructor(P, PJD_ERR_INVALID_ARG);
    }

    auto Q = new tinshiftData();
    P->opaque = (void *)Q;
    P->destructor = destructor;

    try {
        Q->evaluator.reset(new Evaluator(TINShiftFile::parse(jsonStr)));
    } catch (const std::exception &e) {
        proj_log_error(P, "tinshift: invalid model: %s", e.what());
        return destructor(P, PJD_ERR_INVALID_ARG);
    }

    P->destructor = destructor;
    P->fwd4d = tinshift_forward_4d;
    P->inv4d = tinshift_reverse_4d;
    P->left = PJ_IO_UNITS_WHATEVER;
    P->right = PJ_IO_UNITS_WHATEVER;

    return P;
}
