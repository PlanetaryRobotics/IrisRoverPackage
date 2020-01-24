#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

#include <Fw/Log/LogTextPortAc.hpp>
namespace Fw {


    namespace {

        class LogTextPortBuffer : public Fw::SerializeBufferBase {

            public:
                NATIVE_UINT_TYPE getBuffCapacity(void) const {
                    return sizeof(m_buff);
                }

                U8* getBuffAddr(void) {
                    return m_buff;
                }

                const U8* getBuffAddr(void) const {
                    return m_buff;
                }

        private:

            U8 m_buff[InputLogTextPort::SERIALIZED_SIZE];

        };

    }
    InputLogTextPort::InputLogTextPort(void) : 
        Fw::InputPortBase(), 
        m_func(0) {
    }
    
    void InputLogTextPort::init(void) {
        Fw::InputPortBase::init();
    }

    void InputLogTextPort::addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    // call virtual logging function for component
    void InputLogTextPort::invoke(FwEventIdType id, Fw::Time &timeTag, TextLogSeverity severity, Fw::TextLogString &text) {

#if FW_PORT_TRACING == 1        
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        this->m_func(this->m_comp, this->m_portNum, id, timeTag, severity, text);
    }

#if FW_PORT_SERIALIZATION == 1    
    void InputLogTextPort::invokeSerial(Fw::SerializeBufferBase &buffer) {
        Fw::SerializeStatus _status;
#if FW_PORT_TRACING == 1
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);


        FwEventIdType id;
        _status = buffer.deserialize(id);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));

        Fw::Time timeTag;
        _status = buffer.deserialize(timeTag);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));
        FwEnumStoreType severity_val;
        _status = buffer.deserialize(severity_val);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));
        TextLogSeverity severity = static_cast<TextLogSeverity>(severity_val);

        Fw::TextLogString text;
        _status = buffer.deserialize(text);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));

        this->m_func(this->m_comp, this->m_portNum, id, timeTag, severity, text);
    }
#endif

OutputLogTextPort::OutputLogTextPort(void) :
            Fw::OutputPortBase(),
    m_port(0) {
}

void OutputLogTextPort::init(void) {
    Fw::OutputPortBase::init();
}

void OutputLogTextPort::addCallPort(InputLogTextPort* callPort) {
    FW_ASSERT(callPort);
    
    this->m_port = callPort;
    this->m_connObj = callPort;
#if FW_PORT_SERIALIZATION == 1
    this->m_serPort = 0;
#endif
}

void OutputLogTextPort::invoke(FwEventIdType id, Fw::Time &timeTag, TextLogSeverity severity, Fw::TextLogString &text) {
#if FW_PORT_TRACING == 1
    this->trace();
#endif

#if FW_PORT_SERIALIZATION            
    FW_ASSERT(this->m_port||this->m_serPort);
#else
    FW_ASSERT(this->m_port);
#endif

    if (this->m_port) {
        this->m_port->invoke(id, timeTag, severity, text);
#if FW_PORT_SERIALIZATION            
    } else if (this->m_serPort) {
        Fw::SerializeStatus status;
        LogTextPortBuffer _buffer;
        status = _buffer.serialize(id);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        status = _buffer.serialize(timeTag);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        status = _buffer.serialize(static_cast<NATIVE_INT_TYPE>(severity));
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        status = _buffer.serialize(text);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        this->m_serPort->invokeSerial(_buffer);
    }
#else
    }    
#endif

} // end OutputLogTextPort::invoke(...)

} // end namespace Fw
