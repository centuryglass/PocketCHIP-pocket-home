#include <set>
#include "MainConfigFile.h"
#include "MainConfigKeys.h"
#include "NewConfigAppEditor.h"
#include "NewDesktopAppEditor.h"
#include "NewFolderEditor.h"
#include "AppMenuComponent.h"
#include "Utils.h"

//TODO: load these from config, set on input page
const juce::String AppMenuComponent::openPopupMenuBinding = "CTRL + e";
const juce::String AppMenuComponent::reloadMenuBinding = "TAB";

AppMenuComponent::AppMenuComponent(
        const juce::Identifier& componentKey,
        OverlaySpinner& loadingSpinner) :
Localized("AppMenuComponent"),
ConfigurableComponent(componentKey),
loadingState(false),
loadingSpinner(loadingSpinner),
juce::Component(componentKey.toString())
{
    addTrackedKey(MainConfigKeys::maxRowsKey);
    addTrackedKey(MainConfigKeys::maxColumnsKey);
    MainConfigFile mainConfig;
    maxRows = mainConfig.getConfigValue<int>(MainConfigKeys::maxRowsKey);
    maxColumns = mainConfig.getConfigValue<int>(MainConfigKeys::maxColumnsKey);
    setWantsKeyboardFocus(false);
    appLauncher.setLaunchFailureCallback([this]()
    {
        setLoadingState(false);
    });
    loadBaseFolder();
}

AppMenuComponent::~AppMenuComponent()
{
    desktopEntries.clearCallbacks();
}

/*
 * Checks if the menu is currently in the loading state, where it is busy 
 * loading menu items or launching an application.
 */
bool AppMenuComponent::isLoading() const
{
    return loadingState;
}

/*
 * Exit the loading state, hiding the spinner and enabling
 * user input
 */
void AppMenuComponent::exitLoadingState()
{
    setLoadingState(false);
}

/*
 * Open the pop-up menu used for editing menu items.
 */
void AppMenuComponent::openPopupMenu(AppMenuButton::Ptr selectedButton)
{
    using namespace juce;
    if (ignoringInput())
    {
        return;
    }
    PopupMenu editMenu;

    /*
     * The pop-up menu really shouldn't be appearing when the button editor
     * is visible, but if it still is, close and remove the editor.
     */
    if (buttonEditor != nullptr)
    {
        removeChildComponent(buttonEditor);
        buttonEditor = nullptr;
    }

    /* Menu Options:
     * 1. Edit button
     * 2. Delete button (and button source)
     * 3. Create new pinned favorites item
     * 4. Create new desktop entry
     * 5. Create new folder
     * 6. Add application to favorites
     * 7. Move button up
     * 8. Move button down
     */
    if (selectedButton != nullptr)
    {
        editMenu.addItem(1, localeText(edit_app));
        editMenu.addItem(2, localeText(delete_app));
        AppMenuItem selectedMenuItem = selectedButton->getMenuItem();
        DBG("AppMenuComponent::" << __func__ << ": Creating pop-up menu for "
                << "button at index " 
                << selectedMenuItem.getIndex().toString());
        if (selectedMenuItem.isFolder())
        {
            editMenu.addItem(4, localeText(new_shortcut));
        }
        else if (getActiveFolderIndex() > 0)
        {
            editMenu.addItem(6, localeText(add_shortcut));
        }
        if (selectedMenuItem.canMoveIndex(-1))
        {
            editMenu.addItem(7, localeText(move_back));
        }
        if (selectedMenuItem.canMoveIndex(1))
        {
            editMenu.addItem(8, localeText(move_forward));
        }
    }
    else
    {
        editMenu.addItem(3, localeText(new_shortcut));
        editMenu.addItem(4, localeText(new_entry));
        editMenu.addItem(5, localeText(new_folder));
    }

    AppMenuFolder* activeFolder = openFolders[getActiveFolderIndex()];
    const int selection = editMenu.show();
    std::function<void() > confirmNew = [this]()
    {
        loadBaseFolder();
    };
    switch (selection)
    {
        case 1://User selects "Edit"
            showMenuButtonEditor(selectedButton);
            break;
        case 2://User selects "Delete"
            selectedButton->confirmRemoveButtonSource([this, activeFolder]()
            {
                activeFolder->removeButton(activeFolder->getSelectedIndex());
                layoutFolders();
            });
            break;
        case 3://User selects "New favorite application"
            showPopupEditor(new NewConfigAppEditor(confirmNew));
            break;
        case 4://User selects "New application link"
        {
            AppMenuPopupEditor* newAppEditor
                    = new NewDesktopAppEditor(confirmNew);
            if (selectedButton != nullptr)
            {
                newAppEditor->setCategories
                        (selectedButton->getMenuItem().getCategories());
            }
            showPopupEditor(newAppEditor);
            break;
        }
        case 5://User selects "New folder"
        {
            AppMenuPopupEditor* newFolderEditor
                    = new NewFolderEditor(confirmNew);
            showPopupEditor(newFolderEditor);
            break;
        }
        case 6://User selects "Pin to favorites"
        {
            AppConfigFile appConfig;
            AppMenuItem rootItem = appConfig.getRootMenuItem();
            AppMenuItem selectedMenuItem = selectedButton->getMenuItem();
            MenuIndex newIndex = rootItem.getIndex()
                .childIndex(rootItem.getFolderSize());
            appConfig.addMenuItem(selectedMenuItem.getTitle(),
                    selectedMenuItem.getIconName(),
                    selectedMenuItem.getCommand(),
                    selectedMenuItem.getLaunchedInTerm(),
                    selectedMenuItem.getCategories(),
                    newIndex, true);
            confirmNew();
            break;
        }
        case 7://User selects "Move back"
            activeFolder->swapButtons(activeFolder->getSelectedIndex(),
                    activeFolder->getSelectedIndex() - 1);
            break;
        case 8://User selects "Move forward"
            activeFolder->swapButtons(activeFolder->getSelectedIndex(),
                    activeFolder->getSelectedIndex() + 1);
    }
}

/*
 * Pass on key events to the AppMenu from its containing page.
 * Opens the pop-up menu or reloads the AppMenu if the correct shortcuts
 * are detected, otherwise let the AppMenuComponent's subclass determine
 * how to handle the key event.
 */
bool AppMenuComponent::keyPressed(const juce::KeyPress& key)
{
    using namespace juce;
    if (ignoringInput())
    {
        return true;
    }
    if (key == KeyPress::createFromDescription(openPopupMenuBinding))
    {
        openPopupMenu(getSelectedButton());
        return true;
    }
    if (key == KeyPress::createFromDescription(reloadMenuBinding))
    {
        loadBaseFolder();
        return true;
    }
    return folderKeyPressed(key, openFolders.isEmpty()
            ? nullptr : openFolders[getActiveFolderIndex()]);
}

/*
 * Load and display the base menu folder that contains favorite 
 * application shortcuts and all other folders. Any existing open folders
 * will first be closed.  If the base folder was open, its selected 
 * index will be saved and re-selected.
 */
void AppMenuComponent::loadBaseFolder()
{
    using namespace juce;
    if (!isLoading())
    {
        int savedIndex = -1;
        if (!openFolders.isEmpty())
        {
            savedIndex = openFolders.getFirst()->getSelectedIndex();
            DBG("AppMenuComponent::" << __func__
                    << ":folder exists, saving index" << savedIndex);
        }
        while (openFolders.size() > 0)
        {
            closeFolder();
        }
        setLoadingState(true);
        desktopEntries.loadEntries([this](String loadingMsg)
        {
            if (!isLoading())
            {
                setLoadingState(true);
            }
            loadingSpinner.setLoadingText(loadingMsg);
        },
        [this, savedIndex]()
        {
            DBG("AppMenuComponent::" << __func__
                    << ": Loading desktop entries complete,"
                    << " creating base folder");
            AppConfigFile config;
            openFolder(config.getRootMenuItem());
            openFolders.getFirst()->selectIndex(savedIndex);
            loadingSpinner.setLoadingText("Building folder layout:");
            MessageManager::callAsync([this]()
            {
                layoutFolders();
                setLoadingState(false);
            });
        });
    }
}

/*
 * Close the active folder, removing it and all folders after it from
 * the menu component.
 */
void AppMenuComponent::closeFolder()
{
    using namespace juce;
    if (openFolders.size() > 0)
    {
        int targetFolderCount = getActiveFolderIndex();
        DBG("AppMenuComponent::" << __func__ << ": Closing folder "
                << String(openFolders.size() - 1));
        if (getActiveFolderIndex() > 0)
        {
            setActiveFolderIndex(getActiveFolderIndex() - 1);
            layoutFolders(true);
        }
        while (openFolders.size() > targetFolderCount)
        {
            openFolders.getLast()->deselect();
            removeChildComponent(openFolders.getLast());
            openFolders.removeLast();
        }
    }
}

/*
 * Closes the active folder, unless the active folder is at index 0,
 * in which case it will reload the folder.
 */
void AppMenuComponent::closeNonBaseFolder()
{
    if (openFolders.size() > 0)
    {
        if (getActiveFolderIndex() > 0)
        {
            closeFolder();
        }
        else
        {
            loadBaseFolder();
        }
    }
}

/*
 * Sets which open folder is currently focused.  This value will change to
 * the index of the last open folder whenever a folder opens or closes.
 */
void AppMenuComponent::setActiveFolderIndex(int folder)
{
    if (folder >= 0 && folder < openFolders.size())
    {
        activeFolderIndex = folder;
    }
}

/*
 * Get the index of the active folder.  AppMenuComponents should center the
 * folder layout on the active folder, and apply key controls to that
 * folder's menu items.
 */
int AppMenuComponent::getActiveFolderIndex() const
{
    return median<int>(0, activeFolderIndex, openFolders.size() - 1);
}

/*
 * Trigger a click for the selected button in the active AppFolder
 */
void AppMenuComponent::clickSelected()
{
    AppMenuButton::Ptr selected = getSelectedButton();
    if (selected != nullptr)
    {
        onButtonClick(selected);
    }
}

/*
 * Open a PopupEditorComponent for editing the selected button in the active 
 * AppFolder.
 */
void AppMenuComponent::openEditorForSelected()
{
    AppMenuButton::Ptr selected = getSelectedButton();
    if (selected != nullptr)
    {
        showMenuButtonEditor(selected);
    }
}

/*
 * @return the number of currently open folder components
 */
int AppMenuComponent::getNumFolders() const
{
    return openFolders.size();
}

/*
 * @return the number of buttons in the folder at this index, or -1
 * if there is no folder at this index.
 */
int AppMenuComponent::getFolderSize(int index)
{
    if (index < 0 || index >= openFolders.size())
    {
        return -1;
    }
    return openFolders.getUnchecked(index)->getButtonCount();
}

/*
 * @param index
 * @return the minimum width, in pixels, that the folder at this index
 * should have, or -1 if there is no folder at this index.
 */
int AppMenuComponent::getMinimumFolderWidth(int index)
{
    if (index < 0 || index >= openFolders.size())
    {
        return -1;
    }
    return openFolders.getUnchecked(index)->getMinimumWidth();
}

/*
 * @param index the index of one of this menu's AppMenuFolders
 * @return the selected menu button index for the folder, or -1 if
 * the folder at the index parameter doesn't exist or has no selected menu
 * button.
 */
int AppMenuComponent::getFolderSelectedIndex(int index) const
{
    if (index < 0 || index > openFolders.size())
    {
        return -1;
    }
    return openFolders.getUnchecked(index)->getSelectedIndex();
}

/*
 * Updates the folder component layout, optionally animating the transition.
 */
void AppMenuComponent::layoutFolders(bool animate)
{
    using namespace juce;
    if (getBounds().isEmpty())
    {
        return;
    }
    //DBG("AppMenuComponent::" << __func__
    //        << ": Updating all AppMenuFolder bounds");
    for (int i = 0; i < openFolders.size(); i++)
    {
        Rectangle<int> folderBounds = updateFolderBounds(openFolders[i], i);
        if (animate)
        {
            Desktop::getInstance().getAnimator().animateComponent(openFolders[i],
                    folderBounds, 1.0f, animationDuration, true, 0.0, 0.0);
        }
        else
        {
            openFolders[i]->setBounds(folderBounds);
        }
    }
}

/*
 * @return the maximum number of menu item columns to show on screen
 */
int AppMenuComponent::getMaxColumns() const
{
    return maxColumns;
}

/*
 * @return the maximum number of menu item rows to show on screen
 */
int AppMenuComponent::getMaxRows() const
{
    return maxRows;
}

/*
 * @param newVal if set to true, clicking unselected menu buttons 
 * only selects them. If set to false, clicking them also
 * immediately launches their application or opens their folder.
 * This is set to true by default.
 */
void AppMenuComponent::setOnlyTriggerSelected(bool newVal)
{
    onlyTriggerSelected = newVal;
}

/*
 * @return true iff the menu is loading, empty, animating, or otherwise
 * in a state where user input should be ignored.
 */
bool AppMenuComponent::ignoringInput() const
{
    using namespace juce;
    return openFolders.isEmpty() || isLoading()
            || (buttonEditor != nullptr && buttonEditor->isVisible())
            || Desktop::getInstance().getAnimator().isAnimating
            (openFolders[getActiveFolderIndex()]);
}

/*
 * Exit the loading state if the window loses focus.
 */
void AppMenuComponent::windowFocusLost()
{
    setLoadingState(false);
}

/*
 * Updates the layout if row/column size changes.
 */
void AppMenuComponent::configValueChanged(const juce::Identifier& key)
{
    MainConfigFile mainConfig;

    if (key == MainConfigKeys::maxColumnsKey)
    {
        maxColumns = mainConfig.getConfigValue<int>(key);
    }
    else if (key == MainConfigKeys::maxRowsKey)
    {
        maxRows = mainConfig.getConfigValue<int>(key);
    }
    for (AppMenuFolder* folder : openFolders)
    {
        folder->updateGridSize(maxRows, maxColumns);
    }
    layoutFolders();
}

/*
 * Reposition child components, and update folder layout without animating.
 */
void AppMenuComponent::resized()
{
    using namespace juce;
    menuResized();
    Rectangle<int> bounds = getLocalBounds();
//    DBG("AppMenuComponent::" << __func__ << ": bounds set to "
//            << getScreenBounds().toString());
    if (buttonEditor != nullptr)
    {
        buttonEditor->applyConfigBounds();
        buttonEditor->setCentrePosition(bounds.getCentreX(),
                bounds.getCentreY());
    }
    layoutFolders();
}

/*
 * Open an application category folder, creating or adding 
 * AppMenuButtons for all associated desktop applications.
 * If any folders after the active folder are already open, they
 * will first be closed.
 */
void AppMenuComponent::openFolder(const AppMenuItem& folderItem)
{
    while (getActiveFolderIndex() < openFolders.size() - 1)
    {
        removeChildComponent(openFolders.getLast());
        openFolders.removeLast();
    }

    AppMenuFolder* newFolder = createFolderObject(folderItem, buttonNameMap);
    openFolders.add(newFolder);
    newFolder->addMouseListener(this, false);
    newFolder->updateGridSize(maxRows, maxColumns);
    DBG("AppMenuComponent::" << __func__ << ": Opening folder "
            << openFolders.size() << " holding " << newFolder->getButtonCount()
            << " menu buttons.");
    addAndMakeVisible(newFolder);
    layoutFolders();
    setActiveFolderIndex(openFolders.size() - 1);
}

/*
 * @return the selected button in the active folder, or nullptr if
 * the active folder is empty or no folders are open.
 */
AppMenuButton::Ptr AppMenuComponent::getSelectedButton()
{
    if (!openFolders.isEmpty())
    {
        AppMenuFolder* folder = openFolders[getActiveFolderIndex()];
        return folder->getSelectedButton();
    }
    return nullptr;
}

/*
 * Adds and shows a new pop-up editor component, safely removing any previous
 * editor.
 */
void AppMenuComponent::showPopupEditor(AppMenuPopupEditor * editor)
{
    if (buttonEditor != nullptr)
    {
        removeChildComponent(buttonEditor);
    }
    buttonEditor = editor;
    addAndMakeVisible(buttonEditor);
    buttonEditor->setAlwaysOnTop(true);
    resized();
}

/*
 * Create and show a pop-up editor component for a button in this menu.
 * This will only work if the button is in the active folder and the menu 
 * is not ignoring input.
 */
void AppMenuComponent::showMenuButtonEditor(AppMenuButton::Ptr button)
{
    using namespace juce;
    if (ignoringInput() ||
        openFolders[getActiveFolderIndex()]->getButtonIndex(button) == -1)
    {
        return;
    }

    showPopupEditor(button->getEditor(
            [this, button]
            (AppMenuPopupEditor * editor)
            {
                openFolders[getActiveFolderIndex()]->repaint();
            }));
}

/*
 * Sets what should happen when a button is left clicked.
 * This opens selected buttons, and selects unselected buttons
 */
void AppMenuComponent::onButtonClick(AppMenuButton::Ptr button)
{
    using namespace juce;
    if (ignoringInput())
    {
        return;
    }
    for (int i = 0; i < openFolders.size(); i++)
    {
        int buttonIndex = openFolders[i]->getButtonIndex(button);
        //if button is not in this folder, move to the next one
        if (buttonIndex == -1)
        {
            continue;
        }
        //if the button is not in the last opened folder, close folders until
        //the button's folder is the last one.  Then, select the button.
        while (i < getActiveFolderIndex())
        {
            closeFolder();
            if (i == openFolders.size() - 1)
            {
                openFolders[i]->selectIndex(buttonIndex);
                return;
            }
        }
        if (buttonIndex != openFolders[i]->getSelectedIndex())
        {
            openFolders[i]->selectIndex(buttonIndex);
            layoutFolders();
            if (onlyTriggerSelected)
            {
                return;
            }
        }
        AppMenuItem buttonItem = button->getMenuItem();
        if (buttonItem.isFolder())
        {
            openFolder(buttonItem);
            layoutFolders(true);
        }
        else
        {
            setLoadingState(true);
            loadingSpinner.setLoadingText(String("Launching ")
                    + buttonItem.getTitle());
            appLauncher.startOrFocusApp(buttonItem.getCommand());
        }

    }
}

/*
 * Click AppMenuButtons on left click, open the pop-up menu
 * on right click or control click.
 */
void AppMenuComponent::mouseDown(const juce::MouseEvent & event)
{
    if (ignoringInput())
    {
        return;
    }

    AppMenuButton* appClicked = dynamic_cast<AppMenuButton*>
            (event.originalComponent);
    //handle right clicks/ctrl-clicks
    if (event.mods.isPopupMenu() || event.mods.isCtrlDown())
    {
        if (appClicked == nullptr
            || appClicked == getSelectedButton())
        {
            openPopupMenu(appClicked);
        }
        else
        {
            //On right click/ctrl-click, clicking an unselected
            //button should never do anything other than change
            //the selection
            bool selectOnly = onlyTriggerSelected;
            onlyTriggerSelected = true;
            onButtonClick(appClicked);
            onlyTriggerSelected = selectOnly;
        }

    }//handle regular clicks
    else if (event.mods.isLeftButtonDown() && appClicked != nullptr)
    {
        onButtonClick(appClicked);
    }
}

/*
 * Enter or exit the loading state, where the component shows the
 * loading spinner and disables user input.
 */
void AppMenuComponent::setLoadingState(bool loading)
{
    //DBG("AppMenuComponent::" << __func__
    //        << (loading ? ": started loading" : ": stopped loading"));
    if (loading != isLoading())
    {
        loadingState = loading;
        loadingSpinner.setVisible(loading);
        if (!loading)
        {
            loadingSpinner.setLoadingText(juce::String());
        }
    }
}

