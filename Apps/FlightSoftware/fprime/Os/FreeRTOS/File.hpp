#ifndef _File_hpp_
#define _File_hpp_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/BasicTypes.hpp>

#include <Os/FreeRTOS/S25fl064l.hpp>

    #define SYSTEM_FILE_HEADER_MEM_ALLOC  PAGE_SIZE_FLASH  // memory allocated to save the system file look-up table
    #define MAX_NUMBER_OF_FILES           2   // max number of file that can be created 
    #define SIZE_OF_FILENAME              16  // bytes
    #define SIZE_OF_FILE_START_ADDRESS    4   // bytes
    #define SIZE_OF_FILE_CURRENT_POINTER  4   // bytes
    #define SIZE_OF_FILE_SIZE             4   // bytes
    #define NEW_DEFAULT_FILE_SIZE         (MAX_MEMORY_ADDRESS_FLASH - (PAGE_SIZE_FLASH)) / MAX_NUMBER_OF_FILES
    #define SYSTEM_FILE_ENTRY_SIZE        SIZE_OF_FILENAME + SIZE_OF_FILE_START_ADDRESS + SIZE_OF_FILE_CURRENT_POINTER + SIZE_OF_FILE_SIZE   

    typedef char FileName[SIZE_OF_FILENAME];
    typedef uint32_t AddressPointer;
    typedef AddressPointer StartAddress;
    typedef AddressPointer FilePointer;
    typedef uint32_t FileSize;

namespace Os {

    // This class encapsulates a very simple file interface that has the most often-used features

    class File {
        public:

            typedef enum {
                OPEN_NO_MODE,   //!<  File mode not yet selected
                OPEN_READ, //!<  Open file for reading
                OPEN_WRITE, //!<  Open file for writing
                OPEN_SYNC_WRITE, //!<  Open file for writing; writes don't return until data is on disk
                OPEN_SYNC_DIRECT_WRITE, //!<  Open file for writing, bypassing all caching. Requires data alignment
                OPEN_CREATE, //!< Open file for writing and truncates file if it exists, ie same flags as creat()
            } Mode;

            typedef enum {
                OP_OK, //!<  Operation was successful
                DOESNT_EXIST, //!<  File doesn't exist (for read)
                NO_SPACE, //!<  No space left
                NO_PERMISSION, //!<  No permission to read/write file
                BAD_SIZE, //!<  Invalid size parameter
                NOT_OPENED, //!<  file hasn't been opened yet
                OTHER_ERROR, //!<  A catch-all for other errors. Have to look in implementation-specific code
            } Status;

            File(); //!<  Constructor
            virtual ~File(); //!<  Destructor. Will close file if still open
            Status prealloc(NATIVE_INT_TYPE offset, NATIVE_INT_TYPE len);
            Status open(const char* fileName, Mode mode); //!<  open file. Writing creates file if it doesn't exist
            Status seek(NATIVE_INT_TYPE offset, bool absolute = true); //!<  seek to location. If absolute = true, absolute from beginning of file
            Status flush(); //!< flush data to disk. No-op on systems that do not support.
            Status read(void * buffer, NATIVE_INT_TYPE &size, bool waitForFull = true); //!<  read data from file; returns amount read or errno.
                                                                            //!<  waitForFull = true to wait for all bytes to be read
                                                                            // size is modified to actual read size
            Status write(const void * buffer, NATIVE_INT_TYPE &size, bool waitForDone = true); //!<  write size; will return amount written or errno
            Status bulkWrite(const void * buffer, NATIVE_UINT_TYPE &totalSize, NATIVE_INT_TYPE chunkSize); //!<  write size; will return amount written or errno

            void close(void); //!<  close file

            NATIVE_INT_TYPE getLastError(void); //!< read back last error code (typically errno)
            const char* getLastErrorString(void); //!< get a string of the last error (typically from strerror)
            Status calculateCRC32(U32 &crc); //!< calculates the CRC32 of the file

            static Status niceCRC32(U32 &crc, const char* fileName); //!< Calculates CRC32 of file, not burdening FS

            //Return the file start address if asked
            U32 getFileStartAddress();

        private:

            NATIVE_INT_TYPE m_fd; //!<  Stored file descriptor
            Mode m_mode; //!<  Stores mode for error checking
            NATIVE_INT_TYPE m_lastError; //!<  stores last error
        
            // required to run 
            S25fl064l m_flashMem;
            S25fl064l::MemAlloc m_headerMemAlloc;
            File::Status flashMemErrToStatus(const S25fl064l::S25fl064lError err);
            uint8_t m_fileIndex;
            FileName m_fileName;
            StartAddress m_fileStartAddress;
            FilePointer m_fileOffsetPointer;
            FileSize m_fileSize;
    };

}

#endif