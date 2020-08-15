
#include <CubeRoverGround/Top/Components.hpp>

//Set base id section
void setRefIds(void){
	fatalHandler.setIdBase(1);
	rateGroup1Comp.setIdBase(21);
	pingRcvr.setIdBase(41);
	chanTlm.setIdBase(61);
	sendBuffComp.setIdBase(81);
	recvBuffComp.setIdBase(101);
	cmdDisp.setIdBase(121);
	prmDb.setIdBase(141);
	rateGroup2Comp.setIdBase(161);
	localization.setIdBase(181);
	txCmd.setIdBase(201);
	imgRec.setIdBase(221);
	rateGroup3Comp.setIdBase(241);
	fileUplink.setIdBase(261);
	rxData.setIdBase(281);
	fileUplinkBufferManager.setIdBase(301);
	decodeData.setIdBase(321);
	fatalAdapter.setIdBase(341);
	health.setIdBase(361);
	sockGndIf.setIdBase(381);
	fileDownlinkBufferManager.setIdBase(401);
	eventLogger.setIdBase(421);
	linuxTime.setIdBase(441);
	rateGroupDriverComp.setIdBase(461);
	blockDrv.setIdBase(481);
	fileDownlink.setIdBase(501);
	textLogger.setIdBase(521);
	cmdSeq.setIdBase(541);
	encodeCmd.setIdBase(564);
	pullCmd.setIdBase(584);
	pushData.setIdBase(604);
	imgProc.setIdBase(624);
	telemData.setIdBase(644);
}




void constructRefArchitecture(void) {  
	setRefIds();

    // Port connections
    //None
    localization.set_tlmOut_OutputPort(0, pushData.get_tlmIn_InputPort(0));
    //None
    localization.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    localization.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    localization.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    localization.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(12));
    //None
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(0, pullCmd.get_schedIn_InputPort(0));
    //None
    rateGroup1Comp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(2, pullCmd.get_schedIn_InputPort(0));
    //None
    rateGroup1Comp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(0, decodeData.get_schedIn_InputPort(0));
    //None
    rateGroup1Comp.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(7));
    //None
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(2, chanTlm.get_Run_InputPort(0));
    //None
    rateGroup1Comp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    rateGroup1Comp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    pingRcvr.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    pingRcvr.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(10));
    //None
    pingRcvr.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    pingRcvr.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    pingRcvr.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    pingRcvr.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    pingRcvr.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(4));
    //None
    chanTlm.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(0));
    //None
    chanTlm.set_PktSend_OutputPort(0, sockGndIf.get_downlinkPort_InputPort(0));
    //None
    txCmd.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    txCmd.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    txCmd.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    txCmd.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(2));
    //None
    sendBuffComp.set_Data_OutputPort(0, blockDrv.get_BufferIn_InputPort(0));
    //None
    sendBuffComp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    sendBuffComp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    sendBuffComp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    sendBuffComp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    sendBuffComp.set_ParamGet_OutputPort(0, prmDb.get_getPrm_InputPort(0));
    //None
    sendBuffComp.set_ParamSet_OutputPort(0, prmDb.get_setPrm_InputPort(0));
    //None
    sendBuffComp.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(18));
    //None
    imgRec.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    imgRec.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    imgRec.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    imgRec.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    imgRec.set_tlmOut_OutputPort(0, localization.get_tlmIn_InputPort(0));
    //None
    imgRec.set_tlmOut_OutputPort(0, pushData.get_tlmIn_InputPort(0));
    //None
    imgRec.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(16));
    //None
    recvBuffComp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    recvBuffComp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    recvBuffComp.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    recvBuffComp.set_ParamSet_OutputPort(0, prmDb.get_setPrm_InputPort(0));
    //None
    recvBuffComp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    recvBuffComp.set_ParamGet_OutputPort(0, prmDb.get_getPrm_InputPort(0));
    //None
    recvBuffComp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    recvBuffComp.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(15));
    //None
    rxData.set_tlmOut_OutputPort(0, decodeData.get_tlmIn_InputPort(0));
    //None
    rxData.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    rxData.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    rxData.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    rxData.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(6));
    //None
    decodeData.set_tlmOut_OutputPort(0, telemData.get_tlmIn_InputPort(0));
    //None
    decodeData.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    decodeData.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    decodeData.set_tlmOut_OutputPort(0, imgProc.get_tlmIn_InputPort(0));
    //None
    decodeData.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    decodeData.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    decodeData.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(10));
    //None
    cmdDisp.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    cmdDisp.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(1));
    //None
    cmdDisp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    cmdDisp.set_seqCmdStatus_OutputPort(0, cmdSeq.get_cmdResponseIn_InputPort(0));
    //None
    cmdDisp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    cmdDisp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    cmdDisp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(0, imgProc.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(1, eventLogger.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(2, txCmd.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(3, pullCmd.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(4, pingRcvr.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(5, encodeCmd.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(6, rxData.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(7, pushData.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(8, telemData.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(9, health.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(10, decodeData.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(11));
    //None
    cmdDisp.set_compCmdSend_OutputPort(11, cmdDisp.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(12, localization.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(13, prmDb.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(14, cmdSeq.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(15, recvBuffComp.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(16, imgRec.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(17, fileDownlink.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(18, sendBuffComp.get_CmdDisp_InputPort(0));
    //None
    prmDb.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    prmDb.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    prmDb.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    prmDb.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    prmDb.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(6));
    //None
    prmDb.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(13));
    //None
    rateGroup2Comp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(3, imgProc.get_schedIn_InputPort(0));
    //None
    rateGroup2Comp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    rateGroup2Comp.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(8));
    //None
    rateGroup2Comp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(0, cmdSeq.get_schedIn_InputPort(0));
    //None
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(1, sendBuffComp.get_SchedIn_InputPort(0));
    //None
    rateGroup2Comp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    cmdSeq.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    cmdSeq.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    cmdSeq.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    cmdSeq.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    cmdSeq.set_comCmdOut_OutputPort(0, cmdDisp.get_seqCmdBuff_InputPort(0));
    //None
    cmdSeq.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    cmdSeq.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(2));
    //None
    cmdSeq.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(14));
    //None
    rateGroup3Comp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    rateGroup3Comp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(0, health.get_Run_InputPort(0));
    //None
    rateGroup3Comp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(2, blockDrv.get_Sched_InputPort(0));
    //None
    rateGroup3Comp.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(9));
    //None
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(1, imgRec.get_schedIn_InputPort(0));
    //None
    rateGroup3Comp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    fileUplink.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    fileUplink.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    fileUplink.set_bufferSendOut_OutputPort(0, fileUplinkBufferManager.get_bufferSendIn_InputPort(0));
    //None
    fileUplink.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(5));
    //None
    fileUplink.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    encodeCmd.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    encodeCmd.set_tlmOut_OutputPort(0, txCmd.get_tlmIn_InputPort(0));
    //None
    encodeCmd.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    encodeCmd.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    encodeCmd.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    encodeCmd.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(5));
    //None
    fileUplinkBufferManager.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    fileUplinkBufferManager.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    fileUplinkBufferManager.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    fatalAdapter.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    fatalAdapter.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    fatalAdapter.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    health.set_PingSend_OutputPort(5, fileUplink.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(8, rateGroup2Comp.get_PingIn_InputPort(0));
    //None
    health.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    health.set_PingSend_OutputPort(11, blockDrv.get_PingIn_InputPort(0));
    //None
    health.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    health.set_PingSend_OutputPort(3, eventLogger.get_pingIn_InputPort(0));
    //None
    health.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    health.set_PingSend_OutputPort(9, rateGroup3Comp.get_PingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(0, chanTlm.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(1, cmdDisp.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(7, rateGroup1Comp.get_PingIn_InputPort(0));
    //None
    health.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    health.set_PingSend_OutputPort(2, cmdSeq.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(10, pingRcvr.get_PingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(6, prmDb.get_pingIn_InputPort(0));
    //None
    health.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    health.set_PingSend_OutputPort(4, fileDownlink.get_pingIn_InputPort(0));
    //None
    health.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(9));
    //None
    sockGndIf.set_fileUplinkBufferGet_OutputPort(0, fileUplinkBufferManager.get_bufferGetCallee_InputPort(0));
    //None
    sockGndIf.set_uplinkPort_OutputPort(0, cmdDisp.get_seqCmdBuff_InputPort(1));
    //None
    sockGndIf.set_fileDownlinkBufferSendOut_OutputPort(0, fileDownlinkBufferManager.get_bufferSendIn_InputPort(0));
    //None
    sockGndIf.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    sockGndIf.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    sockGndIf.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    sockGndIf.set_fileUplinkBufferSendOut_OutputPort(0, fileUplink.get_bufferSendIn_InputPort(0));
    //None
    fileDownlinkBufferManager.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    fileDownlinkBufferManager.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    fileDownlinkBufferManager.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    eventLogger.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(3));
    //None
    eventLogger.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    eventLogger.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    eventLogger.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    eventLogger.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    eventLogger.set_FatalAnnounce_OutputPort(0, fatalHandler.get_FatalReceive_InputPort(0));
    //None
    eventLogger.set_PktSend_OutputPort(0, sockGndIf.get_downlinkPort_InputPort(0));
    //None
    eventLogger.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(1));
    //None
    pullCmd.set_tlmOut_OutputPort(0, encodeCmd.get_tlmIn_InputPort(0));
    //None
    pullCmd.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    pullCmd.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    pullCmd.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    pullCmd.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(3));
    //None
    pushData.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    pushData.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    pushData.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    pushData.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(7));
    //None
    rateGroupDriverComp.set_CycleOut_OutputPort(1, rateGroup2Comp.get_CycleIn_InputPort(0));
    //None
    rateGroupDriverComp.set_CycleOut_OutputPort(2, rateGroup3Comp.get_CycleIn_InputPort(0));
    //None
    rateGroupDriverComp.set_CycleOut_OutputPort(0, rateGroup1Comp.get_CycleIn_InputPort(0));
    //None
    blockDrv.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    blockDrv.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(11));
    //None
    blockDrv.set_CycleOut_OutputPort(0, rateGroupDriverComp.get_CycleIn_InputPort(0));
    //None
    blockDrv.set_BufferOut_OutputPort(0, recvBuffComp.get_Data_InputPort(0));
    //None
    blockDrv.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    imgProc.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    imgProc.set_tlmOut_OutputPort(0, imgRec.get_tlmIn_InputPort(0));
    //None
    imgProc.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    imgProc.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    imgProc.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    imgProc.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(0));
    //None
    fileDownlink.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(4));
    //None
    fileDownlink.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    fileDownlink.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    fileDownlink.set_bufferSendOut_OutputPort(0, sockGndIf.get_fileDownlinkBufferSendIn_InputPort(0));
    //None
    fileDownlink.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    fileDownlink.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    fileDownlink.set_bufferGetCaller_OutputPort(0, fileDownlinkBufferManager.get_bufferGetCallee_InputPort(0));
    //None
    fileDownlink.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(17));
    //None
    telemData.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    telemData.set_tlmOut_OutputPort(0, localization.get_tlmIn_InputPort(0));
    //None
    telemData.set_tlmOut_OutputPort(0, pushData.get_tlmIn_InputPort(0));
    //None
    telemData.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    telemData.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    telemData.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(8));

}





