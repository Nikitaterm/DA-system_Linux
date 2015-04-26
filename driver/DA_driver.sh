#!/bin/bash

# This script implements launching and finishing of DA_driver module with
# required settings.
# Should be executed after system boot in the autorun mode

DRIVER_PATH="/lib/modules/`uname -r`/kernel/drivers/gpio/DA_protocol.ko"

IRQ_NUM=60  # Interrupt number
CPU_NUM=2   # Related CPU number

function launch {
    echo "DA_driver launching..."
    # install the driver
    insmod $DRIVER_PATH
    if [ $? -ne 0 ]; then
        echo "DA_driver launching error"
        exit $?
    fi
    # set CPU number for interrupt handlers
    chmod u+w /proc/irq/$IRQ_NUM/smp_affinity
    echo $CPU_NUM >> /proc/irq/$IRQ_NUM/smp_affinity
    chmod u-w /proc/irq/$IRQ_NUM/smp_affinity
    echo "Done"
}

function stop {
    echo "DA_driver finishing...";
    # rmmod
    rmmod $DRIVER_PATH
    if [ $? -ne 0 ]; then
        echo "DA_driver finishing error"
        exit $?
    fi
    echo "Done"
}

ARG=$1

case $ARG in
    start)
        launch
        ;;
    stop)
        stop
        ;;
    *)
        echo "Usage: start|stop"
        exit 1
esac
