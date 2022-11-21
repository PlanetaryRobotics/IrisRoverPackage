## 5.2 Iris Transit Mode Checkout
### 3. Power
| Power Circuit  | Voltage | Amps | Watts |
| -------------- | ------- | ---- | ----- |
| Power 1 (Op)   | 28.23   | 0.03 | 0.85  |
| Power 2 (Heat) | n/a     | n/a  | n/a   |
*Note: may be rounding errors in AB's PIMS `V` and `I` measurements leading to inaccuracies in power calculation.*

### 4. Data Rate
**Success**: Buffer not filling.

### 5. Verify Heartbeat
**Success**: Heartbeat telemetry verified.

### 6. Verify Watchdog Commanding

#### a. Enable 24V Rail
**Success**. Response: `Command[#0] -> NO_ERROR[0x0]`

#### b. Power Hercules On
**Success**. Response: `Command[#0] -> NO_ERROR[0x0]`

- b.i. "Battery" power draw increased to `0.0102uA` to `1.87uA`.

## 5.3 Iris Checkout & Deployment Sequence
### 2. Power
| Power Circuit  | Voltage | Amps | Watts |
| -------------- | ------- | ---- | ----- |
| Power 1 (Op)   | 28.23   | 0.03 | 0.85  |
| Power 2 (Heat) | n/a     | n/a  | n/a   |
*Note: may be rounding errors in AB's PIMS `V` and `I` measurements leading to inaccuracies in power calculation.*

### 3. Verify KeepAlive Heartbeat
**Success**: Heartbeats downlinked at ~23s interval.

### 4. Activate WiFi (AB)
**Complete**.

### 5. Verify Watchdog Commanding + Prep for Deployment
#### 6. (should be 5.a) Switch to Service Mode
**Success**. Successfully sent. Response: `Command[#236] -> NO_ERROR[0x0]`
Verified (TVAC-style) Surface Heartbeats received, indicating successful transition to Service Mode. Examples:
`FFFF0F0000000000000000F401FD0C3C0034210401013421` -> 
`RS_SERVICE:	[Heat: ON, Ctrl: ON] 	260.9°K -> 273.6°K +- 0.92K° 	Kp = 500 @ Duty Cycle: 8500/65535`

`FFFF0F0000000000000000F401FD0C3C0034210401010000` -> 
`RS_SERVICE:	[Heat: ON, Ctrl: ON] 	260.9°K -> 273.6°K +- 0.92K° 	Kp = 500 @ Duty Cycle: 0/65535`

#### 7. (should be 5.b) Prepare for Deployment
**Success**. Successfully sent. Response: `Command[#1] -> NO_ERROR[0x0]`

Battery current draw from `0.0102uA` to `0.105A` (note: no cameras attached)

Surface Heartbeats still being received, successfully indicating Mission mode entered. Example:
`FFFF0FA0CD46C374709660F401FD0C3C0034211000010000` -> `RS_MISSION:	[Heat: OFF, Ctrl: ON] 	260.9°K -> 273.6°K +- 0.92K° 	Kp = 500 @ Duty Cycle: 0/65535`

Hercules Heartbeats received over the wire. Example:
`00890254FF1000C0000F00000000FFFFFF1000C0010F00000000FEFFFF1000C0020F00000000A100FF1000C0030F000000006300FF1000C0040F000000000D00FF1000C0050F000000004401FF1000C000050000000000000000FF1000C0000402000000D0070000FF1000C000090200000000FF1000C002090200000000000000FF1000C003090200000000000000FF1000C0000324000000A08C0000FF1000C0000FF2000000FCFFFF1000C0010FF2000000FCFFFF1000C0020FF2000000A300FF1000C0030FF20000004A00FF1000C0040FF2000000F3FFFF1000C0050FF20000005101FF1000C0000525000000E8030000FF1000C0000FE5010000FAFFFF1000C0010FE5010000FDFFFF1000C0020FE5010000A800FF1000C0030FE50100005A00FF1000C0040FE50100000E00FF1000C0050FE50100005D01FF1000C00009F500000000FF1000C00209F500000000000000FF1000C00309F500000000000000FF1000C00009E701000000FF1000C00209E701000000000000FF1000C00309E701000000000000FF1000C0000FD7020000FAFFFF1000C0010FD7020000FEFFFF1000C0020FD7020000A300FF1000C0030FD70200005000FF1000C0040FD70200001800FF1000C0050FD70200007801FF1000C0000433020000B80B0000FF1000C0000FCA030000FDFFFF1000C0010FCA030000FDFFFF1000C0020FCA030000A100FF1000C0030FCA0300003300FF1000C0040FCA030000EAFFFF1000C0050FCA0300001501FF1000C00009DA02000000FF1000C00209DA02000000000000FF1000C00309DA02000000000000FF1000C00009CC03000000FF1000C00209CC03000000000000FF1000C00309CC03000000000000FF1000C0000FA4080000FEFFFF1000C0010FA4080000FCFF` -> 
```
ICP[#0::649]: 	52 T	- 0 E	- 0 B	- 0 C
	 Latest Telemetry: Imu{XAcc}@2212 = -2,	 Imu{YAcc}@2212 = -4,	 Imu{ZAcc}@970 = 161,	 Imu{XAng}@970 = 51,	 Imu{YAng}@970 = 65514,	 Imu{ZAng}@970 = 277,	 ActiveRateGroup-RateGroupHiFreq{RgMaxTime}@37 = 1000,	 ActiveRateGroup-RateGroupMedFreq{RgMaxTime}@563 = 3000,	 GroundInterface{GiUplinkSeqNum}@972 = 0,	 GroundInterface{GiPacketsReceived}@972 = 0,	 GroundInterface{GiPacketsTransmitted}@972 = 0,	 ActiveRateGroup-RateGroupLowFreq{RgMaxTime}@36 = 36000
```

#### 8. Verify Hercules Commanding via Watchdog

**Failed**. No command response from Watchdog (as expected) but Hercules was still downlinking telemetry over the wire. Either primary interface switch is not implemented correctly on Hercules or Watchdog failed to pass the command along to Hercules.

#### 9. Verify Watchdog->Hercules->Watchdog Commanding
**Failed/Untested.**
Prior to command send, power transfer from battery over to lander was observed. Lander power was cut off and battery power returned to roughly nominal Mission mode levels (`87-97mA` fluctuating, no cameras were connected). No telemetry was received for 5+ minutes from Iris.

Sending `power-hercules-off` in an attempt to observe successful receipt of command as indicated by battery current change and/or a watchdog command response.

After send, no telemetry (command response) observed, no change to power draw from batteries observed (`98mA`, no cameras were connected).


In a recovery attempt, the battery and lander power were both removed and restored in order to power-cycle the SBC. The SBC returned drawing nominal Mission mode levels of power from the battery (`116mA`, no cameras connected.). Only a `b'hello, world!\r\n'` was received. No further heartbeat telemetry (or Hercules telemetry) was observed.

## Test end. (Early Failure)

Key Results:

- **Correct parsing of IP/UDP/SLIP by Watchdog:** PASS
- **Correct sending of IP/UDP/SLIP by Watchdog:** PASS
- **Switch to wifi mode:** FAIL
- **Deployment test:** UNTESTED (pre-checkouts FAILED)
