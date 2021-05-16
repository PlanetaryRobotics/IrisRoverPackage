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

    }

  void ComLoggerComponentImpl :: 
    init(
      //NATIVE_INT_TYPE queueDepth, //!< The queue depth
      NATIVE_INT_TYPE instance //!< The instance number
    )
  {
    ComLoggerComponentBase::init(instance);
    // Initialize FLASH
    flash_chip.setupDevice();

    // Setup cfg with correct functions
    cfg->context = static_cast<void*>(flash_chip);
    cfg->read = lfs_read;
    cfg->prog = lfs_prog;
    cfg->erase = lfs_erase;
    cfg->sync = lfs_sync;

    // Mount File System
    int err = lfs_mount(&lfs, &cfg);
    // Reformat File System if received error, should only happen on first boot
    if(err)
    {
      lfs_format(&lsf, &cfg);
      err = lfs_mount(&lfs, &cfg);
      // If still error, log error and let ground know stuff's bad
      if(err)
      {
        //TODO LOG ERROR AND LET GROUND KNOW
      }
    }
    this->fileMode = CLOSED; 
    this->fileType = ukn;
    this->byteCount = 0;
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
    if(this->fileMode == OPEN && this->fileType == log && est_file_size <= MAX_LOG_FILE_SIZE)
    {
      // Write to current file
      writeToFile(static_cast<void*>(data), static_cast<U32>(data.getBuffLength()));
    }
    else
    {
      // Create a new file with log prefix and new time (the current time)
      Fw::Time timestamp = getTime();
      writeToFile(static_cast<void*>(data), static_cast<U32>(data.getBuffLength()), "log", timestamp.getSeconds());
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

    U32 length_read = readFromFile(&m_read_buffer, prefix.toChar(), time);
    // Set buffer size to read size
    data.setsize(length_read);

    // Set buffer data to read data buffer
    data.setdata(static_cast<U64>(m_read_buffer));

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
    strcat(file_name, "%s_", prefix);
    // Get the current time
    strcat(file_name, "%d", time);

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
  }   

  void ComLoggerComponentImpl ::
    closeFile(
    )
  {
    if(OPEN == this->fileMode) 
    {
      // Close file:
      //Must close current file as not log file type
      int err = lfs_file_close(&lfs, &current_file);
      //TODO LOG ERRORS IF CAN'T CLOSE CORRECTLY

      // Update mode and byte count:
      this->fileMode = CLOSED;
      this->fileByteCount = 0;

      // Send event:
      Fw::LogStringArg logStringArg((char*) this->fileName);
      this->log_DIAGNOSTIC_FileClosed(logStringArg);
      return true;
    }
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
        
    //TODO IMPLEMENT ERROR DETECTION
    //Fw::LogStringArg logStringArg((char*) this->fileName);
    //this->log_WARNING_HI_FileReadError(ret, size, logStringArg);
    return true_length;
  }

  FileType ComLoggerComponentImpl :: 
    prefixToType(
        char prefix [3]
      )
  {
    //TODO CHANGE PREFIX INTO A SPECIFIC FileType
    switch(prefix)
    {
      case strncmp("log", prefix) == 0:
        return log;
      case strncmp("cam", prefix) == 0:
        return cam;
      case strncmp("ukn", prefix) == 0:
        return ukn;
      default:
        return ukn;
    }
  }

  int ComLoggerComponentImpl :: 
    lfs_read(
      const struct lfs_config *cfg, 
      lfs_block_t block,
      lfs_off_t offset,
      void *buffer,
      lfs_size_t size
    )
  {
    // Set the correct context for the read
    S25fl064l::MemAlloc flash_alloc = cfg->context;
    // FIXME Is this a correct use of block?
    // Set the correct offset
    uint32_t flash_offset = static_cast<U32>(block)*cfg->block_size + static_cast<U32>(offset);
    // Set the correct size
    uint16_t flash_size = static_cast<U16>(size);
    // Set the correct buffer pointer
    uint8_t *flash_data = static_cast<uint8_t *>(buffer);

    // Create error variable
    S25fl064l::S25fl064lError err = 0;
    err = S25fl064l::readDataFromFlash(flash_alloc,
                                       flash_offset,
                                       flash_data,
                                       flash_size);
    return err;
  }

  int ComLoggerComponentImpl :: 
    lfs_prog(
      const struct lfs_config *cfg, 
      lfs_block_t block,
      lfs_off_t offset,
      void *buffer,
      lfs_size_t size
    )
  {
    // Set the correct context for the read
    S25fl064l::MemAlloc flash_alloc = cfg->context;
    // FIXME Is this a correct use of block?
    // Set the correct offset
    S25fl064l::Address flash_offset = static_cast<S25fl064l::Address>(static_cast<U32>(block)*cfg->block_size + static_cast<U32>(offset));
    // Set the correct size
    uint16_t flash_size = static_cast<U16>(size);
    // Set the correct buffer pointer
    uint8_t *flash_data = static_cast<uint8_t *>(buffer);

    // Create error variable
    S25fl064l::S25fl064lError err = 0;
    err = S25fl064l::writeDataToFlash(flash_alloc,
                                      flash_offset,
                                      flash_data,
                                      flash_size);
    return err;
  }

  int ComLoggerComponentImpl :: 
    lfs_erase(
      const struct lfs_config *cfg, 
      lfs_block_t block
    )
  {
    // Set the correct block
    S25fl064l::Block flash_block = static_cast<S25fl064l::Block>(block);

    // Create error variable
    S25fl064l::S25fl064lError err = 0;
    err = S25fl064l::blockErase(flash_block);

    return err;
  }

  int ComLoggerComponentImpl :: 
    lfs_sync(
      const struct lfs_config *cfg
    )
  {
    // FIXME What do I do here? I though I might use pageProgram() but that requires an address, size, and data buffer
    return 0;
  }
}
