#include "PopupEditorComponent.h"

PopupEditorComponent::PopupEditorComponent
( std::function<void(PopupEditorComponent*) > onConfirm) :
ConfigurableImageComponent(ComponentConfigKeys::popupMenuKey,
0, juce::RectanglePlacement::stretchToFit),
onConfirm(onConfirm),
titleLabel("EditorTitle", "", 2),
cancelButton("cancel.svg"),
confirmButton("confirm.svg")
{
    using namespace juce;
    setWantsKeyboardFocus(true);
#    if JUCE_DEBUG
    setName(String("PopupEditorComponent"));
#    endif
    titleLabel.setJustificationType(Justification::centred);
    
    Colour btnColour = findColour(Label::textColourId);
    cancelButton.setColour(DrawableImageComponent::imageColour0Id,btnColour);
    confirmButton.setColour(DrawableImageComponent::imageColour0Id,btnColour);

    cancelButton.setWantsKeyboardFocus(false);
    cancelButton.addListener(this);

    confirmButton.setWantsKeyboardFocus(false);
    confirmButton.addListener(this);

    setInterceptsMouseClicks(true, true);
}

/*
 * Sets the title printed across the top of the component.
 */
void PopupEditorComponent::setEditorTitle(const juce::String newTitle)
{
    titleLabel.setText(newTitle, juce::NotificationType::dontSendNotification);
}

/*
 * Removes this component from the window.
 */
void PopupEditorComponent::closePopup()
{
    setVisible(false);
    Component* parent = getParentComponent();
    if (parent != nullptr)
    {
        parent->removeChildComponent(this);
    }
}

/*
 * Add, make visible, and set the layout of components below the title
 * label and above the cancel and confirm buttons.
 */
void PopupEditorComponent::setLayout(LayoutManager::Layout layout)
{
    using Row = LayoutManager::Row;
    using RowItem = LayoutManager::RowItem;
    layout.insertRow(0, Row(10, { RowItem(&titleLabel) }));

    layout.addRow(Row(10,
    {
        RowItem(&cancelButton, 10),
        RowItem(&confirmButton, 10)
    }));
    layoutManager.setLayout(layout, this);
    if (!getBounds().isEmpty())
    {
        resized();
    }
}

/*
 * Manages the cancel and confirm buttons passing all other button events
 * to editorButtonClicked().
 */
void PopupEditorComponent::buttonClicked(juce::Button* buttonClicked)
{
    if (buttonClicked == &cancelButton)
    {
        closePopup();
    }
    else if (buttonClicked == &confirmButton)
    {
        onConfirm(this);
        closePopup();
    }
    else
    {
        editorButtonClicked(buttonClicked);
    }
}

/*
 * The escape and return keys work the same as pressing the cancel and confirm 
 * buttons, respectively.
 */
bool PopupEditorComponent::keyPressed(const juce::KeyPress & key)
{
    using namespace juce;
    DBG("PopupEditorComponent::" << __func__ << ": "
            << key.getTextDescription());
    if (key == KeyPress::escapeKey)
    {
        closePopup();
    }
    else if (key == KeyPress::returnKey)
    {
        onConfirm(this);
        closePopup();
    }
    return true;
}

/*
 * Re-apply the layout to fit the new bounds.
 */
void PopupEditorComponent::resized()
{
    this->ConfigurableImageComponent::resized();
    layoutManager.layoutComponents(getLocalBounds());
}

/*
 * Grab keyboard focus when the component becomes visible.
 */
void PopupEditorComponent::visibilityChanged()
{
    if (isShowing())
    {
        grabKeyboardFocus();
    }
}