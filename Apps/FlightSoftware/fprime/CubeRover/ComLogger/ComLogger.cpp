// ----------------------------------------------------------------------
//
// ComLogger.cpp
//
// ----------------------------------------------------------------------

#include <CubeRover/ComLogger/ComLogger.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Os/ValidateFile.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


  int lfs_read(
      const struct lfs_config *config,
      lfs_block_t block,
      lfs_off_t offset,
      void *buffer,
      lfs_size_t size
    )
  {
    // Set the correct context for the read
    S25fl064l::MemAlloc flash_alloc = CubeRover::ComLoggerComponentImpl::flash_context;
    // FIXME Is this a correct use of block?
    // Set the correct offset
    uint32_t flash_offset = static_cast<U32>(block)*config->block_size + static_cast<U32>(offset);
    // Set the correct size
    uint16_t flash_size = static_cast<U16>(size);
    // Set the correct buffer pointer
    uint8_t *flash_data = static_cast<uint8_t *>(buffer);

    // Create error variable
    S25fl064l::S25fl064lError err;
    err = S25fl064l::readDataFromFlash(&flash_alloc,
                                       flash_offset,
                                       flash_data,
                                       flash_size);
    return err;
  }

  int lfs_prog(
      const struct lfs_config *config,
      lfs_block_t block,
      lfs_off_t offset,
      const void *buffer,
      lfs_size_t size
    )
  {
    // Set the correct context for the read
    S25fl064l::MemAlloc flash_alloc = CubeRover::ComLoggerComponentImpl::flash_context;
    // FIXME Is this a correct use of block?
    // Set the correct offset
    uint32_t flash_offset = static_cast<U32>(static_cast<U32>(block)*static_cast<U32>(config->block_size) + static_cast<U32>(offset));
    // Set the correct size
    uint16_t flash_size = static_cast<U16>(size);
    // Set the correct buffer pointer
    uint8_t *flash_data = reinterpret_cast<uint8_t*>(const_cast<void*>(buffer));

    // Create error variable
    S25fl064l::S25fl064lError err;
    err = S25fl064l::writeDataToFlash(&flash_alloc,
                                      flash_offset,
                                      flash_data,
                                      flash_size);
    return err;
  }

  int lfs_erase(
      const struct lfs_config *config,
      lfs_block_t block
    )
  {
    // FIXME: Not sure I need to have an erase function here
    /*
    // Create error variable
    S25fl064l::S25fl064lError err;
    err = S25fl064l::sectorErase(block);

    return err;
    */
    return 0;
  }

  int lfs_sync(
      const struct lfs_config *config
    )
  {
    // Return 0, every write/read is sync-ed in flash
    return 0;
  }

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  ComLoggerComponentImpl ::
#if FW_OBJECT_NAMES == 1
    ComLoggerComponentImpl(
        const char *const compName
    ) :
      ComLoggerComponentBase(compName)
#else
    ComLoggerComponentImpl(void)
#endif
    {
      this->fileMode = CLOSED;
      this->fileType = ukn;
      this->fileByteCount = 0;
      this->bytesRead = 0;
      this->bytesWritten = 0;
    }

    lfs_t lfs;
    lfs_file_t file;

    struct lfs_config cfg = {
      //.read = lfs_read,
      //.prog = lfs_prog,
      //.erase = lfs_erase,
      //.sync = lfs_sync,

      //.context = flash_chip,  // Set the correct flash chip instanciation
      .read_size = 8, // Min read size
      .prog_size = 8, // Min write size, Flash allows for single bit programming, making minimum 8 bytes to match read
      .block_size = 0x10000, // 64KB
      .block_count = 128, // 128 blocks of 64KB each (from FLASH datasheet), ~8MB total? Doesn't make sense as we have 64MB, maybe we have 8 sections of 128 blocks each?
      .cache_size = 256, // Kinda guessed? Just made it the max possible page that we can save
      .lookahead_size = 0, // no idea, don't think flash has one so set to zero?
      .block_cycles = -1,  // disable wear-leveling
      .name_max = 8,  // Max file name is 3 byte char + '_' 1 bytes + U32 (4 byte) time (seconds) = 8 bytes
      .file_max = MAX_FILE_SIZE // Max file size allowed
    };

  void ComLoggerComponentImpl :: 
    init(
      //NATIVE_INT_TYPE queueDepth, //!< The queue depth
      NATIVE_INT_TYPE instance //!< The instance number
    )
  {
    ComLoggerComponentBase::init(instance);
    // Initialize FLASH
    flash_chip.setupDevice();
    flash_context.startAddress = 0;
    flash_context.reservedSize = 0;

    // Setup cfg with correct functions
    cfg.read = lfs_read;
    cfg.prog = lfs_prog;
    cfg.erase = lfs_erase;
    cfg.sync = lfs_sync;

    // Mount File System
    int err = lfs_mount(&lfs, &cfg);
    // Reformat File System if received error, should only happen on first boot
    if(err)
    {
      lfs_format(&lfs, &cfg);
      err = lfs_mount(&lfs, &cfg);
      // If still error, log error and let ground know stuff's bad
      if(err)
      {
        //TODO LOG ERROR AND LET GROUND KNOW
      }
    }
    this->fileMode = CLOSED; 
    this->fileType = ukn;
    this->fileByteCount = 0;
    this->bytesRead = 0;
    this->bytesWritten = 0;
    // Update Telem
    this->tlmWrite_TOTAL_BYTES_READ(bytesRead);
    this->tlmWrite_TOTAL_BYTES_WRITTEN(bytesWritten);
    this->tlmWrite_CUR_FILE_BYTES(fileByteCount);
    this->tlmWrite_CUR_FILE_TIME(0);    // 0 is default time
    this->tlmWrite_CUR_FILE_TYPE(fileType);
    this->tlmWrite_CUR_FILE_STATUS(fileMode);
  }

  ComLoggerComponentImpl ::
    ~ComLoggerComponentImpl(void)
  {
    if(this->fileMode == OPEN) 
    {
      // Close file:
      closeFile();
    }
  }

  // ----------------------------------------------------------------------
  // Handler implementations
  // ----------------------------------------------------------------------

  void ComLoggerComponentImpl ::
    comIn_handler(
        NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    // Calculate the estimated size of the file to see if it's larger than max size of log file
    U32 est_file_size = this->fileByteCount + data.getBuffLength();
    if(this->fileMode == OPEN && this->fileType == log && est_file_size <= MAX_FILE_SIZE)
    {
      // Write to current file
      writeToFile(static_cast<void*>(data.getBuffAddr()), static_cast<U32>(data.getBuffLength()));
    }
    else
    {
      // Create a new file with log prefix and new time (the current time)
      Fw::Time timestamp = getTime();
      char log_prefix [3] = {'l','o','g'};
      writeToFile(static_cast<void*>(data.getBuffAddr()), static_cast<U32>(data.getBuffLength()), log_prefix, timestamp.getSeconds());
    }
  }

  void ComLoggerComponentImpl :: 
    CloseFile_cmdHandler(
      FwOpcodeType opCode,
      U32 cmdSeq
    )
  {
    bool err = closeFile();
    //TODO CHANGE CMD RESPONSE IF ERROR
    if(!err)
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    else
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void ComLoggerComponentImpl ::
    SendLog_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& prefix,
        U32 time
    )
  {
    Fw::Buffer data;
    // Clear read buffer
    memset(&m_read_buffer, 0, sizeof(m_read_buffer));

    char prefix_char [3];
    memcpy(prefix_char, prefix.toChar(), sizeof(prefix_char));
    U32 length_read = readFromFile(&m_read_buffer, prefix_char, time);
    // Set buffer size to read size
    data.setsize(length_read);

    // Set buffer data to read data buffer
    data.setdata((U64)m_read_buffer);

    // Send Output buffer to ground
    this->GndOut_out(0, data);

    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }
  /*
  void ComLogger ::
    pingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
      // return key
      this->pingOut_out(0,key);
  }
  */

  // Should only be called when you know you need a new file
  void ComLoggerComponentImpl ::
    openFile(
      char prefix [3],
      U32 time
    )
  {
    // If a file is already open, we must close the file
    if(this->fileMode == OPEN)
    {
      closeFile();
    }

    // Create string to open new file with
    char file_name[MAX_FILENAME_SIZE];
    strcat(file_name, prefix);
    // Get the current time
    char time_name [10];
    snprintf(time_name, sizeof(time_name), "%d", time);
    strcat(file_name, time_name);

    // Open a file, create one if it doesn't exist
    int err = lfs_file_open(&lfs, &file, file_name, LFS_O_RDWR | LFS_O_CREAT);
    //TODO LOG ERRORS IF CAN'T OPEN CORRECTLY
    //TODO Send event:
    //Fw::LogStringArg logStringArg((char*) this->fileName);
    //this->log_DIAGNOSTIC_FileClosed(logStringArg);
    //TODO SEND TELEMETRY TO LET GROUND KNOW WHAT FILE IS OPEN

    // Change tracked open file type to correct type
    this->fileMode = OPEN;
    this->fileType = prefixToType(prefix);
    this->fileByteCount = static_cast<U32>(lfs_file_size(&lfs, &file));

    // Update Telemetry
    this->tlmWrite_CUR_FILE_BYTES(fileByteCount);
    this->tlmWrite_CUR_FILE_STATUS(fileMode);
    this->tlmWrite_CUR_FILE_TYPE(fileType);
  }   

  bool ComLoggerComponentImpl ::
    closeFile(
    )
  {
    if(this->fileMode == OPEN) 
    {
      // Close file:
      //Must close current file as not log file type
      int err = lfs_file_close(&lfs, &file);
      //TODO LOG ERRORS IF CAN'T CLOSE CORRECTLY

      // Update mode and byte count:
      this->fileMode = CLOSED;
      this->fileByteCount = 0;

      // Send event:
      //Fw::LogStringArg logStringArg((char*) this->fileName);
      //this->log_DIAGNOSTIC_FileClosed(logStringArg);

      // Update Telemetry
      this->tlmWrite_CUR_FILE_STATUS(fileMode);
      this->tlmWrite_CUR_FILE_BYTES(fileByteCount);
    }
    return true;
  }

  void ComLoggerComponentImpl ::
    writeToFile(
      void* data, 
      U32 length,
      char prefix [3],
      U32 time
    )
  {
    // Close the file if file isn't the current file:
    if(this->fileMode == OPEN && this->fileType != prefixToType(prefix))
    {
      closeFile();
    }

    // Open the file if it there is not one open:
    if(this->fileMode == CLOSED)
    {
      openFile(prefix, time);
    }

    U32 true_length;
    if(length < MAX_FILE_SIZE)
    {
      true_length = length;
    }
    else
    {
      true_length = MAX_FILE_SIZE;
    }

    int err = lfs_file_write(&lfs, &file, data, true_length);

    //TODO IMPLEMENT ERROR DETECTION
    //Fw::LogStringArg logStringArg((char*) this->fileName);
    //this->log_WARNING_HI_FileWriteError(ret, size, length, logStringArg);

    this->fileByteCount += true_length;
    this->bytesWritten += true_length;
    this->tlmWrite_TOTAL_BYTES_WRITTEN(bytesWritten);
    this->tlmWrite_CUR_FILE_BYTES(fileByteCount);
  }

  // Overloaded version with no prefix or time parameter, assume file that is open is valid and has space
  void ComLoggerComponentImpl ::
    writeToFile(
      void* data, 
      U32 length
    )
  {
    // No prefix or time argument given means that we want to write to the same file
    U32 true_length;
    if(length < MAX_FILE_SIZE)
    {
      true_length = length;
    }
    else
    {
      true_length = MAX_FILE_SIZE;
    }

    int err = lfs_file_write(&lfs, &file, data, true_length);

    //TODO IMPLEMENT ERROR DETECTION
    //Fw::LogStringArg logStringArg((char*) this->fileName);
    //this->log_WARNING_HI_FileWriteError(ret, size, length, logStringArg);

    this->fileByteCount += true_length;
    this->bytesWritten += true_length;
    this->tlmWrite_TOTAL_BYTES_WRITTEN(bytesWritten);
    this->tlmWrite_CUR_FILE_BYTES(fileByteCount);
  }

  U32 ComLoggerComponentImpl :: 
    readFromFile(
      void* buffer,
      char prefix [3],
      U32 time
    )
  {
    // Automatically close the current file
    closeFile();

    // Open file we want
    openFile(prefix, time);

    // Calculate size to be read
    U32 true_length = static_cast<U32>(lfs_file_size(&lfs, &file));
    if(true_length > MAX_FILE_SIZE)
    {
      true_length = MAX_FILE_SIZE;
    }

    // Read from file, put into buffer
    int err = lfs_file_read(&lfs, &file, buffer, true_length);

    if(err != true_length)
        // TODO: didn't read as much as though or got a negative num meaning error
        
    //TODO IMPLEMENT ERROR DETECTION
    //Fw::LogStringArg logStringArg((char*) this->fileName);
    //this->log_WARNING_HI_FileReadError(ret, size, logStringArg);

    // Update Telemetry
    this->bytesRead += true_length;
    this->tlmWrite_TOTAL_BYTES_READ(bytesRead);
    return true_length;
  }

  ComLoggerComponentImpl::FileType ComLoggerComponentImpl ::
    prefixToType(
        char prefix [3]
      )
  {
    // Update this list as needed to add in more types of stored data
    switch(atol(prefix))
    {
      case log:
        return log;
      case cam:
        return cam;
      case ukn:
        return ukn;
      default:
        return ukn;
    }
  }
}
