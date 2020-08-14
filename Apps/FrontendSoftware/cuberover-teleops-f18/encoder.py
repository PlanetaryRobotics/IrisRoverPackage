# Encoder Module 
# still needs error checking 
def encode(commands): #assuming it takes in a list of commands
	bitString = ""
	for i in range(len(commands)):

		if commands[i] == "ESTOP": # Here is where I'll be tyring out the ESTOP command
			bitString = bitString + "0000"

		elif commands[i] == "Driving": # Here is where I'll be tyring out the driving commands
			bitString = bitString + "0001"
			if len(commands) <= i+1: 
				raise Exception("not enough commands")
			if commands[i+1] == "Forward":
				bitString = bitString + "00"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Distance":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "00"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				elif commands[i+2] == "Speed":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "01"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				elif commands[i+2] == "Images":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "10"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "Reverse":
				bitString = bitString + "01"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Distance":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "00"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				elif commands[i+2] == "Speed":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "01"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				elif commands[i+2] == "Images":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "10"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "Left":
				bitString = bitString + "10"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Distance":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "00"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				elif commands[i+2] == "Speed":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "01"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				elif commands[i+2] == "Images":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "10"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "Right":
				bitString = bitString + "11"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Distance":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "00"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				elif commands[i+2] == "Speed":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "01"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				elif commands[i+2] == "Images":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "10"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			i = i + 3

		elif commands[i] == "Imaging": # Here is where I'll be tyring out the Imaging commands
			bitString = bitString + "0010"
			if len(commands) <= i+1: 
				raise Exception("not enough commands")
			if commands[i+1] == "ImageRequest":
				bitString = bitString + "000"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Camera":
					bitString = bitString + "00"
					if commands[i+3] == "Front":
						bitString = bitString + "0"
					elif commands[i+3] == "Back":
						bitString = bitString + "1"
				elif commands[i+2] == "Resolution":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "01"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				elif commands[i+2] == "Cropping":
					bitString = bitString + "10"
					print("TODO cropping units")
				elif commands[i+2] == "Compression":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "11"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "DrivingImageConfig":
				bitString = bitString + "001"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Camera":
					bitString = bitString + "00"
					if commands[i+3] == "Front":
						bitString = bitString + "0"
					elif commands[i+3] == "Back":
						bitString = bitString + "1"
				elif commands[i+2] == "Resolution":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "01"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				elif commands[i+2] == "Cropping":
					bitString = bitString + "10"
					print("TODO cropping units")
				elif commands[i+2] == "Compression":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "11"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "ISO":
				bitString = bitString + "010"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "ISO":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "Shutter Speed":
				bitString = bitString + "011"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Shutter Speed":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "White Balance":
				bitString = bitString + "100"
				print("TODO: hasn't been filled out in the drive")
			i = i + 3;

		elif commands[i] == "Telemetry Settings": # Here is where I'll be tyring out the Telemetry commands
			bitString = bitString + "0011"
			if len(commands) <= i+1: 
				raise Exception("not enough commands")
			if commands[i+1] == "GyroscopeConfig":
				bitString = bitString + "0000"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Mode":
					bitString = bitString + "0"
					if commands[i+3] == "Roving":
						bitString = bitString + "0"
					elif commands[i+3] == "Idle":
						bitString = bitString + "1"
				elif commands[i+2] == "Frequency":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "1"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "AccelerometerConfig":
				bitString = bitString + "0001"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Mode":
					bitString = bitString + "0"
					if commands[i+3] == "Roving":
						bitString = bitString + "0"
					elif commands[i+3] == "Idle":
						bitString = bitString + "1"
				elif commands[i+2] == "Frequency":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "1"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "ThermistorsConfig":
				bitString = bitString + "0010"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Mode":
					bitString = bitString + "0"
					if commands[i+3] == "Roving":
						bitString = bitString + "0"
					elif commands[i+3] == "Idle":
						bitString = bitString + "1"
				elif commands[i+2] == "Frequency":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "1"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "MotorCurrentConfig":
				bitString = bitString + "0011"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Mode":
					bitString = bitString + "0"
					if commands[i+3] == "Roving":
						bitString = bitString + "0"
					elif commands[i+3] == "Idle":
						bitString = bitString + "1"
				elif commands[i+2] == "Frequency":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "1"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "MotorVelocityConfig":
				bitString = bitString + "0100"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Mode":
					bitString = bitString + "0"
					if commands[i+3] == "Roving":
						bitString = bitString + "0"
					elif commands[i+3] == "Idle":
						bitString = bitString + "1"
				elif commands[i+2] == "Frequency":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "1"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "MotorVoltagesConfig":
				bitString = bitString + "0101"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Mode":
					bitString = bitString + "0"
					if commands[i+3] == "Roving":
						bitString = bitString + "0"
					elif commands[i+3] == "Idle":
						bitString = bitString + "1"
				elif commands[i+2] == "Frequency":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "1"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "PowerSupplyCurrentConfig":
				bitString = bitString + "0110"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Mode":
					bitString = bitString + "0"
					if commands[i+3] == "Roving":
						bitString = bitString + "0"
					elif commands[i+3] == "Idle":
						bitString = bitString + "1"
				elif commands[i+2] == "Frequency":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "1"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "RadioHeartbeatConfig":
				bitString = bitString + "0111"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Mode":
					bitString = bitString + "0"
					if commands[i+3] == "Roving":
						bitString = bitString + "0"
					elif commands[i+3] == "Idle":
						bitString = bitString + "1"
				elif commands[i+2] == "Frequency":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "1"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "HerculesHeartbeatConfig":
				bitString = bitString + "1000"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Mode":
					bitString = bitString + "0"
					if commands[i+3] == "Roving":
						bitString = bitString + "0"
					elif commands[i+3] == "Idle":
						bitString = bitString + "1"
				elif commands[i+2] == "Frequency":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "1"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
			elif commands[i+1] == "CoulombCounterConfig":
				bitString = bitString + "1001"
				if len(commands) <= i+2: 
					raise Exception("not enough commands")
				if commands[i+2] == "Mode":
					bitString = bitString + "0"
					if commands[i+3] == "Roving":
						bitString = bitString + "0"
					elif commands[i+3] == "Idle":
						bitString = bitString + "1"
				elif commands[i+2] == "Frequency":
					if len(commands) <= i+3: 
						raise Exception("not enough commands")
					bitString = bitString + "1"
					bitString = bitString + str(bin(int(commands[i+3])))[2:]
				i = i + 3


		elif commands[i] == "Reset Settings": # Here is where I'll be tyring out the Reset commands
			bitString = bitString + "0100"
			if len(commands) <= i+1: 
				raise Exception("not enough commands")
			if commands[i+1] == "ResetAll":
				bitString = bitString + "00"
			elif commands[i+1] == "ResetDrive":
				bitString = bitString + "01"
			elif commands[i+1] == "ResetCamera":
				bitString = bitString + "10"
			i = i+1


		elif commands[i] == "Settings": # Here is where I'll be tyring out the misc settings
			bitString = bitString + "0101"
			if len(commands) <= i+1: 
				raise Exception("not enough commands")
			if commands[i+1] == "AccelerationCurve":
				bitString = bitString + "00"
			elif commands[i+1] == "MaxDriveTime":
				bitString = bitString + "01"
			elif commands[i+1] == "MaxDriveDistance":
				bitString = bitString + "10"
			i = i+1



	print(bitString)
	return bitString

encode(["Driving", "Forward", "Distance", "5"])