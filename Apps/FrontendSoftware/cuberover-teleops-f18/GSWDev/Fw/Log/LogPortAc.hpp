/*
 * LogPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef LOGPORT_HPP_
#define LOGPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Time/Time.hpp>


namespace Fw {

    typedef enum {
        LOG_FATAL = 1,  // A fatal non-recoverable event
        LOG_WARNING_HI = 2,  // A serious but recoverable event
        LOG_WARNING_LO = 3,  // A less serious but recoverable event
        LOG_COMMAND = 4,  // An activity related to commanding
        LOG_ACTIVITY_HI = 5,  // Important informational events
        LOG_ACTIVITY_LO = 6,  // Less important informational events
        LOG_DIAGNOSTIC = 7,  // Software diagnostic events
        LogSeverity_MAX
    } LogSeverity; //!< LogSeverity enumeration argument

    /// Input Log port description
    /// 

    class InputLogPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = sizeof(FwEventIdType) + Fw::Time::SERIALIZED_SIZE + sizeof(NATIVE_INT_TYPE) + LogBuffer::SERIALIZED_SIZE //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, LogSeverity severity, LogBuffer &args); //!< port callback definition

        InputLogPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(FwEventIdType id, Fw::Time &timeTag, LogSeverity severity, LogBuffer &args); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input Log port description
    /// 
    
    class OutputLogPort : public Fw::OutputPortBase {
      public: 
        OutputLogPort(void);
        void init(void);
        void addCallPort(InputLogPort* callPort);
        void invoke(FwEventIdType id, Fw::Time &timeTag, LogSeverity severity, LogBuffer &args);
      protected:
      private:
        InputLogPort* m_port;
    };
} // end namespace Fw
#endif /* LOG_HPP_ */

