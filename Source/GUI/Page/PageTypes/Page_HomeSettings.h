#pragma once
/**
 * @file  Page_HomeSettings.h
 *
 * @brief  Allows the user to configure the HomePage, setting the page
 *         background, the application menu type, and the application menu
 *         dimensions.
 */

#include "Page_Component.h"
#include "Locale_TextUser.h"
#include "Widgets_BoundedLabel.h"
#include "Settings_HomeBackgroundPicker.h"
#include "Settings_ClockModePicker.h"
#include "AppMenu_SettingsController.h"
#include "Widgets_Counter.h"

namespace Page { class HomeSettings; }

/**
 * @brief  The page component used to hold all HomePage and AppMenu settings
 *         control components.
 */
class Page::HomeSettings : public Component, public Locale::TextUser
{
public:
    /**
     * @brief  Initializes the page layout.
     */
    HomeSettings();

    /**
     * @brief  Updates AppMenu settings when the page closes.
     */
    virtual ~HomeSettings();

private:
    /**
     * @brief  Gets the base page layout used to construct both the landscape
     *         and portrait mode page layouts.
     *
     * @return  The incomplete base page layout.
     */
    Layout::Group::RelativeLayout getBaseLayout();

    /**
     * @brief  Gets the landscape mode page layout.
     *
     * @return  The layout to use when the window is wider than it is tall.
     */
    Layout::Group::RelativeLayout getLandscapeLayout();

    /**
     * @brief  Gets the portrait mode page layout.
     *
     * @return  The layout to use when the window is taller than it is wide.
     */
    Layout::Group::RelativeLayout getPortraitLayout();

    // Page title label:
    Widgets::BoundedLabel title;

    // Used to set the HomePage's background image:
    Widgets::BoundedLabel bgLabel;
    Settings::HomeBackgroundPicker homeBGPicker;

    // Used to set the Clock display mode:
    Widgets::BoundedLabel clockLabel;
    Settings::ClockModePicker clockModePicker;

    // Used to manage AppMenu setting controls:
    AppMenu::SettingsController menuController;

    // Used to select the AppMenu format:
    Widgets::BoundedLabel menuFormatLabel;
    juce::ComboBox menuFormatPicker;

    // Used to select the maximum column count for AppMenu grids:
    Widgets::BoundedLabel columnCountLabel;
    Widgets::Counter columnCounter;

    // Used to select the maximum row count for AppMenu grids:
    Widgets::BoundedLabel rowCountLabel;
    Widgets::Counter rowCounter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HomeSettings)
};
