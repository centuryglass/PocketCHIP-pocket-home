#include "ComponentConfigFile.h"
#include "ComponentConfigKeys.h"
#include "AssetFiles.h"
#include "Utils.h"

/*
 * Gets configured component settings from shared JSON file data.
 */
ComponentSettings ComponentConfigFile::getComponentSettings
(const juce::Identifier& componentKey)
{
    auto config = getReadLockedResource();
    return config->getComponentSettings(componentKey);
}

/*
 * Return the most appropriate font size for drawing text
 */
int ComponentConfigFile::getFontHeight
(juce::Rectangle <int> textBounds, juce::String text)
{
    using namespace juce;
    auto config = getReadLockedResource();

    int numLines = 1;
    for (int i = 0; i < text.length(); i++)
    {
        if (text[i] == '\n')
        {
            numLines++;
        }
    }

    int height = textBounds.getHeight() / numLines;
    Font defaultFont(height);
    int width = defaultFont.getStringWidth(text);
    if (width > textBounds.getWidth())
    {
        height = textBounds.getWidth() * height / width;
    }
    int testHeight = getFontHeight(largeText);
    if (height > testHeight)
    {
        return testHeight;
    }
    testHeight = getFontHeight(mediumText);
    if (height > testHeight)
    {
        return testHeight;
    }
    testHeight = getFontHeight(smallText);
    if (height > testHeight)
    {
        return testHeight;
    }
    return height;
}

/**
 * Gets the height in pixels of one of the three configured text sizes.
 */
int ComponentConfigFile::getFontHeight(TextSize sizeType)
{
    using namespace juce;
    using namespace ComponentConfigKeys;
    const Identifier& key = (sizeType == smallText ? smallTextKey:
        (sizeType == mediumText ? mediumTextKey : largeTextKey));
    double size = getReadLockedResource()->getConfigValue<double>(key);
    if(size > 1)
    {
        return (int) size;
    }
    return (int) (size * getWindowBounds().getHeight());
}