#include "PocketHomeApplication.h"
#include "WifiSettingsComponent.h"

WifiSettingsComponent::WifiSettingsComponent
(std::function<void() > openWifiPage) :
ConnectionSettingsComponent(openWifiPage, "wifi"),
Localized("WifiSettingsComponent")
{
#    if JUCE_DEBUG
    setName("WifiSettingsComponent");
#    endif
    WifiStateManager wifiManager;
    wifiManager.addListener(this);
    refresh();
}

/**
 * Checks if wifi is currently turned on.
 */
bool WifiSettingsComponent::connectionEnabled()
{
    WifiStateManager wifiManager;
    switch (wifiManager.getWifiState())
    {
        case WifiStateManager::turningOn:
        case WifiStateManager::enabled:
        case WifiStateManager::connecting:
        case WifiStateManager::missingPassword:
        case WifiStateManager::connected:
        case WifiStateManager::disconnecting:
            return true;
    }
    return false;
}



/**
 * This method is used by the component to determine if it should show the
 * loading spinner.
 */
bool WifiSettingsComponent::shouldShowSpinner() 
{
    WifiStateManager wifiManager;
    switch (wifiManager.getWifiState())
    {
        case WifiStateManager::turningOn:
        case WifiStateManager::turningOff:
        case WifiStateManager::connecting:
        case WifiStateManager::disconnecting:
        case WifiStateManager::missingPassword:
            return true;
    }
    return false;
}

/**
 * This method is used by the component to determine if the connection 
 * switch should be enabled.
 */
bool WifiSettingsComponent::allowConnectionToggle()
{
    WifiStateManager wifiManager;
    switch (wifiManager.getWifiState())
    {
        case WifiStateManager::turningOn:
        case WifiStateManager::turningOff:
            return false;
    }
    return true;
}

/**
 * This method is used by the component to determine if the connection 
 * page should be accessible.
 * 
 * @return true whenever wifi is enabled and not being disabled.
 */
bool WifiSettingsComponent::connectionPageAvailable() 
{
    WifiStateManager wifiManager;
    switch (wifiManager.getWifiState())
    {
        case WifiStateManager::turningOn:
        case WifiStateManager::turningOff:
        case WifiStateManager::missingNetworkDevice:
        case WifiStateManager::disabled:
            return false;
    }
    return true;
}


/**
 * @return the wifi icon
 */
String WifiSettingsComponent::getIconAsset()
{
    return "wifiIcon.svg";
}

/**
 * Enable or disable the wifi radio.
 */
void WifiSettingsComponent::enabledStateChanged(bool enabled)
{
    WifiStateManager wifiManager;
    if (enabled)
    {
        wifiManager.enableWifi();
    }
    else
    {
        wifiManager.disableWifi();
    }
}

/**
 * Sets the wifi button text based on the current wifi state.
 */
String WifiSettingsComponent::updateButtonText()
{
    WifiStateManager wifiManager;
    switch (wifiManager.getWifiState())
    {
        case WifiStateManager::missingNetworkDevice:
            return localeText(wifi_not_found);
        case WifiStateManager::disabled:
            return localeText(wifi_disabled);
        case WifiStateManager::turningOn:
            return localeText(wifi_turning_on);
        case WifiStateManager::enabled:
            return localeText(not_connected);
        case WifiStateManager::turningOff:
            return localeText(wifi_turning_off);
        case WifiStateManager::connecting:
        {
            WifiAccessPoint::Ptr ap = wifiManager.getConnectingAP();
            if (ap == nullptr)
            {
                DBG("WifiSettingsComponent::" << __func__ << ": wifi is "
                        << "connecting, but can't get the connecting AP.");
                return localeText(connecting_to_unknown);
            }
            return String(localeText(connecting_to_ap)) + ap->getSSID();
        }
        case WifiStateManager::missingPassword:
            return localeText(missing_psk);
        case WifiStateManager::connected:
        {
            WifiAccessPoint::Ptr ap = wifiManager.getConnectedAP();
            if(ap == nullptr)
            {
                return "Error: connected AP missing!";
            }
            return ap->getSSID();
        }
        case WifiStateManager::disconnecting:
            return localeText(disconnecting);
        default:
            return "Unknown State";
    }
}

/**
 * Use wifi status updates to keep the component updated.
 */
void WifiSettingsComponent::wifiStateChanged
(WifiStateManager::WifiState state)
{
    MessageManager::callAsync([this]()
    {
        refresh();
    });
}
