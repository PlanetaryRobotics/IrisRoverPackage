/*
 * Topology.hpp
 *
 *  Created on: Oct 2, 2019
 *      Author: cedric
 */

#ifndef F_PRIME_CUBEROVER_TOP_TOPOLOGY_HPP_
#define F_PRIME_CUBEROVER_TOP_TOPOLOGY_HPP_

#include "CubeRoverTopologyAppAc.hpp"

#define EVENT_LOGGER_ID             0
#define EVENT_LOGGER_AFF            1
#define EVENT_LOGGER_QUEUE_DEPTH    1

#define TLM_CHAN_ID                0
#define TLM_CHAN_AFF               0
#define TLM_CHAN_QUEUE_DEPTH	   1

#define PARAM_DB_ID                0
#define PARAM_DB_AFF               0
#define PARAM_DB_QUEUE_DEPTH	   1

#define CMD_DISP_ID                0
#define CMD_DISP_AFF               2
#define CMD_DISP_QUEUE_DEPTH       1

#define BLOCK_DRV_ID                0
#define BLOCK_DRV_AFF               0
#define BLOCK_DRV_QUEUE_DEPTH       1

/**
 * @brief      Construct application
 */
void constructApp(void);
void run1cycle(void);



#endif /* F_PRIME_CUBEROVER_TOP_TOPOLOGY_HPP_ */
