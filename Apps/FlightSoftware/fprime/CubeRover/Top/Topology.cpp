#include <Os/Task.hpp>
#include <Os/Log.hpp>
#include <HAL/include/FreeRTOS.h>
#include <Include/CubeRoverConfig.hpp> // PrimaryFlightController/FlightMCU

#include "Topology.hpp"

// ---------------------------------------------------------------------------
// Block Driver Component
// Block driver generates trigger signal for rate group driver
Drv::BlockDriverImpl blockDriver(
#if FW_OBJECT_NAMES == 1
    "BlockDriver"
#endif
);

// ---------------------------------------------------------------------------
// Rate Group Driver Component
// That array sets the frequency divider for the rate groups
static NATIVE_INT_TYPE rgDivs[] = {RATEGROUP_DIVIDER_HI_FREQ,
                                   RATEGROUP_DIVIDER_MED_FREQ,
                                   RATEGROUP_DIVIDER_LOW_FREQ};
Svc::RateGroupDriverImpl rateGroupDriver(
#if FW_OBJECT_NAMES == 1
    "RateGroupDriver",
#endif
    rgDivs, FW_NUM_ARRAY_ELEMENTS(rgDivs));

// ---------------------------------------------------------------------------
// Rate group - Low Frequency tasks
static NATIVE_UINT_TYPE rgLoFreqContext[] = {0, 0, 0, 0};
Svc::ActiveRateGroupImpl rateGroupLowFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupLowFreq",
#endif
    rgLoFreqContext, FW_NUM_ARRAY_ELEMENTS(rgLoFreqContext));

// ---------------------------------------------------------------------------
// Rate group - Medium Frequency tasks
static NATIVE_UINT_TYPE rgMedFreqContext[] = {0, 0, 0, 0};
Svc::ActiveRateGroupImpl rateGroupMedFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupMedFreq",
#endif
    rgMedFreqContext, FW_NUM_ARRAY_ELEMENTS(rgMedFreqContext));

// ---------------------------------------------------------------------------
// Rate group - High Frequency tasks
static NATIVE_UINT_TYPE rgHiFreqContext[] = {0, 0, 0, 0};
Svc::ActiveRateGroupImpl rateGroupHiFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupHiFreq",
#endif
    rgHiFreqContext, FW_NUM_ARRAY_ELEMENTS(rgHiFreqContext));

// ---------------------------------------------------------------------------
// Time - contains current CubeRover Time used for time stamping events
Svc::CubeRoverTimeImpl cubeRoverTime(
#if FW_OBJECT_NAMES == 1
    "CubeRoverTime"
#endif
);

// ---------------------------------------------------------------------------
// Telemetric channel component used to centralized of telemetric data
Svc::TlmChanImpl tlmChan(
#if FW_OBJECT_NAMES == 1
    "TlmChan"
#endif
);

// ---------------------------------------------------------------------------
// command dispatcher component used to dispatch commands
Svc::CommandDispatcherImpl cmdDispatcher(
#if FW_OBJECT_NAMES == 1
    "CmdDispatcher"
#endif
);

// --------------------------------------------------------------------------
CubeRover::IMUComponentImpl IMU(
#if FW_OBJECT_NAMES == 1
    "IMU"
#endif
);

// --------------------------------------------------------------------------
CubeRover::MotorControlComponentImpl motorControl(
#if FW_OBJECT_NAMES == 1
    "MotorControl"
#endif
);

// --------------------------------------------------------------------------
CubeRover::NavigationComponentImpl navigation(
#if FW_OBJECT_NAMES == 1
    "Navigation"
#endif
);

// --------------------------------------------------------------------------
CubeRover::GroundInterfaceComponentImpl groundInterface(
#if FW_OBJECT_NAMES == 1
    "GroundInterface"
#endif
);

// ---------------------------------------------------------------------------
// Active Logger data component used to log system events
Svc::ActiveLoggerImpl activeLogger(
#if FW_OBJECT_NAMES == 1
    "ActiveLogger"
#endif
);

// --------------------------------------------------------------------------
CubeRover::NetworkManagerComponentImpl networkManager(
#if FW_OBJECT_NAMES == 1
    "NetworkManager"
#endif
);

// ---------------------------------------------------------------------------
CubeRover::CameraComponentImpl camera(
#if FW_OBJECT_NAMES == 1
    "Camera"
#endif
);

// ---------------------------------------------------------------------------
Svc::HealthImpl health(
#if FW_OBJECT_NAMES == 1
    "Health"
#endif
);

// ---------------------------------------------------------------------------
CubeRover::WatchDogInterfaceComponentImpl watchDogInterface(
#if FW_OBJECT_NAMES == 1
    "Watchdog"
#endif
);

/**
 * @brief      Run 1 cycle (debug)
 */
void run1cycle(void)
{
  // blockDriver.callIsr();
  Svc::TimerVal timer;
  timer.take();
  rateGroupDriver.get_CycleIn_InputPort(0)->invoke(timer);
  // TODO: [CWC] Investigate if we should go back to invoking blockDriver.
}

/**
 * @brief      Construct the F-prime application
 */
void constructApp(void)
{
  // Initialize the block driver (active)
  blockDriver.init(BLK_DRV_QUEUE_DEPTH);

  // Initialize rate group driver driver (passive)
  rateGroupDriver.init();

  // Initialize rate group components (active)
  rateGroupLowFreq.init(RG_LOW_FREQ_QUEUE_DEPTH, RG_LOW_FREQ_ID);
  rateGroupMedFreq.init(RG_MED_FREQ_QUEUE_DEPTH, RG_MED_FREQ_ID);
  rateGroupHiFreq.init(RG_HI_FREQ_QUEUE_DEPTH, RG_HI_FREQ_ID);

  // Initialize the telemetry channel component (active)
  tlmChan.init(TLM_CHAN_QUEUE_DEPTH, TLM_CHAN_ID);

  // Initialize the CommandDispatcher component (active)
  cmdDispatcher.init(CMD_DISP_QUEUE_DEPTH, CMD_DISP_ID);

  // Initialize cubeRover time component (passive)
  cubeRoverTime.init(0);

  // Initialize the active logger component (active)
  // TODO: This hasn't been started yet
  activeLogger.init(ACTIVE_LOGGER_QUEUE_DEPTH, ACTIVE_LOGGER_ID);

  // Initialize the watchdog interface component (active)
  watchDogInterface.init(1,  /*Queue Depth*/
                         0); /*Instance Number*/

  // Initialize the health component (queued)
  // health.init(25,                   /*Queue Depth*/
  //            0);                   /*Instance Number*/

  // Initialize the ground interface (passive)
  groundInterface.init();

  // Initialize the IMU interface (passive)
  IMU.init();

  // Initialize the ground interface (passive)
  networkManager.init();

  // Initialize the camera (passive)
  camera.init();

  // Initialize the Motor control interface (passive)
  motorControl.init();

  // Initialize the navigation component (active)
  navigation.init(NAV_QUEUE_DEPTH, NAV_ID);

  // Construct the application and make all connections between components
  constructCubeRoverArchitecture();

  // Register Health Commands
  // health.regCommands();

  // Register WatchDog Interface Commands
  watchDogInterface.regCommands();

  // Register Camera Commands
  camera.regCommands();

  // Register Camera Commands
  navigation.regCommands();

  groundInterface.regCommands();

  // Set Health Ping Entries
  // TODO: [CWC] Look into why all this is commented and if we should reactivate it.
  // **** THIS IS WHERE YOU CAN ADD ANY COMPONENTS THAT HAVE HEALTH PINGS ****
  // Svc::HealthImpl::PingEntry pingEntries[] = {
  // {3, 5, name.getObjName()},
  // 3 -> number of cycles before WARNING
  // 5 -> number of cycles before FATAL
  // name.getObjName() -> the name of the entry where "name" is replace with component name

  /*    Start of Ping Entry List (Please let Alec know if changed/added to)

      {3, 5, watchDogInterface.getObjName()},    //0
      {3, 5, navigation.getObjName()},    //1
      {3, 5, tlmChan.getObjName()},    //2
      {3, 5, activeLogger.getObjName()},    //3
      {3, 5, comLogger.getObjName()},    //4
      {3, 5, cmdDispatcher.getObjName()},    //5
      {3, 5, rateGroupHiFreq.getObjName()},    //6
      {3, 5, rateGroupMedFreq.getObjName()},    //7
      {3, 5, rateGroupLowFreq.getObjName()},    //8
      {3, 5, blockDriver.getObjName()},    //9

  */
  //};

  // Register ping table
  // health.setPingEntries(pingEntries,FW_NUM_ARRAY_ELEMENTS(pingEntries),0x123);

  rateGroupLowFreq.start(0,                                               /* identifier */
                         RG_LOW_FREQ_AFF,                                 /* Thread affinity */
                         RG_LOW_FREQ_QUEUE_DEPTH * MIN_STACK_SIZE_WORDS); /* stack size */

  rateGroupMedFreq.start(0,                                               /* identifier */
                         RG_MED_FREQ_AFF,                                 /* Thread affinity */
                         RG_MED_FREQ_QUEUE_DEPTH * MIN_STACK_SIZE_WORDS); /* stack size */

  rateGroupHiFreq.start(0,                                              /* identifier */
                        RG_HI_FREQ_AFF,                                 /* Thread affinity */
                        RG_HI_FREQ_QUEUE_DEPTH * MIN_STACK_SIZE_WORDS); /* stack size */

  blockDriver.start(0,                                           /* identifier */
                    BLK_DRV_AFF,                                 /* Thread affinity */
                    BLK_DRV_QUEUE_DEPTH * MIN_STACK_SIZE_WORDS); /* stack size */

  tlmChan.start(0,                                            /* identifier */
                TLM_CHAN_AFF,                                 /* thread affinity */
                TLM_CHAN_QUEUE_DEPTH * MIN_STACK_SIZE_WORDS); /* stack size */

  cmdDispatcher.start(0,
                      CMD_DISP_AFF,
                      CMD_DISP_QUEUE_DEPTH * MIN_STACK_SIZE_WORDS);

  navigation.start(0,
                   NAV_AFF,
                   NAV_QUEUE_DEPTH * MIN_STACK_SIZE_WORDS);

  watchDogInterface.start(0,
                          WATCHDOG_AFF,
                          WATCHDOG_QUEUE_DEPTH * MIN_STACK_SIZE_WORDS);
}
