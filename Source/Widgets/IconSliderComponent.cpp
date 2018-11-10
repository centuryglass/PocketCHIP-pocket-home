#include "Utils.h"
#include "IconSliderComponent.h"

IconSliderComponent::IconSliderComponent
(juce::String lowImgAsset, juce::String highImgAsset) :
lowIcon(lowImgAsset, juce::RectanglePlacement::stretchToFit),
highIcon(highImgAsset, juce::RectanglePlacement::stretchToFit)
{
    using namespace juce;
#    if JUCE_DEBUG
    setName("IconSliderComponent");
#    endif
    Colour imageColour = findColour(Slider::trackColourId);
    lowIcon.setColour(DrawableImageComponent::imageColour0Id, imageColour);
    highIcon.setColour(DrawableImageComponent::imageColour0Id, imageColour);
    slider.setSliderStyle(Slider::LinearHorizontal);
    slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    slider.setRange(0, 100);
    addAndMakeVisible(slider);
    addAndMakeVisible(lowIcon);
    addAndMakeVisible(highIcon);
}

/**
 * Changes the slider's stored value.
 */
void IconSliderComponent::setValue
(double newValue, juce::NotificationType notification)
{
    slider.setValue(newValue, notification);
}

/**
 * @return the current slider position value, between 0 and 100.
 */
double IconSliderComponent::getValue() const
{
    return slider.getValue();
}

/**
 * @param listener   This will receive updates whenever the slider value
 *                    changes.
 */
void IconSliderComponent::addListener(juce::Slider::Listener* listener)
{
    slider.addListener(listener);
}

/**
 * Use this to determine which IconSliderComponent is responsible for
 * a slider callback.
 */
bool IconSliderComponent::ownsSlider(juce::Slider * sliderPtr)
{
    return sliderPtr == &slider;
}

/** Sets the limits that the slider's value can take.
 */
void IconSliderComponent::setRange(double newMinimum,
        double newMaximum,
        double newInterval)
{
    slider.setRange(newMinimum, newMaximum, newInterval);
}

/**
 * Update the icons and slider to fit the component bounds.
 */
void IconSliderComponent::resized()
{
    using namespace juce;
    Rectangle<int> bounds = getLocalBounds();
    lowIcon.setBounds(bounds.withWidth(bounds.getHeight()));
    highIcon.setBounds(bounds.withLeft(bounds.getRight() - bounds.getHeight()));
    slider.setBounds(bounds.reduced(bounds.getHeight(), 0));
}

/**
 * Update the icon color if slider color changes
 */
void IconSliderComponent::colourChanged()
{
    using namespace juce;
    Colour imageColour = findColour(Slider::trackColourId);
    lowIcon.setColour(DrawableImageComponent::imageColour0Id, imageColour);
    highIcon.setColour(DrawableImageComponent::imageColour0Id, imageColour);
}