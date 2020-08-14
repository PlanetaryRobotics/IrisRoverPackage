#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

#include <Fw/Cmd/CmdResponsePortAc.hpp>
namespace Fw {


    namespace {

        class CmdResponsePortBuffer : public Fw::SerializeBufferBase {

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

            U8 m_buff[InputCmdResponsePort::SERIALIZED_SIZE];

        };

    }
    InputCmdResponsePort::InputCmdResponsePort(void) : 
        Fw::InputPortBase(), 
        m_func(0) {
    }
    
    void InputCmdResponsePort::init(void) {
        Fw::InputPortBase::init();
    }

    void InputCmdResponsePort::addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    // call virtual logging function for component
    void InputCmdResponsePort::invoke(FwOpcodeType opCode, U32 cmdSeq, CommandResponse response) {

#if FW_PORT_TRACING == 1        
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        this->m_func(this->m_comp, this->m_portNum, opCode, cmdSeq, response);
    }

#if FW_PORT_SERIALIZATION == 1    
    void InputCmdResponsePort::invokeSerial(Fw::SerializeBufferBase &buffer) {
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
        FwEnumStoreType response_val;
        _status = buffer.deserialize(response_val);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));
        CommandResponse response = static_cast<CommandResponse>(response_val);

        this->m_func(this->m_comp, this->m_portNum, opCode, cmdSeq, response);
    }
#endif

OutputCmdResponsePort::OutputCmdResponsePort(void) :
            Fw::OutputPortBase(),
    m_port(0) {
}

void OutputCmdResponsePort::init(void) {
    Fw::OutputPortBase::init();
}

void OutputCmdResponsePort::addCallPort(InputCmdResponsePort* callPort) {
    FW_ASSERT(callPort);
    
    this->m_port = callPort;
    this->m_connObj = callPort;
#if FW_PORT_SERIALIZATION == 1
    this->m_serPort = 0;
#endif
}

void OutputCmdResponsePort::invoke(FwOpcodeType opCode, U32 cmdSeq, CommandResponse response) {
#if FW_PORT_TRACING == 1
    this->trace();
#endif

#if FW_PORT_SERIALIZATION            
    FW_ASSERT(this->m_port||this->m_serPort);
#else
    FW_ASSERT(this->m_port);
#endif

    if (this->m_port) {
        this->m_port->invoke(opCode, cmdSeq, response);
#if FW_PORT_SERIALIZATION            
    } else if (this->m_serPort) {
        Fw::SerializeStatus status;
        CmdResponsePortBuffer _buffer;
        status = _buffer.serialize(opCode);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        status = _buffer.serialize(cmdSeq);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        status = _buffer.serialize(static_cast<NATIVE_INT_TYPE>(response));
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        this->m_serPort->invokeSerial(_buffer);
    }
#else
    }    
#endif

} // end OutputCmdResponsePort::invoke(...)

} // end namespace Fw
