#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <string.h>

#include <Drv/BlockDriver/BlockDriverComponentAc.hpp>
#include <Svc/GndIf/GndIfComponentAc.hpp>
#include <Svc/PassiveTextLogger/PassiveTextLoggerComponentAc.hpp>
#include <Svc/FileDownlink/FileDownlinkComponentAc.hpp>
#include <Svc/FatalHandler/FatalHandlerComponentAc.hpp>
#include <Svc/AssertFatalAdapter/AssertFatalAdapterComponentAc.hpp>
#include <Svc/ActiveLogger/ActiveLoggerComponentAc.hpp>
#include <Svc/Time/TimeComponentAc.hpp>
#include <CmuApp/RateCounter/RateCounterComponentAc.hpp>
#include <Svc/PrmDb/PrmDbComponentAc.hpp>
#include <Svc/FileUplink/FileUplinkComponentAc.hpp>
#include <Svc/TlmChan/TlmChanComponentAc.hpp>
#include <Svc/BufferManager/BufferManagerComponentAc.hpp>
#include <Svc/RateGroupDriver/RateGroupDriverComponentAc.hpp>
#include <Svc/CmdDispatcher/CommandDispatcherComponentAc.hpp>
#include <Svc/Health/HealthComponentAc.hpp>
#include <Svc/ActiveRateGroup/ActiveRateGroupComponentAc.hpp>
#include <Svc/CmdSequencer/CmdSequencerComponentAc.hpp>

#include <unistd.h>

using namespace std;

extern Fw::SimpleObjRegistry simpleReg;

// Component declarations
extern Svc::ActiveRateGroupImpl rateGroup3Comp("rateGroup3Comp");
extern Svc::FileUplinkImpl fileUplink("fileUplink");
extern Svc::BufferManagerImpl fileUplinkBufferManager("fileUplinkBufferManager");
extern Svc::FatalHandlerImpl fatalHandler("fatalHandler");
extern Svc::AssertFatalAdapterImpl fatalAdapter("fatalAdapter");
extern Svc::ActiveRateGroupImpl rateGroup1Comp("rateGroup1Comp");
extern Svc::TlmChanImpl chanTlm("chanTlm");
extern CmuApp::RateCounterImpl rateCounter("rateCounter");
extern Svc::HealthImpl health("health");
extern Svc::GndIfImpl sockGndIf("sockGndIf");
extern Svc::BufferManagerImpl fileDownlinkBufferManager("fileDownlinkBufferManager");
extern Svc::ActiveLoggerImpl eventLogger("eventLogger");
extern Svc::TimeImpl linuxTime("linuxTime");
extern Svc::CommandDispatcherImpl cmdDisp("cmdDisp");
extern Svc::PrmDbImpl prmDb("prmDb");
extern Svc::ActiveRateGroupImpl rateGroup2Comp("rateGroup2Comp");
extern Svc::RateGroupDriverImpl rateGroupDriverComp("rateGroupDriverComp");
extern Drv::BlockDriverImpl blockDrv("blockDrv");
extern Svc::CmdSequencerImpl cmdSeq("cmdSeq");
extern Svc::FileDownlinkImpl fileDownlink("fileDownlink");
extern Svc::PassiveTextLoggerImpl textLogger("textLogger");



extern "C" {
    void dumparch(void);
    void dumpobj(const char* objName);
}


void dumparch(void);

void dumpobj(const char* objName);

void setRef_Ids(void);

void RefInit(void);

void RefStart(void);

void RefRegister(void);

void constructRefArchitecture(void);

void exitTasks(void) ;