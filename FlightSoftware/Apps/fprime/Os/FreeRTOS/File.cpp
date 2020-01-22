#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <Utils/Hash/libcrc/lib_crc.h> // borrow CRC

#ifdef __cplusplus
}
#endif // __cplusplus

#include <cerrno>
#include <sys/types.h>
#include <limits>
#include <string.h>
#include <stdio.h>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

namespace Os {

    File::File() {
    }

    File::~File() {
    }

    File::Status File::open(const char* fileName, File::Mode mode) {
        Status stat = OP_OK;

        this->m_lastError = errno;
        this->m_mode = mode;
        return stat;
    }

    File::Status File::prealloc(NATIVE_INT_TYPE offset, NATIVE_INT_TYPE len) {
        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            return NOT_OPENED;
        }

        File::Status fileStatus = OP_OK;
        return fileStatus;
    }

    File::Status File::seek(NATIVE_INT_TYPE offset, bool absolute) {
        Status stat = OP_OK;
        return stat;
    }

    File::Status File::read(void * buffer, NATIVE_INT_TYPE &size, bool waitForFull) {
        Status stat = OP_OK;
        return stat;
    }

    File::Status File::write(const void * buffer, NATIVE_INT_TYPE &size, bool waitForDone) {
        Status stat = OP_OK;
        return stat;
    }

    // NOTE(mereweth) - see http://lkml.iu.edu/hypermail/linux/kernel/1005.2/01845.html
    // recommendation from Linus Torvalds, but doesn't seem to be that fast
    File::Status File::bulkWrite(const void * buffer, NATIVE_UINT_TYPE &totalSize,
                                 NATIVE_INT_TYPE chunkSize) {

        return OP_OK;
    }

    File::Status File::flush() {
        File::Status stat = OP_OK;
        return stat;
    }

    void File::close(void) {
    }

    NATIVE_INT_TYPE File::getLastError(void) {
        return this->m_lastError;
    }

    const char* File::getLastErrorString(void) {
        return strerror(this->m_lastError);
    }

    File::Status File::calculateCRC32(U32 &crc)
    {
        return OP_OK;
    }
}
