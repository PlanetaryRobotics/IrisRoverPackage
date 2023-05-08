# Iris Ground Software Backend (GDS) v.3
NOTE: All makefiles are designed to be run in an environment that supports `bash` or `zsh`.

# 1. Install OS-Level Dependencies
OS-level dependency install and setup only needs to be done once per machine.

## 1.1. Automated Dependency Setup:
Prototype automated setup processes exist in the `GroundSoftware/setup` directory. Pick the appropriate script for your platform, make the script executable with `chmod +x ./setup/script_name.sh`, then run it with `./setup/script_name.sh`. Select parts of the process can also be run individually from the `GroundSoftware/setup/parts` directory.

**After completing setup for your platform:**
- Close your terminal.
- Restart Ubuntu
  - If using WSL, you can do this by opening Powershell and running `wsl.exe --shutdown` and then reopening the `Ubuntu` terminal.
- Go to the `Check Your Setup` section below.

### 1.1.1. **Automated Dependency Setup for Linux / WSL:**
The following automated setup was tested on a `Ubuntu 18.04 WSL2` install and a
stand-alone install of `Ubuntu 18.04` (presently the core setup process is the
same for both).

In the `GroundSoftware` directory, run:
```
chmod +x ./setup/linux_setup.sh
./setup/linux_setup.sh
```
or
```
chmod +x ./setup/wsl_setup.sh
./setup/wsl_setup.sh
```
For `WSL` users, if you want to use `XTerm` scripts (used by `makefile` to automatically open multiple windows), it's strongly recommended that you follow the instructions in the `Using Graphical CLI Utilities (like *Iris Console*) in WSL` section below.

### 1.1.2. **Automated Dependency Setup for MacOS:**
This setup has been verified to have worked on MacOS Ventura. However, not from a fresh install, so it's possible some OS-level dependencies are missing.
In the `GroundSoftware` directory, run:
```
chmod +x ./setup/mac_setup.sh
./setup/mac_setup.sh
```

### 1.1.3. **(Re)Running Individual Setup Steps:**
To (re)run just a specific part of the setup, pick a script from `GroundSoftware/setup/parts` (e.g. `./setup/parts/install_redis_linux.sh`), then run it like above:

```
chmod +x ./setup/parts/install_redis_linux.sh
./setup/parts/install_redis_linux.sh
```

## 1.2. Manual Setup
It is advised you set up your environment using the automated scripts above; however, if you prefer or need not to run these scripts, the key setup steps are listed below. In addition, it is recommended that you look through each step in the automated scripts used above to make sure you're not missing anything critical.

This package requires `make`, `python` (>=3.10), and (`pyenv` and `pyenv-virtualenv` via `pyenv-installer`) to be installed, and make sure to add `pyenv` to the local path (follow the instructions in the terminal after `pyenv-installer` completes).

For Ubuntu, install the following prerequisites: `sudo apt-get install build-essential libffi-dev libsqlite3-dev zlib1g-dev xz-utils sqlite3 llvm libreadline-dev libgdbm-dev openssl libxml2-dev libbz2-dev curl libxmlsec1-dev wget tk-dev libssl-dev libncursesw5-dev libgdbm-compat-dev liblzma-dev uuid-dev bzip2 pass gnupg`

**Note:** you should be able to run `pyenv-installer` by running
`curl https://pyenv.run | bash`
but manually inspect this url's contents before running to ensure it hasn't changed ownership or something and been replaced with a malicious target.

**Note:** If `pyenv` has a build failure (when running `make init` or `pyenv install X.X.X`), make sure all `pyenv` prerequisites are present on your machine (see [pyenv - Common Build Problems](https://github.com/pyenv/pyenv/wiki/common-build-problems) for help.)


Optionally, if you want to run the GDS GUI, you'll need to install [Redis Stack](https://redis.io/docs/stack/get-started/install/).

## 1.3. Check Your Setup:
First, if you haven't already:
- Close your terminal
- Restart the OS
### 1.3.1. **Check `pyenv`**:
Make sure `pyenv` installed correctly by running: `which pyenv`:
  - This should echo a directory to the console.
  - If not, or if the terminal started up with new errors at the top, or if any errors were reported during the install process:
      - Open `./setup/parts/install_pyenv.sh`
      - Run each command in the script, one line at a time.
      - Resolve any errors as they come up and report them to the `CubeRoverPackage/Apps/GroundSoftware` devs.
      - If that doesn't work, make sure all the dependencies installed correctly (in `./setup/parts/linux_deps.sh`, etc).

### 1.3.2. **Check `Redis Stack` (for GUI):**
Check that Redis Stack installed correctly:
  - Run `redis-stack-server` in the CLI.
If you don't intend on running the GDS-GUI, Redis Stack is unnecessary and this step can be ignored.

### 1.3.3. **Check `xterm`:**
Check that XTerm is available:
  - Run `xterm` in the CLI.
  
If this doesn't work, you'll need to look into how to get X11 and XTerm working on your platform.
  - This is **only necessary** if you want to use `make` scripts that automatically open windows running various scripts.
  - If you want to manually launch all processes in their own CLI window, this is unnecessary.
  - **If you're using WSL**, see the `Using Graphical CLI Utilities (like *Iris Console*) in WSL` section below.


# 2. Setup Project Environment
## 2.1. Essential Setup
In order to run any part of this package, you'll need to run `make init` to set up the project environment.
- This will perform `make clean` first (in case you already have an environment set up).
- Set up a python environment using `pyenv`.
- Run `make update` to install all python dependencies into the project environment.

If you want to only perform specific parts of the setup, look in the `init` routine of `makefile`.

### 2.1.1. **Check Your Project Setup:**
At this point, you should be able to run `IrisBackendv3` apps. You can test this in three phases:
- Run `./run-script.sh`
  - This should open a python console inside the `py_irisbackendv3` environment.
  - If this doesn't work, the `pyenv` part of`make init` didn't work (or of the `./setup` script).
- Run `make standards`
  - This will test that the core GDS works in a single thread (no inter-app communication).
  - If this doesn't work, one of the most important python dependencies failed to install during the `make update` part of `make init` (likely because some OS-level dependency wasn't installed during the `./setup` phase).
- Run `make run-tests`:
  - This will run a series of unit and End-to-End tests that includes tests of the IPC system used for sharing data between GDS Apps.
  - These should all pass.
  - If this doesn't work, either:
    - A python dependency wasn't installed correctly.
    - A python dependency is using the wrong version
      - in the future, the exact known-good version of each python dependency will be tracked in the the "Release Candidate" `Releases` on `GitHub`.
      - consider tracking down the right version, updating it in `requirements.txt`, and rerunning `make update`.
    - Someone added new code to `IrisBackendv3` that was not tested thoroughly.
    - If an IPC test fails:
      - Someone may have messed with / added new message classes to `IrisBackendv3/ipc` and the restrictive pickler is refusing to decode it because it's module is not on the approved modules list in `IrisBackendv3/ipc/restricted_pickler.py`.
      - You may have some sort of extra restrictive firewall in place, restricting port usage on the local machine.
    - You're trying a new python version that wasn't used before (?)

## 2.2. Develop

*Note*: As a dev, you'll likely want to `pip install pyls-mypy` so that your IDE 
can lint mypy (type annotations) via the Python Language Server.

You can setup the development-only tools like this by running `./__dev-setup.sh` (after `make init`).

### **Known good development and build environment:**

**OS:** MacOS, Ubuntu 18.04, Ubuntu 18.04 in WSL2 inside Windows 11

**Editor:** VSCode (with `Remote - WSL` extension if using WSL)

**Linting:** `mypy` on VSCode using the [mypy](https://marketplace.visualstudio.com/items?itemName=matangover.mypy) extension installed. Be careful with the way the python language server is set up. Some linter setups won't find and lint imported python packages for typing (Spyder <=4.2.0 and vscode with pylance both had this problem). This setup doesn't.

## 2.3. Convenience
If you want tab-complete for `make` commands (it's convenient):
 - Add the following to your `~/.bashrc` if you use `bash` (e.g. Ubuntu 18.04 WSL):
```bash
complete -W "\`grep -oE '^[a-zA-Z0-9_.-]+:([^=]|$)' ?akefile | sed 's/[^a-zA-Z0-9_.-]*$//'\`" make
```
 - Add the following to your `~/.zshrc` if you use `zsh` (e.g. on a Mac):
```bash
# Makefile tab-complete:  
zstyle ':completion:*:*:make:*' tag-order 'targets'
autoload -U compinit && compinit
```
 - Reload your shell or run `source ~/.bashrc` or `source ~/.zshrc` to get the changes.

## 2.4. Authoring Code on Ubuntu (and in WSL)
If you intend to author commits to this repo:
- Run: `git config user.name <your-name>` (this is just the name used on commits, not a log-in)
- Run: `git config user.email <your-email>` (this is just the name used on commits, not a log-in)
- If you're on linux and don't have a password manager or log-in system set up for `git` yet:
    - `GCM` has been installed to manage `git` credentials
    - `pass` has been installed as a password manager
    - `gpg` has been installed and set as `GCM_CREDENTIAL_STORE` if one wasn't set yet.
    - If you haven't initialized `pass` yet, run `pass init <pick-a-gpg-key-for-your-password-manager-and-save-it>`.

## 2.5. Using Graphical CLI Utilities (like `Iris Console`) in WSL:
- Make sure you have WSL2 using a linux kernel from Sept. 2022 or later (`systemd` support was added then)
    - Make sure you're using Windows 11 22H2 or later to get this (Windows 10 22H2 or later may also work but hasn't been tested yet).
- Install `VcXrv` on Windows (1.20.14.0 was used in testing)
- Somewhere conveniently accesssible, create a file called `Ubuntu_GUI.xlaunch` in Windows with the following contents:
```xml
<?xml version="1.0" encoding="UTF-8"?>
<XLaunch WindowMode="MultiWindow" ClientMode="NoClient" LocalClient="False" Display="-1" LocalProgram="xcalc" RemoteProgram="xterm" RemotePassword="" PrivateKey="" RemoteHost="" RemoteUser="" XDMCPHost="" XDMCPBroadcast="False" XDMCPIndirect="False" Clipboard="True" ClipboardPrimary="True" ExtraParams="" Wgl="True" DisableAC="True" XDMCPTerminate="False"/>
```
- Click this file (open with `XLaunch.exe`) to launch XServe. It should open a black window which is where the WSL GUI will appear later.
- In your WSL distro:
    - Forward the WSL display output to XServe in Windows:
        - Add the following `~/.bashrc` (assuming you're using `bash` not `zsh`) in your WSL distro:
```bash
export DISPLAY="`grep nameserver /etc/resolv.conf | sed 's/nameserver //'`:0"
```
  - 
    - Install XApps (`sudo apt install x11-apps`)
    - Enable `systemd` by adding the following lines to `/etc/wsl.conf`:
```ini
[boot]
systemd=true
```
  - Restart WSL
      - Close the WSL terminal
      - Open Powershell in Windows and run `wsl --shutdown`
  - Run a graphical application (example `make xconsole`)

# 3. Help
Running the following will tell you all the standard tasks, tools, and app for the GDS that have been automated in the `makefile` (for non-developers, this is a good starting place):

`make help`

# 4. Update Dependencies
`make update`

# 5. Run a Script
To run a python script using the appropriate `pyenv` as if it was executed from
the `GroundSoftware` directory, use `./run-script.sh`.

Example:
```
./run-script.sh ./scripts/tiny_apps/datastandards_lookup.py -l
```

# 6. Running Apps & Code:
- `make` offers one-line bring-up of a number of common GDS tasks & apps.
  - `make help` will show you everything available.
- In general most apps require at least some GDS infrastructure running in the background.
  - As a rule of thumb, scripts that start with `make run-` will automatically bring up this infrastructure.
  - If you want to run something else, you'll probably want to run `make run-infra` first.
    - You can just leave this running in the background forever.
    - If `xterm` doesn't work on your machine, you'll likely need to open `makefile` and run each command in the `run-infra` target in its own CLI window to accomplish the same result.

## 6.1. Run GDS and All the Core Apps:
The following will rebuild & cache the DataStandards then bring up all core IPC Apps:

`make run`

## 6.2. Run Just the Infrastructure:
If you don't want any of the CLI tools (`TelemetryDisplay`, `MessagePrinter`, etc.) and just want to run the core IPC infrastructure, you can just run:

`make run-infra`

- You can just leave this running in the background forever.
- If `xterm` doesn't work on your machine, you'll likely need to open `makefile` and run each command in the `run-infra` target in its own CLI window to accomplish the same result.

## 6.3. List All Common IPC Apps:

Commonly used IPC apps can be found by using: `make help` and looking for targets that start with `app-` or tab-completing `make app-` (e.g. `make app-commander`, `make app-telemetry-display`, etc.)

Less common (and possibly lower quality) IPC apps can be run using `./run-script.sh` and can be found in
- Release-ready IPC apps can be found in: `GroundSoftware/ipc_apps`.
- IPC test scripts, some of which may be solid and executable, can be found in: `GroundSoftware/scripts/testing/ipc_demo`.

# 8. Common Use Cases:

NOTE: **All of the following are meant to be run from the `GroundSoftware` directory unless otherwise stated.**

## 8.1 Bringing up the Full Stack
This will launch all the GDS tools and core apps. Likely more than you'll need outside of a full stack mission simulation:
1. Run `make run`

## 8.2. `Iris Console` for FSW:
You're a FSW developer and want to use the legacy single-process `Iris Console` CLI script:
1. Run `make console`
    - or `make console-debug` if you're using the debug lander harness (and not the RS422 transceiver that plugs into the EM4 pogo pins).
    - NOTE: you may have to zoom out a bunch in your CLI for this to work properly.
   - or `make xconsole` / `make xconsole-debug` if you want to open an `xterm` with the appropriate zoom settings.

## 8.2. Running the GDS GUI:
Runs a low-level developer / diagnostic interface for working with the rover. Mainly designed for use by Flight Software engineers:
1. Run `make run-infra`
2. Run `make app-gui`
3. In a separate terminal window, connect to a data pipeline:
   1. For data from the real rover:
      1. `make xcvr-rs422` for a RS422+UDP/SLIP transceiver
      2. `make xcvr-wifi` for a WiFi transceiver
      3. `make xcvr` to bring up all transceivers in parallel. Any transceivers that can connect to the rover will forward data to it in both directions.
   2. To replay replay some test data:
      1. For infinite generic data: `make data`
      2. For finite data containing image(s): `make data-image`

## 8.3. Testing a new IPC App:
If you're testing your own new IPC app:
1. Run `make run-infra`
2. Run `./run-script.sh ./path/to/your_app.py`.
3. If you want test rover data to be infinitely played from a recording, open a new terminal and run: `make data`.

# 7. Teardown and Cleanup Development Environment
Run this before reinitializing (in case something broke):

`make clean`