#include <map>
#include "JuceHeader.h"
#include "AssetFiles.h"
#include "JsonWifiInterface.h"
#include "TempTimer.h"

JsonWifiInterface::JsonWifiInterface(CriticalSection& wifiLock) : 
WifiStateManager::NetworkInterface(wifiLock),
wifiLock(wifiLock) 
{
    auto json = JSON::parse(AssetFiles::findAssetFile("wifi.json"));

    for (const auto &apJson : *json.getArray())
    {
        WifiAccessPoint fakeAP(
                apJson["name"],
                apJson["strength"],
                apJson["auth"],
                apJson["name"]);
        jassert(!fakeAP.isNull());
        visibleAPs.add(fakeAP);
    }
    confirmWifiState();
}

JsonWifiInterface::~JsonWifiInterface() { }


bool JsonWifiInterface::wifiDeviceFound()
{
    return true;
}

/*
 * Checks if the simulated wifi device is enabled.
 */
bool JsonWifiInterface::isWifiEnabled()
{
    ScopedLock lock(wifiLock);
    return enabled;
}

/*
 * Checks if a connection event is currently being simulated.
 */
bool JsonWifiInterface::isWifiConnecting()
{
    ScopedLock lock(wifiLock);
    return !waitingToConnect.isNull();
}

/*
 *  Checks if a simulated connection has been created.
 */
bool JsonWifiInterface::isWifiConnected()
{
    ScopedLock lock(wifiLock);
    return connected;
}


/*
 * Returns the connected or connecting access point.
 */
WifiAccessPoint JsonWifiInterface::getActiveAP()
{
    ScopedLock lock(wifiLock);
    if (!connected)
    {
        return waitingToConnect;
    }
    return connectedAP;
}

/*
 * Returns the entire list of access points defined in wifi.json.
 */
Array<WifiAccessPoint> JsonWifiInterface::getVisibleAPs()
{
    Array<WifiAccessPoint> accessPoints;
    if (isWifiEnabled())
    {
        return visibleAPs;
    }
    return {};
}

/*
 * Triggers a wifi connection event.  This will set a timer to simulate
 * the connection's success or failure after a randomized delay of two to
 * four seconds.
 */
void JsonWifiInterface::connectToAccessPoint(const WifiAccessPoint& toConnect,
        String psk)
{
    ScopedLock lock(wifiLock);
    if(toConnect.isNull())
    {
        DBG("JsonWifiInterface::" << __func__
                << ": can't connect to null access point");
        return;
        
    }
    DBG("JsonWifiInterface::" << __func__ << ": trying to connect to "
            << toConnect.getSSID());
    
    if (turningOff)
    {
        DBG("JsonWifiInterface::" << __func__
                << ": can't connect, wifi is turning off");
    }
    else if (turningOn)
    {
        DBG("JsonWifiInterface::" << __func__
                << ": can't connect, wifi still turning on");
    }
    else if (!enabled)
    {
        DBG("JsonWifiInterface::" << __func__ 
                << ": can't connect, wifi isn't on");
    }
    else if (!waitingToConnect.isNull())
    {
        if (waitingToConnect != toConnect)
        {
            DBG("JsonWifiInterface::" << __func__
                    << ": canceling, a connection to " <<
                    waitingToConnect.getSSID() << " is pending ");
        }
        else
        {
            DBG("JsonWifiInterface::" << __func__ << ": already connecting to "
                    << toConnect.getSSID());
        }
    }
    else if (connected)
    {
        DBG("JsonWifiInterface::" << __func__ << ": already connected to "
                << toConnect.getSSID());
    }
    else
    {
        //stop the connection timeout timer, since there are no simulated
        //connection steps generated to reset it
        stopTimer();
        waitingToConnect = toConnect;
        // try to connect to ap, dispatch events on success and failure
        TempTimer::initTimer(Random().nextInt(2000) + 2000,
                [this, psk]()
                {
                    ScopedLock lock(wifiLock);
                    bool isTestCred = (waitingToConnect.getSSID() == "MyFi");
                    if (!isTestCred)
                    {
                        DBG("JsonWifiInterface::" << __func__
                                << ": failed to connect");
                                connected = false;
                                waitingToConnect = WifiAccessPoint();
                                signalConnectionFailed();
                        return;
                    }

                    if (!psk.isEmpty())
                    {
                        DBG("JsonWifiInterface::" << __func__
                                << ": connected with psk");
                    }
                    else
                    {
                        DBG("JsonWifiInterface::" << __func__ 
                                << ": missing psk! (any is valid)");
                        connected = false;
                        waitingToConnect = WifiAccessPoint();
                        signalPskNeeded();
                        return;
                    }
                    connected = true;
                    connectedAP = waitingToConnect;
                    waitingToConnect = WifiAccessPoint();
                    if(connectedAP.isNull())
                    {
                        signalWifiDisconnected();
                    }
                    else
                    {
                        signalWifiConnected(connectedAP);
                    }                
                });
    }
}

/*
 * Triggers a simulated wifi disconnection event.  If a simulated connection
 * exists, after a randomized delay of no more than two seconds, a 
 * disconnection event will trigger.
 */
void JsonWifiInterface::disconnect()
{
    ScopedLock lock(wifiLock);
    if (!connected)
    {
        DBG("JsonWifiInterface::" << __func__ << ": no connection to kill");
    }
    else
    {
        TempTimer::initTimer(Random().nextInt(2000), [this]()
        {
            ScopedLock lock(wifiLock);
            DBG("JsonWifiInterface::" << __func__ << ": wifi disconnected");
            connectedAP = WifiAccessPoint();
            connected = false;
            signalWifiDisconnected();
        });

    }
}

/*
 * Turns on the simulated wifi device.  This will trigger a wifi enabled
 * event after a randomized delay of no more than two seconds.
 */
void JsonWifiInterface::enableWifi()
{

    ScopedLock lock(wifiLock);
    if (turningOn)
    {
        DBG("JsonWifiInterface::" << __func__ << ": already enabling wifi!");
    }
    else if (turningOff)
    {
        DBG("JsonWifiInterface::" << __func__
                << ": can't enable wifi, busy disabling");
    }
    else if (!enabled)
    {

        DBG("JsonWifiInterface::" << __func__ << ": enabling wifi...");
        turningOn = true;
        turningOff = false;
        TempTimer::initTimer(Random().nextInt(2000), [this]()
        {
            ScopedLock lock(wifiLock);
            if (turningOn)
            {
                DBG("JsonWifiInterface::" << __func__ << ": wifi enabled");
                        turningOn = false;
                        enabled = true;
                        signalWifiEnabled();
            }
        });
    }
}

/*
 * Turns off the simulated wifi device.  This will trigger a wifi disabled
 * event after a randomized delay of no more than two seconds.
 */
void JsonWifiInterface::disableWifi()
{
    if (turningOff)
    {
        DBG("JsonWifiInterface::" << __func__ << ": already disabling wifi!");
    }
    else if (enabled)
    {

        DBG("JsonWifiInterface::" << __func__ << ": disabling wifi...");
        turningOn = false;
        turningOff = true;
        TempTimer::initTimer(Random().nextInt(2000), [this]()
        {
            ScopedLock lock(wifiLock);
            if (turningOff)
            {
                DBG("JsonWifiInterface::" << __func__ << ": wifi disabled");
                        turningOff = false;
                        enabled = false;
                        signalWifiDisabled();
            }
        });
    }
}

       
/**
 * Finds the current network state of an access point object.
 */
WifiStateManager::AccessPointState JsonWifiInterface::getAPState
(const WifiAccessPoint& accessPoint)
{
    if(accessPoint.isNull())
    {
        return WifiStateManager::nullAP;
    }
    if(accessPoint == connectedAP)
    {
        if(connected)
        {
            return WifiStateManager::connectedAP;
        }
        if(disconnecting)
        {
            return WifiStateManager::disconnectingAP;
        }
    }
    if(accessPoint == waitingToConnect)
    {
        return WifiStateManager::connectingAP;
    }
    if(enabled && visibleAPs.contains(accessPoint))
    {
        return WifiStateManager::disconnectedAP;
    }
    return WifiStateManager::missingAP;
}