/* Only include this file directly in the AppMenu implementation! */
#ifdef APPMENU_IMPLEMENTATION_ONLY

#pragma once
#include "ConfigurableComponent.h"
#include "AppMenu/Data/ConfigFile.h"
#include "AppMenu/Components/MenuComponent.h"

/**
 * @file  MenuFormats/Scrolling/MenuComponent.h
 *
 * @brief  Creates and arranges application menu folder components for the
 *         scrolling AppMenu format.
 */
class AppMenu::Scrolling::MenuComponent : public AppMenu::MenuComponent,
    public ConfigurableComponent, public ConfigFile::Listener
{
public:
    MenuComponent();

    virtual ~MenuComponent() { }

private:
    /**
     * @brief   Finds the bounds where a menu folder should be placed.
     *
     * @param folderIndex    The index of an open folder component.
     *
     * @param closingFolder  Whether the active folder (not necessarily this
     *                       folder!) is about to be closed.
     *
     * @return               The bounds within the MenuComponent where the
     *                       folder should be placed.
     */
     virtual juce::Rectangle<int> getFolderBounds(const int folderIndex,
            const bool closingFolder = false) final override;

    /**
     * @brief  Gets the duration in milliseconds to animate folder transitions.
     *
     * @return   The AppMenu format's animation duration.
     */
    virtual int getAnimationDuration() const final override;

    /**
     * @brief  Creates a new Scrolling::FolderComponent for an opened folder
     *         menu item.
     *
     * @param folderItem  The folder component's menu data source.
     *
     * @return  The new Scrolling::FolderComponent object.
     */
    virtual AppMenu::FolderComponent* createFolderComponent(MenuItem folderItem)
        const final override;

    /**
     * @brief  Updates the menu's bounds whenever its parent component is
     *         resized.
     *
     * @param parentBounds  The new bounds of the parent component.
     */
    virtual void parentResized(const juce::Rectangle<int> parentBounds)
        final override;

    /**
     * @brief  Updates the menu layout when the scrolling menu row count
     *         changes.
     *
     * @param propertyKey  The scrolling menu row count key.
     */
    virtual void configValueChanged(const juce::Identifier& propertyKey)
        final override;

    /* Cached folder widths to reuse when calculating layout changes: */
    juce::Array<int> folderWidths;

    /* Holds the position of the last folder's right edge, relative to the
       MenuComponent's left edge: */
    int folderRightEdge;

    /* Holds the number of visible button rows to fit on the screen: */
    int maxRows;

};
/* Only include this file directly in the AppMenu implementation! */
#endif
