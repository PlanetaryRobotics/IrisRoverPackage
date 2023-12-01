# Use an official Python base image with the specified version
FROM python:3.10.8-buster

# Set the working directory inside the container
WORKDIR /IRP/Apps/GroundSoftware/

# Add a function to handle any 'pyenv exec python3' (e.g. in the makefile) as 'python3':
COPY --from=gsw ./setup/docker/pyenv.sh /usr/local/bin/pyenv
RUN chmod +x /usr/local/bin/pyenv
# Make sure it works at this stage:
RUN pyenv exec python3

# Perform General Setup (don't need to do redis steps here,
# that's a separate container):
RUN apt-get update && apt-get install -y sudo
COPY --from=gsw ./setup/parts ./setup/parts
RUN chmod +x ./setup/parts/linux_deps_os_core.sh && ./setup/parts/linux_deps_os_core.sh

# Install Python dependencies:
COPY --from=gsw requirements-dev.txt .
RUN pip install --no-cache-dir -r requirements-dev.txt
COPY --from=gsw requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Make sure the python path includess "this" (the running) directory:
ENV PYTHONPATH ".:$PYTHONPATH"

# Copy over the rest of the application code
# (wait till the end to do this so src changes don't invalidate higher layers):
COPY --from=gsw . .
# Copy over relevant Flight Software (FSW) code needed for building the data standards:
COPY --from=fsw ./fprime/ /IRP/Apps/FlightSoftware/fprime/

# Build the datastandards so they're embedded in the image:L
RUN make standards

# Make the default command just leave the image open (so we can interact with
# it):
CMD ["tail", "-f", "/dev/null"]
