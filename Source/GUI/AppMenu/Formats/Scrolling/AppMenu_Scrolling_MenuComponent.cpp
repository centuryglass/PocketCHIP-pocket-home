#define APPMENU_IMPLEMENTATION
#include "AppMenu_Scrolling_MenuComponent.h"
#include "AppMenu_Scrolling_FolderComponent.h"
#include "AppMenu_ConfigKeys.h"
#include "Layout_Component_JSONKeys.h"
#include "Layout_Transition_Animator.h"
#include "Util_Math.h"
#include "Util_TempTimer.h"

// Animation duration in milliseconds:
static const constexpr int animationMS = 300;

// Creates the menu component, listening to the AppMenu::MenuFile for changes
// to the number of rows to fit on screen.
AppMenu::Scrolling::MenuComponent::MenuComponent() :
boundsManager(this, Layout::Component::JSONKeys::scrollingAppMenu)
{
    ConfigFile formatConfig;
    maxRows = formatConfig.getScrollingMenuRows();
    addTrackedKey(ConfigKeys::scrollingMenuRows);
}


// Finds the initial bounds to apply to a newly created folder component.
juce::Rectangle<int> AppMenu::Scrolling::MenuComponent::initialFolderBounds
(const int newFolderIndex) const
{
    using juce::Rectangle;
    Rectangle<int> startingBounds = getFolderBounds(newFolderIndex, false);
    if (newFolderIndex > 0)
    {
        startingBounds.setX(getOpenFolder(newFolderIndex-1)->getRight());
        startingBounds.setWidth(0);
    }
    return startingBounds;
}


// Prepares to update the folder layout, updating cached layout values.
void AppMenu::Scrolling::MenuComponent::layoutUpdateStarting
(const bool closingFolder)
{
    const int openFolders = openFolderCount();
    const int lastIndex   = openFolders - 1;
    int widthSum = 0;
    for (int i = 0; i < openFolders; i++)
    {
        int width = static_cast<FolderComponent*>(getOpenFolder(i))
            ->getMinimumWidth();
        folderWidths.set(i, width);
        if (!closingFolder || i < lastIndex)
        {
            widthSum += width;
        }
    }
    folderRightEdge = (getWidth() / 2)  + folderWidths[lastIndex] / 2;
    if (folderRightEdge < widthSum)
    {
        folderRightEdge = std::min(widthSum, getWidth());
    }
    if (closingFolder)
    {
        folderRightEdge += folderWidths[lastIndex] / 2;
        folderRightEdge += folderWidths[lastIndex - 1] / 2;
    }
}


// Finds the bounds where a menu folder should be placed.
juce::Rectangle<int> AppMenu::Scrolling::MenuComponent::getFolderBounds
(const int folderIndex, const bool closingFolder) const
{
    using juce::Rectangle;
    if (getBounds().isEmpty() || openFolderCount() == 0)
    {
        return Rectangle<int>();
    }
    const int buttonHeight = getHeight() / maxRows;
    const int centerX = getWidth() / 2;
    const int centerY = getHeight() / 2;
    const int openFolders = openFolderCount();
    const int lastIndex = openFolders - 1;

    int folderX = folderRightEdge;
    for (int i = lastIndex; i >= folderIndex; i--)
    {
        folderX -= folderWidths[i];
    }
    FolderComponent* folder = static_cast<FolderComponent*>
            (getOpenFolder(folderIndex));
    const int width = (folderIndex == lastIndex && closingFolder) ?
            0 : folder->getMinimumWidth();
    const int height = folder->getFolderSize() * buttonHeight;
    const int folderY = Util::Math::median<int>(
            buttonHeight,
            centerY - folder->getSelectedItemYOffset(),
            getHeight() - buttonHeight - height);
    return Rectangle<int>(folderX, folderY, width, height);
}


// Gets the duration in milliseconds to animate folder transitions.
int AppMenu::Scrolling::MenuComponent::getAnimationDuration() const
{
    return animationMS;
}


// Creates a new Scrolling::FolderComponent for an opened folder menu item.
AppMenu::FolderComponent*
AppMenu::Scrolling::MenuComponent::createFolderComponent
(MenuItem folderItem) const
{
    return new Scrolling::FolderComponent(folderItem);
}


// Updates the menu's bounds whenever its parent component is resized.
void AppMenu::Scrolling::MenuComponent::parentResized
(const juce::Rectangle<int> parentBounds)
{
    boundsManager.applyConfigBounds();
    updateMenuLayout();
}


// Updates the menu layout when the scrolling menu row count changes.
void AppMenu::Scrolling::MenuComponent::configValueChanged
(const juce::Identifier& propertyKey)
{
    jassert(propertyKey == ConfigKeys::scrollingMenuRows);
    ConfigFile formatConfig;
    maxRows = formatConfig.getScrollingMenuRows();
    updateMenuLayout(false);
}
