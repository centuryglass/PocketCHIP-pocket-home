#ifndef APPMENU_IMPLEMENTATION
    #error File included directly outside of AppMenu implementation.
#endif
#pragma once
/**
 * @file  AppMenu_EntryData.h
 *
 * @brief  Represents menu item data loaded from a .desktop application shortcut
 *         file.
 */

#include "DesktopEntry_EntryFile.h"
#include "AppMenu_ItemData.h"
#include "Locale_TextUser.h"

namespace AppMenu { class EntryData; }

/**
 * @brief  An ItemData subclass that gets its data from a
 *         DesktopEntry::EntryFile object, loaded from a .desktop file.
 *
 *  EntryData objects store and update menu item data stored in a single
 * desktop entry file.
 *
 *  Editing desktop entries works by updating or creating .desktop files in the
 * user's local directory. Changes to desktop entries through this interface
 * will be visible in other applications for the current user, but will not
 * affect other users.
 */
class AppMenu::EntryData : public ItemData, public Locale::TextUser
{
public:
    /**
     * @brief  Creates menu item data from a desktop entry.
     *
     * @param desktopEntry  The desktop entry supplying application menu data.
     */
    EntryData(const DesktopEntry::EntryFile& desktopEntry);

    virtual ~EntryData() { }

    /**
     * @brief  Gets the menu item's displayed title.
     *
     * @return  The displayed title string.
     */
    virtual juce::String getTitle() const override;

    /**
     * @brief  Gets the name or path use to load the menu item's icon file.
     *
     * @return  The name or path of the icon.
     */
    virtual juce::String getIconName() const override;

    /**
     * @brief  Gets the menu item's application launch command.
     *
     * @return  The launch command string, or the empty string if the menu item
     *          does not launch an application.
     */
    virtual juce::String getCommand() const override;

    /**
     * @brief  Checks if this menu item launches an application in a new
     *         terminal window.
     *
     * @return  True if and only if the menu item has a launch command it should
     *          run in a new terminal window
     */
    virtual bool getLaunchedInTerm() const override;

    /**
     * @brief  Gets the application categories connected to this menu item.
     *
     * @return  Any category strings assigned to this menu item.
     */
    virtual juce::StringArray getCategories() const override;

    /**
     * @brief  Gets the unique desktop file ID tied to this menu item.
     *
     * @return   The desktop file ID of this menu item's desktop entry.
     */
    virtual juce::String getID() const final override;

    /**
     * @brief  Sets the menu item's displayed title.
     *
     * @param title  The new title string to display.
     */
    virtual void setTitle(const juce::String& title) override;

    /**
     * @brief  Sets the name or path used to load the menu item's icon file.
     *
     * @param iconName  The new icon name or path.
     */
    virtual void setIconName(const juce::String& iconName) override;

    /**
     * @brief  Sets the menu item's application launch command.
     *
     * @param newCommand  The new command string to run when this menu item is
     *                    clicked.
     */
    virtual void setCommand(const juce::String& newCommand) override;

    /**
     * @brief  Sets if this menu item runs its command in a new terminal window.
     *
     * @param termLaunch  True to run any launch command assigned to this menu
     *                    item within a new terminal window.
     */
    virtual void setLaunchedInTerm(const bool termLaunch) override;

    /**
     * @brief  Sets the application categories connected to this menu item.
     *
     * @param categories  The new set of category strings to assign to this menu
     *                    item.
     */
    virtual void setCategories(const juce::StringArray& categories) override;

    /**
     * @brief  Gets the number of folder items held by this menu item that can
     *         be reordered.
     *
     * @return  Zero, as desktop entry menu items are never folders.
     */
    virtual int getMovableChildCount() const override;

    /**
     * @brief  Checks if this menu item could be moved within its folder,
     *         assuming that another movable menu item exists that could be
     *         swapped with this one.
     *
     * @return  Always false, as desktop entry menu items are simply ordered
     *          alphabetically.
     */
    virtual bool isMovable() const override;

    /**
     * @brief  Writes all changes to this menu item back to its data source.
     */
    virtual void saveChanges() override;

    /**
     * @brief  Gets an appropriate title to use for a deletion confirmation
     *         window.
     *
     * @return  A localized confirmation title string.
     */
    virtual juce::String getConfirmDeleteTitle() const override;

    /**
     * @brief  Gets appropriate descriptive text for a deletion confirmation
     *         window.
     *
     * @return  A localized confirmation description string.
     */
    virtual juce::String getConfirmDeleteMessage() const override;

    /**
     * @brief  Gets an appropriate title to use for a menu item editor.
     *
     * @return  A localized editor title string.
     */
    virtual juce::String getEditorTitle() const override;

    /**
     * @brief  Checks if a data field within this menu item can be edited.
     *
     * @param dataField  The type of data being checked.
     *
     * @return           True if and only if the data field is editable.
     */
    virtual bool isEditable(const DataField dataField) const override;

private:
    /**
     * @brief  Sets this menu item's desktop entry as hidden for the current
     *         user.
     */
    virtual void deleteFromSource() override;

    // Application data source, set on construction.
    DesktopEntry::EntryFile desktopEntry;
};
