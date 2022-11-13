#ifndef FW_IRIS_CMD_BYTE_STRING_TYPE_HPP
#define FW_IRIS_CMD_BYTE_STRING_TYPE_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Cfg/SerIds.hpp>

/**
 * @brief Somewhat hacky last-hour variant of `CmdStringArg` that doesn't use
 * null-termination for determining length, so arbitrary byte sequences (i.e.
 * ones that include '\0') can be passed as command arguments anywhere
 * `CmdStringArg` (or just "string") would be accepted.
 *
 * NOTE: This type still does terminate its strings with a null so they can
 * interoperate safely with normal strings in case it's somehow converted back
 * to one, it just doesn't care if there are nulls inside the data section.
 *
 * @author Connor W. Colombo (connor.w.colombo@gmail.com)
 * @date 11/09/2022
 */

namespace Fw
{

    class IrisCmdByteStringArg : public Fw::CmdStringArg
    {
    public:
        enum
        {
            // Type metadata same as Fw::CmdStringArg
            SERIALIZED_TYPE_ID = FW_TYPEID_CMD_STR,
            SERIALIZED_SIZE = FW_CMD_STRING_MAX_SIZE + sizeof(FwBuffSizeType)
        };

        IrisCmdByteStringArg(const char *src);
        IrisCmdByteStringArg(const StringBase &src);
        IrisCmdByteStringArg(const CmdStringArg &src);
        IrisCmdByteStringArg(const IrisCmdByteStringArg &src);
        IrisCmdByteStringArg(void);
        ~IrisCmdByteStringArg(void);

        const char *toChar(void) const;

        // Returns the number of bytes, EXCLUDING the null termination
        NATIVE_UINT_TYPE length(void) const;

        const char *operator+=(const char *src);             //!< Concatenate a char*
        const StringBase &operator+=(const StringBase &src); //!< Concatenate a StringBase
        bool operator==(const StringBase &other) const;      //!< Check for equality with StringBase
        bool operator==(const char *other) const;            //!< Check for equality with char*
        bool operator!=(const StringBase &other) const;      //!< Inequality with StringBase
        bool operator!=(const char *other) const;            //!< Inequality with char*

        const char *operator=(const char *src);                                   //!< Assign char*
        const StringBase &operator=(const StringBase &src);                       //!< Assign another StringBase
        const CmdStringArg &operator=(const CmdStringArg &other);                 //!< equal operator for other strings
        const IrisCmdByteStringArg &operator=(const IrisCmdByteStringArg &other); //!< equal operator for other strings

        void appendBuff(const char *buff, NATIVE_UINT_TYPE size);

        void format(const char *formatString, ...); //!< write formatted string to buffer
#ifdef BUILD_UT
        // to support GoogleTest framework in unit tests
        friend std::ostream &operator<<(std::ostream &os, const StringBase &str);
#endif
#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
        void toString(StringBase &text) const; //!< write string with contents
#endif

        SerializeStatus serialize(SerializeBufferBase &buffer) const;
        SerializeStatus deserialize(SerializeBufferBase &buffer);

    private:
        void copyBuff(const char *buff, NATIVE_UINT_TYPE size); //!< copy source buffer, overwriting
        NATIVE_UINT_TYPE getCapacity(void) const;               //!< return buffer size
        void terminate(NATIVE_UINT_TYPE size);                  //!< terminate the string

        // Actual number of bytes used in this string, EXCLUDING the null
        // termination:
        NATIVE_UINT_TYPE actual_size;

    protected:
        void setActualSize(NATIVE_UINT_TYPE s);
        NATIVE_UINT_TYPE getActualSize(void) const;
    };

}

#endif // FW_IRIS_CMD_BYTE_STRING_TYPE_HPP
