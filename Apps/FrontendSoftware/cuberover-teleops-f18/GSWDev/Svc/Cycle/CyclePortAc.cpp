#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

#include <Svc/Cycle/CyclePortAc.hpp>
namespace Svc {


    namespace {

        class CyclePortBuffer : public Fw::SerializeBufferBase {

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

            U8 m_buff[InputCyclePort::SERIALIZED_SIZE];

        };

    }
    InputCyclePort::InputCyclePort(void) : 
        Fw::InputPortBase(), 
        m_func(0) {
    }
    
    void InputCyclePort::init(void) {
        Fw::InputPortBase::init();
    }

    void InputCyclePort::addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    // call virtual logging function for component
    void InputCyclePort::invoke(Svc::TimerVal &cycleStart) {

#if FW_PORT_TRACING == 1        
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        this->m_func(this->m_comp, this->m_portNum, cycleStart);
    }

#if FW_PORT_SERIALIZATION == 1    
    void InputCyclePort::invokeSerial(Fw::SerializeBufferBase &buffer) {
        Fw::SerializeStatus _status;
#if FW_PORT_TRACING == 1
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);


        Svc::TimerVal cycleStart;
        _status = buffer.deserialize(cycleStart);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));

        this->m_func(this->m_comp, this->m_portNum, cycleStart);
    }
#endif

OutputCyclePort::OutputCyclePort(void) :
            Fw::OutputPortBase(),
    m_port(0) {
}

void OutputCyclePort::init(void) {
    Fw::OutputPortBase::init();
}

void OutputCyclePort::addCallPort(InputCyclePort* callPort) {
    FW_ASSERT(callPort);
    
    this->m_port = callPort;
    this->m_connObj = callPort;
#if FW_PORT_SERIALIZATION == 1
    this->m_serPort = 0;
#endif
}

void OutputCyclePort::invoke(Svc::TimerVal &cycleStart) {
#if FW_PORT_TRACING == 1
    this->trace();
#endif

#if FW_PORT_SERIALIZATION            
    FW_ASSERT(this->m_port||this->m_serPort);
#else
    FW_ASSERT(this->m_port);
#endif

    if (this->m_port) {
        this->m_port->invoke(cycleStart);
#if FW_PORT_SERIALIZATION            
    } else if (this->m_serPort) {
        Fw::SerializeStatus status;
        CyclePortBuffer _buffer;
        status = _buffer.serialize(cycleStart);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        this->m_serPort->invokeSerial(_buffer);
    }
#else
    }    
#endif

} // end OutputCyclePort::invoke(...)

} // end namespace Svc
