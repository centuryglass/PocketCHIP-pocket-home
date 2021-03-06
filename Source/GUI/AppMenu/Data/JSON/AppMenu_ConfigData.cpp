#define APPMENU_IMPLEMENTATION
#include "AppMenu_ConfigData.h"
#include "AppMenu_MenuKeys.h"
#include "AppMenu_MenuFile.h"

#ifdef JUCE_DEBUG
// Print the full class name before all debug output:
static const constexpr char* dbgPrefix = "AppMenu::ConfigData::";
#endif

// Localized object class key:
static const juce::Identifier localeClassKey = "AppMenu::ConfigData";

// Localized text value keys:
namespace TextKey
{
    namespace App
    {
        static const juce::Identifier remove     = "removeAPP";
        static const juce::Identifier fromMenu   = "fromMenu";
        static const juce::Identifier willRemove = "willRemoveApp";
        static const juce::Identifier edit       = "editApp";
    }
    namespace Folder
    {
        static const juce::Identifier remove     = "deleteNAME";
        static const juce::Identifier folder     = "folder";
        static const juce::Identifier willRemove = "willRemoveFolder";
        static const juce::Identifier edit       = "editFolder";
    }
}


// Creates a new menu item that initially holds no data.
AppMenu::ConfigData::ConfigData() : Locale::TextUser(localeClassKey) { }


// Recursively initializes menu item data, creating and initializing all its
// child folder items.
void AppMenu::ConfigData::initMenuData(juce::var& menuData)
{
    using juce::var;
    using juce::String;
    if (initialized)
    {
        DBG(dbgPrefix << __func__ << ": initialization called more than once!");
        return;
    }
    initialized = true;
    if (menuData.isVoid())
    {
        DBG(dbgPrefix << __func__ << ": Menu data is void");
        return;
    }

    var folderItems;
    if (menuData.isObject())
    {
        title = menuData.getProperty(MenuKeys::titleKey, String());
        iconName = menuData.getProperty(MenuKeys::iconKey, String());
        command = menuData.getProperty(MenuKeys::commandKey, String());
        launchInTerm = menuData.getProperty(MenuKeys::launchInTermKey, false);
        var categoryVar = menuData.getProperty(MenuKeys::categoryKey, var());
        folderItems = menuData.getProperty(MenuKeys::folderItemKey, var());
        if (categoryVar.isArray())
        {
            for (const var& category : *categoryVar.getArray())
            {
                categories.add(category.operator String());
            }
        }
    }
    if (folderItems.isVoid() && menuData.size() > 0)
    {
        folderItems = menuData;
    }
    if (folderItems.isArray())
    {
        DBG(dbgPrefix << __func__ << ": Initializing " << folderItems.size()
                << " folder items.");
        for (var& folderItem : *folderItems.getArray())
        {
            ItemData::Ptr child = createChildItem();
            ((ConfigData*)child.get())->initMenuData(folderItem);
            insertChild(child, getFolderSize());
        }
    }
}


// Gets the menu item's displayed title.
juce::String AppMenu::ConfigData::getTitle() const
{
    return title;
}


// Gets the name or path used to load the menu item's icon file.
juce::String AppMenu::ConfigData::getIconName() const
{
    return iconName;
}


// Gets the menu item's application launch command.
juce::String AppMenu::ConfigData::getCommand() const
{
    return command;
}


// Checks if this menu item launches an application in a new terminal window.
bool AppMenu::ConfigData::getLaunchedInTerm() const
{
    return launchInTerm;
}


// Gets the application categories used to load this item's desktop entry child
// folder items.
juce::StringArray AppMenu::ConfigData::getCategories() const
{
    return categories;
}


// Sets the menu item's displayed title.
void AppMenu::ConfigData::setTitle(const juce::String& title)
{
    this->title = title;
    signalDataChanged(DataField::title);
}


// Sets the name or path used to load the menu item's icon file.
void AppMenu::ConfigData::setIconName(const juce::String& iconName)
{
    this->iconName = iconName;
    signalDataChanged(DataField::icon);
}


// Sets the menu item's application launch command.
void AppMenu::ConfigData::setCommand(const juce::String& command)
{
    this->command = command;
    signalDataChanged(DataField::command);
}


// Sets if this menu item runs its command in a new terminal window.
void AppMenu::ConfigData::setLaunchedInTerm(const bool launchInTerm)
{
    this->launchInTerm = launchInTerm;
    signalDataChanged(DataField::termLaunchOption);
}


// Sets the application categories used to load this item's desktop entry child
// folder items.
void AppMenu::ConfigData::setCategories(const juce::StringArray& categories)
{
    this->categories = categories;
    signalDataChanged(DataField::categories);
}


// Gets the number of folder items held by this menu item that can be reordered.
int AppMenu::ConfigData::getMovableChildCount() const
{
    int maxPossible = getFolderSize();
    for (int i = 0; i < maxPossible; i++)
    {
        if (!getChild(i)->isMovable())
        {
            return i;
        }
    }
    return maxPossible;
}


// Checks if this menu item could be moved within its folder.
bool AppMenu::ConfigData::isMovable() const
{
    return true;
}


// Gets an appropriate title to use for a deletion confirmation window.
juce::String AppMenu::ConfigData::getConfirmDeleteTitle() const
{
    if (isFolder())
    {
        return localeText(TextKey::Folder::remove) + getTitle()
            + localeText(TextKey::Folder::folder);
    }
    else
    {
        return localeText(TextKey::App::remove) + getTitle()
            + localeText(TextKey::App::fromMenu);
    }
}


// Gets appropriate descriptive text for a deletion confirmation window.
juce::String AppMenu::ConfigData::getConfirmDeleteMessage() const
{
    return localeText(isFolder()
            ? TextKey::Folder::willRemove : TextKey::App::willRemove);
}


// Gets an appropriate title to use for a menu item editor.
juce::String AppMenu::ConfigData::getEditorTitle() const
{
    return localeText(isFolder() ? TextKey::Folder::edit : TextKey::App::edit);
}


// Checks if a data field within this menu item can be edited.
bool AppMenu::ConfigData::isEditable(const DataField dataField) const
{
    switch(dataField)
    {
        case DataField::categories:
            return isFolder();
        case DataField::command:
        case DataField::termLaunchOption:
            return getFolderSize() == 0;
        case DataField::icon:
        case DataField::title:
            return true;
    }
    return false;
}
