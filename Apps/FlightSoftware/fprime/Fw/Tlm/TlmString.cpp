#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Tlm/TlmString.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Fw {

    TlmString::TlmString(const char* src) :  StringBase(), m_maxSer(FW_TLM_STRING_MAX_SIZE) {
        this->copyBuff(src,sizeof(this->m_buf));
    }

    TlmString::TlmString(const StringBase& src) : StringBase(), m_maxSer(FW_TLM_STRING_MAX_SIZE) {
        this->copyBuff(src.toChar(),sizeof(this->m_buf));
    }

    TlmString::TlmString(const TlmString& src) : StringBase(), m_maxSer(FW_TLM_STRING_MAX_SIZE) {
        this->copyBuff(src.m_buf,sizeof(this->m_buf));
    }

    TlmString::TlmString(void) : StringBase(), m_maxSer(FW_TLM_STRING_MAX_SIZE) {
        this->m_buf[0] = 0;
    }

    TlmString::~TlmString(void) {
    }

    NATIVE_UINT_TYPE TlmString::length(void) const {
        return strlen(this->m_buf);
    }

    const char* TlmString::toChar(void) const {
        return this->m_buf;
    }

    void TlmString::copyBuff(const char* buff, NATIVE_UINT_TYPE size) {
        FW_ASSERT(buff);
        // check for self copy
        if (buff != this->m_buf) {
            (void)strncpy(this->m_buf,buff,size);
            // NULL terminate
            this->terminate(sizeof(this->m_buf));
        }
    }
    
    SerializeStatus TlmString::serialize(SerializeBufferBase& buffer) const {
        NATIVE_UINT_TYPE strSize = strlen(this->m_buf);
#if FW_AMPCS_COMPATIBLE
        // serialize string in AMPC compatible way
        // AMPC requires an 8-bit argument size value before the string

        // Omit the null terminator character because AMPCS does not like
        // \0 in its strings. So subtract 1 from strSize
        strSize--;

        // serialize 8-bit size
        SerializeStatus stat = buffer.serialize(static_cast<U8>(strSize));
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
        return buffer.serialize((U8*)this->m_buf,strSize,true);
#else
        return buffer.serialize((U8*)this->m_buf,strSize);
#endif
    }
    
    SerializeStatus TlmString::deserialize(SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = sizeof(this->m_buf);
        // deserialize string
#if FW_AMPCS_COMPATIBLE
        // AMPCS encodes 8-bit string size
        U8 strSize;
        SerializeStatus stat = buffer.deserialize(strSize);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
        NATIVE_UINT_TYPE buffSize = strSize;
        // To make sure there is space when we add the null terminator
        // which was omitted in the serialization of this buffer
        strSize++;
        stat = buffer.deserialize((U8*)this->m_buf,buffSize,true);
        this->m_buf[strSize-1] = 0;
#else
        // deserialize string
        SerializeStatus stat = buffer.deserialize((U8*)this->m_buf,maxSize);
#endif
        // make sure it is null-terminated
        this->terminate(maxSize);

        return stat;
    }

    void TlmString::setMaxSerialize(NATIVE_UINT_TYPE size) {
        this->m_maxSer = FW_MIN(size,FW_TLM_STRING_MAX_SIZE);
    }

    NATIVE_UINT_TYPE TlmString::getCapacity(void) const {
        return FW_TLM_STRING_MAX_SIZE;
    }
    
    void TlmString::terminate(NATIVE_UINT_TYPE size) {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf)?size:sizeof(this->m_buf)-1] = 0;
    }

    const TlmString& TlmString::operator=(const TlmString& other) {
        this->copyBuff(other.m_buf,this->getCapacity());
        return *this;
    }


#if FW_SERIALIZABLE_TO_STRING
    void TlmString::toString(StringBase& text) const {
        text = this->m_buf;
    }
#endif    
}
