#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Cmd/CmdString.hpp>
#include <Fw/Cmd/IrisCmdByteString.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Fw
{
    IrisCmdByteStringArg::IrisCmdByteStringArg(const char *src) : CmdStringArg(src)
    {
    }

    IrisCmdByteStringArg::IrisCmdByteStringArg(const StringBase &src) : CmdStringArg(src)
    {
    }

    IrisCmdByteStringArg::IrisCmdByteStringArg(const CmdStringArg &src) : CmdStringArg(src)
    {
    }

    IrisCmdByteStringArg::IrisCmdByteStringArg(const IrisCmdByteStringArg &src) : CmdStringArg()
    {
        this->copyBuff(src.m_buf, this->getCapacity());
    }

    IrisCmdByteStringArg::IrisCmdByteStringArg(void) : CmdStringArg()
    {
        this->m_buf[0] = 0;
        this->actual_size = 0;
    }

    IrisCmdByteStringArg::~IrisCmdByteStringArg(void)
    {
        // Nothing to do.
    }

    NATIVE_UINT_TYPE IrisCmdByteStringArg::getCapacity(void) const
    {
        return FW_CMD_STRING_MAX_SIZE;
    }

    void IrisCmdByteStringArg::setActualSize(NATIVE_UINT_TYPE s)
    {
        if (s >= getCapacity())
        {
            // All the available bytes will be used AND the LAST byte in the
            // array will be the terminator, so the number of actual data bytes
            // will be -1:
            this->actual_size = getCapacity() - 1;
        }
        else
        {
            this->actual_size = s;
        }
    }
    NATIVE_UINT_TYPE IrisCmdByteStringArg::getActualSize(void)
    {
        // Make sure actual_size wasn't corrupted somehow and won't cause an
        // out-of-bounds issue:
        return actual_size < (getCapacity() - 1) ? actual_size : (getCapacity() - 1);
    }

    NATIVE_UINT_TYPE IrisCmdByteStringArg::length(void) const
    {
        return getActualSize();
    }

    const char *IrisCmdByteStringArg::toChar(void) const
    {
        return this->m_buf;
    }

    void IrisCmdByteStringArg::copyBuff(const char *buff, NATIVE_UINT_TYPE size)
    {
        FW_ASSERT(buff);
        // check for self copy
        if (buff != this->m_buf)
        {
            // Cap size:
            size = size < this->getCapacity() ? size : this->getCapacity();
            (void)memcpy(this->m_buf, buff, size);
            // NULL terminate (impl. sets the actual_size):
            this->terminate(size);
        }
    }

    SerializeStatus IrisCmdByteStringArg::serialize(SerializeBufferBase &buffer) const
    {
        return buffer.serialize((U8 *)this->m_buf, this->length());
    }

    SerializeStatus IrisCmdByteStringArg::deserialize(SerializeBufferBase &buffer)
    {
        // Default size to the max size:
        NATIVE_UINT_TYPE size = sizeof(this->m_buf);
        // deserialize string
        SerializeStatus stat = buffer.deserialize((U8 *)this->m_buf, size);
        // NOTE: `size` is passed by reference. After running deserialize
        // above it now contains the ACTUAL stored length that precedes the
        // string bytes in the serialization.
        // make sure it is null-terminated, which sets the actual_size:
        this->terminate(size);

        return stat;
    }

    void IrisCmdByteStringArg::terminate(NATIVE_UINT_TYPE size)
    {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf) ? size : sizeof(this->m_buf) - 1] = 0;
        // Adjust sizing accordingly:
        this->setActualSize(size);
    }

    const char *IrisCmdByteStringArg::operator+=(const char *src)
    {
        // Designed to work like / be compatible with StringBase::operator+= so this still assumes the incoming src is null terminated.
        this->appendBuff(src, strlen(src));
        return this->toChar();
    }

    const StringBase &IrisCmdByteStringArg::operator+=(const StringBase &src)
    {
        this->appendBuff(src.toChar(), src.length());
        return *this;
    }

    bool IrisCmdByteStringArg::operator==(const StringBase &other) const
    {
        NATIVE_UINT_TYPE len = this->length();
        if (len != other.length())
        {
            return false;
        }
        else
        {
            const char *ourData = this->toChar();
            const char *otherData = other->toChar();
            if ((ourData == 0) or (otherData == 0))
            {
                return false;
            }
            for (NATIVE_UINT_TYPE byte = 0; byte < len; byte++)
            {
                if (ourData[byte] != otherData[byte])
                {
                    return false;
                }
            }
            return true;
        }
    }

    bool IrisCmdByteStringArg::operator==(const char *other) const
    {
        // Designed to work like / be compatible with StringBase::operator== so this still assumes the incoming src is null terminated.

        NATIVE_UINT_TYPE len = this->length();

        const char *us = this->toChar();
        if ((us == 0) or (other == 0))
        {
            return false;
        }

        // Need to check the length here since allowing nulls in the data
        // section means it's possible for there to just be a null where the
        // terminator is in the other string, meaning the subsequent loop could
        // go out-of-bounds on `other`:
        // NOTE: we still assume other is null terminated
        if (strlen(other) != len)
        {
            return false;
        }

        // compare buffer
        for (NATIVE_UINT_TYPE byte = 0; byte < len; byte++)
        {
            if (us[byte] != other[byte])
            {
                return false;
            }
        }
        return true;
    }

    void IrisCmdByteStringArg::format(const char *formatString, ...)
    {
        char *us = (char *)this->toChar();
        NATIVE_UINT_TYPE cap = this->getCapacity();
        FW_ASSERT(us);
        va_list args;
        va_start(args, formatString);
        (void)vsnprintf(us, cap, formatString, args);
        va_end(args);
        // null terminate
        this->terminate(cap - 1);
    }

    bool IrisCmdByteStringArg::operator!=(const StringBase &other) const
    {
        return !operator==(other);
    }

    bool IrisCmdByteStringArg::operator!=(const char *other) const
    {
        return !operator==(other);
    }

#if FW_SERIALIZABLE_TO_STRING
    void IrisCmdByteStringArg::toString(StringBase &text) const
    {
        text = this->toChar();
    }
#endif

#ifdef BUILD_UT
    IrisCmdByteStringArg::ostream &operator<<(IrisCmdByteStringArg::ostream &os, const StringBase &str)
    {
        os << str.toChar();
        return os;
    }
#endif

    const char *IrisCmdByteStringArg::operator=(const char *other)
    {
        this->copyBuff(other, this->getCapacity());
        return this->toChar();
    }

    const StringBase &IrisCmdByteStringArg::operator=(const StringBase &other)
    {
        this->copyBuff(other.toChar(), this->getCapacity());
        return *this;
    }

    const CmdStringArg &IrisCmdByteStringArg::operator=(const CmdStringArg &other)
    {
        this->copyBuff(other.m_buf, this->getCapacity());
        return *this;
    }

    const IrisCmdByteStringArg &IrisCmdByteStringArg::operator=(const IrisCmdByteStringArg &other)
    {
        this->copyBuff(other.m_buf, this->getCapacity());
        this->setActualSize(other.getActualSize());
        return *this;
    }

    // Adapted from:
    // http://www.leidinger.net/FreeBSD/dox/libkern/html/d5/de6/strlcat_8c_source.html
    // strlcat source not standard, so borrowing.

    /*-
     *
     * **Modified to ignore null termination in the source and instead use
     * the built-in fixed size.**
     * NOTE: This is designed to work otherwise just like
     * `StringBase::appendBuff`, SO it still expects the incoming `buff` to
     *  be NULL terminated.
     *
     * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions
     * are met:
     * 1. Redistributions of source code must retain the above copyright
     *    notice, this list of conditions and the following disclaimer.
     * 2. Redistributions in binary form must reproduce the above copyright
     *    notice, this list of conditions and the following disclaimer in the
     *    documentation and/or other materials provided with the distribution.
     * 3. The name of the author may not be used to endorse or promote products
     *    derived from this software without specific prior written permission.
     *
     * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
     * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
     * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
     * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
     * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
     * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
     * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
     * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
     * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
     * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
     */

    void IrisCmdByteStringArg::appendBuff(const char *buff, NATIVE_UINT_TYPE size)
    {
        // size not used
        char *d = (char *)this->toChar();
        char *dst = d;

        const char *s = buff;
        // Set n to the number of bytes left AFTER current null termination:
        NATIVE_UINT_TYPE n = this->getCapacity() - this->length() - 1;
        NATIVE_UINT_TYPE siz = n;
        NATIVE_UINT_TYPE dlen;

        // Advance d by number of bytes in source data (EXCLUDING null
        // termination - i.e. landing it where the null terminator
        // should presently be):
        d += this->length();

        dlen = d - dst;
        n = siz - dlen;

        if (n == 0)
        {
            return;
        }

        while (*s != '\0')
        {
            if (n != 1)
            {
                *d++ = *s;
                n--;
            }
            s++;
        }
        // terminate & set new actual length:
        dlen = d - dst;
        this->terminate(dlen);
    }

}
