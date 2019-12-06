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
    m_headerMemAlloc.startAddress = FLASH_MEM_HEADER_START_ADDRESS;
    m_headerMemAlloc.reservedSize = PAGE_SIZE;
  }

  File::~File() {
  }

  File::Status File::open(const char* fileName, File::Mode mode) {
    Status stat = OP_OK;

    // Check that the device can be accessed.
    m_flashMem.setupDevice();

    // First we read the data from the flash header This is a very very very
    // simple file system specific to the implementation of CubeRover. Header of
    // the flash memory contains: 
    // Address 0x0: name of file [16 bytes]
    // Address 0x10: file start address [4 bytes]
    // Address 0x14: file current location [4 bytes]
    // Address 0x18: file size [4 bytes]
    // Address 0x20: name of file #2 [16 bytes] , etc. 
    m_flashMem.readDataFromFlash(m_headerMemAlloc)

    this->m_lastError = errno;
    this->m_mode = mode;
    return stat;
  }

  File::Status File::prealloc(NATIVE_INT_TYPE offset, NATIVE_INT_TYPE len) {
    // make sure it has been opened
    if(OPEN_NO_MODE == this->m_mode) {
        return NOT_OPENED;
    }

    //ignore offset
    S25fl064lError error = allocateFlashMemory(m_fileAlloc, len);

    if(error != S25FL064L_NO_ERROR){
      switch(error){
        case S25FL064L_FAIL_MEM_ALLOCATION:
          fileStatus = NO_SPACE;
          break;
        default:
          fileStatus = OTHER_ERROR;
          break;
      }
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

  File::Status File::calculateCRC32(U32 &crc){
    return OP_OK;
  }
}
