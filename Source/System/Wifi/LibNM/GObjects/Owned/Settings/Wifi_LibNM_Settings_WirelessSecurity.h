#pragma once
/**
 * @file  Wifi_LibNM_Settings_WirelessSecurity.h
 *
 * @brief  Stores the security information needed to open a wireless network
 *         connection.
 */

#include "Wifi_LibNM_Settings_Object.h"
#include <nm-setting-wireless-security.h>

namespace Wifi
{
    namespace LibNM
    {
        namespace Settings { class WirelessSecurity; }
        enum class SecurityType;
    }
}

/**
 * @brief  Holds NMSettingWirelessSecurity* data, used to store wireless
 *         network connection security settings.
 */
class Wifi::LibNM::Settings::WirelessSecurity : public Object
{
public:
    /**
     * @brief  Creates a WirelessSecurity object to hold existing LibNM
     *         settings data.
     *
     * @param toAssign  The LibNM security settings object that the new
     *                  WirelessSecurity object will hold.
     */
    WirelessSecurity(NMSettingWirelessSecurity* toAssign);

    /**
     * @brief  Creates a WirelessSecurity object holding a new, empty LibNM
     *         security settings object.
     */
    WirelessSecurity();

    virtual ~WirelessSecurity() { }

    /**
     * @brief  Adds WPA security settings to this settings object.
     *
     * @param psk  The WPA key or passphrase used to secure this object's
     *             connection.
     *
     * @return     Whether the new settings were successfully added.
     */
    bool addWPASettings(const juce::String psk);

    /**
     * @brief  Adds WEP security settings to this settings object.
     *
     * @param psk  The WEP key used to secure this object's connection.
     *
     * @return     Whether the new settings were successfully added.
     */
    bool addWEPSettings(const juce::String psk);

    /**
     * @brief  Gets the basic security type defined by this settings object.
     *
     * @return  The type of security used by this object's connection.
     */
    SecurityType getSecurityType() const;

    JUCE_LEAK_DETECTOR(Wifi::LibNM::Settings::WirelessSecurity);
};
