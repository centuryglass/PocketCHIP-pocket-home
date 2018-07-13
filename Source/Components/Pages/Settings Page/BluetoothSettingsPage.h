#pragma once

#include "SwitchComponent.h"
#include "DrawableImageComponent.h"
#include "ScalingLabel.h"
#include "Spinner.h"
#include "BluetoothStatus.h"
#include "FocusingListPage.h"

/**
 * @file BluetoothSettingsPage.h
 * 
 * @brief  Shows visible bluetooth devices, providing controls for initializing
 *         or removing connections.  
 * 
 * As Bluetooth support is not yet implemented, this class is currently just an
 * empty FocusingListPage.
 */

class BluetoothSettingsPage :
public FocusingListPage {
public:
    
    BluetoothSettingsPage() { }
    
    ~BluetoothSettingsPage() { }
private:
    
    /**
     * Gets the total number of items that should be in the list.
     * 
     * @return   The number of visible Bluetooth devices.
     */
    virtual unsigned int getListSize() override { return 0;}

    /**
     * Loads or updates the Component layout for each Bluetooth device in the 
     * list.
     * 
     * @param layout  The layout of a single list item.  If empty, appropriate
     *                components should be added to the layout.  Otherwise, any 
     *                existing components in the layout should be updated to fit
     *                the provided list index.  
     *                
     * @param index   The index of a Bluetooth device in the list. 
     */
    virtual void updateListItemLayout(LayoutManager::Layout& layout,
            const unsigned int index) { }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BluetoothSettingsPage)
};
