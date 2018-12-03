#pragma once
#include "Wifi/Listeners/UpdateInterfaces/DeviceUpdateInterface.h"
#include "SharedResource/Handler.h"

/**
 * @file Wifi/Listeners/DeviceListener.h
 *
 * @brief  Checks if a Wifi device is present and enabled, and receives signals
 *         whenever Wifi is enabled or disabled.
 */
class Wifi::DeviceListener : public SharedResource::Handler<DeviceTracker>,
    public DeviceUpdateInterface
{
public:
    DeviceListener() { }

    virtual ~DeviceListener() { }

    /**
     * @brief  Checks if a Wifi device managed by NetworkManager exists.
     *
     * @return  Whether a managed Wifi device was found.
     */
    bool wifiDeviceExists() const;

    /**
     * @brief  Checks if the managed Wifi device is currently enabled.
     *
     * @return  Whether wireless networking is enabled. 
     */
    bool wifiDeviceEnabled() const;

private:
    /**
     * @brief  Called whenever wireless networking is enabled.
     *
     * Override this to define how the DeviceListener should handle the update.
     */
    virtual void wirelessEnabled() override { }

    /**
     * @brief  Called whenever wireless networking is disabled.
     *
     * Override this to define how the DeviceListener should handle the update.
     */
    virtual void wirelessDisabled() override { }
};
