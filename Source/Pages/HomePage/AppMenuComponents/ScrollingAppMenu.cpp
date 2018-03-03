#include "ScrollingAppMenu.h"

ScrollingAppMenu::ScrollingAppMenu(OverlaySpinner& loadingSpinner) :
AppMenuComponent(ComponentConfigFile::scrollingAppMenuKey, loadingSpinner) { }

ScrollingAppMenu::~ScrollingAppMenu() { }

/**
 * Use key presses for menu navigation, setting specific controls based on 
 * AppMenu type. Other classes may call this to pass on or simulate
 * key events.
 */
bool ScrollingAppMenu::keyPressed(const KeyPress& key)
{
    return false;
}

/**
 * Updates the folder component layout, optionally animating the transition.
 */
void ScrollingAppMenu::layoutFolders(bool animateTransition) { }

/**
 * Create a folder component object from a folder menu item.
 * @param folderItem
 */
AppMenuComponent::AppFolder* ScrollingAppMenu::createFolderObject
(AppMenuItem::Ptr folderItem)
{
    return new ScrollingAppFolder(folderItem, this, buttonNameMap, iconThread);
}

ScrollingAppMenu::ScrollingAppFolder::ScrollingAppFolder
(AppMenuItem::Ptr folderItem, MouseListener* btnListener,
        std::map<String, AppMenuButton::Ptr>& buttonNameMap,
        IconThread& iconThread) :
AppFolder(folderItem, btnListener, buttonNameMap, iconThread) { }

ScrollingAppMenu::ScrollingAppFolder::~ScrollingAppFolder() { }

/**
 * Create an AppMenuButton component for an AppMenuItem.
 * @param menuItem
 */
AppMenuButton::Ptr ScrollingAppMenu::ScrollingAppFolder::createMenuButton
(AppMenuItem::Ptr menuItem)
{
    return new ScrollingMenuButton(menuItem, iconThread,
            menuItem->getAppName() + String("Button"));
}

/**
 * Given a list of folder buttons, return an appropriate layout
 * for positioning them in the folder component.
 * 
 * @param buttons
 * @return a Layout containing all items in the button array.
 */
GridLayoutManager::Layout
ScrollingAppMenu::ScrollingAppFolder::buildFolderLayout
(Array<AppMenuButton::Ptr>& buttons){
    return {};
}

/**
 *
 */
ScrollingAppMenu::ScrollingMenuButton::ScrollingMenuButton
(AppMenuItem* menuItem, IconThread& iconThread, String name) :
AppMenuButton(menuItem, iconThread, name) { }

ScrollingAppMenu::ScrollingMenuButton::~ScrollingMenuButton() { }

/**
 * Re-calculates draw values whenever the button is resized
 */
void ScrollingAppMenu::ScrollingMenuButton::resized() { }


//OLD CODE
//ScrollingAppMenu::ScrollingAppMenu() :
//AppMenuComponent(ComponentConfigFile::scrollingAppMenuKey)
//{
//    x_origin = getBounds().getX();
//    y_origin = getBounds().getY();
//
//
//    loadButtons();
//}
//
//ScrollingAppMenu::~ScrollingAppMenu()
//{
//}
//
///**
// * Add a new application button to the active menu column.
// * @param appButton will be added to the bottom of the active menu column.
// */
//void ScrollingAppMenu::addButtonComponent(AppMenuButton* appButton)
//{
//    ScrollingMenuButton* scrollButton =
//            dynamic_cast<ScrollingMenuButton*> (appButton);
//    if (scrollButton != nullptr)
//    {
//        scrollButton->applyConfigBounds();
//        int buttonWidth = scrollButton->getWidth();
//        int buttonHeight = scrollButton->getHeight();
//        int columnIndex = scrollButton->getIndexOfFolder();
//        int rowIndex = scrollButton->getIndexWithinFolder();
//        if (rowIndex == 0 || columnIndex >= columnTops.size())
//        {
//            columnTops.resize(columnIndex+1);
//            if (columnIndex == 0 || selected.size() < columnIndex
//                    || selected[columnIndex - 1] == nullptr)
//            {
//                columnTops[columnIndex]=y_origin;
//            } else
//            {
//                columnTops[columnIndex]=selected[columnIndex - 1]->getY();
//            }
//        }
//        int x = columnIndex*buttonWidth;
//        int y = columnTops[columnIndex] + buttonHeight * rowIndex;
//        addAndMakeVisible(scrollButton);
//        scrollButton->setBounds(x, y, buttonWidth, buttonHeight);
//        if ((x + buttonWidth) > getWidth())
//        {
//            setBounds(getX(), getY(), x + buttonWidth, getHeight());
//        }
//        if ((y + buttonHeight) > getHeight())
//        {
//            setBounds(getX(), getY(), getWidth(), y + buttonHeight);
//        }
//
//    }
//}
//
///**
// * Scroll the menu so that the selected button is centered.
// * @param animatedScroll sets if the menu should animate its scrolling,
// * or just jump immediately to the destination position.
// */
//void ScrollingAppMenu::scrollToSelected(bool animatedScroll)
//{
//    AppMenuButton* selectedButton = getSelectedButton();
//    if (selectedButton != nullptr)
//    {
//        int buttonWidth = selectedButton->getWidth();
//        int buttonHeight = selectedButton->getHeight();
//        int index = selectedButton->getIndexWithinFolder();
//        int column = selectedButton->getIndexOfFolder();
//        Rectangle<int> dest = getBounds();
//        //calculate y-position
//        if (selectedButton->isVisible())
//        {
//            int buttonYPos = selectedButton->getY();
//            int screenHeight = getWindowSize().getHeight();
//
//            int distanceFromCenter = abs(buttonYPos - getY()
//                    + screenHeight / 2);
//            //only scroll vertically if selected button is outside 
//            //the center 3/5 
//            if (distanceFromCenter > screenHeight / 5 * 3)
//            {
//                dest.setY(y_origin - buttonYPos + screenHeight / 2
//                        - buttonHeight / 2);
//            }
//            if (dest.getY() > y_origin)
//            {
//                dest.setY(y_origin);
//            }
//        } else
//        {
//            dest.setY(y_origin - columnTops[column]);
//        }
//        //calculate x-position
//        if (column == 0)
//        {
//            dest.setX(x_origin);
//        } else
//        {
//            dest.setX(x_origin - column * buttonWidth + buttonHeight);
//        }
//        //scroll to the calculated position
//        if (animatedScroll)
//        {
//            ComponentAnimator& animator = Desktop::getInstance().getAnimator();
//            if (animator.isAnimating(this))
//            {
//                animator.cancelAnimation(this, false);
//            }
//            animator.animateComponent(this, dest, 1, 100, false, 1, 1);
//        } else
//        {
//            setBounds(dest);
//        }
//    }
//}
//
///**
// * Receives all keyPress events and uses them for page navigation.
// * @param key
// * @return true iff the key press was used by the AppMenu
// */
//bool ScrollingAppMenu::keyPressed(const KeyPress& key)
//{
//    int keyCode = key.getKeyCode();
//    if (keyCode == KeyPress::tabKey)
//    {
//        DBG("pressed tab");
//        loadButtons();
//    }
//    if (keyCode == KeyPress::upKey || keyCode == KeyPress::downKey)
//    {
//        changeSelection((keyCode == KeyPress::upKey) ? -1 : 1);
//        return true;
//    } else if (keyCode == KeyPress::leftKey || keyCode == KeyPress::escapeKey)
//    {
//        if (activeColumn() > 0)
//        {
//            closeFolder();
//        }
//        return true;
//    } else if (keyCode == KeyPress::returnKey ||
//            keyCode == KeyPress::spaceKey ||
//            keyCode == KeyPress::rightKey)
//    {
//        DBG("HomePage:click selected AppMenuButton");
//        clickSelected();
//        return true;
//    } else if (key == KeyPress::createFromDescription("CTRL+e"))
//    {
//        openPopupMenu(true);
//        return true;
//    }
//    return false;
//}
//
///**
// * Create a new menu button component.
// * @param menuItem menu data to be held by the component
// */
//AppMenuButton::Ptr ScrollingAppMenu::createMenuButton
//(AppMenuItem* menuItem, int columnIndex, int rowIndex)
//{
//    return new ScrollingMenuButton(menuItem, iconThread, columnIndex, rowIndex,
//            menuItem->getAppName() + String("Button"));
//}
//
///**
// * Resize all child components.
// */
//void ScrollingAppMenu::resized()
//{
//    this->AppMenuComponent::resized();
//    ComponentConfigFile config;
//    ComponentConfigFile::ComponentSettings menuSettings =
//            config.getComponentSettings(ComponentConfigFile::scrollingAppMenuKey);
//    ComponentConfigFile::ComponentSettings buttonSettings =
//            config.getComponentSettings(ComponentConfigFile::appMenuButtonKey);
//    Rectangle<int> menuBounds = menuSettings.getBounds();
//    x_origin = menuBounds.getX();
//    y_origin = menuBounds.getY();
//    //resize all buttons
//    Rectangle<int>buttonSize = buttonSettings.getBounds().withZeroOrigin();
//    int buttonWidth = buttonSize.getWidth();
//    int buttonHeight = buttonSize.getHeight();
//    int numColumns = selected.size();
//    if (menuBounds.getWidth() < numColumns * buttonWidth)
//    {
//        menuBounds.setWidth(numColumns * buttonWidth);
//    }
//    for (int c = 0; c < numColumns; c++)
//    {
//        if (c > 0)
//        {
//            columnTops[c] = selected[c - 1]->getY();
//        }
//        int numRows = buttonColumns[c].size();
//        if (menuBounds.getHeight() < numRows * buttonHeight + columnTops[c])
//        {
//            menuBounds.setHeight(numRows * buttonHeight + columnTops[c]);
//        }
//        for (int i = 0; i < numRows; i++)
//        {
//            ScrollingMenuButton * button =
//                    dynamic_cast<ScrollingMenuButton*> (buttonColumns[c][i].get());
//            if (button != nullptr)
//            {
//                button->setBounds(buttonSize.withPosition(c*buttonWidth,
//                        i * buttonHeight + columnTops[c]));
//            } 
//        }
//    }
//    setBounds(menuBounds);
//    if (activeColumn() >= 0 && selected[activeColumn()] != nullptr
//            && !Desktop::getInstance().getAnimator().isAnimating())
//    {
//        scrollToSelected(false);
//    }
//}
//
//ScrollingAppMenu::ScrollingMenuButton::ScrollingMenuButton
//(AppMenuItem* menuItem, IconThread& iconThread, int columnIndex,
//        int rowIndex, String name) :
//AppMenuButton(menuItem, iconThread, columnIndex, rowIndex, name)
//{
//}
//
//ScrollingAppMenu::ScrollingMenuButton::~ScrollingMenuButton()
//{
//}
//
//
//
///**
// * Re-calculates draw values whenever the button is resized
// */
//void ScrollingAppMenu::ScrollingMenuButton::resized()
//{
//    Rectangle<float> bounds = getLocalBounds().toFloat();
//    imageBox = bounds.withWidth(bounds.getHeight());
//    imageBox.reduce(2, 2);
//    textBox = bounds;
//    textBox.setLeft(imageBox.getRight());
//    textBox.reduce(4, 4);
//    //It looks messy if all the fonts are different sizes, so using a default
//    //String for size calculations is preferable even if really long names can 
//    //get clipped.
//    titleFont = fontResizedToFit(titleFont, "DefaultAppNameStr",
//            textBox.toNearestInt());
//}
//
//
