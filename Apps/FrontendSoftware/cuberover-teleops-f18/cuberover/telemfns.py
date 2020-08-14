''' author: Ruijie Fu'''

# this file contains the processing functions for
# all message types except for image

# input: integer
# output: print it, and push to database

def battery(x):
    print("Battery reading is: %d" % x)

def voltage(x):
    print("Voltage is: %d" % x)

def power(x):
    print("Power delivery is: %d" % x)

def thermal(x):
    print("Thermal reading is: %d" % x)

def wheelVel(x):
    print("Wheel velocity is: %d" % x)

def accelerometer(x):
    print("Accelerometer reading is: %d" % x)

def gyroscope(x):
    print("Gyroscope reading is: %d" % x)
