/**
 * @file FolderMenuItem.h
 * 
 * FolderMenuItem is an AppMenuItem that gets its data from a 
 * AppConfigFile::AppFolder structure, representing a set of application 
 * categories.
 * @see AppMenuItem, AppConfigFile, AppMenuComponent
 * 
 */
#pragma once
#include "../../../../Configuration/AppConfigFile.h"
#include "../../IconThread.h"
#include "../AppMenuItem.h"

class FolderMenuItem : public AppMenuItem {
public:
    /**
     * @param appFolder defines the folder data
     */
    FolderMenuItem(AppConfigFile::AppFolder appFolder);

    /**
     * Check if this button is for an application folder
     * @return true
     */
    bool isFolder() const;

    /**
     * @return the display name of the associated folder
     */
    String getAppName() const;

    /**
     * @return the empty string, as FolderMenuItems don't have a command
     */
    String getCommand() const;


    /**
     * @return false, as FolderMenuItems aren't apps at all
     */
    bool isTerminalApp() const;

    /**
     * @return all application categories linked to this folder.
     */
    Array<String> getCategories() const;

    /**
     * @return the name or path used to load the icon file. 
     */
    String getIconName() const;

    /**
     * Return true if this menu item has an index that can be moved by a given 
     * amount.
     * @param offset some value to add to the menu item index
     * @return true if this menu item has an index value i that can be changed 
     * to i+offset 
     */
    bool canChangeIndex(int offset) const;
protected:
    /**
     * Get an appropriate title to use for a deletion confirmation window.
     */
    virtual String getConfirmDeleteTitle() const override;

    /**
     * Gets appropriate descriptive text for a deletion confirmation window.
     */
    virtual String getConfirmDeleteMessage() const override;

    /**
     * @return true, as folder categories can be edited.
     */
    virtual bool hasEditableCategories() const override {
        return true;
    };

    /**
     * @return false, as folders have no command.
     */
    virtual bool hasEditableCommand() const override {
        return false;
    };

    /**
     * @return the title to display over an editor for this menu item. 
     */
    virtual String getEditorTitle() const override;

    /**
     * Gets a PopupEditorComponent callback function that will apply 
     * changes from an AppMenuPopupEditor to this menu item.
     */
    std::function<void(AppMenuPopupEditor*) > getEditorCallback();

    /**
     * Removes the source of this menu item's data, deleting the folder from
     * apps.json
     */
    void removeMenuItemSource();

    /**
     * If possible, change the index of this menu item by some
     * offset amount.
     * @param offset will be added to the menu item's current index, if
     * possible.
     * @return true iff the operation succeeded.
     */
    bool moveDataIndex(int offset);

    /**
     * Assign new properties to this folder, changing configuration files
     * @param name folder display name
     * @param icon folder icon image name/path
     * @param categories list of folder application categories
     */
    void editFolder(String name, String icon, Array<String> categories);
private:
    //Source of this button's folder information
    AppConfigFile::AppFolder appFolder;
};


