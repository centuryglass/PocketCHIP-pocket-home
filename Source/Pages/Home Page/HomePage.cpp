#include "ConfigurableImageComponent.h"
#include "MainConfigFile.h"
#include "PokeLookAndFeel.h"
#include "AssetFiles.h"
#include "PagedAppMenu.h"
#include "ScrollingAppMenu.h"
#include "HomePage.h"

HomePage::HomePage(PageFactoryInterface& pageFactory,
        WifiStateManager& wifiState,
        MainConfigFile& mainConfig,
        ComponentConfigFile& componentConfig) :
PageComponent(pageFactory, "HomePage"),
frame(ComponentConfigFile::menuFrameKey, 0, RectanglePlacement::stretchToFit),
wifiIcon(wifiState),
mainConfig(mainConfig),
componentConfig(componentConfig),
powerButton(ComponentConfigFile::powerButtonKey),
settingsButton(ComponentConfigFile::settingsButtonKey)
{
#    if JUCE_DEBUG
    setName("HomePage");
#    endif

    mainConfig.addListener(this,{
        MainConfigFile::backgroundKey,
        MainConfigFile::menuTypeKey
    });

    setWantsKeyboardFocus(true);
    addAndMakeVisible(frame);
    addAndMakeVisible(clock);

    addAndMakeVisible(batteryIcon);
    addAndMakeVisible(wifiIcon);

    powerButton.addListener(this);
    powerButton.setWantsKeyboardFocus(false);
    addAndMakeVisible(powerButton);

    settingsButton.addListener(this);
    settingsButton.setWantsKeyboardFocus(false);
    addAndMakeVisible(settingsButton);

    settingsPage = new SettingsPage();

    addChildComponent(loadingSpinner);
    loadingSpinner.setAlwaysOnTop(true);

    loadAllConfigProperties();
}

HomePage::~HomePage() { }

void HomePage::loadConfigProperties(ConfigFile* config, String key)
{
    MainConfigFile mainConf = MainConfigFile();
    if (mainConf == *config)
    {
        if (key == MainConfigFile::backgroundKey)
        {
            String background = mainConf.getConfigValue<String>
                    (MainConfigFile::backgroundKey);
            if (background.containsOnly("0123456789ABCDEFXabcdefx"))
            {
                setBackgroundImage(Image());
                Colour bgColour(background.getHexValue32());
                setColour(backgroundColourId, bgColour.withAlpha(1.0f));
            }
            else
            {
                setBackgroundImage(AssetFiles::createImageAsset(background));
            }
        }
        else if (key == MainConfigFile::menuTypeKey)
        {
            String menuType = mainConf.getConfigValue<String>
                    (MainConfigFile::menuTypeKey);
            if (!MainConfigFile::menuTypes.contains(menuType))
            {
                DBG("HomePage::" << __func__ << ": Invalid menu type!");
                return;
            }
            if (appMenu != nullptr)
            {
                removeChildComponent(appMenu);
                appMenu = nullptr;
            }


            DBG("HomePage::" << __func__ << ": Menu type is " << menuType);
            if (menuType == "Scrolling menu")
            {
                if (!isClass<AppMenuComponent, ScrollingAppMenu>(appMenu.get()))
                {
                    DBG("HomePage::" << __func__
                            << ": Initializing scrolling menu");
                    appMenu = new ScrollingAppMenu(mainConfig,
                            componentConfig,
                            appConfig,
                            loadingSpinner);
                }
                else
                {
                    DBG("HomePage::" << __func__
                            << ": Menu was already scrolling, don't recreate");
                }
            }
            else//menuType == "pagedMenu"
            {
                if (!isClass<AppMenuComponent, PagedAppMenu>(appMenu.get()))
                {
                    DBG("HomePage::" << __func__
                            << ": Initializing paged menu");
                    appMenu = new PagedAppMenu(mainConfig,
                            componentConfig,
                            appConfig,
                            loadingSpinner);
                }
                else
                {
                    DBG("HomePage::" << __func__ <<
                            ": Menu was already paged, don't recreate");
                }
            }
            addAndMakeVisible(appMenu);
            appMenu->toBack();
            pageResized();
        }

    }
}


//Forward all clicks (except button clicks) to the appMenu 

void HomePage::mouseDown(const MouseEvent &event)
{
    if (event.mods.isPopupMenu() || event.mods.isCtrlDown())
    {
        appMenu->openPopupMenu(nullptr);
    }
}

void HomePage::pageButtonClicked(Button * button)
{
    if (button == &settingsButton)
    {
        pushPageToStack(PageComponent::PageType::Settings);
    }
    else if (button == &powerButton)
    {
        pushPageToStack(PageComponent::PageType::Power,
                PageComponent::Animation::slideInFromRight);
    }
}

bool HomePage::keyPressed(const KeyPress& key)
{
    //don't interrupt animation or loading
    if (Desktop::getInstance().getAnimator().isAnimating(appMenu)
        || appMenu->isLoading())
    {
        return true;
    }
    else return appMenu->keyPressed(key);
}

void HomePage::visibilityChanged()
{
    if (isShowing())
    {
        MessageManager::callAsync([this]()
        {
            grabKeyboardFocus();
        });
    }
}

void HomePage::pageResized()
{
    if (appMenu != nullptr)
    {
        appMenu->applyConfigBounds();
    }
    loadingSpinner.setBounds(getLocalBounds());
    frame.applyConfigBounds();
    clock.applyConfigBounds();
    batteryIcon.applyConfigBounds();
    wifiIcon.applyConfigBounds();
    powerButton.applyConfigBounds();
    settingsButton.applyConfigBounds();
}
