#ifndef INCLUDE_CUBEROVERCONFIG_HPP_
#define INCLUDE_CUBEROVERCONFIG_HPP_

// Definition of rate group dividers
#define RATEGROUP_DIVIDER_LOW_FREQ  4
#define RATEGROUP_DIVIDER_MED_FREQ  2
#define RATEGROUP_DIVIDER_HI_FREQ   1

// Definition of component instance ID, Affinity and queue depth
#define EVENT_LOGGER_ID             0
#define EVENT_LOGGER_AFF            1
#define EVENT_LOGGER_QUEUE_DEPTH    1

#define TLM_CHAN_ID                0
#define TLM_CHAN_AFF               0
#define TLM_CHAN_QUEUE_DEPTH       1

#define PARAM_DB_ID                0
#define PARAM_DB_AFF               0
#define PARAM_DB_QUEUE_DEPTH       1

#define CMD_DISP_ID                0
#define CMD_DISP_AFF               2
#define CMD_DISP_QUEUE_DEPTH       1

#define BLOCK_DRV_ID                0
#define BLOCK_DRV_AFF               0
#define BLOCK_DRV_QUEUE_DEPTH       1

#define RG_LOW_FREQ_ID              0
#define RG_LOW_FREQ_AFF             0
#define RG_LOW_FREQ_QUEUE_DEPTH     1

#define RG_MED_FREQ_ID              0
#define RG_MED_FREQ_AFF             0
#define RG_MED_FREQ_QUEUE_DEPTH     1

#define RG_HI_FREQ_ID              0
#define RG_HI_FREQ_AFF             0
#define RG_HI_FREQ_QUEUE_DEPTH     1
#endif /* INCLUDE_CUBEROVERCONFIG_HPP_ */
