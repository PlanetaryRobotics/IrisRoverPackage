#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

#include <Fw/Buffer/BufferGetPortAc.hpp>
namespace Fw {


    namespace {

        class BufferGetPortBuffer : public Fw::SerializeBufferBase {

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

            U8 m_buff[InputBufferGetPort::SERIALIZED_SIZE];

        };

    }
    InputBufferGetPort::InputBufferGetPort(void) : 
        Fw::InputPortBase(), 
        m_func(0) {
    }
    
    void InputBufferGetPort::init(void) {
        Fw::InputPortBase::init();
    }

    void InputBufferGetPort::addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    // call virtual logging function for component
    Fw::Buffer InputBufferGetPort::invoke(U32 size) {

#if FW_PORT_TRACING == 1        
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        return this->m_func(this->m_comp, this->m_portNum, size);
    }

#if FW_PORT_SERIALIZATION == 1    
    void InputBufferGetPort::invokeSerial(Fw::SerializeBufferBase &buffer) {
        FW_ASSERT(0);
    }
#endif

OutputBufferGetPort::OutputBufferGetPort(void) :
            Fw::OutputPortBase(),
    m_port(0) {
}

void OutputBufferGetPort::init(void) {
    Fw::OutputPortBase::init();
}

void OutputBufferGetPort::addCallPort(InputBufferGetPort* callPort) {
    FW_ASSERT(callPort);
    
    this->m_port = callPort;
    this->m_connObj = callPort;
#if FW_PORT_SERIALIZATION == 1
    this->m_serPort = 0;
#endif
}

Fw::Buffer OutputBufferGetPort::invoke(U32 size) {
#if FW_PORT_TRACING == 1
    this->trace();
#endif

#if FW_PORT_SERIALIZATION            
    FW_ASSERT(this->m_port||this->m_serPort);
#else
    FW_ASSERT(this->m_port);
#endif

    return this->m_port->invoke(size);

} // end OutputBufferGetPort::invoke(...)

} // end namespace Fw
