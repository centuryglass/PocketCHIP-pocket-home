#include "HomePage.h"
#include "Layout_Component_JSONKeys.h"
#include "Theme_Image_JSONKeys.h"
#include "Assets.h"
#include "AppMenu.h"
#include "AppMenu_ConfigFile.h"
#include "Page_Type.h"
#include "Config_MainFile.h"
#include "Util_SafeCall.h"


// Initializes all page components and creates the AppMenu.
HomePage::HomePage() :
pageListener(*this),
frame(Theme::Image::JSONKeys::menuFrame, 0,
        juce::RectanglePlacement::stretchToFit),
powerButton(Theme::Image::JSONKeys::powerButton),
settingsButton(Theme::Image::JSONKeys::settingsButton)
{
    #if JUCE_DEBUG
    setName("HomePage");
    #endif

    addTrackedKey(Theme::Image::JSONKeys::homeBackground);
    setWantsKeyboardFocus(false);

    using namespace Layout::Component;
    layoutManagers.add(Manager(&clock, JSONKeys::clockLabel));
    layoutManagers.add(Manager(&powerButton, JSONKeys::powerButton));
    layoutManagers.add(Manager(&settingsButton, JSONKeys::settingsButton));
    layoutManagers.add(Manager(&frame, JSONKeys::menuFrame));
    addAndMakeVisible(frame);
    addAndMakeVisible(clock);
    addAndMakeVisible(batteryIcon);

    #ifdef WIFI_SUPPORTED
    layoutManagers.add(Manager(&wifiIcon, JSONKeys::wifiIcon));
    addAndMakeVisible(wifiIcon);
    #endif

    Config::MainFile mainConfig;
    if (mainConfig.getIPLabelOnHomePage())
    {
        ipLabel.reset(new Info::IPLabel);
        layoutManagers.add(Manager(ipLabel.get(), JSONKeys::homeIPLabel));
        addAndMakeVisible(ipLabel.get());
        ipLabel->setJustificationType(juce::Justification::centred);
    }
    powerButton.addListener(&pageListener);
    powerButton.setWantsKeyboardFocus(false);
    addAndMakeVisible(powerButton);

    settingsButton.addListener(&pageListener);
    settingsButton.setWantsKeyboardFocus(false);
    addAndMakeVisible(settingsButton);

    AppMenu::ConfigFile appConfig;
    appMenu.reset(AppMenu::createAppMenu());
    const AppMenu::Format menuFormat = appConfig.getMenuFormat();
    DBG("HomePage::" << __func__ << ": Initialized "
            << appConfig.formatToString(menuFormat) << " AppMenu");
    appMenu->setBounds(getLocalBounds());
    addAndMakeVisible(appMenu.get());
    appMenu->toBack();
    if (isShowing())
    {
        appMenu->grabKeyboardFocus();
    }
    loadAllConfigProperties();
    colourChanged();
}


// Tracks page background changes.
void HomePage::configValueChanged(const juce::Identifier& key)
{
    using juce::String;
    using juce::Image;
    if (key == Theme::Image::JSONKeys::homeBackground)
    {
        String background = imageConfig.getHomeBackgroundPath();
        setBackgroundImage(Assets::loadImageAsset(background));
    }
    else
    {
        //invalid key!
        jassertfalse;
    }
}


// Updates child components when HomePage colour values change.
void HomePage::colourChanged()
{
    const juce::Colour textColour = findColour(textColourId);
    clock.setColour(juce::Label::textColourId, textColour);
    batteryIcon.setTextColour(textColour);
    if (ipLabel != nullptr)
    {
        ipLabel->setColour(juce::Label::textColourId, textColour);
    }
}


// Opens the power page or the settings page, depending on which button was
// clicked.
void HomePage::PageListener::buttonClicked(juce::Button * button)
{
    if (button == &homePage.settingsButton)
    {
        homePage.pushPageToStack(Page::Type::quickSettings);
    }
    else if (button == &homePage.powerButton)
    {
        homePage.pushPageToStack(Page::Type::power,
                Layout::Transition::Type::moveRight);
    }
}


// Grab keyboard focus when the page becomes visible.
void HomePage::visibilityChanged()
{
    if (isShowing())
    {
        Util::SafeCall::callAsync<HomePage>(this, [](HomePage* page)
        {
            page->appMenu->grabKeyboardFocus();
        });
    }
}


// Update all child component bounds when the page is resized.
void HomePage::pageResized()
{
    if (appMenu != nullptr)
    {
        appMenu->setBounds(getLocalBounds());
    }
    batteryIcon.applyConfigBounds();
    for (Layout::Component::Manager& layoutManager : layoutManagers)
    {
        layoutManager.applyConfigBounds();
    }
}
