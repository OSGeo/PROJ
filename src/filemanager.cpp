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

#include <errno.h>
#include <stdlib.h>

#include <algorithm>
#include <limits>
#include <string>

#include "filemanager.hpp"
#include "proj.h"
#include "proj/internal/internal.hpp"
#include "proj/internal/io_internal.hpp"
#include "proj/io.hpp"
#include "proj_internal.h"

#include <sys/stat.h>

#include "proj_config.h"

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#ifdef HAVE_LIBDL
#include <dlfcn.h>
#endif
#include <sys/types.h>
#include <unistd.h>
#endif

//! @cond Doxygen_Suppress

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

using namespace NS_PROJ::internal;

NS_PROJ_START

// ---------------------------------------------------------------------------

File::File(const std::string &name) : name_(name) {}

// ---------------------------------------------------------------------------

File::~File() = default;

// ---------------------------------------------------------------------------

std::string File::read_line(size_t maxLen, bool &maxLenReached,
                            bool &eofReached) {
    constexpr size_t MAX_MAXLEN = 1024 * 1024;
    maxLen = std::min(maxLen, MAX_MAXLEN);
    while (true) {
        // Consume existing lines in buffer
        size_t pos = readLineBuffer_.find_first_of("\r\n");
        if (pos != std::string::npos) {
            if (pos > maxLen) {
                std::string ret(readLineBuffer_.substr(0, maxLen));
                readLineBuffer_ = readLineBuffer_.substr(maxLen);
                maxLenReached = true;
                eofReached = false;
                return ret;
            }
            std::string ret(readLineBuffer_.substr(0, pos));
            if (readLineBuffer_[pos] == '\r' &&
                readLineBuffer_[pos + 1] == '\n') {
                pos += 1;
            }
            readLineBuffer_ = readLineBuffer_.substr(pos + 1);
            maxLenReached = false;
            eofReached = false;
            return ret;
        }

        const size_t prevSize = readLineBuffer_.size();
        if (maxLen <= prevSize) {
            std::string ret(readLineBuffer_.substr(0, maxLen));
            readLineBuffer_ = readLineBuffer_.substr(maxLen);
            maxLenReached = true;
            eofReached = false;
            return ret;
        }

        if (eofReadLine_) {
            std::string ret = readLineBuffer_;
            readLineBuffer_.clear();
            maxLenReached = false;
            eofReached = ret.empty();
            return ret;
        }

        readLineBuffer_.resize(maxLen);
        const size_t nRead =
            read(&readLineBuffer_[prevSize], maxLen - prevSize);
        if (nRead < maxLen - prevSize)
            eofReadLine_ = true;
        readLineBuffer_.resize(prevSize + nRead);
    }
}

// ---------------------------------------------------------------------------

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

std::unique_ptr<File> FileManager::open(PJ_CONTEXT *ctx, const char *filename,
                                        FileAccess access) {
    if (starts_with(filename, "http://") || starts_with(filename, "https://")) {
        if (!proj_context_is_network_enabled(ctx)) {
            pj_log(
                ctx, PJ_LOG_ERROR,
                "Attempt at accessing remote resource not authorized. Either "
                "set PROJ_NETWORK=ON or "
                "proj_context_set_enable_network(ctx, TRUE)");
            return nullptr;
        }
        return pj_network_file_open(ctx, filename);
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
 * @since 7.0
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
 * @since 7.0
 */
void proj_context_set_sqlite3_vfs_name(PJ_CONTEXT *ctx, const char *name) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    ctx->custom_sqlite3_vfs_name = name ? name : std::string();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

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
    if (!ctx)
        ctx = pj_get_default_ctx();
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
            if (home && access(home, W_OK) == 0) {
#if defined(__MACH__) && defined(__APPLE__)
                path = std::string(home) + "/Library/Application Support";
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

void pj_context_set_user_writable_directory(PJ_CONTEXT *ctx,
                                            const std::string &path) {
    if (!ctx)
        ctx = pj_get_default_ctx();
    ctx->user_writable_directory = path;
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

// ---------------------------------------------------------------------------

static std::string pj_get_relative_share_proj_internal_no_check() {
#if defined(_WIN32) || defined(HAVE_LIBDL)
#ifdef _WIN32
    HMODULE hm = NULL;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCSTR)&pj_get_relative_share_proj, &hm) == 0) {
        return std::string();
    }

    DWORD path_size = 1024;

    std::wstring wout;
    for (;;) {
        wout.clear();
        wout.resize(path_size);
        DWORD result = GetModuleFileNameW(hm, &wout[0], path_size - 1);
        DWORD last_error = GetLastError();

        if (result == 0) {
            return std::string();
        } else if (result == path_size - 1) {
            if (ERROR_INSUFFICIENT_BUFFER != last_error) {
                return std::string();
            }
            path_size = path_size * 2;
        } else {
            break;
        }
    }
    wout.resize(wcslen(wout.c_str()));
    std::string out = NS_PROJ::WStringToUTF8(wout);
    constexpr char dir_sep = '\\';
#else
    Dl_info info;
    if (!dladdr((const void *)pj_get_relative_share_proj, &info)) {
        return std::string();
    }
    std::string out(info.dli_fname);
    constexpr char dir_sep = '/';
    // "optimization" for cmake builds where RUNPATH is set to ${prefix}/lib
    out = replaceAll(out, "/bin/../", "/");
#ifdef __linux
    // If we get a filename without any path, this is most likely a static
    // binary. Resolve the executable name
    if (out.find(dir_sep) == std::string::npos) {
        constexpr size_t BUFFER_SIZE = 1024;
        std::vector<char> path(BUFFER_SIZE + 1);
        ssize_t nResultLen = readlink("/proc/self/exe", &path[0], BUFFER_SIZE);
        if (nResultLen >= 0 && static_cast<size_t>(nResultLen) < BUFFER_SIZE) {
            out.assign(path.data(), static_cast<size_t>(nResultLen));
        }
    }
#endif
    if (starts_with(out, "./"))
        out = out.substr(2);
#endif
    auto pos = out.find_last_of(dir_sep);
    if (pos == std::string::npos) {
        // The initial path was something like libproj.so"
        out = "../share/proj";
        return out;
    }
    out.resize(pos);
    pos = out.find_last_of(dir_sep);
    if (pos == std::string::npos) {
        // The initial path was something like bin/libproj.so"
        out = "share/proj";
        return out;
    }
    out.resize(pos);
    // The initial path was something like foo/bin/libproj.so"
    out += "/share/proj";
    return out;
#else
    return std::string();
#endif
}

static std::string
pj_get_relative_share_proj_internal_check_exists(PJ_CONTEXT *ctx) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    std::string path(pj_get_relative_share_proj_internal_no_check());
    if (!path.empty() && NS_PROJ::FileManager::exists(ctx, path.c_str())) {
        return path;
    }
    return std::string();
}

std::string pj_get_relative_share_proj(PJ_CONTEXT *ctx) {
    static std::string path(
        pj_get_relative_share_proj_internal_check_exists(ctx));
    return path;
}

// ---------------------------------------------------------------------------

static const char *get_path_from_relative_share_proj(PJ_CONTEXT *ctx,
                                                     const char *name,
                                                     std::string &out) {
    out = pj_get_relative_share_proj(ctx);
    if (out.empty()) {
        return nullptr;
    }
    out += '/';
    out += name;

    return NS_PROJ::FileManager::exists(ctx, out.c_str()) ? out.c_str()
                                                          : nullptr;
}

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

static bool dontReadUserWritableDirectory() {
    // Env var mostly for testing purposes and being independent from
    // an existing installation
    const char *envVar = getenv("PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY");
    return envVar != nullptr && envVar[0] != '\0';
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

        else if (!dontReadUserWritableDirectory() &&
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
            /* check if it lives in a ../share/proj dir of the proj dll */
        } else if ((sysname = get_path_from_relative_share_proj(
                        ctx, name, fname)) != nullptr) {
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
/*                  pj_get_default_searchpaths()                        */
/************************************************************************/

std::vector<std::string> pj_get_default_searchpaths(PJ_CONTEXT *ctx) {
    std::vector<std::string> ret;

    // Env var mostly for testing purposes and being independent from
    // an existing installation
    const char *ignoreUserWritableDirectory =
        getenv("PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY");
    if (ignoreUserWritableDirectory == nullptr ||
        ignoreUserWritableDirectory[0] == '\0') {
        ret.push_back(pj_context_get_user_writable_directory(ctx, false));
    }
    const std::string envPROJ_LIB = NS_PROJ::FileManager::getProjLibEnvVar(ctx);
    if (!envPROJ_LIB.empty()) {
        ret.push_back(envPROJ_LIB);
    }
    if (envPROJ_LIB.empty()) {
        const std::string relativeSharedProj = pj_get_relative_share_proj(ctx);
        if (!relativeSharedProj.empty()) {
            ret.push_back(relativeSharedProj);
        }
    }
#ifdef PROJ_LIB
    if (envPROJ_LIB.empty()) {
        ret.push_back(PROJ_LIB);
    }
#endif
    return ret;
}

/************************************************************************/
/*                  pj_open_file_with_manager()                         */
/************************************************************************/

static void *pj_open_file_with_manager(projCtx ctx, const char *name,
                                       const char * /* mode */) {
    return NS_PROJ::FileManager::open(ctx, name, NS_PROJ::FileAccess::READ_ONLY)
        .release();
}

// ---------------------------------------------------------------------------

static NS_PROJ::io::DatabaseContextPtr getDBcontext(PJ_CONTEXT *ctx) {
    try {
        if (ctx->cpp_context == nullptr) {
            ctx->cpp_context = new projCppContext(ctx);
        }
        return ctx->cpp_context->getDatabaseContext().as_nullable();
    } catch (const std::exception &e) {
        pj_log(ctx, PJ_LOG_DEBUG, "%s", e.what());
        return nullptr;
    }
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

    // Retry with the new proj grid name if the file name doesn't end with .tif
    std::string tmpString; // keep it in this upper scope !
    if (file == nullptr && !is_tilde_slash(name) &&
        !is_rel_or_absolute_filename(name) && !starts_with(name, "http://") &&
        !starts_with(name, "https://") && strcmp(name, "proj.db") != 0 &&
        strstr(name, ".tif") == nullptr) {

        auto dbContext = getDBcontext(ctx);
        if (dbContext) {
            try {
                auto filename = dbContext->getProjGridName(name);
                if (!filename.empty()) {
                    file.reset(reinterpret_cast<NS_PROJ::File *>(
                        pj_open_lib_internal(ctx, filename.c_str(), "rb",
                                             pj_open_file_with_manager, nullptr,
                                             0)));
                    if (file) {
                        pj_ctx_set_errno(ctx, 0);
                    } else {
                        // For final network access attempt, use the new
                        // name.
                        tmpString = filename;
                        name = tmpString.c_str();
                    }
                }
            } catch (const std::exception &e) {
                pj_log(ctx, PJ_LOG_DEBUG, "%s", e.what());
                return nullptr;
            }
        }
    }
    // Retry with the old proj grid name if the file name ends with .tif
    else if (file == nullptr && !is_tilde_slash(name) &&
             !is_rel_or_absolute_filename(name) &&
             !starts_with(name, "http://") && !starts_with(name, "https://") &&
             strstr(name, ".tif") != nullptr) {

        auto dbContext = getDBcontext(ctx);
        if (dbContext) {
            try {
                auto filename = dbContext->getOldProjGridName(name);
                if (!filename.empty()) {
                    file.reset(reinterpret_cast<NS_PROJ::File *>(
                        pj_open_lib_internal(ctx, filename.c_str(), "rb",
                                             pj_open_file_with_manager, nullptr,
                                             0)));
                    if (file) {
                        pj_ctx_set_errno(ctx, 0);
                    }
                }
            } catch (const std::exception &e) {
                pj_log(ctx, PJ_LOG_DEBUG, "%s", e.what());
                return nullptr;
            }
        }
    }

    if (file == nullptr && !is_tilde_slash(name) &&
        !is_rel_or_absolute_filename(name) && !starts_with(name, "http://") &&
        !starts_with(name, "https://") &&
        proj_context_is_network_enabled(ctx)) {
        std::string remote_file(pj_context_get_url_endpoint(ctx));
        if (!remote_file.empty()) {
            if (remote_file.back() != '/') {
                remote_file += '/';
            }
            remote_file += name;
            file =
                open(ctx, remote_file.c_str(), NS_PROJ::FileAccess::READ_ONLY);
            if (file) {
                pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "Using %s",
                       remote_file.c_str());
                pj_ctx_set_errno(ctx, 0);
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
 * @param short_filename short filename (e.g. us_nga_egm96_15.tif).
 *                       Must not be NULL.
 * @param out_full_filename output buffer, of size out_full_filename_size, that
 *                          will receive the full filename on success.
 *                          Will be zero-terminated.
 * @param out_full_filename_size size of out_full_filename.
 * @return 1 if the file was found, 0 otherwise.
 */
int pj_find_file(projCtx ctx, const char *short_filename,
                 char *out_full_filename, size_t out_full_filename_size) {
    auto file = std::unique_ptr<NS_PROJ::File>(
        reinterpret_cast<NS_PROJ::File *>(pj_open_lib_internal(
            ctx, short_filename, "rb", pj_open_file_with_manager,
            out_full_filename, out_full_filename_size)));

    // Retry with the old proj grid name if the file name ends with .tif
    if (file == nullptr && strstr(short_filename, ".tif") != nullptr) {

        auto dbContext = getDBcontext(ctx);
        if (dbContext) {
            try {
                auto filename = dbContext->getOldProjGridName(short_filename);
                if (!filename.empty()) {
                    file.reset(reinterpret_cast<NS_PROJ::File *>(
                        pj_open_lib_internal(ctx, filename.c_str(), "rb",
                                             pj_open_file_with_manager,
                                             out_full_filename,
                                             out_full_filename_size)));
                }
            } catch (const std::exception &e) {
                pj_log(ctx, PJ_LOG_DEBUG, "%s", e.what());
                return false;
            }
        }
    }

    return file != nullptr;
}

/************************************************************************/
/*                    pj_context_get_url_endpoint()                     */
/************************************************************************/

std::string pj_context_get_url_endpoint(PJ_CONTEXT *ctx) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
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
