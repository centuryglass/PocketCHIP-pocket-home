#!/bin/bash
# Checks if a battery is available
#
# Prints the battery voltage file or device if a battery is found.

# Check for PocketCHIP battery voltage file:
voltagePath="/usr/lib/pocketchip-batt/voltage"
if [ -f "$voltagePath" ]; then
    echo "$voltagePath"
fi

# Check for battery devices through upower:
if [ ! -z `command -v upower` ]; then
    batteryDevice=`upower -e | grep -i -m 1 'BAT'`
    if [ ! -z "$batteryDevice" ]; then
        echo "$batteryDevice"
        exit 0
    fi
fi

# Check for battery devices through acpi:
if [ ! -z `command -v acpi` ]; then
    batteryDevice=`acpi -b`
    if [ ! -z "$batteryDevice" ]; then
        echo "$batteryDevice"
        exit 0
    fi
fi

# Check for battery directories in /sys/class/power_supply:
if [ -d /sys/class/power_supply ]; then
    batteryDir=`ls /sys/class/power_supply`
    if [ ! -z "$batteryDir" ]; then
        echo "$batteryDir"
        exit 0
    fi
fi

# Check for battery directories in /proc/acpi/battery (deprecated):
if [ -d /proc/apci/battery ]; then
    batteryDir=`ls /proc/acpi/battery`
    if [ ! -z "$batteryDir" ]; then
        echo "$batteryDir"
        exit 0
    fi
fi