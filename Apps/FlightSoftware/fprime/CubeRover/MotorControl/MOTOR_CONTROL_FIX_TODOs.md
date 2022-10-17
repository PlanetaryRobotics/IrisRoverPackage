TODO:
- turn pollStatus() into waitForConvergence()
- make sure to wait for convergence unless type=STOP or dist=0 and speed=0

- checkMotorsStatus() needs some work

- why is rotate different than straight. Why does spin have cmd resp but nothing else?

- MC I2C in Herc is NOT async!!
-- Get Matt's WD I2C async lib. into MC MSP
-- Get Matt to look at Herc MC I2C

- i2cMasterTransmit and i2cMasterReceive can halt Herc indefinitely (no timeout) if i2c remains busy forever

- return values aren't used for anything in a lot of places (pollStatus, checkMotorStatus, startMotorMovement, etc.)

- err does nothing in motorCommandIn_handler

- ** currently MC halts until convergence after each command.
- should just push command tuple into array and every rate group update, pop a tuple out if its not moving (MC MSP says convergence is reached).
^- or just have Nav check over the port to see if MC is ready, if not, check again on the rate group update
... actually schedIn pokes a State Machine in Navigation which, if in actively driving state, only sends commands to MC if not currently moving

- !! motorDataOut_out never gets called in MC which is the only place the m_motorDataOut_OutputPort is invoked, which is the only way Nav updates m_currMoving via motorDataIn_handler. m_currMoving is used for lots of Nav StateMachine logic.

1 Move Straight = 6 MC Transfers = 16 i2cMasterTransmit = 16 delayForI2C = 1.6M cycles  = 0.1s
1 Poll = 4 i2cMasterTransmit + 4 i2cMasterReceive then 0.5s delay = 4*delayForI2C + 4*delayForI2C + 0.5s = 400kCycles + 0.5s = 0.525s
1 updateTelemetry = 2*4 MC Transfers = 


Immediate todos:
- Fix convergence on MC MSP[1] (doesnt converge)
- Get MC Comp talking to Nav and remove polling wait (Nav state machine will take over and Herc wont halt)
-- check SchedIn for Nav, make sure not more freq than every 0.5s
-- have Nav handle what happens if still "moving" (not all motors converged) after a certain time period - maybe have MC tell Nav a bitfield of which have converged instead of just moving or not so it can diagnose.
--- basically add an "if motor error" branch to Nav State machine (also, just redraw ideal nav state machine and compare to impl.)
---- if error, abort and tell ground how far you actually moved (force telem update)

