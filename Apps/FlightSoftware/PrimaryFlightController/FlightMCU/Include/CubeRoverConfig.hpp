#ifndef INCLUDE_CUBEROVERCONFIG_HPP_
#define INCLUDE_CUBEROVERCONFIG_HPP_

// Definition of rate group dividers
// Tick Interrupt @ 1kHz
#define RATEGROUP_DIVIDER_LOW_FREQ  1000 // 1Hz
#define RATEGROUP_DIVIDER_MED_FREQ  100  // 100Hz
#define RATEGROUP_DIVIDER_HI_FREQ   1    // 1kHz

// Definition of component instance ID, Affinity and queue depth
#define EVENT_LOGGER_ID             0
#define EVENT_LOGGER_AFF            1
#define EVENT_LOGGER_QUEUE_DEPTH    1

#define TLM_CHAN_ID                0
#define TLM_CHAN_AFF               31
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
#define RG_LOW_FREQ_AFF             30
#define RG_LOW_FREQ_QUEUE_DEPTH     1

#define RG_MED_FREQ_ID              1
#define RG_MED_FREQ_AFF             29
#define RG_MED_FREQ_QUEUE_DEPTH     1

#define RG_HI_FREQ_ID              2
#define RG_HI_FREQ_AFF             28
#define RG_HI_FREQ_QUEUE_DEPTH     1

#define BLK_DRV_ID                  0
#define BLK_DRV_AFF                 32
#define BLK_DRV_QUEUE_DEPTH         1

#define ACTIVE_LOGGER_ID            0
#define ACTIVE_LOGGER_AFF           26
#define ACTIVE_LOGGER_QUEUE_DEPTH   1

#define NAV_ID                      0
#define NAV_AFF                     25
#define NAV_QUEUE_DEPTH             1

#define WATCHDOG_ID                 0
#define WATCHDOG_AFF                26    // TODO: Good value for this
#define WATCHDOG_QUEUE_DEPTH        1

#define MIN_STACK_SIZE_WORDS        1024

// Configuration of I2C interfaces address and ports
// For the motor controllers
#define ALL_MOTOR_ADDR			        0x00
#define FRONT_LEFT_MC_I2C_ADDR      0x48
#define FRONT_RIGHT_MC_I2C_ADDR     0x49
#define REAR_LEFT_MC_I2C_ADDR       0x4A
#define REAR_RIGHT_MC_I2C_ADDR      0x4B
#define MOTOR_CONTROL_I2CREG     	i2cREG1

#define CUBEROVER_WHEEL_DIAMETER_CM			20.0f
#define CUBEROVER_COM_TO_WHEEL_CIRC_CM	78.54f
#define MOTOR_NB_PAIR_POLES							1.0f
#define MOTOR_GEAR_BOX_REDUCTION				5.0f
#define MAX_SPIN_DISTANCE								0x7FFFFFFF

// Navigation Configs
#define NAV_MOVE_BUFFER_LIMIT           5
#define NAV_LOW_RES_SIZE                5
#define NAV_MED_RES_SIZE                10

// Active Logger Min Bytes
#define MIN_STACK_SIZE_BYTES        1024

#endif /* INCLUDE_CUBEROVERCONFIG_HPP_ */
