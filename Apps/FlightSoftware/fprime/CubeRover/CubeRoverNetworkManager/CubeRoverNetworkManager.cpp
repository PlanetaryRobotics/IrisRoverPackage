#include "CubeRoverNetworkManager.hpp"


ErrorCode CubeRoverNetworkManager :: InitializeCubeRoverNetworkManager(){
    return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_EventEndpointSyntaxError(const uint16_t result, const Endpoint endpoint){
    return NO_ERROR;
}


ErrorCode CubeRoverNetworkManager :: cb_CommandHelloSystem(){
    return NO_ERROR;
}

ErrorCode CubeRoverNetworkManager :: cb_CommandTurnOnWifi(){
    return NO_ERROR;
}
