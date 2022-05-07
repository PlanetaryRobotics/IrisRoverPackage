#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <string.h>

#include <Svc/GroundInterface/GroundInterfaceComponentAc.hpp>
#include <Drv/BlockDriver/BlockDriverComponentAc.hpp>
#include <Svc/ActiveRateGroup/ActiveRateGroupComponentAc.hpp>
#include <Svc/LinuxTime/CubeRoverTimeComponentAc.hpp>
#include <Svc/RateGroupDriver/RateGroupDriverComponentAc.hpp>
#include <Svc/CmdDispatcher/CommandDispatcherComponentAc.hpp>
#include <CubeRover/IMU/IMUComponentAc.hpp>
#include <Svc/TlmChan/TlmChanComponentAc.hpp>

#include <unistd.h>

using namespace std;

extern Fw::SimpleObjRegistry simpleReg;

// Component declarations
extern Drv::BlockDriverImpl blockDriver("blockDriver");
extern Svc::RateGroupDriverImpl rateGroupDriver("rateGroupDriver");
extern Svc::ActiveRateGroupImpl rateGroupLowFreq("rateGroupLowFreq");
extern Svc::ActiveRateGroupImpl rateGroupMedFreq("rateGroupMedFreq");
extern Svc::ActiveRateGroupImpl rateGroupHiFreq("rateGroupHiFreq");
extern Svc::CubeRoverTimeImpl cubeRoverTime("cubeRoverTime");
extern Svc::TlmChanImpl tlmChan("tlmChan");
extern Svc::CommandDispatcherImpl cmdDispatcher("cmdDispatcher");
extern CubeRover::IMUImpl IMU("IMU");



extern "C" {
    void dumparch(void);
    void dumpobj(const char* objName);
}


void dumparch(void);

void dumpobj(const char* objName);

void setRef_Ids(void);

void CubeRoverInit(void);

void CubeRoverStart(void);

void CubeRoverRegister(void);

void constructCubeRoverArchitecture(void);

void exitTasks(void) ;
