// ----------------------------------------------------------------------
//
// ComLogger.cpp
//
// ----------------------------------------------------------------------

#include <CubeRover/ComLogger/ComLogger.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Os/ValidateFile.hpp>
#include <Os/FreeRTOS/FRAM_File.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  Os::FRAM_File::FRAM_File file_system;

  ComLogger ::
#if FW_OBJECT_NAMES == 1
    ComLogger(const char* compName, const char* incomingFilePrefix, U32 maxFileSize, bool storeBufferLength) :
    ComLoggerComponentBase(compName), 
#else
    ComLogger(const char* incomingFilePrefix, U32 maxFileSize, bool storeBufferLength) :
#endif
    writeErrorOccured(false),
    readErrorOccured(false),
    openErrorOccured(false)
  {

  }

  void ComLogger :: 
    init(
      //NATIVE_INT_TYPE queueDepth, //!< The queue depth
      NATIVE_INT_TYPE instance //!< The instance number
    )
  {
    ComLoggerComponentBase::init(instance);
  }

  /*ComLogger ::
    ~ComLogger(void)
  {
  }*/

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
    FW_ASSERT(portNum == 0);

    // Get length of buffer:
    U32 size32 = data.getBuffLength();
    // ComLogger only writes 8-bit sizes to save space on disk:
    // TODO: SEE WHEN WE WOULD EVER HAVE SIZES GREATER THAN UINT8_T
    FW_ASSERT(size32 < 65536, size32);
    U8 size = size32 & 0xFF;

    // Create time value
    Fw::Time timestamp = getTime();

    // Write value to FRAM
    Os::FRAM_File::Status Write_Error = file_system.write((void*)&data, size, timestamp.getSeconds());

    //TODO: CHECK ERROR
    //TODO: Update write telemetry
  }

  void ComLogger ::
    SendSetofLogs_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 start,
        U32 end
    )
  {
  // Create ComBuffer to store read data
    Fw::ComBuffer data;

    Os::FRAM_File::Status Read_Error = file_system.read((void *) &data, start, end);

    //TODO: CHECK ERROR

    if (this->isConnected_GndOut_OutputPort(0)) 
    {
      this->GndOut_out(0, data,0);
    }

    // Send Output buffer to ground
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    //TODO: Update read telemetry
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
  
}
