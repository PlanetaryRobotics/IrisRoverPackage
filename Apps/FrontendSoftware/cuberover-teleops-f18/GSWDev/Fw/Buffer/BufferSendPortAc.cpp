#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

#include <Fw/Buffer/BufferSendPortAc.hpp>
namespace Fw {


    namespace {

        class BufferSendPortBuffer : public Fw::SerializeBufferBase {

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

            U8 m_buff[InputBufferSendPort::SERIALIZED_SIZE];

        };

    }
    InputBufferSendPort::InputBufferSendPort(void) : 
        Fw::InputPortBase(), 
        m_func(0) {
    }
    
    void InputBufferSendPort::init(void) {
        Fw::InputPortBase::init();
    }

    void InputBufferSendPort::addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    // call virtual logging function for component
    void InputBufferSendPort::invoke(Fw::Buffer &fwBuffer) {

#if FW_PORT_TRACING == 1        
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        this->m_func(this->m_comp, this->m_portNum, fwBuffer);
    }

#if FW_PORT_SERIALIZATION == 1    
    void InputBufferSendPort::invokeSerial(Fw::SerializeBufferBase &buffer) {
        Fw::SerializeStatus _status;
#if FW_PORT_TRACING == 1
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);


        Fw::Buffer fwBuffer;
        _status = buffer.deserialize(fwBuffer);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));

        this->m_func(this->m_comp, this->m_portNum, fwBuffer);
    }
#endif

OutputBufferSendPort::OutputBufferSendPort(void) :
            Fw::OutputPortBase(),
    m_port(0) {
}

void OutputBufferSendPort::init(void) {
    Fw::OutputPortBase::init();
}

void OutputBufferSendPort::addCallPort(InputBufferSendPort* callPort) {
    FW_ASSERT(callPort);
    
    this->m_port = callPort;
    this->m_connObj = callPort;
#if FW_PORT_SERIALIZATION == 1
    this->m_serPort = 0;
#endif
}

void OutputBufferSendPort::invoke(Fw::Buffer &fwBuffer) {
#if FW_PORT_TRACING == 1
    this->trace();
#endif

#if FW_PORT_SERIALIZATION            
    FW_ASSERT(this->m_port||this->m_serPort);
#else
    FW_ASSERT(this->m_port);
#endif

    if (this->m_port) {
        this->m_port->invoke(fwBuffer);
#if FW_PORT_SERIALIZATION            
    } else if (this->m_serPort) {
        Fw::SerializeStatus status;
        BufferSendPortBuffer _buffer;
        status = _buffer.serialize(fwBuffer);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        this->m_serPort->invokeSerial(_buffer);
    }
#else
    }    
#endif

} // end OutputBufferSendPort::invoke(...)

} // end namespace Fw
