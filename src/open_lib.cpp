/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implementation of pj_open_lib(), and pj_set_finder().  These
 *           provide a standard interface for opening projections support
 *           data files.
 * Author:   Gerald Evenden, Frank Warmerdam <warmerdam@pobox.com>
 *
 ******************************************************************************
 * Copyright (c) 1995, Gerald Evenden
 * Copyright (c) 2002, Frank Warmerdam <warmerdam@pobox.com>
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

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj/internal/internal.hpp"

#include "proj_internal.h"
#include "filemanager.hpp"

static const char * proj_lib_name =
#ifdef PROJ_LIB
PROJ_LIB;
#else
nullptr;
#endif

using namespace NS_PROJ::internal;

/************************************************************************/
/*                           pj_set_finder()                            */
/************************************************************************/

void pj_set_finder( const char *(*new_finder)(const char *) )

{
    auto ctx = pj_get_default_ctx();
    if( ctx ) {
        ctx->file_finder_legacy = new_finder;
    }
}

/************************************************************************/
/*                   proj_context_set_file_finder()                     */
/************************************************************************/

/** \brief Assign a file finder callback to a context.
 * 
 * This callback will be used whenever PROJ must open one of its resource files
 * (proj.db database, grids, etc...)
 * 
 * The callback will be called with the context currently in use at the moment
 * where it is used (not necessarily the one provided during this call), and
 * with the provided user_data (which may be NULL).
 * The user_data must remain valid during the whole lifetime of the context.
 * 
 * A finder set on the default context will be inherited by contexts created
 * later.
 * 
 * @param ctx PROJ context, or NULL for the default context.
 * @param finder Finder callback. May be NULL
 * @param user_data User data provided to the finder callback. May be NULL.
 * 
 * @since PROJ 6.0
 */
void proj_context_set_file_finder(PJ_CONTEXT *ctx, proj_file_finder finder,
                                  void* user_data)
{
    if( !ctx )
        ctx = pj_get_default_ctx();
    if( !ctx )
        return;
    ctx->file_finder = finder;
    ctx->file_finder_user_data = user_data;
}

/************************************************************************/
/*                  proj_context_set_search_paths()                     */
/************************************************************************/


/** \brief Sets search paths.
 * 
 * Those search paths will be used whenever PROJ must open one of its resource files
 * (proj.db database, grids, etc...)
 *
 * If set on the default context, they will be inherited by contexts created
 * later.
 * 
 * @param ctx PROJ context, or NULL for the default context.
 * @param count_paths Number of paths. 0 if paths == NULL.
 * @param paths Paths. May be NULL.
 * 
 * @since PROJ 6.0
 */
void proj_context_set_search_paths(PJ_CONTEXT *ctx,
                                   int count_paths,
                                   const char* const* paths)
{
    if( !ctx )
        ctx = pj_get_default_ctx();
    if( !ctx )
        return;
    try {
        std::vector<std::string> vector_of_paths;
        for (int i = 0; i < count_paths; i++)
        {
            vector_of_paths.emplace_back(paths[i]);
        }
        ctx->set_search_paths(vector_of_paths);
    } catch( const std::exception& )
    {
    }
}

/************************************************************************/
/*                         pj_set_searchpath()                          */
/*                                                                      */
/*      Path control for callers that can't practically provide         */
/*      pj_set_finder() style callbacks.  Call with (0,NULL) as args    */
/*      to clear the searchpath set.                                    */
/************************************************************************/

void pj_set_searchpath ( int count, const char **path )
{
    proj_context_set_search_paths( nullptr, count, const_cast<const char* const*>(path) );
}

#ifdef _WIN32
#include <windows.h>
#include <sys/stat.h>
static const char *get_path_from_win32_projlib(const char *name, std::string& out) {
    /* Check if proj.db lieves in a share/proj dir parallel to bin/proj.dll */
    /* Based in https://stackoverflow.com/questions/9112893/how-to-get-path-to-executable-in-c-running-on-windows */

    DWORD path_size = 1024;

    for (;;) {
        out.resize(path_size);
        memset(&out[0], 0, path_size);
        DWORD result     = GetModuleFileNameA(nullptr, &out[0], path_size - 1);
        DWORD last_error = GetLastError();

        if (result == 0) {
            return nullptr;
        }
        else if (result == path_size - 1) {
            if (ERROR_INSUFFICIENT_BUFFER != last_error) {
                return nullptr;
            }
            path_size = path_size * 2;
        }
        else {
            break;
        }
    }
    // Now remove the program's name. It was (example) "C:\programs\gmt6\bin\gdal_translate.exe"
    size_t k = strlen(out.c_str());
    while (k > 0 && out[--k] != '\\') {}
    out.resize(k);

    out += "/../share/proj/";
    out += name;

    struct stat fileInfo; 
    if (stat(out.c_str(), &fileInfo) == 0)	// Check if file exists (probably there are simpler ways)
        return out.c_str();
    else {
        return nullptr;
    }
}
#endif

/************************************************************************/
/*                      pj_open_lib_internal()                          */
/************************************************************************/

#ifdef WIN32
static const char dir_chars[] = "/\\";
static const char dirSeparator = ';';
#else
static const char dir_chars[] = "/";
static const char dirSeparator = ':';
#endif

static bool is_tilde_slash(const char* name)
{
    return *name == '~' && strchr(dir_chars,name[1]);
}

static bool is_rel_or_absolute_filename(const char *name)
{
    return strchr(dir_chars,*name)
            || (*name == '.' && strchr(dir_chars,name[1]))
            || (!strncmp(name, "..", 2) && strchr(dir_chars,name[2]))
            || (name[0] != '\0' && name[1] == ':' && strchr(dir_chars,name[2]));
}

static void*
pj_open_lib_internal(projCtx ctx, const char *name, const char *mode,
                     void* (*open_file)(projCtx, const char*, const char*),
                     char* out_full_filename, size_t out_full_filename_size) {
    try {
        std::string fname;
        const char *sysname = nullptr;
        void* fid = nullptr;

        if( ctx == nullptr ) {
            ctx = pj_get_default_ctx();
        }

        if( out_full_filename != nullptr && out_full_filename_size > 0 )
            out_full_filename[0] = '\0';

        /* check if ~/name */
        if (is_tilde_slash(name))
            if ((sysname = getenv("HOME")) != nullptr) {
                fname = sysname;
                fname += DIR_CHAR;
                fname += name;
                sysname = fname.c_str();
            } else
                return nullptr;

        /* or fixed path: /name, ./name or ../name or http[s]:// */
        else if (is_rel_or_absolute_filename(name)
                || starts_with(name, "http://")
                || starts_with(name, "https://"))
            sysname = name;

        /* or try to use application provided file finder */
        else if( ctx->file_finder != nullptr && (sysname = ctx->file_finder( ctx, name, ctx->file_finder_user_data )) != nullptr )
            ;

        else if( ctx->file_finder_legacy != nullptr && (sysname = ctx->file_finder_legacy( name )) != nullptr )
            ;

        /* The user has search paths set */
        else if( !ctx->search_paths.empty() ) {
            for( const auto& path: ctx->search_paths ) {
                try {
                    fname = path;
                    fname += DIR_CHAR;
                    fname += name;
                    sysname = fname.c_str();
                    fid = open_file(ctx, sysname, mode);
                } catch( const std::exception& )
                {
                }
                if( fid )
                    break;
            }
        }
        /* if is environment PROJ_LIB defined */
        else if ((sysname = getenv("PROJ_LIB")) != nullptr) {
            auto paths = NS_PROJ::internal::split(std::string(sysname), dirSeparator);
            for( const auto& path: paths ) {
                fname = path;
                fname += DIR_CHAR;
                fname += name;
                sysname = fname.c_str();
                fid = open_file(ctx, sysname, mode);
                if( fid )
                    break;
            }
#ifdef _WIN32
        /* check if it lives in a ../share/proj dir of the proj dll */
        } else if ((sysname = get_path_from_win32_projlib(name, fname)) != nullptr) {
#endif
        /* or hardcoded path */
        } else if ((sysname = proj_lib_name) != nullptr) {
            fname = sysname;
            fname += DIR_CHAR;
            fname += name;
            sysname = fname.c_str();
        /* just try it bare bones */
        } else {
            sysname = name;
        }

        assert(sysname); // to make Coverity Scan happy
        if ( fid != nullptr || (fid = open_file(ctx, sysname, mode)) != nullptr)
        {
            if( out_full_filename != nullptr && out_full_filename_size > 0 )
            {
                // cppcheck-suppress nullPointer
                strncpy(out_full_filename, sysname, out_full_filename_size);
                out_full_filename[out_full_filename_size-1] = '\0';
            }
            errno = 0;
        }

        if( ctx->last_errno == 0 && errno != 0 )
            pj_ctx_set_errno( ctx, errno );

        pj_log( ctx, PJ_LOG_DEBUG_MAJOR, 
                "pj_open_lib(%s): call fopen(%s) - %s",
                name, sysname,
                fid == nullptr ? "failed" : "succeeded" );

        return(fid);
    }
    catch( const std::exception& ) {

        pj_log( ctx, PJ_LOG_DEBUG_MAJOR, 
                "pj_open_lib(%s): out of memory",
                name );

        return nullptr;
    }
}

/************************************************************************/
/*                  pj_open_file_with_manager()                         */
/************************************************************************/

static void* pj_open_file_with_manager(projCtx ctx, const char *name,
                                       const char * /* mode */)
{
    return NS_PROJ::FileManager::open(ctx, name).release();
}

/************************************************************************/
/*                 FileManager::open_resource_file()                    */
/************************************************************************/

std::unique_ptr<NS_PROJ::File> NS_PROJ::FileManager::open_resource_file(
                                                projCtx ctx, const char *name)
{
    auto file = std::unique_ptr<NS_PROJ::File>(
        reinterpret_cast<NS_PROJ::File*>(
               pj_open_lib_internal(ctx, name, "rb",
                                    pj_open_file_with_manager,
                                    nullptr, 0)));
    if( file == nullptr &&
        !is_tilde_slash(name) &&
        !is_rel_or_absolute_filename(name) &&
        !starts_with(name, "http://") &&
        !starts_with(name, "https://") &&
        pj_context_is_network_enabled(ctx) ) {
        std::string remote_file(pj_context_get_url_endpoint(ctx));
        if( !remote_file.empty() ) {
            if( remote_file.back() != '/' ) {
                remote_file += '/';
            }
            remote_file += name;
            auto pos = remote_file.rfind('.');
            if( pos + 4 == remote_file.size() ) {
                remote_file = remote_file.substr(0, pos) + ".tif";
                file = open(ctx, remote_file.c_str());
                if( file ) {
                    pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                            "Using %s", remote_file.c_str() );
                    pj_ctx_set_errno( ctx, 0 );
                }
            } else {
                // For example for resource files like 'alaska'
                auto remote_file_tif = remote_file + ".tif";
                file = open(ctx, remote_file_tif.c_str());
                if( file ) {
                    pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                            "Using %s", remote_file_tif.c_str() );
                    pj_ctx_set_errno( ctx, 0 );
                } else {
                    // Init files
                    file = open(ctx, remote_file.c_str());
                    if( file ) {
                        pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                                "Using %s", remote_file.c_str() );
                        pj_ctx_set_errno( ctx, 0 );
                    }
                }
            }
        }
    }
    return file;
}

/************************************************************************/
/*                            pj_open_lib()                             */
/************************************************************************/

#ifndef REMOVE_LEGACY_SUPPORT

// Used by following legacy function
static void* pj_ctx_fopen_adapter(projCtx ctx, const char *name, const char *mode)
{
    return pj_ctx_fopen(ctx, name, mode);
}

// Legacy function
PAFile
pj_open_lib(projCtx ctx, const char *name, const char *mode) {
    return (PAFile)pj_open_lib_internal(ctx, name, mode, pj_ctx_fopen_adapter, nullptr, 0);
}

#endif // REMOVE_LEGACY_SUPPORT

/************************************************************************/
/*                           pj_find_file()                             */
/************************************************************************/


/** Returns the full filename corresponding to a proj resource file specified
 *  as a short filename.
 * 
 * @param ctx context.
 * @param short_filename short filename (e.g. egm96_15.gtx). Must not be NULL.
 * @param out_full_filename output buffer, of size out_full_filename_size, that
 *                          will receive the full filename on success.
 *                          Will be zero-terminated.
 * @param out_full_filename_size size of out_full_filename.
 * @return 1 if the file was found, 0 otherwise.
 */
int pj_find_file(projCtx ctx, const char *short_filename,
                 char* out_full_filename, size_t out_full_filename_size)
{
    auto f = reinterpret_cast<NS_PROJ::File*>(
               pj_open_lib_internal(ctx, short_filename, "rb",
                                    pj_open_file_with_manager,
                                    out_full_filename,
                                    out_full_filename_size));
    if( f != nullptr )
    {
        delete f;
        return 1;
    }
    return 0;
}

/************************************************************************/
/*                    pj_context_get_url_endpoint()                     */
/************************************************************************/

std::string pj_context_get_url_endpoint(PJ_CONTEXT* ctx)
{
    if( !ctx->endpoint.empty() ) {
        return ctx->endpoint;
    }
    pj_load_ini(ctx);
    return ctx->endpoint;
}

/************************************************************************/
/*                              trim()                                  */
/************************************************************************/

static std::string trim(const std::string& s) {
    const auto first = s.find_first_not_of(' ');
    const auto last = s.find_last_not_of(' ');
    if( first == std::string::npos || last == std::string::npos ) {
        return std::string();
    }
    return s.substr(first, last - first + 1);
}

/************************************************************************/
/*                            pj_load_ini()                             */
/************************************************************************/

void pj_load_ini(projCtx ctx)
{
    if( ctx->iniFileLoaded )
        return;

    const char* endpoint_from_env = getenv("PROJ_NETWORK_ENDPOINT");
    if( endpoint_from_env && endpoint_from_env[0] != '\0' ) {
        ctx->endpoint = endpoint_from_env;
    }

    ctx->iniFileLoaded = true;
    auto file = std::unique_ptr<NS_PROJ::File>(
        reinterpret_cast<NS_PROJ::File*>(
               pj_open_lib_internal(ctx, "proj.ini", "rb",
                                    pj_open_file_with_manager,
                                    nullptr, 0)));
    if( !file )
        return;
    file->seek(0, SEEK_END);
    const auto filesize = file->tell();
    if( filesize == 0 || filesize > 100 * 1024U )
        return;
    file->seek(0, SEEK_SET);
    std::string content;
    content.resize(static_cast<size_t>(filesize));
    const auto nread = file->read(&content[0], content.size());
    if( nread != content.size() )
        return;
    content += '\n';
    size_t pos = 0;
    while( pos != std::string::npos ) {
        const auto eol = content.find_first_of("\r\n", pos);
        if( eol == std::string::npos ) {
            break;
        }

        const auto equal = content.find('=', pos);
        if( equal < eol )
        {
            const auto key = trim(content.substr(pos, equal-pos));
            const auto value = trim(content.substr(equal + 1,
                                                    eol - (equal+1)));
            if( ctx->endpoint.empty() && key == "cdn_endpoint" ) {
                ctx->endpoint = value;
            } else if ( key == "network" ) {
                const char *enabled = getenv("PROJ_NETWORK");
                if (enabled == nullptr || enabled[0] == '\0') {
                    ctx->networking.enabled = ci_equal(value, "ON") ||
                                                ci_equal(value, "YES") ||
                                                ci_equal(value, "TRUE");
                }
            } else if ( key == "cache_enabled" ) {
                ctx->gridChunkCache.enabled = ci_equal(value, "ON") ||
                                              ci_equal(value, "YES") ||
                                              ci_equal(value, "TRUE");
            } else if ( key == "cache_size_MB" ) {
                const int val = atoi(value.c_str());
                ctx->gridChunkCache.max_size = val > 0 ?
                    static_cast<long long>(val) * 1024 * 1024 : -1;
            } else if ( key == "cache_ttl_sec" ) {
                ctx->gridChunkCache.ttl = atoi(value.c_str());
            }
        }

        pos = content.find_first_not_of("\r\n", eol);
    }
}
