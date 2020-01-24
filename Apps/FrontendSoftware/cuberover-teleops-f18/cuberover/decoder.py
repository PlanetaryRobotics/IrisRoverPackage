''' Author: Ruijie Fu'''

from bit_int import *
from telemfns import *
#from ImageProcessing import *

''' this function transfers avionics output into telemetry command
    input: char string x
    output: calls different functions to deal with message content
    dependency: ImageProcessing.py
                bit_int.py
                telemfns.py
    all bit strings are just 0/1 strings, with '0b' prefix removed
'''
def decoder(x):
    x = str2bit(x)
    op_code = bit2int(x[0:2])
    size = bit2int(x[2:12])
    # the first 2 bits represent op_code
    # op_code 00 means the message will be heart beat
    # op_code 01 means otherwise
    # the integer represented by bits #3-12 represent the message size
    # for heartbeat the size has to be 1023
    if (op_code == 0):
        if (size==1023):
            return ("heartbeat")
        else:
            raise ValueError('wrong format')
    elif (op_code == 1):
        # bits #13-16 are the setting code
        # setting code chart:
        # 0001 - Battery Reading/Health
        # 0010 - Voltage
        # 0011 - Power Delivery
        # 0100 - Thermal Readings
        # 0101 - Wheel Velocity
        # 0110 - Accelerometer
        # 0111 - Gyroscope
        # 1000 - Image
        # bits starting from #17 are the message content
        setting_code = bit2int(x[12:16])
        content = bit2int(x[16:(16+size)])
        if (setting_code == 1):
            battery(content)
            return("Battery reading is: %d" % content)
        elif (setting_code == 2):
            voltage(content)
            return("Voltage is: %d" % content)
        elif (setting_code == 3):
            power(content)
            return("Power delivery is: %d" % content)
        elif (setting_code == 4):
            thermal(content)
            return("Thermal reading is: %d" % content)
        elif (setting_code == 5):
            wheelVel(content)
            return("Wheel velocity is: %d" % content)
        elif (setting_code == 6):
            accelerometer(content)
            return("Accelerometer reading is: %d" % content)
        elif (setting_code == 7):
            gyroscope(content)
            return("gyroscope reading is: %d" % content)
        elif (setting_code == 8):
            print("image")
            # need to update with the change of ImageProcessing.py
            improc = ImageProccessing(8)
            smt = improc.read_byte_string(x[16:(16+size)])
            return "I don't know how to test image"
        else:
            raise ValueError('wrong format')
    else:
        raise ValueError('wrong format')

if __name__ == "__main__":
    error = 0
    try:
        assert(decoder("?ð")=="heartbeat") # "00 1111111111"
    except:
        error += 1
    try:
        assert(decoder("@A°")=="Battery reading is: 11") # "01 0000000100 0001 1011"
    except:
        error += 1
    try:
        assert(decoder("@BÀ")=="Voltage is: 12") # "01 0000000100 0010 1100"
    except:
        error += 1
    try:
        assert(decoder("@S¸")=="Power delivery is: 23") # "01 0000000101 0011 10111"
    except:
        error += 1
    try:
        assert(decoder("@\x94ª\x00")=="Thermal reading is: 340") # "01 0000001001 0100 101010100"
    except:
        error += 1
    try:
        assert(decoder("@¥Â@")=="Wheel velocity is: 777") # "01 0000001010 0101 1100001001"
    except:
        error += 1
    try:
        assert(decoder("@\x96ª\x00")=="Accelerometer reading is: 340") # "01 0000001001 0110 101010100"
    except:
        error += 1
    try:
        assert(decoder("@\x97ª\x00")=="gyroscope reading is: 340") # "01 0000001001 0111 101010100"
    except:
        error += 1
    print("function decoder tests passed %d/8" % (8-error))
