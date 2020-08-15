/*
 * PolyPort.hpp
 *
 *  Created on: Saturday, 23 March 2019
 *  Author:     raewynd
 *
 */
#ifndef POLYPORT_HPP_
#define POLYPORT_HPP_

#include <cstring>
#include <cstdio>
#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Types/PolyType.hpp>
#include <Fw/Time/Time.hpp>


namespace Svc {

    typedef enum {
        MEASUREMENT_OK,  // Measurement was good
        MEASUREMENT_FAILURE,  // Failure to retrieve measurement
        MEASUREMENT_STALE,  // Measurement is stale
        MeasurementStatus_MAX
    } MeasurementStatus; //!< MeasurementStatus enumeration argument

    /// Input Poly port description
    /// 

    class InputPolyPort : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = sizeof(U32) + sizeof(NATIVE_INT_TYPE) + Fw::Time::SERIALIZED_SIZE + Fw::PolyType::SERIALIZED_SIZE //!< serialized size of port arguments
        };
        typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val); //!< port callback definition

        InputPolyPort(void); //!< constructor
        void init(void); //!< initialization function
        void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component
        void invoke(U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val); //!< invoke port interface
    protected:
    private:
        CompFuncPtr m_func; //!< pointer to port callback function
#if FW_PORT_SERIALIZATION == 1        
        void invokeSerial(Fw::SerializeBufferBase &buffer); //!< invoke the port with serialized arguments
#endif
};
    /// Input Poly port description
    /// 
    
    class OutputPolyPort : public Fw::OutputPortBase {
      public: 
        OutputPolyPort(void);
        void init(void);
        void addCallPort(InputPolyPort* callPort);
        void invoke(U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val);
      protected:
      private:
        InputPolyPort* m_port;
    };
} // end namespace Svc
#endif /* POLY_HPP_ */

