#include "GPPDBusProxy.h"
#include "GLibSignalHandler.h"

GPPDBusProxy::GPPDBusProxy
(const char* name, const char* path, const char* interface)
{
    if(name == nullptr || path == nullptr || interface == nullptr)
    {
        //creating an invalid/null interface, skip initialization
        return;
    }

    callInMainContext([this, name, path, interface]()
    {
        GError * error = nullptr;
        GDBusProxy * proxy = g_dbus_proxy_new_for_bus_sync(
                G_BUS_TYPE_SYSTEM,
                G_DBUS_PROXY_FLAGS_NONE,
                nullptr,
                name,
                path,
                interface,
                nullptr,
                &error);
        if (error != nullptr)
        {
            DBG(__func__ << "Opening DBus adapter proxy "
                    << name << " failed!");
            DBG("Error: " << String(error->message));
            g_clear_error(&error);
        }
        if(proxy != nullptr)
        {
            assignData(G_OBJECT(proxy));
        }
    });
    
}


/*
 * Called whenever the DBus object emits a signal.  DBusSignalHandler
 * subclasses should override this to handle the specific signals
 * they expect to receive.
 */
void GPPDBusProxy::DBusSignalHandler::dBusSignalReceived(GPPDBusProxy* source, 
        String senderName, String signalName, GVariant* parameters)
{
    DBG("GPPDBusProxy::DBusSignalHandler::" << __func__ <<
            ": Received un-handled signal " << signalName << " from "
            << senderName);
}

/*
 * Called whenever a property of the DBus object changes. 
 * DBusSignalHandler subclasses should override this to handle the
 * specific property changes they need to receive.
 */
void GPPDBusProxy::DBusSignalHandler::dBusPropertyChanged
(GPPDBusProxy* source, String propertyName, GVariant* newValue)
{   
    DBG("GPPDBusProxy::DBusSignalHandler::" << __func__ <<
            ": Received un-handled change to property" << propertyName);
}

/*
 * Called whenever a property of the DBus object becomes invalid. 
 * DBusSignalHandler subclasses should override this to handle the
 * specific property changes they need to receive.
 */
void GPPDBusProxy::DBusSignalHandler::dBusPropertyInvalidated
(GPPDBusProxy* source, String propertyName)
{   
    DBG("GPPDBusProxy::DBusSignalHandler::" << __func__ <<
            ": Received un-handled invalidation message for " << propertyName);
}

/*
 * Calls one of the methods provided by this interface.
 */
GVariant* GPPDBusProxy::callMethod
(const char *  methodName, GVariant* params, GError ** error)
{
    if(params != nullptr && !g_variant_is_of_type(params, G_VARIANT_TYPE_TUPLE))
    {
        GVariant* tuple = g_variant_new_tuple(&params, 1);
        params = tuple;
    }
    GDBusProxy* proxy = G_DBUS_PROXY(getGObject());
    if(proxy == nullptr)
    {
        DBG("GPPDBusProxy::" << __func__ << ": invalid DBus proxy!");
        return nullptr;
    }
    GError * defaultError = nullptr;
    GVariant* result = g_dbus_proxy_call_sync(proxy,
            methodName,
            params,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            nullptr,
            (error==nullptr ? &defaultError: error));
    g_clear_object(&proxy);
    if (defaultError != nullptr)
    {
        DBG("GPPDBusProxy::" << __func__ 
                << ": calling DBus adapter proxy method "
                << methodName << " failed!");
        DBG("Error: " << String(defaultError->message));
        g_clear_error(&defaultError);
    }
    if(result != nullptr  && g_variant_is_container(result))
    {
        gsize resultSize = g_variant_n_children(result);
        if(resultSize == 0)
        {
            g_variant_unref(result);
            return nullptr;
        }
        else if(resultSize == 1)
        {
            GVariant* extracted = nullptr;
            g_variant_get(result, "(*)" , &extracted);
            g_variant_unref(result);
            result = extracted;
        }
    }
    return result;
}

/**
 * Register a signal handler to receive DBus signals.
 */
void GPPDBusProxy::addDBusSignalHandler(DBusSignalHandler* signalHandler)
{
    addSignalHandler(static_cast<SignalHandler*>(signalHandler),
            "g-signal", G_CALLBACK(dBusSignalCallback));
}

/**
 * Register a signal handler to receive DBus property updates.
 */
void GPPDBusProxy::addDBusPropChangeHandler(DBusSignalHandler* signalHandler)
{
    addSignalHandler(static_cast<SignalHandler*>(signalHandler),
            "g-properties-changed",
            G_CALLBACK(dBusPropertiesChanged));
}

 /**
 * Get GDbusProxy's GLib class type.
 */
GType GPPDBusProxy::getType() const
{
    return G_TYPE_DBUS_PROXY;
}

/**
 * Callback function for handling all DBus signals.
 */
void GPPDBusProxy::dBusSignalCallback(GDBusProxy* proxy,
        gchar* senderName,
        gchar* signalName,
        GVariant* parameters,
        DBusSignalHandler* handler)
{
    if(isSignalHandlerValid(handler))
    {
        GPPDBusProxy* proxyWrapper = dynamic_cast<GPPDBusProxy*>
                (findObjectWrapper(G_OBJECT(proxy),
                static_cast<SignalHandler*>(handler)));
        if(proxyWrapper != nullptr)
        {
            handler->dBusSignalReceived(proxyWrapper,
                    String(senderName),
                    String(signalName),
                    parameters);
            return;
        }
    }
    g_variant_unref(parameters);
}

/**
 * Callback function for handling DBus property change signals.
 */
void GPPDBusProxy::dBusPropertiesChanged(GDBusProxy* proxy,
        GVariant* changedProperties,
        GStrv invalidatedProperties,
        DBusSignalHandler* handler)
{
    if(isSignalHandlerValid(handler))
    {
        GPPDBusProxy* proxyWrapper = dynamic_cast<GPPDBusProxy*>
                (findObjectWrapper(G_OBJECT(proxy),
                dynamic_cast<SignalHandler*>(handler)));
        if(proxyWrapper != nullptr)
        {
            using namespace GVariantConverter;
            iterateDict(changedProperties,[proxyWrapper,handler]
                    (GVariant* key, GVariant* property)
            {
               String propName = getValue<String>(key);
               handler->dBusPropertyChanged(proxyWrapper, propName, property);
            });
            for(int i = 0; invalidatedProperties[i] != nullptr; i++)
            {
                String invalidProp(invalidatedProperties[i]);
                handler->dBusPropertyInvalidated(proxyWrapper, invalidProp);
            }
        }
    }
    g_variant_unref(changedProperties);
}