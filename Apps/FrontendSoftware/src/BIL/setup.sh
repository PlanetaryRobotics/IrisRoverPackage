# Install pyenv:
## PYENV
# Configure shell for pyenv (before installing):
# the sed invocation inserts the lines at the start of the file
# after any initial comment lines
sed -Ei -e '/^([^#]|$)/ {a \
export PYENV_ROOT="$HOME/.pyenv"
a \
export PATH="$PYENV_ROOT/bin:$PATH"
a \
' -e ':a' -e '$!{n;ba};}' ~/.profile
echo 'eval "$(pyenv init --path)"' >>~/.profile

echo 'eval "$(pyenv init -)"' >> ~/.bashrc

# Add pyenv to bashrc (before installing):
echo '' >> ~/.bashrc
echo '# Add pyenv to local path automatically:' >> ~/.bashrc
echo 'eval "$(pyenv virtualenv-init -)"' >> ~/.bashrc

# Install pyenv
set -e
curl -s -S -L https://raw.githubusercontent.com/pyenv/pyenv-installer/master/bin/pyenv-installer | bash

# Set up tools used for development:
pyenv exec python3 -m pip install pyls-mypy
pyenv exec python3 -m pip install autopep8