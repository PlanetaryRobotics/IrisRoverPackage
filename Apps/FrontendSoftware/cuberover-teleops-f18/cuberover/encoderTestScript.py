#Encoder Unit Test Script

print("Beginning Testing")
commands = ["ESTOP"]
assert(encode(commands), "0000")
print("ESTOP is good")

print("Entering Driving")
commands = ["Driving"]
assert(encode(commands), "0001")
print("Driving is good")

print("Entering Imaging")
commands = ["Imaging"]
assert(encode(commands), "0010")
print("Imaging is good")


print("Entering Telemetry Settings")
commands = ["Telemetry Settings"]
assert(encode(commands), "0011")
print("Telemetry Settings are good")

#Entering Reset Settings
print("Entering Reset Settings")
commands = ["Reset Settings"]
assert(encode(commands), "0100")
print("Reset Settings good")


#Entering Settings
print("Now onto Settings")
commands = ["Settings"]
assert(encode(commands), "0101")
print("Settings are good")

print("exhaustive testing commencing")

ISO = 1
SHUTTERSPEED = 1
WHITEBALANCE = 1
FREQUENCY = 1


headings = ["ESTOP", "Driving", "Imaging", "Telemetry Settings", "Reset Settings", "Settings"]

estopSettings = [""]
drivingHeadings = ["Forward", "Reverse", "Left", "Right"]
drivingSubheadings = ["Distance", "Speed", "Images"]
imagingHeadings = ["ImageRequest", "DrivingImageConfig", "ISO", "Shutter Speed", "White Balance"]
imageRequestSubheadings = ["Camera", "Resolution", "Cropping", "Compression"]
cameraSubheadings = ["Front", "Back"]
iso = [ISO]
shutterSpeed = [SHUTTERSPEED]
whiteBalance = [WHITEBALANCE]
telemetrySettings = ["GyroscopeConfig", "AccelerometerConfig","ThermistorsConfig", "MotorCurrentConfig",
	 					"MotorVelocityConfig","BatteryVoltagesConfig","PowerSupplyCurrentConfig", "RadioHeartbeatConfig", 
	 					"HerculesHeartbeatConfig" , "CoulombCounterConfig"]
telemetrySubheadings = ["Mode", "Frequency"]
modeSubheading = ["Roving", "Idle"]
frequencySubheading = [FREQUENCY]
resetSettings = ["ResetAll", "ResetDrive", "ResetCamera"]
settings = ["AccelerationCurve", "MaxDriveTime", "MaxDriveDistance"]

subheadings = [estopSettings, drivingHeadings, imagingHeadings, telemetrySettings, resetSettings, settings]


for i in range(len(headings)):
	commandList = [];
	header = headings[i]
	commandList.append(header)
	for j in range(subheadings[i]):
		commandList.append(subheadings[i][j])
		encode(commandList)
		commandList.pop()

