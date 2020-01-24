#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

#include <Fw/Cmd/CmdPortAc.hpp>
namespace Fw {


    namespace {

        class CmdPortBuffer : public Fw::SerializeBufferBase {

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

            U8 m_buff[InputCmdPort::SERIALIZED_SIZE];

        };

    }
    InputCmdPort::InputCmdPort(void) : 
        Fw::InputPortBase(), 
        m_func(0) {
    }
    
    void InputCmdPort::init(void) {
        Fw::InputPortBase::init();
    }

    void InputCmdPort::addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    // call virtual logging function for component
    void InputCmdPort::invoke(FwOpcodeType opCode, U32 cmdSeq, CmdArgBuffer &args) {

#if FW_PORT_TRACING == 1        
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        this->m_func(this->m_comp, this->m_portNum, opCode, cmdSeq, args);
    }

#if FW_PORT_SERIALIZATION == 1    
    void InputCmdPort::invokeSerial(Fw::SerializeBufferBase &buffer) {
        Fw::SerializeStatus _status;
#if FW_PORT_TRACING == 1
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);


        FwOpcodeType opCode;
        _status = buffer.deserialize(opCode);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));

        U32 cmdSeq;
        _status = buffer.deserialize(cmdSeq);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));

        CmdArgBuffer args;
        _status = buffer.deserialize(args);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));

        this->m_func(this->m_comp, this->m_portNum, opCode, cmdSeq, args);
    }
#endif

OutputCmdPort::OutputCmdPort(void) :
            Fw::OutputPortBase(),
    m_port(0) {
}

void OutputCmdPort::init(void) {
    Fw::OutputPortBase::init();
}

void OutputCmdPort::addCallPort(InputCmdPort* callPort) {
    FW_ASSERT(callPort);
    
    this->m_port = callPort;
    this->m_connObj = callPort;
#if FW_PORT_SERIALIZATION == 1
    this->m_serPort = 0;
#endif
}

void OutputCmdPort::invoke(FwOpcodeType opCode, U32 cmdSeq, CmdArgBuffer &args) {
#if FW_PORT_TRACING == 1
    this->trace();
#endif

#if FW_PORT_SERIALIZATION            
    FW_ASSERT(this->m_port||this->m_serPort);
#else
    FW_ASSERT(this->m_port);
#endif

    if (this->m_port) {
        this->m_port->invoke(opCode, cmdSeq, args);
#if FW_PORT_SERIALIZATION            
    } else if (this->m_serPort) {
        Fw::SerializeStatus status;
        CmdPortBuffer _buffer;
        status = _buffer.serialize(opCode);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        status = _buffer.serialize(cmdSeq);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        status = _buffer.serialize(args);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        this->m_serPort->invokeSerial(_buffer);
    }
#else
    }    
#endif

} // end OutputCmdPort::invoke(...)

} // end namespace Fw
