#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

#include <Svc/PolyIf/PolyPortAc.hpp>
namespace Svc {


    namespace {

        class PolyPortBuffer : public Fw::SerializeBufferBase {

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

            U8 m_buff[InputPolyPort::SERIALIZED_SIZE];

        };

    }
    InputPolyPort::InputPolyPort(void) : 
        Fw::InputPortBase(), 
        m_func(0) {
    }
    
    void InputPolyPort::init(void) {
        Fw::InputPortBase::init();
    }

    void InputPolyPort::addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    // call virtual logging function for component
    void InputPolyPort::invoke(U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val) {

#if FW_PORT_TRACING == 1        
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        this->m_func(this->m_comp, this->m_portNum, entry, status, time, val);
    }

#if FW_PORT_SERIALIZATION == 1    
    void InputPolyPort::invokeSerial(Fw::SerializeBufferBase &buffer) {
        Fw::SerializeStatus _status;
#if FW_PORT_TRACING == 1
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);


        U32 entry;
        _status = buffer.deserialize(entry);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));
        FwEnumStoreType status_val;
        _status = buffer.deserialize(status_val);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));
        MeasurementStatus status = static_cast<MeasurementStatus>(status_val);

        Fw::Time time;
        _status = buffer.deserialize(time);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));

        Fw::PolyType val;
        _status = buffer.deserialize(val);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == _status,static_cast<AssertArg>(_status));

        this->m_func(this->m_comp, this->m_portNum, entry, status, time, val);
    }
#endif

OutputPolyPort::OutputPolyPort(void) :
            Fw::OutputPortBase(),
    m_port(0) {
}

void OutputPolyPort::init(void) {
    Fw::OutputPortBase::init();
}

void OutputPolyPort::addCallPort(InputPolyPort* callPort) {
    FW_ASSERT(callPort);
    
    this->m_port = callPort;
    this->m_connObj = callPort;
#if FW_PORT_SERIALIZATION == 1
    this->m_serPort = 0;
#endif
}

void OutputPolyPort::invoke(U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val) {
#if FW_PORT_TRACING == 1
    this->trace();
#endif

#if FW_PORT_SERIALIZATION            
    FW_ASSERT(this->m_port||this->m_serPort);
#else
    FW_ASSERT(this->m_port);
#endif

    if (this->m_port) {
        this->m_port->invoke(entry, status, time, val);
#if FW_PORT_SERIALIZATION            
    } else if (this->m_serPort) {
        Fw::SerializeStatus status;
        PolyPortBuffer _buffer;
        status = _buffer.serialize(entry);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        status = _buffer.serialize(static_cast<NATIVE_INT_TYPE>(status));
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        status = _buffer.serialize(time);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        status = _buffer.serialize(val);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<AssertArg>(status));

        this->m_serPort->invokeSerial(_buffer);
    }
#else
    }    
#endif

} // end OutputPolyPort::invoke(...)

} // end namespace Svc
