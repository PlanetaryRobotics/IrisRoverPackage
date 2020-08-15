#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <string.h>

#include <CubeRoverGround/TelemData/TelemDataComponentAc.hpp>
#include <CubeRoverGround/TxCmd/TxCmdComponentAc.hpp>
#include <Svc/ActiveLogger/ActiveLoggerComponentAc.hpp>
#include <Ref/SendBuffApp/SendBuffComponentAc.hpp>
#include <Svc/CmdSequencer/CmdSequencerComponentAc.hpp>
#include <Svc/BufferManager/BufferManagerComponentAc.hpp>
#include <CubeRoverGround/PullCmd/PullCmdComponentAc.hpp>
#include <CubeRoverGround/ImgRec/ImgRecComponentAc.hpp>
#include <Svc/GndIf/GndIfComponentAc.hpp>
#include <CubeRoverGround/ImgProc/ImgProcComponentAc.hpp>
#include <Svc/PassiveTextLogger/PassiveTextLoggerComponentAc.hpp>
#include <Svc/FatalHandler/FatalHandlerComponentAc.hpp>
#include <Svc/ActiveRateGroup/ActiveRateGroupComponentAc.hpp>
#include <Drv/BlockDriver/BlockDriverComponentAc.hpp>
#include <CubeRoverGround/Localization/LocalizationComponentAc.hpp>
#include <Svc/CmdDispatcher/CommandDispatcherComponentAc.hpp>
#include <Svc/TlmChan/TlmChanComponentAc.hpp>
#include <Ref/PingReceiver/PingReceiverComponentAc.hpp>
#include <CubeRoverGround/DecodeData/DecodeDataComponentAc.hpp>
#include <CubeRoverGround/PushData/PushDataComponentAc.hpp>
#include <Svc/FileUplink/FileUplinkComponentAc.hpp>
#include <Svc/Health/HealthComponentAc.hpp>
#include <Svc/AssertFatalAdapter/AssertFatalAdapterComponentAc.hpp>
#include <Svc/Time/TimeComponentAc.hpp>
#include <Svc/PrmDb/PrmDbComponentAc.hpp>
#include <CubeRoverGround/EncodeCmd/EncodeCmdComponentAc.hpp>
#include <CubeRoverGround/RxData/RxDataComponentAc.hpp>
#include <Svc/FileDownlink/FileDownlinkComponentAc.hpp>
#include <Svc/RateGroupDriver/RateGroupDriverComponentAc.hpp>
#include <Ref/RecvBuffApp/RecvBuffComponentAc.hpp>

#include <unistd.h>

using namespace std;

extern Fw::SimpleObjRegistry simpleReg;

// Component declarations
extern CubeRoverGround::LocalizationImpl localization("localization");
extern Svc::FatalHandlerImpl fatalHandler("fatalHandler");
extern Svc::ActiveRateGroupImpl rateGroup1Comp("rateGroup1Comp");
extern Ref::PingReceiverImpl pingRcvr("pingRcvr");
extern Svc::TlmChanImpl chanTlm("chanTlm");
extern CubeRoverGround::TxCmdImpl txCmd("txCmd");
extern Ref::SendBuffImpl sendBuffComp("sendBuffComp");
extern CubeRoverGround::ImgRecImpl imgRec("imgRec");
extern Ref::RecvBuffImpl recvBuffComp("recvBuffComp");
extern CubeRoverGround::RxDataImpl rxData("rxData");
extern CubeRoverGround::DecodeDataImpl decodeData("decodeData");
extern Svc::CommandDispatcherImpl cmdDisp("cmdDisp");
extern Svc::PrmDbImpl prmDb("prmDb");
extern Svc::ActiveRateGroupImpl rateGroup2Comp("rateGroup2Comp");
extern Svc::CmdSequencerImpl cmdSeq("cmdSeq");
extern Svc::ActiveRateGroupImpl rateGroup3Comp("rateGroup3Comp");
extern Svc::FileUplinkImpl fileUplink("fileUplink");
extern CubeRoverGround::EncodeCmdImpl encodeCmd("encodeCmd");
extern Svc::BufferManagerImpl fileUplinkBufferManager("fileUplinkBufferManager");
extern Svc::AssertFatalAdapterImpl fatalAdapter("fatalAdapter");
extern Svc::HealthImpl health("health");
extern Svc::GndIfImpl sockGndIf("sockGndIf");
extern Svc::BufferManagerImpl fileDownlinkBufferManager("fileDownlinkBufferManager");
extern Svc::ActiveLoggerImpl eventLogger("eventLogger");
extern Svc::TimeImpl linuxTime("linuxTime");
extern CubeRoverGround::PullCmdImpl pullCmd("pullCmd");
extern CubeRoverGround::PushDataImpl pushData("pushData");
extern Svc::RateGroupDriverImpl rateGroupDriverComp("rateGroupDriverComp");
extern Drv::BlockDriverImpl blockDrv("blockDrv");
extern CubeRoverGround::ImgProcImpl imgProc("imgProc");
extern Svc::FileDownlinkImpl fileDownlink("fileDownlink");
extern Svc::PassiveTextLoggerImpl textLogger("textLogger");
extern CubeRoverGround::TelemDataImpl telemData("telemData");



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