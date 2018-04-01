/**
 * @file BluetoothStatus.h
 * 
 * TODO: Make this actually do something.
 */
#pragma once
#include "BluetoothDevice.h"
#include "JuceHeader.h"



class BluetoothStatus {
public:
    BluetoothStatus();
    
    OwnedArray<BluetoothDevice> devices;
    bool enabled = false;

    void populateFromJson(const var &json);
};