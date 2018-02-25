/**
 * @file DateTimePage.h
 * 
 * DateTimePage is a UI page component that allows the user to change
 * the clock display mode and adjust system time.
 */
#pragma once
#include "../Basic Components/DrawableImageButton.h"
#include "../Basic Components/ScalingLabel.h"
#include "../PageComponent.h"
#include "../../JuceLibraryCode/JuceHeader.h"

class DateTimePage : public PageComponent, private ComboBox::Listener {
public:
    DateTimePage();
    ~DateTimePage();
private:
    void pageButtonClicked(Button*) override;
    void comboBoxChanged(ComboBox*) override;

    static const Colour bgColour;
    static const String pageTitle;
    
    static const String clockModeLabelText;
    static const String clockMode24h;
    static const String clockModeAmPm;
    static const String clockModeNoShow;
    
    static const String reconfigureBtnText;
    static const String reconfigureCommand;
    static const String reconfErrorTitle;
    static const String reconfErrorPreCmd;
    static const String reconfErrorPostCmd;
    
    
    ScalingLabel titleLabel;
    ScalingLabel clockModeLabel;
    ComboBox setClockMode;

    //Button for setting system time
    TextButton reconfigureBtn;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DateTimePage)
};