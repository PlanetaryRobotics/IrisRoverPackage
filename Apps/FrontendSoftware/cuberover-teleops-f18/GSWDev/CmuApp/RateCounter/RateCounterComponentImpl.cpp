// ====================================================================== 
// \title  RateCounterImpl.cpp
// \author sofiahurtado
// \brief  cpp file for RateCounter component implementation class
// ====================================================================== 


#include <CmuApp/RateCounter/RateCounterComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CmuApp {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  RateCounterComponentImpl ::
#if FW_OBJECT_NAMES == 1
    RateCounterComponentImpl(
        const char *const compName
    ) :
      RateCounterComponentBase(compName), /*comma needed */  
#else
    RateCounterComponentBaseImpl(void), /*comma needed*/ 
#endif
    m_count(0)
  {

  }

  void RateCounterComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    RateCounterComponentBase::init(queueDepth, instance);
  }

  RateCounterComponentImpl ::
    ~RateCounterComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void RateCounterComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    m_count++;
    if(m_count%10 == 0 ) 
    {
      this->log_ACTIVITY_LO_Rate_CounterLevelReached(m_count);
    }
  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------

  void RateCounterComponentImpl ::
    RateCounter_report_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    /* call handler that we have*/ 
    this->log_ACTIVITY_LO_Rate_CounterLevelReached(m_count);
  }

} // end namespace CmuApp
