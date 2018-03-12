/**
 * @file PageAppFolder.h
 * 
 * PageAppFolder organizes one folder of an application menu as 
 * a grid of menu icons, divided into horizontal scrolling pages.
 */

#pragma once
#include "../AppMenuFolder.h"

class PageAppFolder : public AppMenuFolder {
public:
    PageAppFolder(AppMenuItem::Ptr folderItem, MouseListener* btnListener,
            std::map<String, AppMenuButton::Ptr>& buttonNameMap,
            IconThread& iconThread);
    virtual ~PageAppFolder();

    /**
     * Create an AppMenuButton component for an AppMenuItem.
     * @param menuItem
     */
    virtual AppMenuButton::Ptr createMenuButton
    (AppMenuItem::Ptr menuItem, IconThread& iconThread) override;

    /**
     * Given a list of folder buttons, return an appropriate layout
     * for positioning them in the folder component.
     * 
     * @param buttons
     * @return a Layout containing all items in the button array.
     */
    virtual GridLayoutManager::Layout buildFolderLayout
    (Array<AppMenuButton::Ptr>& buttons);

    /**
     * @return the number of pages this folder needs to display all menu
     * buttons.
     */
    int getNumFolderPages() const;

    /**
     * @return the index of the page that's currently visible, or 0 if 
     * there is no current page. 
     */
    int getCurrentFolderPage() const;

    /**
     * Set which folder page should currently be visible.
     * @param pageNum must be a valid page index, or the selection will
     * not change.
     * @return true if the page selection changed.
     */
    bool setCurrentFolderPage(int pageNum);

    /**
     * @return the folder page index containing the selected folder 
     * button, or -1 if there is no selection.
     */
    int getSelectionPage() const;

    /**
     * @return the index of the selected button within its folder page,
     * or -1 if there is no selection.
     */
    int getSelectedIndexInFolderPage() const;

    /**
     * @return the column index of the selected button within its
     * folder page, or -1 if there is no selection.
     */
    int getSelectionColumn() const;


    /**
     * @return the row index of the selected button within its
     * folder page, or -1 if there is no selection.
     */
    int getSelectionRow() const;

    /**
     * Finds what index value a button would have at a particular
     * position within the folder.
     * @param page folder page index
     * @param column folder page column index
     * @param row folder page row index
     * @return button index value, or -1 for invalid
     *  positions
     */
    int positionIndex(int page, int column, int row) const;

    /**
     * Set the button selection based on its position in the
     * folder.  The selection will not change if there isn't
     * a button located at the given position.  If necessary,
     * the current folder page will change to the one containing
     * the new selection.
     *
     * @param page the new selected button's folder page index
     * @param column the new selected button's column number within
     * its folder page.
     * @param row the new selected button's row number withing
     * its folder page.
     * @return true if the selection changed, false otherwise.
     */
    bool setSelectedPosition(int page, int column, int row);

private:

    /**
     * Resizes padding, then calls AppFolder::resized().
     */
    void resized() override;
    int currentPage = 0;


    //############################  PageMenuButton  ############################

    /**
     *
     */
    class PageMenuButton : public AppMenuButton {
    public:
        /**
         * 
         * @param menuItem
         * @param iconThread
         * @param name
         */
        PageMenuButton(AppMenuItem::Ptr menuItem, IconThread& iconThread,
                String name = String());
        virtual ~PageMenuButton();
    private:

        /**
         * Re-calculates draw values whenever the button is resized
         */
        void resized() override;

        /**
         * Set the background and border to only draw for selected buttons.
         */
        void selectionStateChanged() override;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PageMenuButton)
    };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PageAppFolder)
};