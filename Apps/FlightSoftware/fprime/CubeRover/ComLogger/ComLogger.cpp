// ----------------------------------------------------------------------
//
// ComLogger.cpp
//
// ----------------------------------------------------------------------

#include <CubeRover/ComLogger/ComLogger.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Os/ValidateFile.hpp>
#include <Os/FreeRTOS/S25fl064l.hpp>
#include <Os/FreeRTOS/lfs.h>
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

  void ComLogger :: 
    init(
      //NATIVE_INT_TYPE queueDepth, //!< The queue depth
      NATIVE_INT_TYPE instance //!< The instance number
    )
  {
    ComLoggerComponentBase::init(instance);
    // Initialize FLASH
    flash_chip.setupDevice();
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
    this->byteCount = 0;
  }

  ComLogger ::
    ~ComLogger(void)
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

  void ComLogger ::
    comIn_handler(
        NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    //TODO CALCULATE HOW BIG FILE WILL BE TO SEE IF SHOULD WRITE INTO FILE OR CREATE NEW ONE
    if(this->fileMode == OPEN && this->fileType == log && )
    {
      writeToFile(static_cast<void*>(data), static_cast<U32>(data.getBuffLength()), "log", /*TIME OF CURRENT FILE*/);
    }
    else
    {
      Fw::Time timestamp = getTime();
      writeToFile(static_cast<void*>(data), static_cast<U32>(data.getBuffLength()), "log", timestamp.getSeconds());
    }
  }

  void ComLogger :: 
    CloseFile_cmdHandler(
      FwOpcodeType opCode,
      U32 cmdSeq
    )
  {
    //TODO MAKE CLOSEFILE() A BOOLEAN TO KNOW IF CMD SUCEEDED
    closeFile();
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void ComLogger ::
    SendLog_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& prefix,
        U32 time
    )
  {

    // Send Output buffer to ground
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
  void ComLogger ::
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
    this->fileMode = OPEN
    this->fileType = log;
  }   

  void ComLogger ::
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
    }
  }

  void ComLogger ::
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

    int err = lfs_file_write(&lfs, &file, data, length);

    //TODO IMPLEMENT ERROR DETECTION
    //Fw::LogStringArg logStringArg((char*) this->fileName);
    //this->log_WARNING_HI_FileWriteError(ret, size, length, logStringArg);

    this->fileByteCount += length;
  }

  void ComLogger :: 
    readFromFile(
      void* buffer,
      U32 length,
      char prefix [3],
      U32 time
    )
  {
    // Automatically close the current file
    closeFile();

    // Open file we want
    openFile(prefix, time);

    // Read from file, put into buffer
    if(length < MAX_FILE_SIZE)
      U8 read_buffer[length];
    else
      U8 read_buffer[MAX_FILE_SIZE];

    int err = lfs_file_read(&lfs, &file, &read_buffer, sizeof(read_buffer));
        
    //TODO IMPLEMENT ERROR DETECTION
    //Fw::LogStringArg logStringArg((char*) this->fileName);
    //this->log_WARNING_HI_FileReadError(ret, size, logStringArg);
  }

  FileType prefixToType(
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
}
