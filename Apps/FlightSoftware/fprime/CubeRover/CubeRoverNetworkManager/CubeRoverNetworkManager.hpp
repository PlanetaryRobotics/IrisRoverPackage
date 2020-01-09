#ifndef __CUBEROVER_NETWORK_MANAGER__
#define __CUBEROVER_NETWORK_MANAGER__

#include "CubeRover/Wf121/Wf121.hpp"

using namespace Wf121;

class CubeRoverNetworkManager : public Wf121Driver{
public:

  // Callback event
  ErrorCode cb_EventEndpointSyntaxError(const uint16_t result, const Endpoint endpoint);

  // Callback command
  ErrorCode cb_CommandHelloSystem();
private:
};

#endif
