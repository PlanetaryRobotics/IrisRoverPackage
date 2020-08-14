/*
 * LogTextPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef LOGTEXTPORT_HPP_
#define LOGTEXTPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Log/TextLogString.hpp>


namespace Fw {

    typedef enum {
        TEXT_LOG_FATAL = 1,  // A fatal non-recoverable event
        TEXT_LOG_WARNING_HI = 2,  // A serious but recoverable event
        TEXT_LOG_WARNING_LO = 3,  // A less serious but recoverable event
        TEXT_LOG_COMMAND = 4,  // An activity related to commanding
        TEXT_LOG_ACTIVITY_HI = 5,  // Important informational events
        TEXT_LOG_ACTIVITY_LO = 6,  // Less important informational events
        TEXT_LOG_DIAGNOSTIC = 7,  // Software diagnostic events
        TextLogSeverity_MAX
    } TextLogSeverity; //!< TextLogSeverity enumeration argument

    /// Input LogText port description
    /// 

    class InputLogTextPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = sizeof(FwEventIdType) + Fw::Time::SERIALIZED_SIZE + sizeof(NATIVE_INT_TYPE) + Fw::TextLogString::SERIALIZED_SIZE //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, TextLogSeverity severity, Fw::TextLogString &text); //!< port callback definition

        InputLogTextPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(FwEventIdType id, Fw::Time &timeTag, TextLogSeverity severity, Fw::TextLogString &text); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input LogText port description
    /// 
    
    class OutputLogTextPort : public Fw::OutputPortBase {
      public: 
        OutputLogTextPort(void);
        void init(void);
        void addCallPort(InputLogTextPort* callPort);
        void invoke(FwEventIdType id, Fw::Time &timeTag, TextLogSeverity severity, Fw::TextLogString &text);
      protected:
      private:
        InputLogTextPort* m_port;
    };
} // end namespace Fw
#endif /* LOGTEXT_HPP_ */

