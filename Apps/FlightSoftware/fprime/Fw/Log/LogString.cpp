#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Log/LogString.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Fw {

    LogStringArg::LogStringArg(const char* src)
            :  StringBase(), m_maxSer(FW_LOG_STRING_MAX_SIZE) {
        this->copyBuff(src,sizeof(this->m_buf));
    }

    LogStringArg::LogStringArg(const StringBase& src)
            : StringBase(), m_maxSer(FW_LOG_STRING_MAX_SIZE) {
        this->copyBuff(src.toChar(),sizeof(this->m_buf));
    }

    LogStringArg::LogStringArg(const LogStringArg& src)
            : StringBase(), m_maxSer(FW_LOG_STRING_MAX_SIZE) {
        this->copyBuff(src.m_buf,sizeof(this->m_buf));
    }

    LogStringArg::LogStringArg(void)
            : StringBase(), m_maxSer(FW_LOG_STRING_MAX_SIZE) {
        this->m_buf[0] = 0;
    }

    LogStringArg::~LogStringArg(void) {
    }

    NATIVE_UINT_TYPE LogStringArg::length(void) const {
        return (NATIVE_UINT_TYPE) strlen(this->m_buf);
    }

    const char* LogStringArg::toChar(void) const {
        return this->m_buf;
    }

    void LogStringArg::copyBuff(const char* buff, NATIVE_UINT_TYPE size) {
        FW_ASSERT(buff);
        // check for self copy
        if (buff != this->m_buf) {
            (void)strncpy(this->m_buf,buff,size);
            // NULL terminate
            this->terminate(sizeof(this->m_buf));
        }
    }
    
    SerializeStatus LogStringArg::serialize(SerializeBufferBase& buffer) const {
        // serialize string
        NATIVE_UINT_TYPE strSize = FW_MIN(this->m_maxSer,static_cast<NATIVE_UINT_TYPE>(strlen(this->m_buf)));
#if FW_AMPCS_COMPATIBLE
        // serialize string in AMPC compatible way
        // AMPC requires an 8-bit argument size value before the string

        // Omit the null terminator character because AMPCS does not like
        // \0 in its strings. So subtract 1 from strSize
        strSize--;

        // serialize 8-bit size
        // cap at 8 bit size value if higher
        if (strSize > 256) {
            strSize = 256;
        }
        SerializeStatus stat = buffer.serialize(static_cast<U8>(strSize));
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
        return buffer.serialize((U8*)this->m_buf,strSize,true);
#else
        return buffer.serialize((U8*)this->m_buf,strSize);
#endif
    }
    
    SerializeStatus LogStringArg::deserialize(SerializeBufferBase& buffer) {
        SerializeStatus stat;
#if FW_AMPCS_COMPATIBLE
        // serialize string in AMPC compatible way
        // AMPCS requires an 8-bit argument size value before the string

        // deserialize 8-bit size
        U8 deserSize;
        stat = buffer.deserialize(deserSize);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        NATIVE_UINT_TYPE deserSize_native = static_cast<NATIVE_UINT_TYPE>(deserSize);
        buffer.deserialize((U8*)this->m_buf,deserSize_native,true);
        this->m_buf[deserSize_native] = 0;
        return stat;
#else
        NATIVE_UINT_TYPE maxSize = sizeof(this->m_buf);
        // deserialize string
        stat = buffer.deserialize((U8*)this->m_buf,maxSize);
        // make sure it is null-terminated
        this->terminate(maxSize);

#endif
        return stat;
    }

    void LogStringArg::setMaxSerialize(NATIVE_UINT_TYPE size) {
        this->m_maxSer = FW_MIN(size,FW_LOG_STRING_MAX_SIZE);
    }

    NATIVE_UINT_TYPE LogStringArg::getCapacity(void) const {
        return FW_LOG_STRING_MAX_SIZE;
    }

    void LogStringArg::terminate(NATIVE_UINT_TYPE size) {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf)?size:sizeof(this->m_buf)-1] = 0;
    }

    const LogStringArg& LogStringArg::operator=(const LogStringArg& other) {
        this->copyBuff(other.m_buf,this->getCapacity());
        return *this;
    }

#if FW_SERIALIZABLE_TO_STRING
    void LogStringArg::toString(StringBase& text) const {
        text = this->m_buf;
    }
#endif

}
