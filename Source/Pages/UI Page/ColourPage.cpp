#include "TempTimer.h"
#include "ColourPage.h"

ColourPage::ColourPage(PageComponent::PageFactoryInterface& pageFactory,
        ComponentConfigFile& config) :
PageComponent(pageFactory, "ColourPage",{
    {4,
        {
            {&colourList, 1}
        }},
    {1,
        {
            {&testSwitch, 1}
        }}

},
true),
config(config),
colourList("colourList", &listModel)
{
    addAndShowLayoutComponents();
    colourList.setOutlineThickness(2);
}

ColourPage::~ColourPage() { }

void ColourPage::pageResized()
{
    listModel.listResized(colourList);
    colourList.updateContent();
    colourList.repaint();
}

ColourPage::ColourListModel::ColourListModel() :
Configurable(new ComponentConfigFile(),{})
{
    colourKeys = config.getColourKeys();
    addTrackedKeys(colourKeys);
    DBG(__func__ << ": adding " << colourKeys.size() << " colors");
    for (const String& key : colourKeys)
    {
        colours.add(Colour(config.getConfigValue<String>(key).getHexValue32()));
    }

    DBG(__func__ << ": found " << colours.size() << " colors");
}

ColourPage::ColourListModel::~ColourListModel() { }

int ColourPage::ColourListModel::getNumRows()
{
    return colourKeys.size();
}

void ColourPage::ColourListModel::listResized(ListBox& list)
{
    textHeight = config.getComponentSettings(ComponentConfigFile::mediumTextKey)
            .getBounds().getHeight();
    list.setRowHeight(textHeight * 1.5);
    list.updateContent();
    list.repaint();
}

void ColourPage::ColourListModel::paintListBoxItem(int rowNumber, Graphics& g,
        int width, int height, bool rowIsSelected)
{
    Rectangle<int> fillArea(0, 0, width, height);
    float checkSize = fillArea.getHeight() / 4;
    g.fillCheckerBoard(fillArea.toFloat(), checkSize, checkSize,
            Colours::dimgrey, Colours::grey);
    g.setColour(colours[rowNumber]);
    g.fillRect(fillArea);
    fillArea.reduce(height / 5, height / 5);
    g.setColour((colours[rowNumber].getAlpha() < 0x88) ?
            Colours::white : colours[rowNumber].contrasting());
    if (rowIsSelected)
    {
        g.fillRoundedRectangle(fillArea.toFloat(), textHeight / 5);
        g.setColour(colours[rowNumber]);
    }
    g.setOpacity(1);
    g.setFont(Font(textHeight));
    g.drawText(colourKeys[rowNumber], fillArea, Justification::centred);
}

Component* ColourPage::ColourListModel::refreshComponentForRow(int rowNumber,
        bool isRowSelected, Component* toUpdate)
{
    if (toUpdate != nullptr)
    {
        delete toUpdate;
    }
    return nullptr;
}

void ColourPage::ColourListModel::selectedRowsChanged(int lastRowSelected) { }

void ColourPage::ColourListModel::loadConfigProperties(ConfigFile* config,
        String key)
{
    int colourIndex = colourKeys.indexOf(key);
    if (colourIndex >= 0)
    {
        colours[colourIndex] =
                Colour(config->getConfigValue<String>(key).getHexValue32());
    }
}