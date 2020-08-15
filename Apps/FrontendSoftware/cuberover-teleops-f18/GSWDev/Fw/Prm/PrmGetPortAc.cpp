#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

#include <Fw/Prm/PrmGetPortAc.hpp>
namespace Fw {


    namespace {

        class PrmGetPortBuffer : public Fw::SerializeBufferBase {

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

            U8 m_buff[InputPrmGetPort::SERIALIZED_SIZE];

        };

    }
    InputPrmGetPort::InputPrmGetPort(void) : 
        Fw::InputPortBase(), 
        m_func(0) {
    }
    
    void InputPrmGetPort::init(void) {
        Fw::InputPortBase::init();
    }

    void InputPrmGetPort::addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    // call virtual logging function for component
    ParamValid InputPrmGetPort::invoke(FwPrmIdType id, ParamBuffer &val) {

#if FW_PORT_TRACING == 1        
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        return this->m_func(this->m_comp, this->m_portNum, id, val);
    }

#if FW_PORT_SERIALIZATION == 1    
    void InputPrmGetPort::invokeSerial(Fw::SerializeBufferBase &buffer) {
        FW_ASSERT(0);
    }
#endif

OutputPrmGetPort::OutputPrmGetPort(void) :
            Fw::OutputPortBase(),
    m_port(0) {
}

void OutputPrmGetPort::init(void) {
    Fw::OutputPortBase::init();
}

void OutputPrmGetPort::addCallPort(InputPrmGetPort* callPort) {
    FW_ASSERT(callPort);
    
    this->m_port = callPort;
    this->m_connObj = callPort;
#if FW_PORT_SERIALIZATION == 1
    this->m_serPort = 0;
#endif
}

ParamValid OutputPrmGetPort::invoke(FwPrmIdType id, ParamBuffer &val) {
#if FW_PORT_TRACING == 1
    this->trace();
#endif

#if FW_PORT_SERIALIZATION            
    FW_ASSERT(this->m_port||this->m_serPort);
#else
    FW_ASSERT(this->m_port);
#endif

    return this->m_port->invoke(id, val);

} // end OutputPrmGetPort::invoke(...)

} // end namespace Fw
