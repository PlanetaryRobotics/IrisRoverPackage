#include "CubeRoverNetworkManager.hpp"

CubeRoverNetworkManager :: CubeRoverNetworkManager(){
    m_wifiModuleDetected = false;
    m_wifiModuleTurnedOn = false;
    m_wifiModuleIdentified = false;
    m_powerSavingState = NOT_DEFINED;
    m_macAddressIdentified = false;
    m_powerSavingStateSet = false;
}

ErrorCode CubeRoverNetworkManager :: InitializeCubeRoverNetworkManager(){
    uint16_t tries;
    ErrorCode errorCode;

    // Check if the hardware can be detected
    errorCode = HelloSystem();
    if(errorCode != NO_ERROR) return errorCode;

    // Block until it timeouts
    tries = TRIES_EXECUTE_CALLBACK;
    while(tries > 0 && m_wifiModuleDetected == false){
        errorCode = ExecuteCallbacks();
        if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
        tries--;
    }

    // Failed to execute the command because likely module is not present
    if(!tries) return HARDWARE_FAILURE;

    // Reset system
    errorCode = ResetSystemWifi(SYSTEM_BOOT);
    if(errorCode != NO_ERROR) return errorCode;

    tries = TRIES_EXECUTE_CALLBACK;
    while(tries > 0 && m_wifiModuleIdentified == false){
        errorCode = ExecuteCallbacks();
        if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
        tries--;
    }

    // Get the mac address
    errorCode = GetMacAddress(WIFI);
    if(errorCode != NO_ERROR) return errorCode;

    tries = TRIES_EXECUTE_CALLBACK;
    while(tries > 0 && m_macAddressIdentified == false){
        errorCode = ExecuteCallbacks();
        if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
        tries--;
    }

    // Set the power saving state
    errorCode = SetPowerSavingState(LOW_LATENCY);
    if(errorCode != NO_ERROR) return errorCode;

    // Block until it timeouts or generate an error
    tries = TRIES_EXECUTE_CALLBACK;
    while(tries > 0 && m_powerSavingStateSet == false){
        errorCode = ExecuteCallbacks();
        if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
        tries--;
    }

    // Failed to execute the command
    if(!tries) return TIMEOUT;

    // Turn on the wifi module
    errorCode = TurnOnWifi();
    if(errorCode != NO_ERROR) return errorCode;

    // Block until it timeouts or generate an error
    tries = TRIES_EXECUTE_CALLBACK;
    while(tries > 0 && m_wifiModuleTurnedOn == false){
        errorCode = ExecuteCallbacks();
        if(errorCode != TRY_AGAIN && errorCode != NO_ERROR) return errorCode;
        tries--;
    }

    errorCode = ConfigureTcpIp(m_roverIpAddress,
                               m_roverMaskAddress,
                               m_udpGatewayAddress,
                               false /* do not use dhcp */);

    // Failed to execute the command
    if(!tries) return TIMEOUT;

    return NO_ERROR;
}

//--------------------------------------------------------------------------------------
// Implementation of virtual COMMAND callbacks from base class
//--------------------------------------------------------------------------------------
ErrorCode CubeRoverNetworkManager :: cb_CommandHelloSystem(){
    m_wifiModuleDetected = true;
    return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_CommandTurnOnWifi(const uint16_t result){
    m_wifiModuleTurnedOn = (result == NO_ERROR) ? true : false;
    return (ErrorCode) result;
}

ErrorCode CubeRoverNetworkManager :: cb_CommandSetPowerSavingState(const uint16_t result){
    m_powerSavingStateSet = true;
    return (ErrorCode) result;
}

ErrorCode CubeRoverNetworkManager :: cb_CommandConfigureTcpIp(const uint16_t result){

}


//--------------------------------------------------------------------------------------
// Implementation of virtual EVENT callbacks from base class
//--------------------------------------------------------------------------------------

ErrorCode CubeRoverNetworkManager :: cb_EventPowerSavingState(const PowerSavingState state){
    m_powerSavingState = state;
    return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_EventMacAddress(const HardwareInterface interface,
                                                        const HardwareAddress hwAddr){
    memcpy(m_macAddress,
           hwAddr,
           sizeof(m_macAddress));
    m_macAddressIdentified = true;
    return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_EventEndpointSyntaxError(const uint16_t result, const Endpoint endpoint){
    if(result != NO_ERROR){
        return (ErrorCode)result;
    }

    return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_EventBoot(const uint16_t major,
                                                   const uint16_t minor,
                                                   const uint16_t patch,
                                                   const uint16_t build,
                                                   const uint16_t bootloaderVersion,
                                                   const uint16_t tcpIpVersion,
                                                   const uint16_t hwVersion){
    // save versions if needed - but so far it seems no needed for final design
    m_wifiModuleIdentified = true;
    return NO_ERROR;
}




