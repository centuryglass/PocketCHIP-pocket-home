#pragma once
/**
 * @file  GLib_Borrowed_SharedContainer.h
 *
 * @brief  Holds GObject* data shared by GLib::Borrowed::Object instances.
 */

#include "Util_Nullable.h"
#include "JuceHeader.h"
#include <glib-object.h>

namespace GLib { namespace Borrowed { class SharedContainer; } }

/**
 * @brief  A GObject* container used to share a single GObject value with every
 *         single GLib Object wrapper that holds that value.
 *
 *  This lets the ObjectLender remove a GObject* value from every single Object
 * that holds it by calling clearData() on a single SharedContainer.
 *
 *  SharedContainer holds a single GObject* provided to it on construction. That
 * value may be replaced with nullptr using the clearData() function, but it may
 * not be changed in any other way.
 */
class GLib::Borrowed::SharedContainer : public Util::Nullable<GObject*>,
    public juce::ReferenceCountedObject
{
public:
    // Reference-counted pointer type used to share a SharedContainer.
    typedef juce::ReferenceCountedObjectPtr<SharedContainer> Ptr;

    /**
     * @brief  Creates a SharedContainer holding GObject* data.
     *
     * @param object  The GLib object data the container will hold.
     */
    SharedContainer(GObject* object);

    virtual ~SharedContainer() { }

    /**
     * @brief  Casts the SharedContainer directly to its stored GObject* value.
     *
     * @return  The container's GObject data.
     */
    operator GObject*() const;

    /**
     * @brief  Permanently sets the SharedContainer object's data to nullptr.
     */
    void clearData();

    JUCE_LEAK_DETECTOR(GLib::Borrowed::SharedContainer);
};


