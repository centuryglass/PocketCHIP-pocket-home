#pragma once
#include "Localized.h"
#include "IconThread.h"
#include "MenuItemData.h"

/**
 * @file ConfigItemData.h
 * 
 * @brief  A MenuItemData object that reads its data from a JSON object.
 */

class ConfigItemData : public MenuItemData, private Localized {
public:
    /**
     * @param menuData  The JSON object holding menu data.
     *
     * @param index     Index of this menu item within its folder.
     *
     * @param parent    The parent folder item, if this menu item is not in the
     *                  root folder.
     */
    ConfigItemData(juce::var& jsonData, const int index,
            const ConfigItemData* parent = nullptr);
    
    virtual ~ConfigItemData() { }

    /**
     * @brief  Creates a copy of this object.
     *
     * The caller is responsible for ensuring this object is deleted.
     *
     * @return  A new DataSource object copying this object's JSON data.
     */
    virtual MenuItemData* clone() const override;

    /**
     * @brief  Gets the menu item's displayed title.
     *
     * @return  The displayed title string.
     */
    virtual juce::String getTitle() const override;

    /**
     * @brief  Sets the menu item's displayed title.
     *
     * @param title  The new title string to display.
     */
    virtual void setTitle(const juce::String& title) override;

    /**
     * @brief  Gets the name or path use to load the menu item's icon file.
     *
     * @return  The name or path of the icon.
     */
    virtual juce::String getIconName() const override;

    /**
     * @brief  Sets the name or path used to load the menu item's icon file.
     *
     * @param iconName  The new icon name or path.
     */
    virtual void setIconName(const juce::String& iconName) override;

    /**
     * @brief  Gets the application categories connected to this menu item.
     *
     * @return  Any category strings assigned to this menu item.
     */
    virtual juce::StringArray getCategories() override;

    /**
     * @brief  Sets the application categories connected to this menu item.
     *
     * @param categories  The new set of category strings to assign to this menu
     *                    item.
     */
    virtual void setCategories(const juce::StringArray& categories) override;

    /**
     * @brief  Gets the menu item's application launch command.
     *
     * @return  The launch command string, or the empty string if the menu item
     *          does not launch an application.
     */
    virtual juce::String getCommand() const override;

    /**
     * @brief  Sets the menu item's application launch command.
     *
     * @param newCommand  The new command string to run when this menu item is
     *                    clicked.
     */
    virtual void setCommand(const juce::String& newCommand) override;

    /**
     * @brief  Checks if this menu item launches an application in a new
     *         terminal window.
     *
     * @return  True if and only if the menu item has a launch command it should
     *          run in a new terminal window
     */
    virtual bool getLaunchedInTerm() const override;

    /**
     * @brief  Sets if this menu item runs its command in a new terminal window.
     *
     * @param termLaunch  True to run any launch command assigned to this
     *                    menu item within a new terminal window.
     */
    virtual void setLaunchedInTerm(const bool termLaunch) override;

    /**
     * @brief  Deletes this menu item data from its source JSON file.
     */
    virtual void deleteFromSource() override;


    /**
     * @brief  Writes all changes to this menu item back to its data source.
     */
    virtual void updateSource() override;

    /**
     * @brief  Checks if this menu item can be moved within its menu folder.
     *
     * @param offset  The amount to offset the menu item index.
     *
     * @return        True if and only if the menu item can be moved, and the 
     *                offset is valid.
     */
    virtual bool canMoveIndex(const int offset) override;

    /**
     * @brief  Attempts to move this menu item within its menu folder.
     *
     * @param offset  The amount to offset the menu item index.
     *
     * @return        True if the menu item was moved, false if it couldn't be
     *                moved by the given offset value.
     */
    virtual bool moveIndex(const int offset) override;
    
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
    virtual bool isEditable(const DataField dataField) override;

    /**
     * @brief  Gets the number of menu items in the folder opened by this menu
     *         item.
     *
     * @return  The number of folder items, or zero if this menu item does not
     *          open a folder.
     */
    virtual int folderItemCount() override;

    /**
     * @brief  Gets a single menu item in the folder this menu item would open.
     *
     * @param index  The index of a menu item in the folder.
     *
     * @return  A menu item, or nullptr if this menu item doesn't open a folder
     *          or index is out of bounds.  The caller is responsible for
     *          ensuring non-null return values are deleted.
     */
    virtual MenuItemData* getFolderItem(int index) override;

    /**
     * @brief  Gets all menu items in the folder this menu item would open.
     *
     * @return  An array of menu item objects.  The caller is responsible for
     *          ensuring these objects are deleted.
     */
    virtual juce::Array<MenuItemData*> getFolderItems() override;

private:
    /* JSON object data */
    juce::var jsonData;

    //localized text keys: 
    static const constexpr char * remove_APP = "remove_APP";
    static const constexpr char * from_favorites = "from_favorites";
    static const constexpr char * will_remove_link = "will_remove_link";
    static const constexpr char * edit_app = "edit_app";
};


