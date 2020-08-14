# author: Ruijie Fu

# this file contains bit-int transfer functions
# put 'from bit_int import *' in front of your file to use it

# input: 0/1 string
# output: corresponding integer
def bit2int(x):
    return int(x,2)

# input: integer
# output: corresponding 0/1 string, without '0b' prefix
def int2bit(x):
    return format(x,'b')

# input: char string
# output: corresponding bit string according to ascii number
def str2bit(x):
    ans = ""
    for l in x:
        temp = int2bit(ord(l))
        if len(temp)<8:
            temp = "0"*(8-len(temp))+temp;
        ans += temp
    return ans


if __name__ == "__main__":
    error = 0
    try:
        assert(bit2int("101")==5)
    except:
        error += 1
    try:
        assert(bit2int("1000001")==65)
    except:
        error += 1
    print("function bit2int tests passed %d/2" % (2-error))

    error = 0
    try:
        assert(int2bit(5)=="101")
    except:
        error += 1
    try:
        assert(int2bit(65)=="1000001")
    except:
        error += 1
    print("function int2bit tests passed %d/2" % (2-error))

    error = 0
    try:
        assert(str2bit("A")=="01000001")
    except:
        error += 1
    try:
        assert(str2bit("AB")=="0100000101000010")
    except:
        error += 1
    print("function str2bit tests passed %d/2" % (2-error))
