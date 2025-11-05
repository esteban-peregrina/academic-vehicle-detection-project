FROM ubuntu

### Image building configuration ###
# === CURRENT BUILDING USER IS ROOT === #

# Define running arguments
ARG USERNAME=devcontainer
ARG USER_ID=1001
ARG GROUPNAME=$USERNAME
ARG GROUP_ID=$USER_ID

# Create a non-root user, a hidden "config" directory, and configure its permission (because we're currently building as root)
RUN groupadd --gid $GROUP_ID $GROUPNAME \
    && useradd -s /bin/bash --uid $USER_ID --gid $GROUP_ID -m $USERNAME \
    && mkdir /home/$USERNAME/.config \
    && chown $USER_ID:$GROUP_ID /home/$USERNAME/.config

# Install necessary packages at build
RUN apt-get update \
    && apt-get install -y \
        gcc \
        g++ \
        make \
        git \
        bash-completion \
        libopencv-dev \
        libboost-system-dev \
        libsfml-dev \
        x11-apps \
        libxcb-xinerama0 \
        libxcb-xinput0 \
    && rm -rf /var/lib/apt/lists/*

# Change current user to $USERNAME
USER $USERNAME

# === CURRENT BUILDING USER IS NOW $USERNAME === #

# Change current working directory to workspace (creates it)
WORKDIR /workspace
