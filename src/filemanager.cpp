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

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif
#define LRU11_DO_NOT_DEFINE_OUT_OF_CLASS_METHODS

#include <stdlib.h>

#include <algorithm>
#include <limits>
#include <string>

#include "filemanager.hpp"
#include "proj.h"
#include "proj/internal/internal.hpp"
#include "proj/internal/lru_cache.hpp"
#include "proj_internal.h"
#include "sqlite3.hpp"

#ifdef __MINGW32__
// mingw32-win32 doesn't implement std::mutex
namespace {
class MyMutex {
  public:
    // cppcheck-suppress functionStatic
    void lock() { pj_acquire_lock(); }
    // cppcheck-suppress functionStatic
    void unlock() { pj_release_lock(); }
};
}
#else
#include <mutex>
#define MyMutex std::mutex
#endif

#ifdef CURL_ENABLED
#include <curl/curl.h>
#include <sqlite3.h> // for sqlite3_snprintf
#endif

#include <sys/stat.h>

#ifdef _WIN32
#include <shlobj.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#elif defined(__MACH__) && defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__FreeBSD__)
#include <sys/sysctl.h>
#include <sys/types.h>
#endif

#include <time.h>

//! @cond Doxygen_Suppress

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

using namespace NS_PROJ::internal;

NS_PROJ_START

// ---------------------------------------------------------------------------

static void proj_sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

// ---------------------------------------------------------------------------

File::File(const std::string &name) : name_(name) {}

// ---------------------------------------------------------------------------

File::~File() = default;

#ifdef _WIN32

/* The bulk of utf8towc()/utf8fromwc() is derived from the utf.c module from
 * FLTK. It was originally downloaded from:
 *    http://svn.easysw.com/public/fltk/fltk/trunk/src/utf.c
 * And already used by GDAL
 */
/************************************************************************/
/* ==================================================================== */
/*      UTF.C code from FLTK with some modifications.                   */
/* ==================================================================== */
/************************************************************************/

/* Set to 1 to turn bad UTF8 bytes into ISO-8859-1. If this is to zero
   they are instead turned into the Unicode REPLACEMENT CHARACTER, of
   value 0xfffd.
   If this is on utf8decode will correctly map most (perhaps all)
   human-readable text that is in ISO-8859-1. This may allow you
   to completely ignore character sets in your code because virtually
   everything is either ISO-8859-1 or UTF-8.
*/
#define ERRORS_TO_ISO8859_1 1

/* Set to 1 to turn bad UTF8 bytes in the 0x80-0x9f range into the
   Unicode index for Microsoft's CP1252 character set. You should
   also set ERRORS_TO_ISO8859_1. With this a huge amount of more
   available text (such as all web pages) are correctly converted
   to Unicode.
*/
#define ERRORS_TO_CP1252 1

/* A number of Unicode code points are in fact illegal and should not
   be produced by a UTF-8 converter. Turn this on will replace the
   bytes in those encodings with errors. If you do this then converting
   arbitrary 16-bit data to UTF-8 and then back is not an identity,
   which will probably break a lot of software.
*/
#define STRICT_RFC3629 0

#if ERRORS_TO_CP1252
// Codes 0x80..0x9f from the Microsoft CP1252 character set, translated
// to Unicode:
constexpr unsigned short cp1252[32] = {
    0x20ac, 0x0081, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
    0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008d, 0x017d, 0x008f,
    0x0090, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
    0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0x009d, 0x017e, 0x0178};
#endif

/************************************************************************/
/*                             utf8decode()                             */
/************************************************************************/

/*
    Decode a single UTF-8 encoded character starting at \e p. The
    resulting Unicode value (in the range 0-0x10ffff) is returned,
    and \e len is set the number of bytes in the UTF-8 encoding
    (adding \e len to \e p will point at the next character).

    If \a p points at an illegal UTF-8 encoding, including one that
    would go past \e end, or where a code is uses more bytes than
    necessary, then *reinterpret_cast<const unsigned char*>(p) is translated as
though it is
    in the Microsoft CP1252 character set and \e len is set to 1.
    Treating errors this way allows this to decode almost any
    ISO-8859-1 or CP1252 text that has been mistakenly placed where
    UTF-8 is expected, and has proven very useful.

    If you want errors to be converted to error characters (as the
    standards recommend), adding a test to see if the length is
    unexpectedly 1 will work:

\code
    if( *p & 0x80 )
    {  // What should be a multibyte encoding.
      code = utf8decode(p, end, &len);
      if( len<2 ) code = 0xFFFD;  // Turn errors into REPLACEMENT CHARACTER.
    }
    else
    {  // Handle the 1-byte utf8 encoding:
      code = *p;
      len = 1;
    }
\endcode

    Direct testing for the 1-byte case (as shown above) will also
    speed up the scanning of strings where the majority of characters
    are ASCII.
*/
static unsigned utf8decode(const char *p, const char *end, int *len) {
    unsigned char c = *reinterpret_cast<const unsigned char *>(p);
    if (c < 0x80) {
        *len = 1;
        return c;
#if ERRORS_TO_CP1252
    } else if (c < 0xa0) {
        *len = 1;
        return cp1252[c - 0x80];
#endif
    } else if (c < 0xc2) {
        goto FAIL;
    }
    if (p + 1 >= end || (p[1] & 0xc0) != 0x80)
        goto FAIL;
    if (c < 0xe0) {
        *len = 2;
        return ((p[0] & 0x1f) << 6) + ((p[1] & 0x3f));
    } else if (c == 0xe0) {
        if ((reinterpret_cast<const unsigned char *>(p))[1] < 0xa0)
            goto FAIL;
        goto UTF8_3;
#if STRICT_RFC3629
    } else if (c == 0xed) {
        // RFC 3629 says surrogate chars are illegal.
        if ((reinterpret_cast<const unsigned char *>(p))[1] >= 0xa0)
            goto FAIL;
        goto UTF8_3;
    } else if (c == 0xef) {
        // 0xfffe and 0xffff are also illegal characters.
        if ((reinterpret_cast<const unsigned char *>(p))[1] == 0xbf &&
            (reinterpret_cast<const unsigned char *>(p))[2] >= 0xbe)
            goto FAIL;
        goto UTF8_3;
#endif
    } else if (c < 0xf0) {
    UTF8_3:
        if (p + 2 >= end || (p[2] & 0xc0) != 0x80)
            goto FAIL;
        *len = 3;
        return ((p[0] & 0x0f) << 12) + ((p[1] & 0x3f) << 6) + ((p[2] & 0x3f));
    } else if (c == 0xf0) {
        if ((reinterpret_cast<const unsigned char *>(p))[1] < 0x90)
            goto FAIL;
        goto UTF8_4;
    } else if (c < 0xf4) {
    UTF8_4:
        if (p + 3 >= end || (p[2] & 0xc0) != 0x80 || (p[3] & 0xc0) != 0x80)
            goto FAIL;
        *len = 4;
#if STRICT_RFC3629
        // RFC 3629 says all codes ending in fffe or ffff are illegal:
        if ((p[1] & 0xf) == 0xf &&
            (reinterpret_cast<const unsigned char *>(p))[2] == 0xbf &&
            (reinterpret_cast<const unsigned char *>(p))[3] >= 0xbe)
            goto FAIL;
#endif
        return ((p[0] & 0x07) << 18) + ((p[1] & 0x3f) << 12) +
               ((p[2] & 0x3f) << 6) + ((p[3] & 0x3f));
    } else if (c == 0xf4) {
        if ((reinterpret_cast<const unsigned char *>(p))[1] > 0x8f)
            goto FAIL; // After 0x10ffff.
        goto UTF8_4;
    } else {
    FAIL:
        *len = 1;
#if ERRORS_TO_ISO8859_1
        return c;
#else
        return 0xfffd; // Unicode REPLACEMENT CHARACTER
#endif
    }
}

/************************************************************************/
/*                              utf8towc()                              */
/************************************************************************/

/*  Convert a UTF-8 sequence into an array of wchar_t. These
    are used by some system calls, especially on Windows.

    \a src points at the UTF-8, and \a srclen is the number of bytes to
    convert.

    \a dst points at an array to write, and \a dstlen is the number of
    locations in this array. At most \a dstlen-1 words will be
    written there, plus a 0 terminating word. Thus this function
    will never overwrite the buffer and will always return a
    zero-terminated string. If \a dstlen is zero then \a dst can be
    null and no data is written, but the length is returned.

    The return value is the number of words that \e would be written
    to \a dst if it were long enough, not counting the terminating
    zero. If the return value is greater or equal to \a dstlen it
    indicates truncation, you can then allocate a new array of size
    return+1 and call this again.

    Errors in the UTF-8 are converted as though each byte in the
    erroneous string is in the Microsoft CP1252 encoding. This allows
    ISO-8859-1 text mistakenly identified as UTF-8 to be printed
    correctly.

    Notice that sizeof(wchar_t) is 2 on Windows and is 4 on Linux
    and most other systems. Where wchar_t is 16 bits, Unicode
    characters in the range 0x10000 to 0x10ffff are converted to
    "surrogate pairs" which take two words each (this is called UTF-16
    encoding). If wchar_t is 32 bits this rather nasty problem is
    avoided.
*/
static unsigned utf8towc(const char *src, unsigned srclen, wchar_t *dst,
                         unsigned dstlen) {
    const char *p = src;
    const char *e = src + srclen;
    unsigned count = 0;
    if (dstlen)
        while (true) {
            if (p >= e) {
                dst[count] = 0;
                return count;
            }
            if (!(*p & 0x80)) {
                // ASCII
                dst[count] = *p++;
            } else {
                int len = 0;
                unsigned ucs = utf8decode(p, e, &len);
                p += len;
#ifdef _WIN32
                if (ucs < 0x10000) {
                    dst[count] = static_cast<wchar_t>(ucs);
                } else {
                    // Make a surrogate pair:
                    if (count + 2 >= dstlen) {
                        dst[count] = 0;
                        count += 2;
                        break;
                    }
                    dst[count] = static_cast<wchar_t>(
                        (((ucs - 0x10000u) >> 10) & 0x3ff) | 0xd800);
                    dst[++count] = static_cast<wchar_t>((ucs & 0x3ff) | 0xdc00);
                }
#else
                dst[count] = static_cast<wchar_t>(ucs);
#endif
            }
            if (++count == dstlen) {
                dst[count - 1] = 0;
                break;
            }
        }
    // We filled dst, measure the rest:
    while (p < e) {
        if (!(*p & 0x80)) {
            p++;
        } else {
            int len = 0;
#ifdef _WIN32
            const unsigned ucs = utf8decode(p, e, &len);
            p += len;
            if (ucs >= 0x10000)
                ++count;
#else
            utf8decode(p, e, &len);
            p += len;
#endif
        }
        ++count;
    }

    return count;
}

// ---------------------------------------------------------------------------

struct NonValidUTF8Exception : public std::exception {};

// May throw exceptions
static std::wstring UTF8ToWString(const std::string &str) {
    std::wstring wstr;
    wstr.resize(str.size());
    wstr.resize(utf8towc(str.data(), static_cast<unsigned>(str.size()),
                         &wstr[0], static_cast<unsigned>(wstr.size()) + 1));
    for (const auto ch : wstr) {
        if (ch == 0xfffd) {
            throw NonValidUTF8Exception();
        }
    }
    return wstr;
}

// ---------------------------------------------------------------------------

/************************************************************************/
/*                             utf8fromwc()                             */
/************************************************************************/
/* Turn "wide characters" as returned by some system calls
    (especially on Windows) into UTF-8.

    Up to \a dstlen bytes are written to \a dst, including a null
    terminator. The return value is the number of bytes that would be
    written, not counting the null terminator. If greater or equal to
    \a dstlen then if you malloc a new array of size n+1 you will have
    the space needed for the entire string. If \a dstlen is zero then
    nothing is written and this call just measures the storage space
    needed.

    \a srclen is the number of words in \a src to convert. On Windows
    this is not necessarily the number of characters, due to there
    possibly being "surrogate pairs" in the UTF-16 encoding used.
    On Unix wchar_t is 32 bits and each location is a character.

    On Unix if a src word is greater than 0x10ffff then this is an
    illegal character according to RFC 3629. These are converted as
    though they are 0xFFFD (REPLACEMENT CHARACTER). Characters in the
    range 0xd800 to 0xdfff, or ending with 0xfffe or 0xffff are also
    illegal according to RFC 3629. However I encode these as though
    they are legal, so that utf8towc will return the original data.

    On Windows "surrogate pairs" are converted to a single character
    and UTF-8 encoded (as 4 bytes). Mismatched halves of surrogate
    pairs are converted as though they are individual characters.
*/
static unsigned int utf8fromwc(char *dst, unsigned dstlen, const wchar_t *src,
                               unsigned srclen) {
    unsigned int i = 0;
    unsigned int count = 0;
    if (dstlen)
        while (true) {
            if (i >= srclen) {
                dst[count] = 0;
                return count;
            }
            unsigned int ucs = src[i++];
            if (ucs < 0x80U) {
                dst[count++] = static_cast<char>(ucs);
                if (count >= dstlen) {
                    dst[count - 1] = 0;
                    break;
                }
            } else if (ucs < 0x800U) {
                // 2 bytes.
                if (count + 2 >= dstlen) {
                    dst[count] = 0;
                    count += 2;
                    break;
                }
                dst[count++] = 0xc0 | static_cast<char>(ucs >> 6);
                dst[count++] = 0x80 | static_cast<char>(ucs & 0x3F);
#ifdef _WIN32
            } else if (ucs >= 0xd800 && ucs <= 0xdbff && i < srclen &&
                       src[i] >= 0xdc00 && src[i] <= 0xdfff) {
                // Surrogate pair.
                unsigned int ucs2 = src[i++];
                ucs = 0x10000U + ((ucs & 0x3ff) << 10) + (ucs2 & 0x3ff);
// All surrogate pairs turn into 4-byte utf8.
#else
            } else if (ucs >= 0x10000) {
                if (ucs > 0x10ffff) {
                    ucs = 0xfffd;
                    goto J1;
                }
#endif
                if (count + 4 >= dstlen) {
                    dst[count] = 0;
                    count += 4;
                    break;
                }
                dst[count++] = 0xf0 | static_cast<char>(ucs >> 18);
                dst[count++] = 0x80 | static_cast<char>((ucs >> 12) & 0x3F);
                dst[count++] = 0x80 | static_cast<char>((ucs >> 6) & 0x3F);
                dst[count++] = 0x80 | static_cast<char>(ucs & 0x3F);
            } else {
#ifndef _WIN32
            J1:
#endif
                // All others are 3 bytes:
                if (count + 3 >= dstlen) {
                    dst[count] = 0;
                    count += 3;
                    break;
                }
                dst[count++] = 0xe0 | static_cast<char>(ucs >> 12);
                dst[count++] = 0x80 | static_cast<char>((ucs >> 6) & 0x3F);
                dst[count++] = 0x80 | static_cast<char>(ucs & 0x3F);
            }
        }

    // We filled dst, measure the rest:
    while (i < srclen) {
        unsigned int ucs = src[i++];
        if (ucs < 0x80U) {
            count++;
        } else if (ucs < 0x800U) {
            // 2 bytes.
            count += 2;
#ifdef _WIN32
        } else if (ucs >= 0xd800 && ucs <= 0xdbff && i < srclen - 1 &&
                   src[i + 1] >= 0xdc00 && src[i + 1] <= 0xdfff) {
            // Surrogate pair.
            ++i;
#else
        } else if (ucs >= 0x10000 && ucs <= 0x10ffff) {
#endif
            count += 4;
        } else {
            count += 3;
        }
    }
    return count;
}

// ---------------------------------------------------------------------------

static std::string WStringToUTF8(const std::wstring &wstr) {
    std::string str;
    str.resize(wstr.size());
    str.resize(utf8fromwc(&str[0], static_cast<unsigned>(str.size() + 1),
                          wstr.data(), static_cast<unsigned>(wstr.size())));
    return str;
}

// ---------------------------------------------------------------------------

static std::string Win32Recode(const char *src, unsigned src_code_page,
                               unsigned dst_code_page) {
    // Convert from source code page to Unicode.

    // Compute the length in wide characters.
    int wlen = MultiByteToWideChar(src_code_page, MB_ERR_INVALID_CHARS, src, -1,
                                   nullptr, 0);
    if (wlen == 0 && GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
        return std::string();
    }

    // Do the actual conversion.
    std::wstring wbuf;
    wbuf.resize(wlen);
    MultiByteToWideChar(src_code_page, 0, src, -1, &wbuf[0], wlen);

    // Convert from Unicode to destination code page.

    // Compute the length in chars.
    int len = WideCharToMultiByte(dst_code_page, 0, &wbuf[0], -1, nullptr, 0,
                                  nullptr, nullptr);

    // Do the actual conversion.
    std::string out;
    out.resize(len);
    WideCharToMultiByte(dst_code_page, 0, &wbuf[0], -1, &out[0], len, nullptr,
                        nullptr);
    out.resize(strlen(out.c_str()));

    return out;
}

// ---------------------------------------------------------------------------

class FileWin32 : public File {
    PJ_CONTEXT *m_ctx;
    HANDLE m_handle;

    FileWin32(const FileWin32 &) = delete;
    FileWin32 &operator=(const FileWin32 &) = delete;

  protected:
    FileWin32(const std::string &name, PJ_CONTEXT *ctx, HANDLE handle)
        : File(name), m_ctx(ctx), m_handle(handle) {}

  public:
    ~FileWin32() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    size_t write(const void *buffer, size_t sizeBytes) override;
    bool seek(unsigned long long offset, int whence = SEEK_SET) override;
    unsigned long long tell() override;
    void reassign_context(PJ_CONTEXT *ctx) override { m_ctx = ctx; }

    // We may lie, but the real use case is only for network files
    bool hasChanged() const override { return false; }

    static std::unique_ptr<File> open(PJ_CONTEXT *ctx, const char *filename,
                                      FileAccess access);
};

// ---------------------------------------------------------------------------

FileWin32::~FileWin32() { CloseHandle(m_handle); }

// ---------------------------------------------------------------------------

size_t FileWin32::read(void *buffer, size_t sizeBytes) {
    DWORD dwSizeRead = 0;
    size_t nResult = 0;

    if (!ReadFile(m_handle, buffer, static_cast<DWORD>(sizeBytes), &dwSizeRead,
                  nullptr))
        nResult = 0;
    else
        nResult = dwSizeRead;

    return nResult;
}

// ---------------------------------------------------------------------------

size_t FileWin32::write(const void *buffer, size_t sizeBytes) {
    DWORD dwSizeWritten = 0;
    size_t nResult = 0;

    if (!WriteFile(m_handle, buffer, static_cast<DWORD>(sizeBytes),
                   &dwSizeWritten, nullptr))
        nResult = 0;
    else
        nResult = dwSizeWritten;

    return nResult;
}

// ---------------------------------------------------------------------------

bool FileWin32::seek(unsigned long long offset, int whence) {
    LONG dwMoveMethod, dwMoveHigh;
    uint32_t nMoveLow;
    LARGE_INTEGER li;

    switch (whence) {
    case SEEK_CUR:
        dwMoveMethod = FILE_CURRENT;
        break;
    case SEEK_END:
        dwMoveMethod = FILE_END;
        break;
    case SEEK_SET:
    default:
        dwMoveMethod = FILE_BEGIN;
        break;
    }

    li.QuadPart = offset;
    nMoveLow = li.LowPart;
    dwMoveHigh = li.HighPart;

    SetLastError(0);
    SetFilePointer(m_handle, nMoveLow, &dwMoveHigh, dwMoveMethod);

    return GetLastError() == NO_ERROR;
}

// ---------------------------------------------------------------------------

unsigned long long FileWin32::tell() {
    LARGE_INTEGER li;

    li.HighPart = 0;
    li.LowPart = SetFilePointer(m_handle, 0, &(li.HighPart), FILE_CURRENT);

    return static_cast<unsigned long long>(li.QuadPart);
}
// ---------------------------------------------------------------------------

std::unique_ptr<File> FileWin32::open(PJ_CONTEXT *ctx, const char *filename,
                                      FileAccess access) {
    DWORD dwDesiredAccess = access == FileAccess::READ_ONLY
                                ? GENERIC_READ
                                : GENERIC_READ | GENERIC_WRITE;
    DWORD dwCreationDisposition =
        access == FileAccess::CREATE ? CREATE_ALWAYS : OPEN_EXISTING;
    DWORD dwFlagsAndAttributes = (dwDesiredAccess == GENERIC_READ)
                                     ? FILE_ATTRIBUTE_READONLY
                                     : FILE_ATTRIBUTE_NORMAL;
    try {
        HANDLE hFile = CreateFileW(
            UTF8ToWString(std::string(filename)).c_str(), dwDesiredAccess,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            dwCreationDisposition, dwFlagsAndAttributes, nullptr);
        return std::unique_ptr<File>(hFile != INVALID_HANDLE_VALUE
                                         ? new FileWin32(filename, ctx, hFile)
                                         : nullptr);
    } catch (const std::exception &e) {
        pj_log(ctx, PJ_LOG_DEBUG, "%s", e.what());
        return nullptr;
    }
}
#else

// ---------------------------------------------------------------------------

class FileStdio : public File {
    PJ_CONTEXT *m_ctx;
    FILE *m_fp;

    FileStdio(const FileStdio &) = delete;
    FileStdio &operator=(const FileStdio &) = delete;

  protected:
    FileStdio(const std::string &name, PJ_CONTEXT *ctx, FILE *fp)
        : File(name), m_ctx(ctx), m_fp(fp) {}

  public:
    ~FileStdio() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    size_t write(const void *buffer, size_t sizeBytes) override;
    bool seek(unsigned long long offset, int whence = SEEK_SET) override;
    unsigned long long tell() override;
    void reassign_context(PJ_CONTEXT *ctx) override { m_ctx = ctx; }

    // We may lie, but the real use case is only for network files
    bool hasChanged() const override { return false; }

    static std::unique_ptr<File> open(PJ_CONTEXT *ctx, const char *filename,
                                      FileAccess access);
};

// ---------------------------------------------------------------------------

FileStdio::~FileStdio() { fclose(m_fp); }

// ---------------------------------------------------------------------------

size_t FileStdio::read(void *buffer, size_t sizeBytes) {
    return fread(buffer, 1, sizeBytes, m_fp);
}

// ---------------------------------------------------------------------------

size_t FileStdio::write(const void *buffer, size_t sizeBytes) {
    return fwrite(buffer, 1, sizeBytes, m_fp);
}

// ---------------------------------------------------------------------------

bool FileStdio::seek(unsigned long long offset, int whence) {
    // TODO one day: use 64-bit offset compatible API
    if (offset != static_cast<unsigned long long>(static_cast<long>(offset))) {
        pj_log(m_ctx, PJ_LOG_ERROR,
               "Attempt at seeking to a 64 bit offset. Not supported yet");
        return false;
    }
    return fseek(m_fp, static_cast<long>(offset), whence) == 0;
}

// ---------------------------------------------------------------------------

unsigned long long FileStdio::tell() {
    // TODO one day: use 64-bit offset compatible API
    return ftell(m_fp);
}

// ---------------------------------------------------------------------------

std::unique_ptr<File> FileStdio::open(PJ_CONTEXT *ctx, const char *filename,
                                      FileAccess access) {
    auto fp = fopen(filename,
                    access == FileAccess::READ_ONLY
                        ? "rb"
                        : access == FileAccess::READ_UPDATE ? "r+b" : "w+b");
    return std::unique_ptr<File>(fp ? new FileStdio(filename, ctx, fp)
                                    : nullptr);
}

#endif // _WIN32

// ---------------------------------------------------------------------------

#ifndef REMOVE_LEGACY_SUPPORT

class FileLegacyAdapter : public File {
    PJ_CONTEXT *m_ctx;
    PAFile m_fp;

    FileLegacyAdapter(const FileLegacyAdapter &) = delete;
    FileLegacyAdapter &operator=(const FileLegacyAdapter &) = delete;

  protected:
    FileLegacyAdapter(const std::string &name, PJ_CONTEXT *ctx, PAFile fp)
        : File(name), m_ctx(ctx), m_fp(fp) {}

  public:
    ~FileLegacyAdapter() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    size_t write(const void *, size_t) override { return 0; }
    bool seek(unsigned long long offset, int whence = SEEK_SET) override;
    unsigned long long tell() override;
    void reassign_context(PJ_CONTEXT *ctx) override { m_ctx = ctx; }

    // We may lie, but the real use case is only for network files
    bool hasChanged() const override { return false; }

    static std::unique_ptr<File> open(PJ_CONTEXT *ctx, const char *filename,
                                      FileAccess access);
};

// ---------------------------------------------------------------------------

FileLegacyAdapter::~FileLegacyAdapter() { pj_ctx_fclose(m_ctx, m_fp); }

// ---------------------------------------------------------------------------

size_t FileLegacyAdapter::read(void *buffer, size_t sizeBytes) {
    return pj_ctx_fread(m_ctx, buffer, 1, sizeBytes, m_fp);
}

// ---------------------------------------------------------------------------

bool FileLegacyAdapter::seek(unsigned long long offset, int whence) {
    if (offset != static_cast<unsigned long long>(static_cast<long>(offset))) {
        pj_log(m_ctx, PJ_LOG_ERROR,
               "Attempt at seeking to a 64 bit offset. Not supported yet");
        return false;
    }
    return pj_ctx_fseek(m_ctx, m_fp, static_cast<long>(offset), whence) == 0;
}

// ---------------------------------------------------------------------------

unsigned long long FileLegacyAdapter::tell() {
    return pj_ctx_ftell(m_ctx, m_fp);
}

// ---------------------------------------------------------------------------

std::unique_ptr<File>
FileLegacyAdapter::open(PJ_CONTEXT *ctx, const char *filename, FileAccess) {
    auto fid = pj_ctx_fopen(ctx, filename, "rb");
    return std::unique_ptr<File>(fid ? new FileLegacyAdapter(filename, ctx, fid)
                                     : nullptr);
}

#endif // REMOVE_LEGACY_SUPPORT

// ---------------------------------------------------------------------------

class FileApiAdapter : public File {
    PJ_CONTEXT *m_ctx;
    PROJ_FILE_HANDLE *m_fp;

    FileApiAdapter(const FileApiAdapter &) = delete;
    FileApiAdapter &operator=(const FileApiAdapter &) = delete;

  protected:
    FileApiAdapter(const std::string &name, PJ_CONTEXT *ctx,
                   PROJ_FILE_HANDLE *fp)
        : File(name), m_ctx(ctx), m_fp(fp) {}

  public:
    ~FileApiAdapter() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    size_t write(const void *, size_t) override;
    bool seek(unsigned long long offset, int whence = SEEK_SET) override;
    unsigned long long tell() override;
    void reassign_context(PJ_CONTEXT *ctx) override { m_ctx = ctx; }

    // We may lie, but the real use case is only for network files
    bool hasChanged() const override { return false; }

    static std::unique_ptr<File> open(PJ_CONTEXT *ctx, const char *filename,
                                      FileAccess access);
};

// ---------------------------------------------------------------------------

FileApiAdapter::~FileApiAdapter() {
    m_ctx->fileApi.close_cbk(m_ctx, m_fp, m_ctx->fileApi.user_data);
}

// ---------------------------------------------------------------------------

size_t FileApiAdapter::read(void *buffer, size_t sizeBytes) {
    return m_ctx->fileApi.read_cbk(m_ctx, m_fp, buffer, sizeBytes,
                                   m_ctx->fileApi.user_data);
}

// ---------------------------------------------------------------------------

size_t FileApiAdapter::write(const void *buffer, size_t sizeBytes) {
    return m_ctx->fileApi.write_cbk(m_ctx, m_fp, buffer, sizeBytes,
                                    m_ctx->fileApi.user_data);
}

// ---------------------------------------------------------------------------

bool FileApiAdapter::seek(unsigned long long offset, int whence) {
    return m_ctx->fileApi.seek_cbk(m_ctx, m_fp, static_cast<long long>(offset),
                                   whence, m_ctx->fileApi.user_data) != 0;
}

// ---------------------------------------------------------------------------

unsigned long long FileApiAdapter::tell() {
    return m_ctx->fileApi.tell_cbk(m_ctx, m_fp, m_ctx->fileApi.user_data);
}

// ---------------------------------------------------------------------------

std::unique_ptr<File> FileApiAdapter::open(PJ_CONTEXT *ctx,
                                           const char *filename,
                                           FileAccess eAccess) {
    PROJ_OPEN_ACCESS eCAccess = PROJ_OPEN_ACCESS_READ_ONLY;
    switch (eAccess) {
    case FileAccess::READ_ONLY:
        // Initialized above
        break;
    case FileAccess::READ_UPDATE:
        eCAccess = PROJ_OPEN_ACCESS_READ_UPDATE;
        break;
    case FileAccess::CREATE:
        eCAccess = PROJ_OPEN_ACCESS_CREATE;
        break;
    }
    auto fp =
        ctx->fileApi.open_cbk(ctx, filename, eCAccess, ctx->fileApi.user_data);
    return std::unique_ptr<File>(fp ? new FileApiAdapter(filename, ctx, fp)
                                    : nullptr);
}

// ---------------------------------------------------------------------------

constexpr size_t DOWNLOAD_CHUNK_SIZE = 16 * 1024;
constexpr int MAX_CHUNKS = 64;

struct FileProperties {
    unsigned long long size = 0;
    time_t lastChecked = 0;
    std::string lastModified{};
    std::string etag{};
};

class NetworkChunkCache {
  public:
    void insert(PJ_CONTEXT *ctx, const std::string &url,
                unsigned long long chunkIdx, std::vector<unsigned char> &&data);

    std::shared_ptr<std::vector<unsigned char>>
    get(PJ_CONTEXT *ctx, const std::string &url, unsigned long long chunkIdx);

    std::shared_ptr<std::vector<unsigned char>> get(PJ_CONTEXT *ctx,
                                                    const std::string &url,
                                                    unsigned long long chunkIdx,
                                                    FileProperties &props);

    void clearMemoryCache();

    static void clearDiskChunkCache(PJ_CONTEXT *ctx);

  private:
    struct Key {
        std::string url;
        unsigned long long chunkIdx;

        Key(const std::string &urlIn, unsigned long long chunkIdxIn)
            : url(urlIn), chunkIdx(chunkIdxIn) {}
        bool operator==(const Key &other) const {
            return url == other.url && chunkIdx == other.chunkIdx;
        }
    };

    struct KeyHasher {
        std::size_t operator()(const Key &k) const {
            return std::hash<std::string>{}(k.url) ^
                   (std::hash<unsigned long long>{}(k.chunkIdx) << 1);
        }
    };

    lru11::Cache<
        Key, std::shared_ptr<std::vector<unsigned char>>, MyMutex,
        std::unordered_map<
            Key,
            typename std::list<lru11::KeyValuePair<
                Key, std::shared_ptr<std::vector<unsigned char>>>>::iterator,
            KeyHasher>>
        cache_{MAX_CHUNKS};
};

// ---------------------------------------------------------------------------

static NetworkChunkCache gNetworkChunkCache{};

// ---------------------------------------------------------------------------

class NetworkFilePropertiesCache {
  public:
    void insert(PJ_CONTEXT *ctx, const std::string &url, FileProperties &props);

    bool tryGet(PJ_CONTEXT *ctx, const std::string &url, FileProperties &props);

    void clearMemoryCache();

  private:
    lru11::Cache<std::string, FileProperties, MyMutex> cache_{};
};

// ---------------------------------------------------------------------------

static NetworkFilePropertiesCache gNetworkFileProperties{};

// ---------------------------------------------------------------------------

class DiskChunkCache {
    PJ_CONTEXT *ctx_ = nullptr;
    std::string path_{};
    sqlite3 *hDB_ = nullptr;
    std::string thisNamePtr_{};
    std::unique_ptr<SQLite3VFS> vfs_{};

    explicit DiskChunkCache(PJ_CONTEXT *ctx, const std::string &path);

    bool createDBStructure();
    bool checkConsistency();
    bool get_links(sqlite3_int64 chunk_id, sqlite3_int64 &link_id,
                   sqlite3_int64 &prev, sqlite3_int64 &next,
                   sqlite3_int64 &head, sqlite3_int64 &tail);
    bool update_links_of_prev_and_next_links(sqlite3_int64 prev,
                                             sqlite3_int64 next);
    bool update_linked_chunks(sqlite3_int64 link_id, sqlite3_int64 prev,
                              sqlite3_int64 next);
    bool update_linked_chunks_head_tail(sqlite3_int64 head, sqlite3_int64 tail);

    DiskChunkCache(const DiskChunkCache &) = delete;
    DiskChunkCache &operator=(const DiskChunkCache &) = delete;

  public:
    static std::unique_ptr<DiskChunkCache> open(PJ_CONTEXT *ctx);
    ~DiskChunkCache();

    sqlite3 *handle() { return hDB_; }
    std::unique_ptr<SQLiteStatement> prepare(const char *sql);
    bool move_to_head(sqlite3_int64 chunk_id);
    bool move_to_tail(sqlite3_int64 chunk_id);
    void closeAndUnlink();
};

// ---------------------------------------------------------------------------

static bool pj_context_get_grid_cache_is_enabled(PJ_CONTEXT *ctx) {
    pj_load_ini(ctx);
    return ctx->gridChunkCache.enabled;
}

// ---------------------------------------------------------------------------

static long long pj_context_get_grid_cache_max_size(PJ_CONTEXT *ctx) {
    pj_load_ini(ctx);
    return ctx->gridChunkCache.max_size;
}

// ---------------------------------------------------------------------------

static int pj_context_get_grid_cache_ttl(PJ_CONTEXT *ctx) {
    pj_load_ini(ctx);
    return ctx->gridChunkCache.ttl;
}

// ---------------------------------------------------------------------------

std::unique_ptr<DiskChunkCache> DiskChunkCache::open(PJ_CONTEXT *ctx) {
    if (!pj_context_get_grid_cache_is_enabled(ctx)) {
        return nullptr;
    }
    const auto cachePath = pj_context_get_grid_cache_filename(ctx);
    if (cachePath.empty()) {
        return nullptr;
    }

    auto diskCache =
        std::unique_ptr<DiskChunkCache>(new DiskChunkCache(ctx, cachePath));
    if (!diskCache->hDB_)
        diskCache.reset();
    return diskCache;
}

// ---------------------------------------------------------------------------

DiskChunkCache::DiskChunkCache(PJ_CONTEXT *ctx, const std::string &path)
    : ctx_(ctx), path_(path) {
    std::string vfsName;
    if (ctx->custom_sqlite3_vfs_name.empty()) {
        vfs_ = SQLite3VFS::create(true, false, false);
        if (vfs_ == nullptr) {
            return;
        }
        vfsName = vfs_->name();
    } else {
        vfsName = ctx->custom_sqlite3_vfs_name;
    }
    sqlite3_open_v2(path.c_str(), &hDB_,
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                    vfsName.c_str());
    if (!hDB_) {
        return;
    }
    for (int i = 0;; i++) {
        int ret =
            sqlite3_exec(hDB_, "BEGIN EXCLUSIVE", nullptr, nullptr, nullptr);
        if (ret == SQLITE_OK) {
            break;
        }
        if (ret != SQLITE_BUSY) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            sqlite3_close(hDB_);
            hDB_ = nullptr;
            return;
        }
        const char *max_iters = getenv("PROJ_LOCK_MAX_ITERS");
        if (i >= (max_iters && max_iters[0] ? atoi(max_iters)
                                            : 30)) { // A bit more than 1 second
            pj_log(ctx_, PJ_LOG_ERROR, "Cannot take exclusive lock on %s",
                   path.c_str());
            sqlite3_close(hDB_);
            hDB_ = nullptr;
            return;
        }
        pj_log(ctx, PJ_LOG_TRACE, "Lock taken on cache. Waiting a bit...");
        // Retry every 5 ms for 50 ms, then every 10 ms for 100 ms, then
        // every 100 ms
        proj_sleep_ms(i < 10 ? 5 : i < 20 ? 10 : 100);
    }
    char **pasResult = nullptr;
    int nRows = 0;
    int nCols = 0;
    sqlite3_get_table(hDB_,
                      "SELECT 1 FROM sqlite_master WHERE name = 'properties'",
                      &pasResult, &nRows, &nCols, nullptr);
    sqlite3_free_table(pasResult);
    if (nRows == 0) {
        if (!createDBStructure()) {
            sqlite3_close(hDB_);
            hDB_ = nullptr;
            return;
        }
    }

    if (getenv("PROJ_CHECK_CACHE_CONSISTENCY")) {
        checkConsistency();
    }
}

// ---------------------------------------------------------------------------

static const char *cache_db_structure_sql =
    "CREATE TABLE properties("
    " url          TEXT PRIMARY KEY NOT NULL,"
    " lastChecked  TIMESTAMP NOT NULL,"
    " fileSize     INTEGER NOT NULL,"
    " lastModified TEXT,"
    " etag         TEXT"
    ");"
    "CREATE TABLE downloaded_file_properties("
    " url          TEXT PRIMARY KEY NOT NULL,"
    " lastChecked  TIMESTAMP NOT NULL,"
    " fileSize     INTEGER NOT NULL,"
    " lastModified TEXT,"
    " etag         TEXT"
    ");"
    "CREATE TABLE chunk_data("
    " id        INTEGER PRIMARY KEY AUTOINCREMENT CHECK (id > 0),"
    " data      BLOB NOT NULL"
    ");"
    "CREATE TABLE chunks("
    " id        INTEGER PRIMARY KEY AUTOINCREMENT CHECK (id > 0),"
    " url       TEXT NOT NULL,"
    " offset    INTEGER NOT NULL,"
    " data_id   INTEGER NOT NULL,"
    " data_size INTEGER NOT NULL,"
    " CONSTRAINT fk_chunks_url FOREIGN KEY (url) REFERENCES properties(url),"
    " CONSTRAINT fk_chunks_data FOREIGN KEY (data_id) REFERENCES chunk_data(id)"
    ");"
    "CREATE INDEX idx_chunks ON chunks(url, offset);"
    "CREATE TABLE linked_chunks("
    " id        INTEGER PRIMARY KEY AUTOINCREMENT CHECK (id > 0),"
    " chunk_id  INTEGER NOT NULL,"
    " prev      INTEGER,"
    " next      INTEGER,"
    " CONSTRAINT fk_links_chunkid FOREIGN KEY (chunk_id) REFERENCES chunks(id),"
    " CONSTRAINT fk_links_prev FOREIGN KEY (prev) REFERENCES linked_chunks(id),"
    " CONSTRAINT fk_links_next FOREIGN KEY (next) REFERENCES linked_chunks(id)"
    ");"
    "CREATE INDEX idx_linked_chunks_chunk_id ON linked_chunks(chunk_id);"
    "CREATE TABLE linked_chunks_head_tail("
    "  head       INTEGER,"
    "  tail       INTEGER,"
    "  CONSTRAINT lht_head FOREIGN KEY (head) REFERENCES linked_chunks(id),"
    "  CONSTRAINT lht_tail FOREIGN KEY (tail) REFERENCES linked_chunks(id)"
    ");"
    "INSERT INTO linked_chunks_head_tail VALUES (NULL, NULL);";

bool DiskChunkCache::createDBStructure() {

    pj_log(ctx_, PJ_LOG_TRACE, "Creating cache DB structure");
    if (sqlite3_exec(hDB_, cache_db_structure_sql, nullptr, nullptr, nullptr) !=
        SQLITE_OK) {
        pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------

#define INVALIDATED_SQL_LITERAL "'invalidated'"

bool DiskChunkCache::checkConsistency() {

    auto stmt = prepare("SELECT * FROM chunk_data WHERE id NOT IN (SELECT "
                        "data_id FROM chunks)");
    if (!stmt) {
        return false;
    }
    if (stmt->execute() != SQLITE_DONE) {
        fprintf(stderr, "Rows in chunk_data not referenced by chunks.\n");
        return false;
    }

    stmt = prepare("SELECT * FROM chunks WHERE id NOT IN (SELECT chunk_id FROM "
                   "linked_chunks)");
    if (!stmt) {
        return false;
    }
    if (stmt->execute() != SQLITE_DONE) {
        fprintf(stderr, "Rows in chunks not referenced by linked_chunks.\n");
        return false;
    }

    stmt = prepare("SELECT * FROM chunks WHERE url <> " INVALIDATED_SQL_LITERAL
                   " AND url "
                   "NOT IN (SELECT url FROM properties)");
    if (!stmt) {
        return false;
    }
    if (stmt->execute() != SQLITE_DONE) {
        fprintf(stderr, "url values in chunks not referenced by properties.\n");
        return false;
    }

    stmt = prepare("SELECT head, tail FROM linked_chunks_head_tail");
    if (!stmt) {
        return false;
    }
    if (stmt->execute() != SQLITE_ROW) {
        fprintf(stderr, "linked_chunks_head_tail empty.\n");
        return false;
    }
    const auto head = stmt->getInt64();
    const auto tail = stmt->getInt64();
    if (stmt->execute() != SQLITE_DONE) {
        fprintf(stderr, "linked_chunks_head_tail has more than one row.\n");
        return false;
    }

    stmt = prepare("SELECT COUNT(*) FROM linked_chunks");
    if (!stmt) {
        return false;
    }
    if (stmt->execute() != SQLITE_ROW) {
        fprintf(stderr, "linked_chunks_head_tail empty.\n");
        return false;
    }
    const auto count_linked_chunks = stmt->getInt64();

    if (head) {
        auto id = head;
        std::set<sqlite3_int64> visitedIds;
        stmt = prepare("SELECT next FROM linked_chunks WHERE id = ?");
        if (!stmt) {
            return false;
        }
        while (true) {
            visitedIds.insert(id);
            stmt->reset();
            stmt->bindInt64(id);
            if (stmt->execute() != SQLITE_ROW) {
                fprintf(stderr, "cannot find linked_chunks.id = %d.\n",
                        static_cast<int>(id));
                return false;
            }
            auto next = stmt->getInt64();
            if (next == 0) {
                if (id != tail) {
                    fprintf(stderr,
                            "last item when following next is not tail.\n");
                    return false;
                }
                break;
            }
            if (visitedIds.find(next) != visitedIds.end()) {
                fprintf(stderr, "found cycle on linked_chunks.next = %d.\n",
                        static_cast<int>(next));
                return false;
            }
            id = next;
        }
        if (visitedIds.size() != static_cast<size_t>(count_linked_chunks)) {
            fprintf(stderr,
                    "ghost items in linked_chunks when following next.\n");
            return false;
        }
    } else if (count_linked_chunks) {
        fprintf(stderr, "linked_chunks_head_tail.head = NULL but linked_chunks "
                        "not empty.\n");
        return false;
    }

    if (tail) {
        auto id = tail;
        std::set<sqlite3_int64> visitedIds;
        stmt = prepare("SELECT prev FROM linked_chunks WHERE id = ?");
        if (!stmt) {
            return false;
        }
        while (true) {
            visitedIds.insert(id);
            stmt->reset();
            stmt->bindInt64(id);
            if (stmt->execute() != SQLITE_ROW) {
                fprintf(stderr, "cannot find linked_chunks.id = %d.\n",
                        static_cast<int>(id));
                return false;
            }
            auto prev = stmt->getInt64();
            if (prev == 0) {
                if (id != head) {
                    fprintf(stderr,
                            "last item when following prev is not head.\n");
                    return false;
                }
                break;
            }
            if (visitedIds.find(prev) != visitedIds.end()) {
                fprintf(stderr, "found cycle on linked_chunks.prev = %d.\n",
                        static_cast<int>(prev));
                return false;
            }
            id = prev;
        }
        if (visitedIds.size() != static_cast<size_t>(count_linked_chunks)) {
            fprintf(stderr,
                    "ghost items in linked_chunks when following prev.\n");
            return false;
        }
    } else if (count_linked_chunks) {
        fprintf(stderr, "linked_chunks_head_tail.tail = NULL but linked_chunks "
                        "not empty.\n");
        return false;
    }

    fprintf(stderr, "check ok\n");
    return true;
}

// ---------------------------------------------------------------------------

DiskChunkCache::~DiskChunkCache() {
    if (hDB_) {
        sqlite3_exec(hDB_, "COMMIT", nullptr, nullptr, nullptr);
        sqlite3_close(hDB_);
    }
}

// ---------------------------------------------------------------------------

void DiskChunkCache::closeAndUnlink() {
    if (hDB_) {
        sqlite3_exec(hDB_, "COMMIT", nullptr, nullptr, nullptr);
        sqlite3_close(hDB_);
        hDB_ = nullptr;
    }
    if (vfs_) {
        vfs_->raw()->xDelete(vfs_->raw(), path_.c_str(), 0);
    }
}

// ---------------------------------------------------------------------------

std::unique_ptr<SQLiteStatement> DiskChunkCache::prepare(const char *sql) {
    sqlite3_stmt *hStmt = nullptr;
    sqlite3_prepare_v2(hDB_, sql, -1, &hStmt, nullptr);
    if (!hStmt) {
        pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
        return nullptr;
    }
    return std::unique_ptr<SQLiteStatement>(new SQLiteStatement(hStmt));
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::get_links(sqlite3_int64 chunk_id, sqlite3_int64 &link_id,
                               sqlite3_int64 &prev, sqlite3_int64 &next,
                               sqlite3_int64 &head, sqlite3_int64 &tail) {
    auto stmt =
        prepare("SELECT id, prev, next FROM linked_chunks WHERE chunk_id = ?");
    if (!stmt)
        return false;
    stmt->bindInt64(chunk_id);
    {
        const auto ret = stmt->execute();
        if (ret != SQLITE_ROW) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }
    link_id = stmt->getInt64();
    prev = stmt->getInt64();
    next = stmt->getInt64();

    stmt = prepare("SELECT head, tail FROM linked_chunks_head_tail");
    {
        const auto ret = stmt->execute();
        if (ret != SQLITE_ROW) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }
    head = stmt->getInt64();
    tail = stmt->getInt64();
    return true;
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::update_links_of_prev_and_next_links(sqlite3_int64 prev,
                                                         sqlite3_int64 next) {
    if (prev) {
        auto stmt = prepare("UPDATE linked_chunks SET next = ? WHERE id = ?");
        if (!stmt)
            return false;
        if (next)
            stmt->bindInt64(next);
        else
            stmt->bindNull();
        stmt->bindInt64(prev);
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }

    if (next) {
        auto stmt = prepare("UPDATE linked_chunks SET prev = ? WHERE id = ?");
        if (!stmt)
            return false;
        if (prev)
            stmt->bindInt64(prev);
        else
            stmt->bindNull();
        stmt->bindInt64(next);
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::update_linked_chunks(sqlite3_int64 link_id,
                                          sqlite3_int64 prev,
                                          sqlite3_int64 next) {
    auto stmt =
        prepare("UPDATE linked_chunks SET prev = ?, next = ? WHERE id = ?");
    if (!stmt)
        return false;
    if (prev)
        stmt->bindInt64(prev);
    else
        stmt->bindNull();
    if (next)
        stmt->bindInt64(next);
    else
        stmt->bindNull();
    stmt->bindInt64(link_id);
    const auto ret = stmt->execute();
    if (ret != SQLITE_DONE) {
        pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::update_linked_chunks_head_tail(sqlite3_int64 head,
                                                    sqlite3_int64 tail) {
    auto stmt =
        prepare("UPDATE linked_chunks_head_tail SET head = ?, tail = ?");
    if (!stmt)
        return false;
    if (head)
        stmt->bindInt64(head);
    else
        stmt->bindNull(); // shouldn't happen normally
    if (tail)
        stmt->bindInt64(tail);
    else
        stmt->bindNull(); // shouldn't happen normally
    const auto ret = stmt->execute();
    if (ret != SQLITE_DONE) {
        pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::move_to_head(sqlite3_int64 chunk_id) {

    sqlite3_int64 link_id = 0;
    sqlite3_int64 prev = 0;
    sqlite3_int64 next = 0;
    sqlite3_int64 head = 0;
    sqlite3_int64 tail = 0;
    if (!get_links(chunk_id, link_id, prev, next, head, tail)) {
        return false;
    }

    if (link_id == head) {
        return true;
    }

    if (!update_links_of_prev_and_next_links(prev, next)) {
        return false;
    }

    if (head) {
        auto stmt = prepare("UPDATE linked_chunks SET prev = ? WHERE id = ?");
        if (!stmt)
            return false;
        stmt->bindInt64(link_id);
        stmt->bindInt64(head);
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }

    return update_linked_chunks(link_id, 0, head) &&
           update_linked_chunks_head_tail(link_id,
                                          (link_id == tail) ? prev : tail);
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::move_to_tail(sqlite3_int64 chunk_id) {
    sqlite3_int64 link_id = 0;
    sqlite3_int64 prev = 0;
    sqlite3_int64 next = 0;
    sqlite3_int64 head = 0;
    sqlite3_int64 tail = 0;
    if (!get_links(chunk_id, link_id, prev, next, head, tail)) {
        return false;
    }

    if (link_id == tail) {
        return true;
    }

    if (!update_links_of_prev_and_next_links(prev, next)) {
        return false;
    }

    if (tail) {
        auto stmt = prepare("UPDATE linked_chunks SET next = ? WHERE id = ?");
        if (!stmt)
            return false;
        stmt->bindInt64(link_id);
        stmt->bindInt64(tail);
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }

    return update_linked_chunks(link_id, tail, 0) &&
           update_linked_chunks_head_tail((link_id == head) ? next : head,
                                          link_id);
}

// ---------------------------------------------------------------------------

void NetworkChunkCache::insert(PJ_CONTEXT *ctx, const std::string &url,
                               unsigned long long chunkIdx,
                               std::vector<unsigned char> &&data) {
    auto dataPtr(std::make_shared<std::vector<unsigned char>>(std::move(data)));
    cache_.insert(Key(url, chunkIdx), dataPtr);

    auto diskCache = DiskChunkCache::open(ctx);
    if (!diskCache)
        return;
    auto hDB = diskCache->handle();

    // Always insert DOWNLOAD_CHUNK_SIZE bytes to avoid fragmentation
    std::vector<unsigned char> blob(*dataPtr);
    assert(blob.size() <= DOWNLOAD_CHUNK_SIZE);
    blob.resize(DOWNLOAD_CHUNK_SIZE);

    // Check if there is an existing entry for that URL and offset
    auto stmt = diskCache->prepare(
        "SELECT id, data_id FROM chunks WHERE url = ? AND offset = ?");
    if (!stmt)
        return;
    stmt->bindText(url.c_str());
    stmt->bindInt64(chunkIdx * DOWNLOAD_CHUNK_SIZE);

    const auto mainRet = stmt->execute();
    if (mainRet == SQLITE_ROW) {
        const auto chunk_id = stmt->getInt64();
        const auto data_id = stmt->getInt64();
        stmt =
            diskCache->prepare("UPDATE chunk_data SET data = ? WHERE id = ?");
        if (!stmt)
            return;
        stmt->bindBlob(blob.data(), blob.size());
        stmt->bindInt64(data_id);
        {
            const auto ret = stmt->execute();
            if (ret != SQLITE_DONE) {
                pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
                return;
            }
        }

        diskCache->move_to_head(chunk_id);

        return;
    } else if (mainRet != SQLITE_DONE) {
        pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
        return;
    }

    // Lambda to recycle an existing entry that was either invalidated, or
    // least recently used.
    const auto reuseExistingEntry = [ctx, &blob, &diskCache, hDB, &url,
                                     chunkIdx, &dataPtr](
        std::unique_ptr<SQLiteStatement> &stmtIn) {
        const auto chunk_id = stmtIn->getInt64();
        const auto data_id = stmtIn->getInt64();
        if (data_id <= 0) {
            pj_log(ctx, PJ_LOG_ERROR, "data_id <= 0");
            return;
        }

        auto l_stmt =
            diskCache->prepare("UPDATE chunk_data SET data = ? WHERE id = ?");
        if (!l_stmt)
            return;
        l_stmt->bindBlob(blob.data(), blob.size());
        l_stmt->bindInt64(data_id);
        {
            const auto ret2 = l_stmt->execute();
            if (ret2 != SQLITE_DONE) {
                pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
                return;
            }
        }

        l_stmt = diskCache->prepare("UPDATE chunks SET url = ?, "
                                    "offset = ?, data_size = ?, data_id = ? "
                                    "WHERE id = ?");
        if (!l_stmt)
            return;
        l_stmt->bindText(url.c_str());
        l_stmt->bindInt64(chunkIdx * DOWNLOAD_CHUNK_SIZE);
        l_stmt->bindInt64(dataPtr->size());
        l_stmt->bindInt64(data_id);
        l_stmt->bindInt64(chunk_id);
        {
            const auto ret2 = l_stmt->execute();
            if (ret2 != SQLITE_DONE) {
                pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
                return;
            }
        }

        diskCache->move_to_head(chunk_id);
    };

    // Find if there is an invalidated chunk we can reuse
    stmt = diskCache->prepare(
        "SELECT id, data_id FROM chunks "
        "WHERE id = (SELECT tail FROM linked_chunks_head_tail) AND "
        "url = " INVALIDATED_SQL_LITERAL);
    if (!stmt)
        return;
    {
        const auto ret = stmt->execute();
        if (ret == SQLITE_ROW) {
            reuseExistingEntry(stmt);
            return;
        } else if (ret != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }

    // Check if we have not reached the max size of the cache
    stmt = diskCache->prepare("SELECT COUNT(*) FROM chunks");
    if (!stmt)
        return;
    {
        const auto ret = stmt->execute();
        if (ret != SQLITE_ROW) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }

    const auto max_size = pj_context_get_grid_cache_max_size(ctx);
    if (max_size > 0 &&
        static_cast<long long>(stmt->getInt64() * DOWNLOAD_CHUNK_SIZE) >=
            max_size) {
        stmt = diskCache->prepare(
            "SELECT id, data_id FROM chunks "
            "WHERE id = (SELECT tail FROM linked_chunks_head_tail)");
        if (!stmt)
            return;

        const auto ret = stmt->execute();
        if (ret != SQLITE_ROW) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
        reuseExistingEntry(stmt);
        return;
    }

    // Otherwise just append a new entry
    stmt = diskCache->prepare("INSERT INTO chunk_data(data) VALUES (?)");
    if (!stmt)
        return;
    stmt->bindBlob(blob.data(), blob.size());
    {
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }

    const auto chunk_data_id = sqlite3_last_insert_rowid(hDB);

    stmt = diskCache->prepare("INSERT INTO chunks(url, offset, data_id, "
                              "data_size) VALUES (?,?,?,?)");
    if (!stmt)
        return;
    stmt->bindText(url.c_str());
    stmt->bindInt64(chunkIdx * DOWNLOAD_CHUNK_SIZE);
    stmt->bindInt64(chunk_data_id);
    stmt->bindInt64(dataPtr->size());
    {
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }

    const auto chunk_id = sqlite3_last_insert_rowid(hDB);

    stmt = diskCache->prepare(
        "INSERT INTO linked_chunks(chunk_id, prev, next) VALUES (?,NULL,NULL)");
    if (!stmt)
        return;
    stmt->bindInt64(chunk_id);
    if (stmt->execute() != SQLITE_DONE) {
        pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
        return;
    }

    stmt = diskCache->prepare("SELECT head FROM linked_chunks_head_tail");
    if (!stmt)
        return;
    if (stmt->execute() != SQLITE_ROW) {
        pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
        return;
    }
    if (stmt->getInt64() == 0) {
        stmt = diskCache->prepare(
            "UPDATE linked_chunks_head_tail SET head = ?, tail = ?");
        if (!stmt)
            return;
        stmt->bindInt64(chunk_id);
        stmt->bindInt64(chunk_id);
        if (stmt->execute() != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }

    diskCache->move_to_head(chunk_id);
}

// ---------------------------------------------------------------------------

std::shared_ptr<std::vector<unsigned char>>
NetworkChunkCache::get(PJ_CONTEXT *ctx, const std::string &url,
                       unsigned long long chunkIdx) {
    std::shared_ptr<std::vector<unsigned char>> ret;
    if (cache_.tryGet(Key(url, chunkIdx), ret)) {
        return ret;
    }

    auto diskCache = DiskChunkCache::open(ctx);
    if (!diskCache)
        return ret;
    auto hDB = diskCache->handle();

    auto stmt = diskCache->prepare(
        "SELECT chunks.id, chunks.data_size, chunk_data.data FROM chunks "
        "JOIN chunk_data ON chunks.id = chunk_data.id "
        "WHERE chunks.url = ? AND chunks.offset = ?");
    if (!stmt)
        return ret;

    stmt->bindText(url.c_str());
    stmt->bindInt64(chunkIdx * DOWNLOAD_CHUNK_SIZE);

    const auto mainRet = stmt->execute();
    if (mainRet == SQLITE_ROW) {
        const auto chunk_id = stmt->getInt64();
        const auto data_size = stmt->getInt64();
        int blob_size = 0;
        const void *blob = stmt->getBlob(blob_size);
        if (blob_size < data_size) {
            pj_log(ctx, PJ_LOG_ERROR,
                   "blob_size=%d < data_size for chunk_id=%d", blob_size,
                   static_cast<int>(chunk_id));
            return ret;
        }
        if (data_size > static_cast<sqlite3_int64>(DOWNLOAD_CHUNK_SIZE)) {
            pj_log(ctx, PJ_LOG_ERROR, "data_size > DOWNLOAD_CHUNK_SIZE");
            return ret;
        }
        ret.reset(new std::vector<unsigned char>());
        ret->assign(reinterpret_cast<const unsigned char *>(blob),
                    reinterpret_cast<const unsigned char *>(blob) +
                        static_cast<size_t>(data_size));
        cache_.insert(Key(url, chunkIdx), ret);

        if (!diskCache->move_to_head(chunk_id))
            return ret;
    } else if (mainRet != SQLITE_DONE) {
        pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
    }

    return ret;
}

// ---------------------------------------------------------------------------

std::shared_ptr<std::vector<unsigned char>>
NetworkChunkCache::get(PJ_CONTEXT *ctx, const std::string &url,
                       unsigned long long chunkIdx, FileProperties &props) {
    if (!gNetworkFileProperties.tryGet(ctx, url, props)) {
        return nullptr;
    }

    return get(ctx, url, chunkIdx);
}

// ---------------------------------------------------------------------------

void NetworkChunkCache::clearMemoryCache() { cache_.clear(); }

// ---------------------------------------------------------------------------

void NetworkChunkCache::clearDiskChunkCache(PJ_CONTEXT *ctx) {
    auto diskCache = DiskChunkCache::open(ctx);
    if (!diskCache)
        return;
    diskCache->closeAndUnlink();
}

// ---------------------------------------------------------------------------

void NetworkFilePropertiesCache::insert(PJ_CONTEXT *ctx, const std::string &url,
                                        FileProperties &props) {
    time(&props.lastChecked);
    cache_.insert(url, props);

    auto diskCache = DiskChunkCache::open(ctx);
    if (!diskCache)
        return;
    auto hDB = diskCache->handle();
    auto stmt = diskCache->prepare("SELECT fileSize, lastModified, etag "
                                   "FROM properties WHERE url = ?");
    if (!stmt)
        return;
    stmt->bindText(url.c_str());
    if (stmt->execute() == SQLITE_ROW) {
        FileProperties cachedProps;
        cachedProps.size = stmt->getInt64();
        const char *lastModified = stmt->getText();
        cachedProps.lastModified = lastModified ? lastModified : std::string();
        const char *etag = stmt->getText();
        cachedProps.etag = etag ? etag : std::string();
        if (props.size != cachedProps.size ||
            props.lastModified != cachedProps.lastModified ||
            props.etag != cachedProps.etag) {

            // If cached properties don't match recent fresh ones, invalidate
            // cached chunks
            stmt = diskCache->prepare("SELECT id FROM chunks WHERE url = ?");
            if (!stmt)
                return;
            stmt->bindText(url.c_str());
            std::vector<sqlite3_int64> ids;
            while (stmt->execute() == SQLITE_ROW) {
                ids.emplace_back(stmt->getInt64());
                stmt->resetResIndex();
            }

            for (const auto id : ids) {
                diskCache->move_to_tail(id);
            }

            stmt = diskCache->prepare(
                "UPDATE chunks SET url = " INVALIDATED_SQL_LITERAL ", "
                "offset = -1, data_size = 0 WHERE url = ?");
            if (!stmt)
                return;
            stmt->bindText(url.c_str());
            if (stmt->execute() != SQLITE_DONE) {
                pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
                return;
            }
        }

        stmt = diskCache->prepare("UPDATE properties SET lastChecked = ?, "
                                  "fileSize = ?, lastModified = ?, etag = ? "
                                  "WHERE url = ?");
        if (!stmt)
            return;
        stmt->bindInt64(props.lastChecked);
        stmt->bindInt64(props.size);
        if (props.lastModified.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.lastModified.c_str());
        if (props.etag.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.etag.c_str());
        stmt->bindText(url.c_str());
        if (stmt->execute() != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    } else {
        stmt = diskCache->prepare("INSERT INTO properties (url, lastChecked, "
                                  "fileSize, lastModified, etag) VALUES "
                                  "(?,?,?,?,?)");
        if (!stmt)
            return;
        stmt->bindText(url.c_str());
        stmt->bindInt64(props.lastChecked);
        stmt->bindInt64(props.size);
        if (props.lastModified.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.lastModified.c_str());
        if (props.etag.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.etag.c_str());
        if (stmt->execute() != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }
}

// ---------------------------------------------------------------------------

bool NetworkFilePropertiesCache::tryGet(PJ_CONTEXT *ctx, const std::string &url,
                                        FileProperties &props) {
    if (cache_.tryGet(url, props)) {
        return true;
    }

    auto diskCache = DiskChunkCache::open(ctx);
    if (!diskCache)
        return false;
    auto stmt =
        diskCache->prepare("SELECT lastChecked, fileSize, lastModified, etag "
                           "FROM properties WHERE url = ?");
    if (!stmt)
        return false;
    stmt->bindText(url.c_str());
    if (stmt->execute() != SQLITE_ROW) {
        return false;
    }
    props.lastChecked = stmt->getInt64();
    props.size = stmt->getInt64();
    const char *lastModified = stmt->getText();
    props.lastModified = lastModified ? lastModified : std::string();
    const char *etag = stmt->getText();
    props.etag = etag ? etag : std::string();

    const auto ttl = pj_context_get_grid_cache_ttl(ctx);
    if (ttl > 0) {
        time_t curTime;
        time(&curTime);
        if (curTime > props.lastChecked + ttl) {
            props = FileProperties();
            return false;
        }
    }
    cache_.insert(url, props);
    return true;
}

// ---------------------------------------------------------------------------

void NetworkFilePropertiesCache::clearMemoryCache() { cache_.clear(); }

// ---------------------------------------------------------------------------

class NetworkFile : public File {
    PJ_CONTEXT *m_ctx;
    std::string m_url;
    PROJ_NETWORK_HANDLE *m_handle;
    unsigned long long m_pos = 0;
    size_t m_nBlocksToDownload = 1;
    unsigned long long m_lastDownloadedOffset;
    FileProperties m_props;
    proj_network_close_cbk_type m_closeCbk;
    bool m_hasChanged = false;

    NetworkFile(const NetworkFile &) = delete;
    NetworkFile &operator=(const NetworkFile &) = delete;

  protected:
    NetworkFile(PJ_CONTEXT *ctx, const std::string &url,
                PROJ_NETWORK_HANDLE *handle,
                unsigned long long lastDownloadOffset,
                const FileProperties &props)
        : File(url), m_ctx(ctx), m_url(url), m_handle(handle),
          m_lastDownloadedOffset(lastDownloadOffset), m_props(props),
          m_closeCbk(ctx->networking.close) {}

  public:
    ~NetworkFile() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    size_t write(const void *, size_t) override { return 0; }
    bool seek(unsigned long long offset, int whence) override;
    unsigned long long tell() override;
    void reassign_context(PJ_CONTEXT *ctx) override;
    bool hasChanged() const override { return m_hasChanged; }

    static std::unique_ptr<File> open(PJ_CONTEXT *ctx, const char *filename);

    static bool get_props_from_headers(PJ_CONTEXT *ctx,
                                       PROJ_NETWORK_HANDLE *handle,
                                       FileProperties &props);
};

// ---------------------------------------------------------------------------

bool NetworkFile::get_props_from_headers(PJ_CONTEXT *ctx,
                                         PROJ_NETWORK_HANDLE *handle,
                                         FileProperties &props) {
    const char *contentRange = ctx->networking.get_header_value(
        ctx, handle, "Content-Range", ctx->networking.user_data);
    if (contentRange) {
        const char *slash = strchr(contentRange, '/');
        if (slash) {
            props.size = std::stoull(slash + 1);

            const char *lastModified = ctx->networking.get_header_value(
                ctx, handle, "Last-Modified", ctx->networking.user_data);
            if (lastModified)
                props.lastModified = lastModified;

            const char *etag = ctx->networking.get_header_value(
                ctx, handle, "ETag", ctx->networking.user_data);
            if (etag)
                props.etag = etag;

            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

std::unique_ptr<File> NetworkFile::open(PJ_CONTEXT *ctx, const char *filename) {
    FileProperties props;
    if (gNetworkChunkCache.get(ctx, filename, 0, props)) {
        return std::unique_ptr<File>(new NetworkFile(
            ctx, filename, nullptr,
            std::numeric_limits<unsigned long long>::max(), props));
    } else {
        std::vector<unsigned char> buffer(DOWNLOAD_CHUNK_SIZE);
        size_t size_read = 0;
        std::string errorBuffer;
        errorBuffer.resize(1024);

        auto handle = ctx->networking.open(
            ctx, filename, 0, buffer.size(), &buffer[0], &size_read,
            errorBuffer.size(), &errorBuffer[0], ctx->networking.user_data);
        buffer.resize(size_read);
        if (!handle) {
            errorBuffer.resize(strlen(errorBuffer.data()));
            pj_log(ctx, PJ_LOG_ERROR, "Cannot open %s: %s", filename,
                   errorBuffer.c_str());
        }

        bool ok = false;
        if (handle) {
            if (get_props_from_headers(ctx, handle, props)) {
                ok = true;
                gNetworkFileProperties.insert(ctx, filename, props);
                gNetworkChunkCache.insert(ctx, filename, 0, std::move(buffer));
            }
        }

        return std::unique_ptr<File>(
            ok ? new NetworkFile(ctx, filename, handle, size_read, props)
               : nullptr);
    }
}

// ---------------------------------------------------------------------------

size_t NetworkFile::read(void *buffer, size_t sizeBytes) {

    if (sizeBytes == 0)
        return 0;

    auto iterOffset = m_pos;
    while (sizeBytes) {
        const auto chunkIdxToDownload = iterOffset / DOWNLOAD_CHUNK_SIZE;
        const auto offsetToDownload = chunkIdxToDownload * DOWNLOAD_CHUNK_SIZE;
        std::vector<unsigned char> region;
        auto pChunk = gNetworkChunkCache.get(m_ctx, m_url, chunkIdxToDownload);
        if (pChunk != nullptr) {
            region = *pChunk;
        } else {
            if (offsetToDownload == m_lastDownloadedOffset) {
                // In case of consecutive reads (of small size), we use a
                // heuristic that we will read the file sequentially, so
                // we double the requested size to decrease the number of
                // client/server roundtrips.
                if (m_nBlocksToDownload < 100)
                    m_nBlocksToDownload *= 2;
            } else {
                // Random reads. Cancel the above heuristics.
                m_nBlocksToDownload = 1;
            }

            // Ensure that we will request at least the number of blocks
            // to satisfy the remaining buffer size to read.
            const auto endOffsetToDownload =
                ((iterOffset + sizeBytes + DOWNLOAD_CHUNK_SIZE - 1) /
                 DOWNLOAD_CHUNK_SIZE) *
                DOWNLOAD_CHUNK_SIZE;
            const auto nMinBlocksToDownload = static_cast<size_t>(
                (endOffsetToDownload - offsetToDownload) / DOWNLOAD_CHUNK_SIZE);
            if (m_nBlocksToDownload < nMinBlocksToDownload)
                m_nBlocksToDownload = nMinBlocksToDownload;

            // Avoid reading already cached data.
            // Note: this might get evicted if concurrent reads are done, but
            // this should not cause bugs. Just missed optimization.
            for (size_t i = 1; i < m_nBlocksToDownload; i++) {
                if (gNetworkChunkCache.get(m_ctx, m_url,
                                           chunkIdxToDownload + i) != nullptr) {
                    m_nBlocksToDownload = i;
                    break;
                }
            }

            if (m_nBlocksToDownload > MAX_CHUNKS)
                m_nBlocksToDownload = MAX_CHUNKS;

            region.resize(m_nBlocksToDownload * DOWNLOAD_CHUNK_SIZE);
            size_t nRead = 0;
            std::string errorBuffer;
            errorBuffer.resize(1024);
            if (!m_handle) {
                m_handle = m_ctx->networking.open(
                    m_ctx, m_url.c_str(), offsetToDownload,
                    m_nBlocksToDownload * DOWNLOAD_CHUNK_SIZE, &region[0],
                    &nRead, errorBuffer.size(), &errorBuffer[0],
                    m_ctx->networking.user_data);
                if (!m_handle) {
                    return 0;
                }
            } else {
                nRead = m_ctx->networking.read_range(
                    m_ctx, m_handle, offsetToDownload,
                    m_nBlocksToDownload * DOWNLOAD_CHUNK_SIZE, &region[0],
                    errorBuffer.size(), &errorBuffer[0],
                    m_ctx->networking.user_data);
            }
            if (nRead == 0) {
                errorBuffer.resize(strlen(errorBuffer.data()));
                if (!errorBuffer.empty()) {
                    pj_log(m_ctx, PJ_LOG_ERROR, "Cannot read in %s: %s",
                           m_url.c_str(), errorBuffer.c_str());
                }
                return 0;
            }

            if (!m_hasChanged) {
                FileProperties props;
                if (get_props_from_headers(m_ctx, m_handle, props)) {
                    if (props.size != m_props.size ||
                        props.lastModified != m_props.lastModified ||
                        props.etag != m_props.etag) {
                        gNetworkFileProperties.insert(m_ctx, m_url, props);
                        gNetworkChunkCache.clearMemoryCache();
                        m_hasChanged = true;
                    }
                }
            }

            region.resize(nRead);
            m_lastDownloadedOffset = offsetToDownload + nRead;

            const auto nChunks =
                (region.size() + DOWNLOAD_CHUNK_SIZE - 1) / DOWNLOAD_CHUNK_SIZE;
            for (size_t i = 0; i < nChunks; i++) {
                std::vector<unsigned char> chunk(
                    region.data() + i * DOWNLOAD_CHUNK_SIZE,
                    region.data() +
                        std::min((i + 1) * DOWNLOAD_CHUNK_SIZE, region.size()));
                gNetworkChunkCache.insert(m_ctx, m_url, chunkIdxToDownload + i,
                                          std::move(chunk));
            }
        }
        const size_t nToCopy = static_cast<size_t>(
            std::min(static_cast<unsigned long long>(sizeBytes),
                     region.size() - (iterOffset - offsetToDownload)));
        memcpy(buffer, region.data() + iterOffset - offsetToDownload, nToCopy);
        buffer = static_cast<char *>(buffer) + nToCopy;
        iterOffset += nToCopy;
        sizeBytes -= nToCopy;
        if (region.size() < static_cast<size_t>(DOWNLOAD_CHUNK_SIZE) &&
            sizeBytes != 0) {
            break;
        }
    }

    size_t nRead = static_cast<size_t>(iterOffset - m_pos);
    m_pos = iterOffset;
    return nRead;
}

// ---------------------------------------------------------------------------

bool NetworkFile::seek(unsigned long long offset, int whence) {
    if (whence == SEEK_SET) {
        m_pos = offset;
    } else if (whence == SEEK_CUR) {
        m_pos += offset;
    } else {
        if (offset != 0)
            return false;
        m_pos = m_props.size;
    }
    return true;
}

// ---------------------------------------------------------------------------

unsigned long long NetworkFile::tell() { return m_pos; }

// ---------------------------------------------------------------------------

NetworkFile::~NetworkFile() {
    if (m_handle) {
        m_ctx->networking.close(m_ctx, m_handle, m_ctx->networking.user_data);
    }
}

// ---------------------------------------------------------------------------

void NetworkFile::reassign_context(PJ_CONTEXT *ctx) {
    m_ctx = ctx;
    if (m_closeCbk != m_ctx->networking.close) {
        pj_log(m_ctx, PJ_LOG_ERROR,
               "Networking close callback has changed following context "
               "reassignment ! This is highly suspicious");
    }
}

// ---------------------------------------------------------------------------

std::unique_ptr<File> FileManager::open(PJ_CONTEXT *ctx, const char *filename,
                                        FileAccess access) {
    if (starts_with(filename, "http://") || starts_with(filename, "https://")) {
        if (!pj_context_is_network_enabled(ctx)) {
            pj_log(
                ctx, PJ_LOG_ERROR,
                "Attempt at accessing remote resource not authorized. Either "
                "set PROJ_NETWORK=ON or "
                "proj_context_set_enable_network(ctx, TRUE)");
            return nullptr;
        }
        return NetworkFile::open(ctx, filename);
    }
#ifndef REMOVE_LEGACY_SUPPORT
    // If the user has specified a legacy fileapi, use it
    if (ctx->fileapi_legacy != pj_get_default_fileapi()) {
        return FileLegacyAdapter::open(ctx, filename, access);
    }
#endif
    if (ctx->fileApi.open_cbk != nullptr) {
        return FileApiAdapter::open(ctx, filename, access);
    }
#ifdef _WIN32
    return FileWin32::open(ctx, filename, access);
#else
    return FileStdio::open(ctx, filename, access);
#endif
}

// ---------------------------------------------------------------------------

bool FileManager::exists(PJ_CONTEXT *ctx, const char *filename) {
    if (ctx->fileApi.exists_cbk) {
        return ctx->fileApi.exists_cbk(ctx, filename, ctx->fileApi.user_data) !=
               0;
    }

#ifdef _WIN32
    struct __stat64 buf;
    try {
        return _wstat64(UTF8ToWString(filename).c_str(), &buf) == 0;
    } catch (const std::exception &e) {
        pj_log(ctx, PJ_LOG_DEBUG, "%s", e.what());
        return false;
    }
#else
    (void)ctx;
    struct stat sStat;
    return stat(filename, &sStat) == 0;
#endif
}

// ---------------------------------------------------------------------------

bool FileManager::mkdir(PJ_CONTEXT *ctx, const char *filename) {
    if (ctx->fileApi.mkdir_cbk) {
        return ctx->fileApi.mkdir_cbk(ctx, filename, ctx->fileApi.user_data) !=
               0;
    }

#ifdef _WIN32
    try {
        return _wmkdir(UTF8ToWString(filename).c_str()) == 0;
    } catch (const std::exception &e) {
        pj_log(ctx, PJ_LOG_DEBUG, "%s", e.what());
        return false;
    }
#else
    (void)ctx;
    return ::mkdir(filename, 0755) == 0;
#endif
}

// ---------------------------------------------------------------------------

bool FileManager::unlink(PJ_CONTEXT *ctx, const char *filename) {
    if (ctx->fileApi.unlink_cbk) {
        return ctx->fileApi.unlink_cbk(ctx, filename, ctx->fileApi.user_data) !=
               0;
    }

#ifdef _WIN32
    try {
        return _wunlink(UTF8ToWString(filename).c_str()) == 0;
    } catch (const std::exception &e) {
        pj_log(ctx, PJ_LOG_DEBUG, "%s", e.what());
        return false;
    }
#else
    (void)ctx;
    return ::unlink(filename) == 0;
#endif
}

// ---------------------------------------------------------------------------

bool FileManager::rename(PJ_CONTEXT *ctx, const char *oldPath,
                         const char *newPath) {
    if (ctx->fileApi.rename_cbk) {
        return ctx->fileApi.rename_cbk(ctx, oldPath, newPath,
                                       ctx->fileApi.user_data) != 0;
    }

#ifdef _WIN32
    try {
        return _wrename(UTF8ToWString(oldPath).c_str(),
                        UTF8ToWString(newPath).c_str()) == 0;
    } catch (const std::exception &e) {
        pj_log(ctx, PJ_LOG_DEBUG, "%s", e.what());
        return false;
    }
#else
    (void)ctx;
    return ::rename(oldPath, newPath) == 0;
#endif
}

// ---------------------------------------------------------------------------

std::string FileManager::getProjLibEnvVar(PJ_CONTEXT *ctx) {
    if (!ctx->env_var_proj_lib.empty()) {
        return ctx->env_var_proj_lib;
    }
    (void)ctx;
    std::string str;
    const char *envvar = getenv("PROJ_LIB");
    if (!envvar)
        return str;
    str = envvar;
#ifdef _WIN32
    // Assume this is UTF-8. If not try to convert from ANSI page
    bool looksLikeUTF8 = false;
    try {
        UTF8ToWString(envvar);
        looksLikeUTF8 = true;
    } catch (const std::exception &) {
    }
    if (!looksLikeUTF8 || !exists(ctx, envvar)) {
        str = Win32Recode(envvar, CP_ACP, CP_UTF8);
        if (str.empty() || !exists(ctx, str.c_str()))
            str = envvar;
    }
#endif
    ctx->env_var_proj_lib = str;
    return str;
}

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

struct CurlFileHandle {
    std::string m_url;
    CURL *m_handle;
    std::string m_headers{};
    std::string m_lastval{};
    std::string m_useragent{};
    char m_szCurlErrBuf[CURL_ERROR_SIZE + 1] = {};

    CurlFileHandle(const CurlFileHandle &) = delete;
    CurlFileHandle &operator=(const CurlFileHandle &) = delete;

    explicit CurlFileHandle(const char *url, CURL *handle);
    ~CurlFileHandle();

    static PROJ_NETWORK_HANDLE *
    open(PJ_CONTEXT *, const char *url, unsigned long long offset,
         size_t size_to_read, void *buffer, size_t *out_size_read,
         size_t error_string_max_size, char *out_error_string, void *);
};

// ---------------------------------------------------------------------------

static std::string GetExecutableName() {
#if defined(__linux)
    std::string path;
    path.resize(1024);
    const auto ret = readlink("/proc/self/exe", &path[0], path.size());
    if (ret > 0) {
        path.resize(ret);
        const auto pos = path.rfind('/');
        if (pos != std::string::npos) {
            path = path.substr(pos + 1);
        }
        return path;
    }
#elif defined(_WIN32)
    std::string path;
    path.resize(1024);
    if (GetModuleFileNameA(nullptr, &path[0],
                           static_cast<DWORD>(path.size()))) {
        path.resize(strlen(path.c_str()));
        const auto pos = path.rfind('\\');
        if (pos != std::string::npos) {
            path = path.substr(pos + 1);
        }
        return path;
    }
#elif defined(__MACH__) && defined(__APPLE__)
    std::string path;
    path.resize(1024);
    uint32_t size = static_cast<uint32_t>(path.size());
    if (_NSGetExecutablePath(&path[0], &size) == 0) {
        path.resize(strlen(path.c_str()));
        const auto pos = path.rfind('/');
        if (pos != std::string::npos) {
            path = path.substr(pos + 1);
        }
        return path;
    }
#elif defined(__FreeBSD__)
    int mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;
    std::string path;
    path.resize(1024);
    size_t size = path.size();
    if (sysctl(mib, 4, &path[0], &size, nullptr, 0) == 0) {
        path.resize(strlen(path.c_str()));
        const auto pos = path.rfind('/');
        if (pos != std::string::npos) {
            path = path.substr(pos + 1);
        }
        return path;
    }
#endif

    return std::string();
}

// ---------------------------------------------------------------------------

CurlFileHandle::CurlFileHandle(const char *url, CURL *handle)
    : m_url(url), m_handle(handle) {
    curl_easy_setopt(handle, CURLOPT_URL, m_url.c_str());

    if (getenv("PROJ_CURL_VERBOSE"))
        curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);

// CURLOPT_SUPPRESS_CONNECT_HEADERS is defined in curl 7.54.0 or newer.
#if LIBCURL_VERSION_NUM >= 0x073600
    curl_easy_setopt(handle, CURLOPT_SUPPRESS_CONNECT_HEADERS, 1L);
#endif

    // Enable following redirections.  Requires libcurl 7.10.1 at least.
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 10);

    if (getenv("PROJ_UNSAFE_SSL")) {
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
    }

    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, m_szCurlErrBuf);

    if (getenv("PROJ_NO_USERAGENT") == nullptr) {
        m_useragent = "PROJ " STR(PROJ_VERSION_MAJOR) "." STR(
            PROJ_VERSION_MINOR) "." STR(PROJ_VERSION_PATCH);
        const auto exeName = GetExecutableName();
        if (!exeName.empty()) {
            m_useragent = exeName + " using " + m_useragent;
        }
        curl_easy_setopt(handle, CURLOPT_USERAGENT, m_useragent.data());
    }
}

// ---------------------------------------------------------------------------

CurlFileHandle::~CurlFileHandle() { curl_easy_cleanup(m_handle); }

// ---------------------------------------------------------------------------

static size_t pj_curl_write_func(void *buffer, size_t count, size_t nmemb,
                                 void *req) {
    const size_t nSize = count * nmemb;
    auto pStr = static_cast<std::string *>(req);
    if (pStr->size() + nSize > pStr->capacity()) {
        // to avoid servers not honouring Range to cause excessive memory
        // allocation
        return 0;
    }
    pStr->append(static_cast<const char *>(buffer), nSize);
    return nmemb;
}

// ---------------------------------------------------------------------------

static double GetNewRetryDelay(int response_code, double dfOldDelay,
                               const char *pszErrBuf,
                               const char *pszCurlError) {
    if (response_code == 429 || response_code == 500 ||
        (response_code >= 502 && response_code <= 504) ||
        // S3 sends some client timeout errors as 400 Client Error
        (response_code == 400 && pszErrBuf &&
         strstr(pszErrBuf, "RequestTimeout")) ||
        (pszCurlError && strstr(pszCurlError, "Connection timed out"))) {
        // Use an exponential backoff factor of 2 plus some random jitter
        // We don't care about cryptographic quality randomness, hence:
        // coverity[dont_call]
        return dfOldDelay * (2 + rand() * 0.5 / RAND_MAX);
    } else {
        return 0;
    }
}

// ---------------------------------------------------------------------------

constexpr double MIN_RETRY_DELAY_MS = 500;
constexpr double MAX_RETRY_DELAY_MS = 60000;

PROJ_NETWORK_HANDLE *CurlFileHandle::open(PJ_CONTEXT *ctx, const char *url,
                                          unsigned long long offset,
                                          size_t size_to_read, void *buffer,
                                          size_t *out_size_read,
                                          size_t error_string_max_size,
                                          char *out_error_string, void *) {
    CURL *hCurlHandle = curl_easy_init();
    if (!hCurlHandle)
        return nullptr;

    auto file =
        std::unique_ptr<CurlFileHandle>(new CurlFileHandle(url, hCurlHandle));

    double oldDelay = MIN_RETRY_DELAY_MS;
    std::string headers;
    std::string body;

    char szBuffer[128];
    sqlite3_snprintf(sizeof(szBuffer), szBuffer, "%llu-%llu", offset,
                     offset + size_to_read - 1);

    while (true) {
        curl_easy_setopt(hCurlHandle, CURLOPT_RANGE, szBuffer);

        headers.clear();
        headers.reserve(16 * 1024);
        curl_easy_setopt(hCurlHandle, CURLOPT_HEADERDATA, &headers);
        curl_easy_setopt(hCurlHandle, CURLOPT_HEADERFUNCTION,
                         pj_curl_write_func);

        body.clear();
        body.reserve(size_to_read);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, &body);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION,
                         pj_curl_write_func);

        file->m_szCurlErrBuf[0] = '\0';

        curl_easy_perform(hCurlHandle);

        long response_code = 0;
        curl_easy_getinfo(hCurlHandle, CURLINFO_HTTP_CODE, &response_code);

        curl_easy_setopt(hCurlHandle, CURLOPT_HEADERDATA, nullptr);
        curl_easy_setopt(hCurlHandle, CURLOPT_HEADERFUNCTION, nullptr);

        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, nullptr);

        if (response_code == 0 || response_code >= 300) {
            const double delay =
                GetNewRetryDelay(static_cast<int>(response_code), oldDelay,
                                 body.c_str(), file->m_szCurlErrBuf);
            if (delay != 0 && delay < MAX_RETRY_DELAY_MS) {
                pj_log(ctx, PJ_LOG_TRACE,
                       "Got a HTTP %ld error. Retrying in %d ms", response_code,
                       static_cast<int>(delay));
                proj_sleep_ms(static_cast<int>(delay));
                oldDelay = delay;
            } else {
                if (out_error_string) {
                    if (file->m_szCurlErrBuf[0]) {
                        snprintf(out_error_string, error_string_max_size, "%s",
                                 file->m_szCurlErrBuf);
                    } else {
                        snprintf(out_error_string, error_string_max_size,
                                 "HTTP error %ld: %s", response_code,
                                 body.c_str());
                    }
                }
                return nullptr;
            }
        } else {
            break;
        }
    }

    if (out_error_string && error_string_max_size) {
        out_error_string[0] = '\0';
    }

    if (!body.empty()) {
        memcpy(buffer, body.data(), std::min(size_to_read, body.size()));
    }
    *out_size_read = std::min(size_to_read, body.size());

    file->m_headers = std::move(headers);
    return reinterpret_cast<PROJ_NETWORK_HANDLE *>(file.release());
}

// ---------------------------------------------------------------------------

static void pj_curl_close(PJ_CONTEXT *, PROJ_NETWORK_HANDLE *handle,
                          void * /*user_data*/) {
    delete reinterpret_cast<CurlFileHandle *>(handle);
}

// ---------------------------------------------------------------------------

static size_t pj_curl_read_range(PJ_CONTEXT *ctx,
                                 PROJ_NETWORK_HANDLE *raw_handle,
                                 unsigned long long offset, size_t size_to_read,
                                 void *buffer, size_t error_string_max_size,
                                 char *out_error_string, void *) {
    auto handle = reinterpret_cast<CurlFileHandle *>(raw_handle);
    auto hCurlHandle = handle->m_handle;

    double oldDelay = MIN_RETRY_DELAY_MS;
    std::string headers;
    std::string body;

    char szBuffer[128];
    sqlite3_snprintf(sizeof(szBuffer), szBuffer, "%llu-%llu", offset,
                     offset + size_to_read - 1);

    while (true) {
        curl_easy_setopt(hCurlHandle, CURLOPT_RANGE, szBuffer);

        headers.clear();
        headers.reserve(16 * 1024);
        curl_easy_setopt(hCurlHandle, CURLOPT_HEADERDATA, &headers);
        curl_easy_setopt(hCurlHandle, CURLOPT_HEADERFUNCTION,
                         pj_curl_write_func);

        body.clear();
        body.reserve(size_to_read);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, &body);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION,
                         pj_curl_write_func);

        handle->m_szCurlErrBuf[0] = '\0';

        curl_easy_perform(hCurlHandle);

        long response_code = 0;
        curl_easy_getinfo(hCurlHandle, CURLINFO_HTTP_CODE, &response_code);

        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, nullptr);

        if (response_code == 0 || response_code >= 300) {
            const double delay =
                GetNewRetryDelay(static_cast<int>(response_code), oldDelay,
                                 body.c_str(), handle->m_szCurlErrBuf);
            if (delay != 0 && delay < MAX_RETRY_DELAY_MS) {
                pj_log(ctx, PJ_LOG_TRACE,
                       "Got a HTTP %ld error. Retrying in %d ms", response_code,
                       static_cast<int>(delay));
                proj_sleep_ms(static_cast<int>(delay));
                oldDelay = delay;
            } else {
                if (out_error_string) {
                    if (handle->m_szCurlErrBuf[0]) {
                        snprintf(out_error_string, error_string_max_size, "%s",
                                 handle->m_szCurlErrBuf);
                    } else {
                        snprintf(out_error_string, error_string_max_size,
                                 "HTTP error %ld: %s", response_code,
                                 body.c_str());
                    }
                }
                return 0;
            }
        } else {
            break;
        }
    }
    if (out_error_string && error_string_max_size) {
        out_error_string[0] = '\0';
    }

    if (!body.empty()) {
        memcpy(buffer, body.data(), std::min(size_to_read, body.size()));
    }
    handle->m_headers = std::move(headers);

    return std::min(size_to_read, body.size());
}

// ---------------------------------------------------------------------------

static const char *pj_curl_get_header_value(PJ_CONTEXT *,
                                            PROJ_NETWORK_HANDLE *raw_handle,
                                            const char *header_name, void *) {
    auto handle = reinterpret_cast<CurlFileHandle *>(raw_handle);
    auto pos = ci_find(handle->m_headers, header_name);
    if (pos == std::string::npos)
        return nullptr;
    pos += strlen(header_name);
    const char *c_str = handle->m_headers.c_str();
    if (c_str[pos] == ':')
        pos++;
    while (c_str[pos] == ' ')
        pos++;
    auto posEnd = pos;
    while (c_str[posEnd] != '\r' && c_str[posEnd] != '\n' &&
           c_str[posEnd] != '\0')
        posEnd++;
    handle->m_lastval = handle->m_headers.substr(pos, posEnd - pos);
    return handle->m_lastval.c_str();
}

#else

// ---------------------------------------------------------------------------

static PROJ_NETWORK_HANDLE *
no_op_network_open(PJ_CONTEXT *, const char * /* url */,
                   unsigned long long, /* offset */
                   size_t,             /* size to read */
                   void *,             /* buffer to update with bytes read*/
                   size_t *,           /* output: size actually read */
                   size_t error_string_max_size, char *out_error_string,
                   void * /*user_data*/) {
    if (out_error_string) {
        snprintf(out_error_string, error_string_max_size, "%s",
                 "Network functionality not available");
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

static void no_op_network_close(PJ_CONTEXT *, PROJ_NETWORK_HANDLE *,
                                void * /*user_data*/) {}

#endif

// ---------------------------------------------------------------------------

void FileManager::fillDefaultNetworkInterface(PJ_CONTEXT *ctx) {
#ifdef CURL_ENABLED
    ctx->networking.open = CurlFileHandle::open;
    ctx->networking.close = pj_curl_close;
    ctx->networking.read_range = pj_curl_read_range;
    ctx->networking.get_header_value = pj_curl_get_header_value;
#else
    ctx->networking.open = no_op_network_open;
    ctx->networking.close = no_op_network_close;
#endif
}

// ---------------------------------------------------------------------------

void FileManager::clearMemoryCache() {
    gNetworkChunkCache.clearMemoryCache();
    gNetworkFileProperties.clearMemoryCache();
}

// ---------------------------------------------------------------------------

NS_PROJ_END

//! @endcond

// ---------------------------------------------------------------------------

/** Set a file API
 *
 * All callbacks should be provided (non NULL pointers). If read-only usage
 * is intended, then the callbacks might have a dummy implementation.
 *
 * \note Those callbacks will not be used for SQLite3 database access. If
 * custom I/O is desired for that, then proj_context_set_sqlite3_vfs_name()
 * should be used.
 *
 * @param ctx PROJ context, or NULL
 * @param fileapi Pointer to file API structure (content will be copied).
 * @param user_data Arbitrary pointer provided by the user, and passed to the
 * above callbacks. May be NULL.
 * @return TRUE in case of success.
 */
int proj_context_set_fileapi(PJ_CONTEXT *ctx, const PROJ_FILE_API *fileapi,
                             void *user_data) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    if (!fileapi) {
        return false;
    }
    if (fileapi->version != 1) {
        return false;
    }
    if (!fileapi->open_cbk || !fileapi->close_cbk || !fileapi->read_cbk ||
        !fileapi->write_cbk || !fileapi->seek_cbk || !fileapi->tell_cbk ||
        !fileapi->exists_cbk || !fileapi->mkdir_cbk || !fileapi->unlink_cbk ||
        !fileapi->rename_cbk) {
        return false;
    }
    ctx->fileApi.open_cbk = fileapi->open_cbk;
    ctx->fileApi.close_cbk = fileapi->close_cbk;
    ctx->fileApi.read_cbk = fileapi->read_cbk;
    ctx->fileApi.write_cbk = fileapi->write_cbk;
    ctx->fileApi.seek_cbk = fileapi->seek_cbk;
    ctx->fileApi.tell_cbk = fileapi->tell_cbk;
    ctx->fileApi.exists_cbk = fileapi->exists_cbk;
    ctx->fileApi.mkdir_cbk = fileapi->mkdir_cbk;
    ctx->fileApi.unlink_cbk = fileapi->unlink_cbk;
    ctx->fileApi.rename_cbk = fileapi->rename_cbk;
    ctx->fileApi.user_data = user_data;
    return true;
}

// ---------------------------------------------------------------------------

/** Set the name of a custom SQLite3 VFS.
 *
 * This should be a valid SQLite3 VFS name, such as the one passed to the
 * sqlite3_vfs_register(). See https://www.sqlite.org/vfs.html
 *
 * It will be used to read proj.db or create&access the cache.db file in the
 * PROJ user writable directory.
 *
 * @param ctx PROJ context, or NULL
 * @param name SQLite3 VFS name. If NULL is passed, default implementation by
 * SQLite will be used.
 */
void proj_context_set_sqlite3_vfs_name(PJ_CONTEXT *ctx, const char *name) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    ctx->custom_sqlite3_vfs_name = name ? name : std::string();
}

// ---------------------------------------------------------------------------

/** Define a custom set of callbacks for network access.
 *
 * All callbacks should be provided (non NULL pointers).
 *
 * @param ctx PROJ context, or NULL
 * @param open_cbk Callback to open a remote file given its URL
 * @param close_cbk Callback to close a remote file.
 * @param get_header_value_cbk Callback to get HTTP headers
 * @param read_range_cbk Callback to read a range of bytes inside a remote file.
 * @param user_data Arbitrary pointer provided by the user, and passed to the
 * above callbacks. May be NULL.
 * @return TRUE in case of success.
 */
int proj_context_set_network_callbacks(
    PJ_CONTEXT *ctx, proj_network_open_cbk_type open_cbk,
    proj_network_close_cbk_type close_cbk,
    proj_network_get_header_value_cbk_type get_header_value_cbk,
    proj_network_read_range_type read_range_cbk, void *user_data) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    if (!open_cbk || !close_cbk || !get_header_value_cbk || !read_range_cbk) {
        return false;
    }
    ctx->networking.open = open_cbk;
    ctx->networking.close = close_cbk;
    ctx->networking.get_header_value = get_header_value_cbk;
    ctx->networking.read_range = read_range_cbk;
    ctx->networking.user_data = user_data;
    return true;
}

// ---------------------------------------------------------------------------

/** Enable or disable network access.
*
* This overrides the default endpoint in the PROJ configuration file or with
* the PROJ_NETWORK environment variable.
*
* @param ctx PROJ context, or NULL
* @param enable TRUE if network access is allowed.
* @return TRUE if network access is possible. That is either libcurl is
*         available, or an alternate interface has been set.
*/
int proj_context_set_enable_network(PJ_CONTEXT *ctx, int enable) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its network settings
    pj_load_ini(ctx);
    ctx->networking.enabled_env_variable_checked = true;
    ctx->networking.enabled = enable != FALSE;
#ifdef CURL_ENABLED
    return ctx->networking.enabled;
#else
    return ctx->networking.enabled &&
           ctx->networking.open != NS_PROJ::no_op_network_open;
#endif
}

// ---------------------------------------------------------------------------

/** Define the URL endpoint to query for remote grids.
*
* This overrides the default endpoint in the PROJ configuration file or with
* the PROJ_NETWORK_ENDPOINT environment variable.
*
* @param ctx PROJ context, or NULL
* @param url Endpoint URL. Must NOT be NULL.
*/
void proj_context_set_url_endpoint(PJ_CONTEXT *ctx, const char *url) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its network settings
    pj_load_ini(ctx);
    ctx->endpoint = url;
}

// ---------------------------------------------------------------------------

/** Enable or disable the local cache of grid chunks
*
* This overrides the setting in the PROJ configuration file.
*
* @param ctx PROJ context, or NULL
* @param enabled TRUE if the cache is enabled.
*/
void proj_grid_cache_set_enable(PJ_CONTEXT *ctx, int enabled) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its settings
    pj_load_ini(ctx);
    ctx->gridChunkCache.enabled = enabled != FALSE;
}

// ---------------------------------------------------------------------------

/** Override, for the considered context, the path and file of the local
* cache of grid chunks.
*
* @param ctx PROJ context, or NULL
* @param fullname Full name to the cache (encoded in UTF-8). If set to NULL,
*                 caching will be disabled.
*/
void proj_grid_cache_set_filename(PJ_CONTEXT *ctx, const char *fullname) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its settings
    pj_load_ini(ctx);
    ctx->gridChunkCache.filename = fullname ? fullname : std::string();
}

// ---------------------------------------------------------------------------

/** Override, for the considered context, the maximum size of the local
* cache of grid chunks.
*
* @param ctx PROJ context, or NULL
* @param max_size_MB Maximum size, in mega-bytes (1024*1024 bytes), or
*                    negative value to set unlimited size.
*/
void proj_grid_cache_set_max_size(PJ_CONTEXT *ctx, int max_size_MB) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its settings
    pj_load_ini(ctx);
    ctx->gridChunkCache.max_size =
        max_size_MB < 0 ? -1
                        : static_cast<long long>(max_size_MB) * 1024 * 1024;
    if (max_size_MB == 0) {
        // For debug purposes only
        const char *env_var = getenv("PROJ_GRID_CACHE_MAX_SIZE_BYTES");
        if (env_var && env_var[0] != '\0') {
            ctx->gridChunkCache.max_size = atoi(env_var);
        }
    }
}

// ---------------------------------------------------------------------------

/** Override, for the considered context, the time-to-live delay for
* re-checking if the cached properties of files are still up-to-date.
*
* @param ctx PROJ context, or NULL
* @param ttl_seconds Delay in seconds. Use negative value for no expiration.
*/
void proj_grid_cache_set_ttl(PJ_CONTEXT *ctx, int ttl_seconds) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its settings
    pj_load_ini(ctx);
    ctx->gridChunkCache.ttl = ttl_seconds;
}

// ---------------------------------------------------------------------------

/** Clear the local cache of grid chunks.
*
* @param ctx PROJ context, or NULL
*/
void proj_grid_cache_clear(PJ_CONTEXT *ctx) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    NS_PROJ::gNetworkChunkCache.clearDiskChunkCache(ctx);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

bool pj_context_is_network_enabled(PJ_CONTEXT *ctx) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    if (ctx->networking.enabled_env_variable_checked) {
        return ctx->networking.enabled;
    }
    const char *enabled = getenv("PROJ_NETWORK");
    if (enabled && enabled[0] != '\0') {
        ctx->networking.enabled = ci_equal(enabled, "ON") ||
                                  ci_equal(enabled, "YES") ||
                                  ci_equal(enabled, "TRUE");
    }
    pj_load_ini(ctx);
    ctx->networking.enabled_env_variable_checked = true;
    return ctx->networking.enabled;
}

// ---------------------------------------------------------------------------

static void CreateDirectoryRecursively(PJ_CONTEXT *ctx,
                                       const std::string &path) {
    if (NS_PROJ::FileManager::exists(ctx, path.c_str()))
        return;
    auto pos = path.find_last_of("/\\");
    if (pos == 0 || pos == std::string::npos)
        return;
    CreateDirectoryRecursively(ctx, path.substr(0, pos));
    NS_PROJ::FileManager::mkdir(ctx, path.c_str());
}

// ---------------------------------------------------------------------------

std::string pj_context_get_user_writable_directory(PJ_CONTEXT *ctx,
                                                   bool create) {
    if (ctx->user_writable_directory.empty()) {
        // For testing purposes only
        const char *env_var_PROJ_USER_WRITABLE_DIRECTORY =
            getenv("PROJ_USER_WRITABLE_DIRECTORY");
        if (env_var_PROJ_USER_WRITABLE_DIRECTORY &&
            env_var_PROJ_USER_WRITABLE_DIRECTORY[0] != '\0') {
            ctx->user_writable_directory = env_var_PROJ_USER_WRITABLE_DIRECTORY;
        }
    }
    if (ctx->user_writable_directory.empty()) {
        std::string path;
#ifdef _WIN32
        std::wstring wPath;
        wPath.resize(MAX_PATH);
        if (SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0,
                             &wPath[0]) == S_OK) {
            wPath.resize(wcslen(wPath.data()));
            path = NS_PROJ::WStringToUTF8(wPath);
        } else {
            const char *local_app_data = getenv("LOCALAPPDATA");
            if (!local_app_data) {
                local_app_data = getenv("TEMP");
                if (!local_app_data) {
                    local_app_data = "c:/users";
                }
            }
            path = local_app_data;
        }
#else
        const char *xdg_data_home = getenv("XDG_DATA_HOME");
        if (xdg_data_home != nullptr) {
            path = xdg_data_home;
        } else {
            const char *home = getenv("HOME");
            if (home) {
#if defined(__MACH__) && defined(__APPLE__)
                path = std::string(home) + "/Library/Logs";
#else
                path = std::string(home) + "/.local/share";
#endif
            } else {
                path = "/tmp";
            }
        }
#endif
        path += "/proj";
        ctx->user_writable_directory = path;
    }
    if (create) {
        CreateDirectoryRecursively(ctx, ctx->user_writable_directory);
    }
    return ctx->user_writable_directory;
}

// ---------------------------------------------------------------------------

std::string pj_context_get_grid_cache_filename(PJ_CONTEXT *ctx) {
    pj_load_ini(ctx);
    if (!ctx->gridChunkCache.filename.empty()) {
        return ctx->gridChunkCache.filename;
    }
    const std::string path(pj_context_get_user_writable_directory(ctx, true));
    ctx->gridChunkCache.filename = path + "/cache.db";
    return ctx->gridChunkCache.filename;
}

// ---------------------------------------------------------------------------

#ifdef WIN32
static const char dir_chars[] = "/\\";
#else
static const char dir_chars[] = "/";
#endif

static bool is_tilde_slash(const char *name) {
    return *name == '~' && strchr(dir_chars, name[1]);
}

static bool is_rel_or_absolute_filename(const char *name) {
    return strchr(dir_chars, *name) ||
           (*name == '.' && strchr(dir_chars, name[1])) ||
           (!strncmp(name, "..", 2) && strchr(dir_chars, name[2])) ||
           (name[0] != '\0' && name[1] == ':' && strchr(dir_chars, name[2]));
}

static std::string build_url(PJ_CONTEXT *ctx, const char *name) {
    if (!is_tilde_slash(name) && !is_rel_or_absolute_filename(name) &&
        !starts_with(name, "http://") && !starts_with(name, "https://")) {
        std::string remote_file(pj_context_get_url_endpoint(ctx));
        if (!remote_file.empty()) {
            if (remote_file.back() != '/') {
                remote_file += '/';
            }
            remote_file += name;
            auto pos = remote_file.rfind('.');
            if (pos + 4 == remote_file.size()) {
                remote_file = remote_file.substr(0, pos) + ".tif";
            } else {
                // For example for resource files like 'alaska'
                remote_file += ".tif";
            }
        }
        return remote_file;
    }
    return name;
}

// ---------------------------------------------------------------------------

#ifdef _WIN32
static const char *get_path_from_win32_projlib(PJ_CONTEXT *ctx,
                                               const char *name,
                                               std::string &out) {
    /* Check if proj.db lieves in a share/proj dir parallel to bin/proj.dll */
    /* Based in
     * https://stackoverflow.com/questions/9112893/how-to-get-path-to-executable-in-c-running-on-windows
     */

    DWORD path_size = 1024;

    std::wstring wout;
    for (;;) {
        wout.clear();
        wout.resize(path_size);
        DWORD result = GetModuleFileNameW(nullptr, &wout[0], path_size - 1);
        DWORD last_error = GetLastError();

        if (result == 0) {
            return nullptr;
        } else if (result == path_size - 1) {
            if (ERROR_INSUFFICIENT_BUFFER != last_error) {
                return nullptr;
            }
            path_size = path_size * 2;
        } else {
            break;
        }
    }
    // Now remove the program's name. It was (example)
    // "C:\programs\gmt6\bin\gdal_translate.exe"
    wout.resize(wcslen(wout.c_str()));
    out = NS_PROJ::WStringToUTF8(wout);
    size_t k = out.size();
    while (k > 0 && out[--k] != '\\') {
    }
    out.resize(k);

    out += "/../share/proj/";
    out += name;

    return NS_PROJ::FileManager::exists(ctx, out.c_str()) ? out.c_str()
                                                          : nullptr;
}
#endif

/************************************************************************/
/*                      pj_open_lib_internal()                          */
/************************************************************************/

#ifdef WIN32
static const char dirSeparator = ';';
#else
static const char dirSeparator = ':';
#endif

static const char *proj_lib_name =
#ifdef PROJ_LIB
    PROJ_LIB;
#else
    nullptr;
#endif

static bool ignoreUserWritableDirectory() {
    // Env var mostly for testing purposes and being independent from
    // an existing installation
    const char *envVarIgnoreUserWritableDirectory =
        getenv("PROJ_IGNORE_USER_WRITABLE_DIRECTORY");
    return envVarIgnoreUserWritableDirectory != nullptr &&
           envVarIgnoreUserWritableDirectory[0] != '\0';
}

static void *
pj_open_lib_internal(projCtx ctx, const char *name, const char *mode,
                     void *(*open_file)(projCtx, const char *, const char *),
                     char *out_full_filename, size_t out_full_filename_size) {
    try {
        std::string fname;
        const char *sysname = nullptr;
        void *fid = nullptr;
        std::string projLib;

        if (ctx == nullptr) {
            ctx = pj_get_default_ctx();
        }

        if (out_full_filename != nullptr && out_full_filename_size > 0)
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

        /* or fixed path: /name, ./name or ../name  */
        else if (is_rel_or_absolute_filename(name)) {
            sysname = name;
#ifdef _WIN32
            try {
                NS_PROJ::UTF8ToWString(name);
            } catch (const std::exception &) {
                fname = NS_PROJ::Win32Recode(name, CP_ACP, CP_UTF8);
                sysname = fname.c_str();
            }
#endif
        }

        else if (starts_with(name, "http://") || starts_with(name, "https://"))
            sysname = name;

        /* or try to use application provided file finder */
        else if (ctx->file_finder != nullptr &&
                 (sysname = ctx->file_finder(
                      ctx, name, ctx->file_finder_user_data)) != nullptr)
            ;

        else if (ctx->file_finder_legacy != nullptr &&
                 (sysname = ctx->file_finder_legacy(name)) != nullptr)
            ;

        /* The user has search paths set */
        else if (!ctx->search_paths.empty()) {
            for (const auto &path : ctx->search_paths) {
                try {
                    fname = path;
                    fname += DIR_CHAR;
                    fname += name;
                    sysname = fname.c_str();
                    fid = open_file(ctx, sysname, mode);
                } catch (const std::exception &) {
                }
                if (fid)
                    break;
            }
        }

        else if (!ignoreUserWritableDirectory() &&
                 (fid = open_file(
                      ctx, (pj_context_get_user_writable_directory(ctx, false) +
                            DIR_CHAR + name)
                               .c_str(),
                      mode)) != nullptr) {
            fname = pj_context_get_user_writable_directory(ctx, false);
            fname += DIR_CHAR;
            fname += name;
            sysname = fname.c_str();
        }

        /* if is environment PROJ_LIB defined */
        else if (!(projLib = NS_PROJ::FileManager::getProjLibEnvVar(ctx))
                      .empty()) {
            auto paths = NS_PROJ::internal::split(projLib, dirSeparator);
            for (const auto &path : paths) {
                fname = path;
                fname += DIR_CHAR;
                fname += name;
                sysname = fname.c_str();
                fid = open_file(ctx, sysname, mode);
                if (fid)
                    break;
            }
#ifdef _WIN32
            /* check if it lives in a ../share/proj dir of the proj dll */
        } else if ((sysname = get_path_from_win32_projlib(ctx, name, fname)) !=
                   nullptr) {
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
        if (fid != nullptr ||
            (fid = open_file(ctx, sysname, mode)) != nullptr) {
            if (out_full_filename != nullptr && out_full_filename_size > 0) {
                // cppcheck-suppress nullPointer
                strncpy(out_full_filename, sysname, out_full_filename_size);
                out_full_filename[out_full_filename_size - 1] = '\0';
            }
            errno = 0;
        }

        if (ctx->last_errno == 0 && errno != 0)
            pj_ctx_set_errno(ctx, errno);

        pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "pj_open_lib(%s): call fopen(%s) - %s",
               name, sysname, fid == nullptr ? "failed" : "succeeded");

        return (fid);
    } catch (const std::exception &) {

        pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "pj_open_lib(%s): out of memory", name);

        return nullptr;
    }
}

/************************************************************************/
/*                  pj_open_file_with_manager()                         */
/************************************************************************/

static void *pj_open_file_with_manager(projCtx ctx, const char *name,
                                       const char * /* mode */) {
    return NS_PROJ::FileManager::open(ctx, name, NS_PROJ::FileAccess::READ_ONLY)
        .release();
}

/************************************************************************/
/*                 FileManager::open_resource_file()                    */
/************************************************************************/

std::unique_ptr<NS_PROJ::File>
NS_PROJ::FileManager::open_resource_file(projCtx ctx, const char *name) {

    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }

    auto file = std::unique_ptr<NS_PROJ::File>(
        reinterpret_cast<NS_PROJ::File *>(pj_open_lib_internal(
            ctx, name, "rb", pj_open_file_with_manager, nullptr, 0)));

    // Retry with a .tif extension if the file name doesn't end with .tif
    if (file == nullptr && !is_tilde_slash(name) &&
        !is_rel_or_absolute_filename(name) && !starts_with(name, "http://") &&
        !starts_with(name, "https://") && strcmp(name, "proj.db") != 0 &&
        strstr(name, ".tif") == nullptr) {
        std::string filename(name);
        auto pos = filename.rfind('.');
        if (pos + 4 == filename.size()) {
            filename = filename.substr(0, pos) + ".tif";
            file.reset(reinterpret_cast<NS_PROJ::File *>(
                pj_open_lib_internal(ctx, filename.c_str(), "rb",
                                     pj_open_file_with_manager, nullptr, 0)));
        } else {
            // For example for resource files like 'alaska'
            filename += ".tif";
            file.reset(reinterpret_cast<NS_PROJ::File *>(
                pj_open_lib_internal(ctx, filename.c_str(), "rb",
                                     pj_open_file_with_manager, nullptr, 0)));
        }
        if (file) {
            pj_ctx_set_errno(ctx, 0);
        }
    }

    if (file == nullptr && !is_tilde_slash(name) &&
        !is_rel_or_absolute_filename(name) && !starts_with(name, "http://") &&
        !starts_with(name, "https://") && pj_context_is_network_enabled(ctx)) {
        std::string remote_file(pj_context_get_url_endpoint(ctx));
        if (!remote_file.empty()) {
            if (remote_file.back() != '/') {
                remote_file += '/';
            }
            remote_file += name;
            auto pos = remote_file.rfind('.');
            if (pos + 4 == remote_file.size()) {
                remote_file = remote_file.substr(0, pos) + ".tif";
                file = open(ctx, remote_file.c_str(),
                            NS_PROJ::FileAccess::READ_ONLY);
                if (file) {
                    pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "Using %s",
                           remote_file.c_str());
                    pj_ctx_set_errno(ctx, 0);
                }
            } else {
                // For example for resource files like 'alaska'
                auto remote_file_tif = remote_file + ".tif";
                file = open(ctx, remote_file_tif.c_str(),
                            NS_PROJ::FileAccess::READ_ONLY);
                if (file) {
                    pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "Using %s",
                           remote_file_tif.c_str());
                    pj_ctx_set_errno(ctx, 0);
                } else {
                    // Init files
                    file = open(ctx, remote_file.c_str(),
                                NS_PROJ::FileAccess::READ_ONLY);
                    if (file) {
                        pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "Using %s",
                               remote_file.c_str());
                        pj_ctx_set_errno(ctx, 0);
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
static void *pj_ctx_fopen_adapter(projCtx ctx, const char *name,
                                  const char *mode) {
    return pj_ctx_fopen(ctx, name, mode);
}

// Legacy function
PAFile pj_open_lib(projCtx ctx, const char *name, const char *mode) {
    return (PAFile)pj_open_lib_internal(ctx, name, mode, pj_ctx_fopen_adapter,
                                        nullptr, 0);
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
                 char *out_full_filename, size_t out_full_filename_size) {
    auto f = reinterpret_cast<NS_PROJ::File *>(pj_open_lib_internal(
        ctx, short_filename, "rb", pj_open_file_with_manager, out_full_filename,
        out_full_filename_size));
    if (f != nullptr) {
        delete f;
        return 1;
    }
    return 0;
}

/************************************************************************/
/*                    pj_context_get_url_endpoint()                     */
/************************************************************************/

std::string pj_context_get_url_endpoint(PJ_CONTEXT *ctx) {
    if (!ctx->endpoint.empty()) {
        return ctx->endpoint;
    }
    pj_load_ini(ctx);
    return ctx->endpoint;
}

/************************************************************************/
/*                              trim()                                  */
/************************************************************************/

static std::string trim(const std::string &s) {
    const auto first = s.find_first_not_of(' ');
    const auto last = s.find_last_not_of(' ');
    if (first == std::string::npos || last == std::string::npos) {
        return std::string();
    }
    return s.substr(first, last - first + 1);
}

/************************************************************************/
/*                            pj_load_ini()                             */
/************************************************************************/

void pj_load_ini(projCtx ctx) {
    if (ctx->iniFileLoaded)
        return;

    const char *endpoint_from_env = getenv("PROJ_NETWORK_ENDPOINT");
    if (endpoint_from_env && endpoint_from_env[0] != '\0') {
        ctx->endpoint = endpoint_from_env;
    }

    ctx->iniFileLoaded = true;
    auto file = std::unique_ptr<NS_PROJ::File>(
        reinterpret_cast<NS_PROJ::File *>(pj_open_lib_internal(
            ctx, "proj.ini", "rb", pj_open_file_with_manager, nullptr, 0)));
    if (!file)
        return;
    file->seek(0, SEEK_END);
    const auto filesize = file->tell();
    if (filesize == 0 || filesize > 100 * 1024U)
        return;
    file->seek(0, SEEK_SET);
    std::string content;
    content.resize(static_cast<size_t>(filesize));
    const auto nread = file->read(&content[0], content.size());
    if (nread != content.size())
        return;
    content += '\n';
    size_t pos = 0;
    while (pos != std::string::npos) {
        const auto eol = content.find_first_of("\r\n", pos);
        if (eol == std::string::npos) {
            break;
        }

        const auto equal = content.find('=', pos);
        if (equal < eol) {
            const auto key = trim(content.substr(pos, equal - pos));
            const auto value =
                trim(content.substr(equal + 1, eol - (equal + 1)));
            if (ctx->endpoint.empty() && key == "cdn_endpoint") {
                ctx->endpoint = value;
            } else if (key == "network") {
                const char *enabled = getenv("PROJ_NETWORK");
                if (enabled == nullptr || enabled[0] == '\0') {
                    ctx->networking.enabled = ci_equal(value, "ON") ||
                                              ci_equal(value, "YES") ||
                                              ci_equal(value, "TRUE");
                }
            } else if (key == "cache_enabled") {
                ctx->gridChunkCache.enabled = ci_equal(value, "ON") ||
                                              ci_equal(value, "YES") ||
                                              ci_equal(value, "TRUE");
            } else if (key == "cache_size_MB") {
                const int val = atoi(value.c_str());
                ctx->gridChunkCache.max_size =
                    val > 0 ? static_cast<long long>(val) * 1024 * 1024 : -1;
            } else if (key == "cache_ttl_sec") {
                ctx->gridChunkCache.ttl = atoi(value.c_str());
            }
        }

        pos = content.find_first_not_of("\r\n", eol);
    }
}

//! @endcond

// ---------------------------------------------------------------------------

/** Return if a file must be downloaded or is already available in the
 * PROJ user-writable directory.
 *
 * The file will be determinted to have to be downloaded if it does not exist
 * yet in the user-writable directory, or if it is determined that a more recent
 * version exists. To determine if a more recent version exists, PROJ will
 * use the "downloaded_file_properties" table of its grid cache database.
 * Consequently files manually placed in the user-writable
 * directory without using this function would be considered as
 * non-existing/obsolete and would be unconditionnaly downloaded again.
 *
 * This function can only be used if networking is enabled, and either
 * the default curl network API or a custom one have been installed.
 *
 * @param ctx PROJ context, or NULL
 * @param url_or_filename URL or filename (without directory component)
 * @param ignore_ttl_setting If set to FALSE, PROJ will only check the
 *                           recentness of an already downloaded file, if
 *                           the delay between the last time it has been
 *                           verified and the current time exceeds the TTL
 *                           setting. This can save network accesses.
 *                           If set to TRUE, PROJ will unconditionnally
 *                           check from the server the recentness of the file.
 * @return TRUE if the file must be downloaded with proj_download_file()
 * @since 7.0
 */

int proj_is_download_needed(PJ_CONTEXT *ctx, const char *url_or_filename,
                            int ignore_ttl_setting) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    if (!pj_context_is_network_enabled(ctx)) {
        pj_log(ctx, PJ_LOG_ERROR, "Networking capabilities are not enabled");
        return false;
    }

    const auto url(build_url(ctx, url_or_filename));
    const char *filename = strrchr(url.c_str(), '/');
    if (filename == nullptr)
        return false;
    const auto localFilename(
        pj_context_get_user_writable_directory(ctx, false) + filename);

    auto f = NS_PROJ::FileManager::open(ctx, localFilename.c_str(),
                                        NS_PROJ::FileAccess::READ_ONLY);
    if (!f) {
        return true;
    }
    f.reset();

    auto diskCache = NS_PROJ::DiskChunkCache::open(ctx);
    if (!diskCache)
        return false;
    auto stmt =
        diskCache->prepare("SELECT lastChecked, fileSize, lastModified, etag "
                           "FROM downloaded_file_properties WHERE url = ?");
    if (!stmt)
        return true;
    stmt->bindText(url.c_str());
    if (stmt->execute() != SQLITE_ROW) {
        return true;
    }

    NS_PROJ::FileProperties cachedProps;
    cachedProps.lastChecked = stmt->getInt64();
    cachedProps.size = stmt->getInt64();
    const char *lastModified = stmt->getText();
    cachedProps.lastModified = lastModified ? lastModified : std::string();
    const char *etag = stmt->getText();
    cachedProps.etag = etag ? etag : std::string();

    if (!ignore_ttl_setting) {
        const auto ttl = NS_PROJ::pj_context_get_grid_cache_ttl(ctx);
        if (ttl > 0) {
            time_t curTime;
            time(&curTime);
            if (curTime > cachedProps.lastChecked + ttl) {

                unsigned char dummy;
                size_t size_read = 0;
                std::string errorBuffer;
                errorBuffer.resize(1024);
                auto handle = ctx->networking.open(
                    ctx, url.c_str(), 0, 1, &dummy, &size_read,
                    errorBuffer.size(), &errorBuffer[0],
                    ctx->networking.user_data);
                if (!handle) {
                    errorBuffer.resize(strlen(errorBuffer.data()));
                    pj_log(ctx, PJ_LOG_ERROR, "Cannot open %s: %s", url.c_str(),
                           errorBuffer.c_str());
                    return false;
                }
                NS_PROJ::FileProperties props;
                if (!NS_PROJ::NetworkFile::get_props_from_headers(ctx, handle,
                                                                  props)) {
                    ctx->networking.close(ctx, handle,
                                          ctx->networking.user_data);
                    return false;
                }
                ctx->networking.close(ctx, handle, ctx->networking.user_data);

                if (props.size != cachedProps.size ||
                    props.lastModified != cachedProps.lastModified ||
                    props.etag != cachedProps.etag) {
                    return true;
                }

                stmt = diskCache->prepare(
                    "UPDATE downloaded_file_properties SET lastChecked = ? "
                    "WHERE url = ?");
                if (!stmt)
                    return false;
                stmt->bindInt64(curTime);
                stmt->bindText(url.c_str());
                if (stmt->execute() != SQLITE_DONE) {
                    auto hDB = diskCache->handle();
                    pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
                    return false;
                }
            }
        }
    }

    return false;
}

// ---------------------------------------------------------------------------

/** Download a file in the PROJ user-writable directory.
 *
 * The file will only be downloaded if it does not exist yet in the
 * user-writable directory, or if it is determined that a more recent
 * version exists. To determine if a more recent version exists, PROJ will
 * use the "downloaded_file_properties" table of its grid cache database.
 * Consequently files manually placed in the user-writable
 * directory without using this function would be considered as
 * non-existing/obsolete and would be unconditionnaly downloaded again.
 *
 * This function can only be used if networking is enabled, and either
 * the default curl network API or a custom one have been installed.
 *
 * @param ctx PROJ context, or NULL
 * @param url_or_filename URL or filename (without directory component)
 * @param ignore_ttl_setting If set to FALSE, PROJ will only check the
 *                           recentness of an already downloaded file, if
 *                           the delay between the last time it has been
 *                           verified and the current time exceeds the TTL
 *                           setting. This can save network accesses.
 *                           If set to TRUE, PROJ will unconditionnally
 *                           check from the server the recentness of the file.
 * @param progress_cbk Progress callback, or NULL.
 *                     The passed percentage is in the [0, 1] range.
 *                     The progress callback must return TRUE
 *                     if download must be continued.
 * @param user_data User data to provide to the progress callback, or NULL
 * @return TRUE if the download was successful (or not needed)
 * @since 7.0
 */

int proj_download_file(PJ_CONTEXT *ctx, const char *url_or_filename,
                       int ignore_ttl_setting,
                       int (*progress_cbk)(PJ_CONTEXT *, double pct,
                                           void *user_data),
                       void *user_data) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    if (!pj_context_is_network_enabled(ctx)) {
        pj_log(ctx, PJ_LOG_ERROR, "Networking capabilities are not enabled");
        return false;
    }
    if (!proj_is_download_needed(ctx, url_or_filename, ignore_ttl_setting)) {
        return true;
    }

    const auto url(build_url(ctx, url_or_filename));
    const char *filename = strrchr(url.c_str(), '/');
    if (filename == nullptr)
        return false;
    const auto localFilename(pj_context_get_user_writable_directory(ctx, true) +
                             filename);

#ifdef _WIN32
    const int nPID = GetCurrentProcessId();
#else
    const int nPID = getpid();
#endif
    char szUniqueSuffix[128];
    snprintf(szUniqueSuffix, sizeof(szUniqueSuffix), "%d_%p", nPID, &url);
    const auto localFilenameTmp(localFilename + szUniqueSuffix);
    auto f = NS_PROJ::FileManager::open(ctx, localFilenameTmp.c_str(),
                                        NS_PROJ::FileAccess::CREATE);
    if (!f) {
        pj_log(ctx, PJ_LOG_ERROR, "Cannot create %s", localFilenameTmp.c_str());
        return false;
    }

    constexpr size_t FULL_FILE_CHUNK_SIZE = 1024 * 1024;
    std::vector<unsigned char> buffer(FULL_FILE_CHUNK_SIZE);
    // For testing purposes only
    const char *env_var_PROJ_FULL_FILE_CHUNK_SIZE =
        getenv("PROJ_FULL_FILE_CHUNK_SIZE");
    if (env_var_PROJ_FULL_FILE_CHUNK_SIZE &&
        env_var_PROJ_FULL_FILE_CHUNK_SIZE[0] != '\0') {
        buffer.resize(atoi(env_var_PROJ_FULL_FILE_CHUNK_SIZE));
    }
    size_t size_read = 0;
    std::string errorBuffer;
    errorBuffer.resize(1024);
    auto handle = ctx->networking.open(
        ctx, url.c_str(), 0, buffer.size(), &buffer[0], &size_read,
        errorBuffer.size(), &errorBuffer[0], ctx->networking.user_data);
    if (!handle) {
        errorBuffer.resize(strlen(errorBuffer.data()));
        pj_log(ctx, PJ_LOG_ERROR, "Cannot open %s: %s", url.c_str(),
               errorBuffer.c_str());
        f.reset();
        NS_PROJ::FileManager::unlink(ctx, localFilenameTmp.c_str());
        return false;
    }

    time_t curTime;
    time(&curTime);
    NS_PROJ::FileProperties props;
    if (!NS_PROJ::NetworkFile::get_props_from_headers(ctx, handle, props)) {
        ctx->networking.close(ctx, handle, ctx->networking.user_data);
        f.reset();
        NS_PROJ::FileManager::unlink(ctx, localFilenameTmp.c_str());
        return false;
    }

    if (size_read <
        std::min(static_cast<unsigned long long>(buffer.size()), props.size)) {
        pj_log(ctx, PJ_LOG_ERROR, "Did not get as many bytes as expected");
        ctx->networking.close(ctx, handle, ctx->networking.user_data);
        f.reset();
        NS_PROJ::FileManager::unlink(ctx, localFilenameTmp.c_str());
        return false;
    }
    if (f->write(buffer.data(), size_read) != size_read) {
        pj_log(ctx, PJ_LOG_ERROR, "Write error");
        ctx->networking.close(ctx, handle, ctx->networking.user_data);
        f.reset();
        NS_PROJ::FileManager::unlink(ctx, localFilenameTmp.c_str());
        return false;
    }

    unsigned long long totalDownloaded = size_read;
    while (totalDownloaded < props.size) {
        if (totalDownloaded + buffer.size() > props.size) {
            buffer.resize(static_cast<size_t>(props.size - totalDownloaded));
        }
        errorBuffer.resize(1024);
        size_read = ctx->networking.read_range(
            ctx, handle, totalDownloaded, buffer.size(), &buffer[0],
            errorBuffer.size(), &errorBuffer[0], ctx->networking.user_data);

        if (size_read < buffer.size()) {
            pj_log(ctx, PJ_LOG_ERROR, "Did not get as many bytes as expected");
            ctx->networking.close(ctx, handle, ctx->networking.user_data);
            f.reset();
            NS_PROJ::FileManager::unlink(ctx, localFilenameTmp.c_str());
            return false;
        }
        if (f->write(buffer.data(), size_read) != size_read) {
            pj_log(ctx, PJ_LOG_ERROR, "Write error");
            ctx->networking.close(ctx, handle, ctx->networking.user_data);
            f.reset();
            NS_PROJ::FileManager::unlink(ctx, localFilenameTmp.c_str());
            return false;
        }

        totalDownloaded += size_read;
        if (progress_cbk &&
            !progress_cbk(ctx, double(totalDownloaded) / props.size,
                          user_data)) {
            ctx->networking.close(ctx, handle, ctx->networking.user_data);
            f.reset();
            NS_PROJ::FileManager::unlink(ctx, localFilenameTmp.c_str());
            return false;
        }
    }

    ctx->networking.close(ctx, handle, ctx->networking.user_data);
    f.reset();
    NS_PROJ::FileManager::unlink(ctx, localFilename.c_str());
    if (!NS_PROJ::FileManager::rename(ctx, localFilenameTmp.c_str(),
                                      localFilename.c_str())) {
        pj_log(ctx, PJ_LOG_ERROR, "Cannot rename %s to %s",
               localFilenameTmp.c_str(), localFilename.c_str());
        return false;
    }

    auto diskCache = NS_PROJ::DiskChunkCache::open(ctx);
    if (!diskCache)
        return false;
    auto stmt =
        diskCache->prepare("SELECT lastChecked, fileSize, lastModified, etag "
                           "FROM downloaded_file_properties WHERE url = ?");
    if (!stmt)
        return false;
    stmt->bindText(url.c_str());

    props.lastChecked = curTime;
    auto hDB = diskCache->handle();

    if (stmt->execute() == SQLITE_ROW) {
        stmt = diskCache->prepare(
            "UPDATE downloaded_file_properties SET lastChecked = ?, "
            "fileSize = ?, lastModified = ?, etag = ? "
            "WHERE url = ?");
        if (!stmt)
            return false;
        stmt->bindInt64(props.lastChecked);
        stmt->bindInt64(props.size);
        if (props.lastModified.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.lastModified.c_str());
        if (props.etag.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.etag.c_str());
        stmt->bindText(url.c_str());
        if (stmt->execute() != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return false;
        }
    } else {
        stmt = diskCache->prepare(
            "INSERT INTO downloaded_file_properties (url, lastChecked, "
            "fileSize, lastModified, etag) VALUES "
            "(?,?,?,?,?)");
        if (!stmt)
            return false;
        stmt->bindText(url.c_str());
        stmt->bindInt64(props.lastChecked);
        stmt->bindInt64(props.size);
        if (props.lastModified.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.lastModified.c_str());
        if (props.etag.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.etag.c_str());
        if (stmt->execute() != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return false;
        }
    }
    return true;
}

/************************************************************************/
/*                           pj_set_finder()                            */
/************************************************************************/

void pj_set_finder(const char *(*new_finder)(const char *))

{
    auto ctx = pj_get_default_ctx();
    if (ctx) {
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
                                  void *user_data) {
    if (!ctx)
        ctx = pj_get_default_ctx();
    if (!ctx)
        return;
    ctx->file_finder = finder;
    ctx->file_finder_user_data = user_data;
}

/************************************************************************/
/*                  proj_context_set_search_paths()                     */
/************************************************************************/

/** \brief Sets search paths.
 *
 * Those search paths will be used whenever PROJ must open one of its resource
 * files
 * (proj.db database, grids, etc...)
 *
 * If set on the default context, they will be inherited by contexts created
 * later.
 *
 * Starting with PROJ 7.0, the path(s) should be encoded in UTF-8.
 *
 * @param ctx PROJ context, or NULL for the default context.
 * @param count_paths Number of paths. 0 if paths == NULL.
 * @param paths Paths. May be NULL.
 *
 * @since PROJ 6.0
 */
void proj_context_set_search_paths(PJ_CONTEXT *ctx, int count_paths,
                                   const char *const *paths) {
    if (!ctx)
        ctx = pj_get_default_ctx();
    if (!ctx)
        return;
    try {
        std::vector<std::string> vector_of_paths;
        for (int i = 0; i < count_paths; i++) {
            vector_of_paths.emplace_back(paths[i]);
        }
        ctx->set_search_paths(vector_of_paths);
    } catch (const std::exception &) {
    }
}

/************************************************************************/
/*                         pj_set_searchpath()                          */
/*                                                                      */
/*      Path control for callers that can't practically provide         */
/*      pj_set_finder() style callbacks.  Call with (0,NULL) as args    */
/*      to clear the searchpath set.                                    */
/************************************************************************/

void pj_set_searchpath(int count, const char **path) {
    proj_context_set_search_paths(nullptr, count,
                                  const_cast<const char *const *>(path));
}
