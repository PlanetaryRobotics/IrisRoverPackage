# Iris Ground Software Backend (GDS) v.3
NOTE: All makefiles are designed to be run in an environment that supports `bash` or `zsh`.

# Install Dependencies

## Automated Dependencies Setup on Ubuntu (in WSL)
**Note:** The following automated setup was testing on a `Ubuntu 18.04 WSL2` install but will likely also work on a stand-alone install of `Ubuntu`. It's also been verified to have worked on MacOS Ventura as a shortcut.

To install and setup the dependencies for this project on `Ubuntu`:
- Run: `chmod +x ./__wsl-ubuntu-setup.sh`
- Run: `sudo ./__wsl-ubuntu-setup.sh`
- Close your terminal.
- Restart Ubuntu
    - If using WSL, you can do this by opening Powershell and running `wsl.exe --shutdown` and then reopening the `Ubuntu` terminal.
- Make sure `pyenv` installed correctly by running: `which pyenv`:
    - This should echo a directory to the console.
    - If not, or if the terminal started up with new errors at the top, or if any errors were reported during the install process:
        - Open `./__wsl-ubuntu-setup.sh`
        - Run each command in the script, one line at a time.
        - Resolve any errors as they come up and report them to the `CubeRoverPackage/Apps/GroundSofware` devs.

## Authoring Code on Ubuntu (in WSL)
If you intend to author commits to this repo:
- Run: `git config user.name <your-name>` (this is just the name used on commits, not a log-in)
- Run: `git config user.email <your-email>` (this is just the name used on commits, not a log-in)
- If you're on linux and don't have a password manager or log-in system set up for `git` yet:
    - `GCM` has been installed to manage `git` credentials
    - `pass` has been installed as a password manager
    - `gpg` has been installed and set as `GCM_CREDENTIAL_STORE` if one wasn't set yet.
    - If you haven't initialized `pass` yet, run `pass init <pick-a-gpg-key-for-your-password-manager-and-save-it>`.

### Using graphical utilities (like `Iris Console`) in WSL:
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
    - Install XApps (`sudo apt install x11-apps`)
    - Create or edit an XTerm config file at `~/XTerm` with the following contents:
```bash
XTerm*Background: Grey15
XTerm*Foreground: white
XTerm*faceName: DejaVu Sans Mono:size=8:antialias=true:hinting=true
XTerm*font: 6x10
XTerm*saveLines: 500
XTerm*HiForeColor: white
XTerm*HiBackColor: Grey15
XTerm*geometery: 1920x1080+0+0
```
    - Enable `systemd` by adding the following lines to `/etc/wsl.conf`:
```ini
[boot]
systemd=true
``
    - Restart WSL
        - Close the WSL terminal
        - Open Powershell in Windows and run `wsl --shutdown`
    - Run a graphical application (example `make xconsole`)

## Manual Setup
It is advised you set up your environment using the automated scripts above; however, if you prefer or need not to run these scripts, the key setup steps are listed below. In addition, it is recommended that you look through each step in the automated scripts used above to make sure you're not missing anything critical.

This package requires `make`, `python` (>=3.10), and (`pyenv` and `pyenv-virtualenv` via `pyenv-installer`) to be installed, and make sure to add `pyenv` to the local path (follow the instructions in the terminal after `pyenv-installer` completes).

For ubuntu, install the following prerequisites: `sudo apt-get install build-essential libffi-dev libsqlite3-dev zlib1g-dev xz-utils sqlite3 llvm libreadline-dev libgdbm-dev openssl libxml2-dev libbz2-dev curl libxmlsec1-dev wget tk-dev libssl-dev libncursesw5-dev libgdbm-compat-dev liblzma-dev uuid-dev bzip2 pass gnupg`

**Note:** you should be able to run `pyenv-installer` by running
`curl https://pyenv.run | bash`
but manually inspect this url's contents before running to ensure it hasn't changed ownership or something and been replaced with a malicious target.

**Note:** If `pyenv` has a build failure (when running `make init` or `pyenv install X.X.X`), make sure all `pyenv` prerequisites are present on your machine (see [pyenv - Common Build Problems](https://github.com/pyenv/pyenv/wiki/common-build-problems) for help.)

# Setup Development Environment
## Essentials
Run:
`make init`
Will perform `make clean` first. If you want to only perform specific parts of the setup, look in the `init` routine of `makefile`.

## Develop

*Note*: As a dev, you'll likely want to `pip install pyls-mypy` so that your IDE 
can lint mypy (type annotations) via the Python Language Server.

You can setup the development-only tools like this by running `./__dev-setup.sh` (after `make init`).

### Known good development and build environment:

**OS:** MacOS, Ubuntu 18.04, Ubuntu 18.04 in WSL2 inside Windows 11

**Editor:** VSCode (with `Remote - WSL` extension if using WSL)

**Linting:** `mypy` on VSCode using the [mypy](https://marketplace.visualstudio.com/items?itemName=matangover.mypy) extension installed. Be careful with the way the python language server is set up. Some linter setups won't find and lint imported python packages for typing (Spyder <=4.2.0 and vscode with pylance both had this problem). This setup does.

## Convenience
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


# Help
Running the following will tell you all the standard tasks for the GDS that have been automated in the `makefile` (for non-developers, this is a good starting place):

`make help`

# Update Dependencies:
`make update`

# Run a Script:
To run a python script using the appropriate `pyenv` as if it was executed from
the `GroundSoftware` directory, use `./run-script.sh`.

Example:
```
./run-script.sh ./scripts/tiny_apps/datastandards_lookup.py -l
```

# Run the Core:
The following will rebuild & cache the DataStandards then bring up all core IPC Apps:
`make run`

# Teardown and Cleanup Development Environment
Run this before reinitializing (in case something broke):

`make clean`

## Build
*TODO*
