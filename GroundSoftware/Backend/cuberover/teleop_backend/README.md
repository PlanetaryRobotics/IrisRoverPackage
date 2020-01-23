###Note: The teleoperation backend requires Python 3.5 or newer in order to run.

###Set-up
1. Change directory to the root of this repository
2. Install the pre-requisite packages:
    ```
    python3 -m pip install --user -r cuberover/requirements.txt
    ```
3. Generate the python files from the IDL and put them in a package named "idl_msgs" in `cuberover/teleop_backend`:
    ```
    python3 IDL/idl/compiler.py -f -o cuberover/teleop_backend/ -i IDL/messages/ python -p idl_msgs
    ```

###Back-end verification
Verifying the back-end requires three terminals. In all three terminals, the current working directory should be the root of this repository.
1. In the first terminal, start the teleop_fake_server:
    ```
    python3 cuberover/teleop_backend/teleop_fake_server.py --address 127.0.0.1 --port 5001
    ```
2. In the second terminal, start the teleop_backend:
    ```
    python3 cuberover/teleop_backend/teleop_backend_main.py --address 127.0.0.1 --port 5001
    ```
3. In the third terminal, start the teleop_fake_frontend:
    ```
    python3 cuberover/teleop_backend/teleop_fake_frontend.py
    ```
4. With all three of these commands running, you should be seeing activity in all three terminals. The terminal running `teleop_fake_frontend.py` will be spitting out the commands it is writing to the database. The terminal running `teleop_backend_main.py` will be spitting out updates as it pulls commands from the database, sends them out over TCP, and then writes status updates back to the database. The terminal running `teleop_fake_server.py` will be spitting out how many bytes it receives when it reads data from it's TCP port, and will also print out the parsed command it received.

Note that the `python3` command might not exist on your machine, particularly if you're running on Windows. In that case, confirm you have python 3.5 or newer installed by running `python --version`, and if that's the case then you can just replace `python3` in the above commands with `python`. If not, you need to install python 3.5 or newer.