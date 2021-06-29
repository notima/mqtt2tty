## Dependencies

    apt install libmosquitto-dev libconfig-dev liblog4cpp5-dev

## Create Snap package

    snapcraft

## Install and run Snap application

    sudo snap install mqtt2tty_0.1_amd64.snap --dangerous
    sudo snap connect mqtt2tty:fs-access
    sudo systemctl restart snap.mqtt2tty.daemon.service

## Configure application

configuration files are located in `/var/snap/mqtt2tty/current/etc`