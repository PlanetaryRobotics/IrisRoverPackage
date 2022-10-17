import json
import sys
from datetime import datetime, timedelta
from time import sleep

from yamcs.client import YamcsClient
from yamcs.core.auth import Credentials

HOSTNAME = "192.168.10.105"
PORT = "8090"
URI = "{0}:{1}".format(HOSTNAME, PORT)

YAMCS_USERNAME = "example"
YAMCS_PASSWORD = "justanexample"

YAMCS_INSTANCE = 'Astrobotic-M1'
YAMCS_PROCESSOR = 'realtime'

usageList = {
    "overall": "Invalid usage. Expected \"python3 {0} <operation_to_run>".format(sys.argv[0]),
    "send_command": "- python3 {0} send_command <yamcs_command> {{ \"tc-binary\":\"...\" }}\n-Please refer to the Ground Software Interface Document doc for how to send a command.".format(sys.argv[0]),
    "list_commands": "- python3 {0} list_commands".format(sys.argv[0]),
    "get_parameter": "- python3 {0} get_parameter <parameter>",
    "list_parameter_values": "- python3 {0} list_parameter_values <parameter>",
    "get_parameter_socket": "- python3 {0} get_parameter_socket <parameter>"
}


def listCommands(client):
    # If you want to list all available commands to you.
    # Could be useful early on to ensure nothing is missing
    # or a quick list if you forget a command.
    mdb = client.get_mdb(instance=YAMCS_INSTANCE)
    return mdb.list_commands()


def sendCommand(client, cmd, additionalArgs):
    # Actually sends a command to Yamcs. Payloads will use the format:
    # python3 pythonTemplate.py /astrobotic/command_name \"{\"tc-binary\":\"1341ab\"}\"
    # Where the value of tc-binary is hex encoded
    processor = client.get_processor(
        instance=YAMCS_INSTANCE, processor=YAMCS_PROCESSOR)
    return processor.issue_command(cmd, args=json.loads(additionalArgs))


def getParameter(client, parameter):
    # Retrieves the last updated value of a parameter. Includes timestamp and value.
    processor = client.get_processor(
        instance=YAMCS_INSTANCE, processor=YAMCS_PROCESSOR)
    return processor.get_parameter_value(parameter)


def listParameterValues(client, parameter):
    # Lists the last 10 values for a parameter.
    # The for loop can be expanded to more data.
    archive = client.get_archive(instance=YAMCS_INSTANCE)
    iterable = iter(archive.list_parameter_values(parameter,
                                                  descending=True))

    for i in range(0, 10, 1):
        currVal = next(iterable, None)
        if currVal == None:
            print("Quitting at {0}".format(i))
            return
        print(currVal)


def getParameterSocket(client, parameter):
    # Open a socket up to recieve a constant flow of telemetry data.
    # TBD on getting it to work with auth.
    processor = client.get_processor(
        instance=YAMCS_INSTANCE, processor=YAMCS_PROCESSOR)

    subscription = processor.create_parameter_subscription([
        parameter
    ])

    while(1):
        sleep(5)
        print("Latest value: {0}".format(subscription.get_value(parameter)))


def lengthCheck(lengthOfArgs, lenExpected, usage="overall"):
    # A helper function.
    if lengthOfArgs < lenExpected:
        print(usageList[usage])
        exit(1)


def main():
    # Sign in with username and password defined at top of file
    credentials = Credentials(username=YAMCS_USERNAME, password=YAMCS_PASSWORD)

    # Create a yamcs client that connects with our server.
    # Currently connects to localhost but will change later
    client = YamcsClient(URI, credentials=credentials)

    # This just helps make this python program flexible in accepting
    # Command line arguments
    numOfArgs = len(sys.argv)
    lengthCheck(numOfArgs, 2)
    api = sys.argv[1]

    # This just lays out what user input is dispatched where.
    if api == "send_command":
        lengthCheck(numOfArgs, 3, api)
        print("Sending command: ", sys.argv[2])
        args = "{}"
        if numOfArgs > 3:
            args = sys.argv[3]
        status = sendCommand(client, sys.argv[2], args)
        print("Command status: ", status.__dict__)
    elif api == "list_commands":
        lengthCheck(numOfArgs, 2, api)
        cmds = listCommands(client)
        for cmd in cmds:
            print(cmd)
    elif api == "get_parameter":
        lengthCheck(numOfArgs, 3, api)
        print(getParameter(client, sys.argv[2]))
    elif api == "list_parameter_values":
        lengthCheck(numOfArgs, 3, api)
        listParameterValues(client, sys.argv[2])
    elif api == "get_parameter_socket":
        lengthCheck(numOfArgs, 3, api)
        getParameterSocket(client, sys.argv[2])
    else:
        print(usageList["overall"])


if __name__ == "__main__":
    main()
