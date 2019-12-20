/******************************************************************************
 * Project:  PROJ
 * Purpose:  File manager
 * Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2019, Even Rouault, <even.rouault at spatialys.com>
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

#ifndef FILEMANAGER_HPP_INCLUDED
#define FILEMANAGER_HPP_INCLUDED

#include <memory>

#include "proj.h"
#include "proj/util.hpp"

NS_PROJ_START

//! @cond Doxygen_Suppress

class File;

class FileManager {
  private:
    FileManager() = delete;

  public:
    // "Low-level" interface.
    static std::unique_ptr<File> open(PJ_CONTEXT *ctx, const char *filename);

    // "High-level" interface, honoring PROJ_LIB and the like.
    static std::unique_ptr<File> open_resource_file(PJ_CONTEXT *ctx,
                                                    const char *name);

    static void fillDefaultNetworkInterface(PJ_CONTEXT *ctx);

    static void clearCache();
};

// ---------------------------------------------------------------------------

class File {
  protected:
    File();

  public:
    virtual ~File();
    virtual size_t read(void *buffer, size_t sizeBytes) = 0;
    virtual bool seek(unsigned long long offset, int whence = SEEK_SET) = 0;
    virtual unsigned long long tell() = 0;
};

//! @endcond Doxygen_Suppress

NS_PROJ_END

#endif // FILEMANAGER_HPP_INCLUDED