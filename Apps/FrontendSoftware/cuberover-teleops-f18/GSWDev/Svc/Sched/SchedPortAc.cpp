#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

#include <Svc/Sched/SchedPortAc.hpp>
namespace Svc {


    namespace {

        class SchedPortBuffer : public Fw::SerializeBufferBase {

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

            U8 m_buff[InputSchedPort::SERIALIZED_SIZE];

        };

    }
    InputSchedPort::InputSchedPort(void) : 
        Fw::InputPortBase(), 
        m_func(0) {
    }
    
    void InputSchedPort::init(void) {
        Fw::InputPortBase::init();
    }

    void InputSchedPort::addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    // call virtual logging function for component
    void InputSchedPort::invoke(NATIVE_UINT_TYPE context) {

#if FW_PORT_TRACING == 1        
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        this->m_func(this->m_comp, this->m_portNum, context);
    }

#if FW_PORT_SERIALIZATION == 1    
    void InputSchedPort::invokeSerial(Fw::SerializeBufferBase &buffer) {
        Fw::SerializeStatus _status;
#if FW_PORT_TRACING == 1
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);


        NATIVE_UINT_TYPE context;
        _status = buffer.deserialize(context);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));

        this->m_func(this->m_comp, this->m_portNum, context);
    }
#endif

OutputSchedPort::OutputSchedPort(void) :
            Fw::OutputPortBase(),
    m_port(0) {
}

void OutputSchedPort::init(void) {
    Fw::OutputPortBase::init();
}

void OutputSchedPort::addCallPort(InputSchedPort* callPort) {
    FW_ASSERT(callPort);
    
    this->m_port = callPort;
    this->m_connObj = callPort;
#if FW_PORT_SERIALIZATION == 1
    this->m_serPort = 0;
#endif
}

void OutputSchedPort::invoke(NATIVE_UINT_TYPE context) {
#if FW_PORT_TRACING == 1
    this->trace();
#endif

#if FW_PORT_SERIALIZATION            
    FW_ASSERT(this->m_port||this->m_serPort);
#else
    FW_ASSERT(this->m_port);
#endif

    if (this->m_port) {
        this->m_port->invoke(context);
#if FW_PORT_SERIALIZATION            
    } else if (this->m_serPort) {
        Fw::SerializeStatus status;
        SchedPortBuffer _buffer;
        status = _buffer.serialize(context);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        this->m_serPort->invokeSerial(_buffer);
    }
#else
    }    
#endif

} // end OutputSchedPort::invoke(...)

} // end namespace Svc
