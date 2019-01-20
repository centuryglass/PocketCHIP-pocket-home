#pragma once
/**
 * @file  Wifi_Manager.h
 *
 * @brief  Ensures Wifi resources and signal handlers remain initialized as
 *         long as a Wifi::Manager object exists.
 */

#include "LibNM/ThreadHandler.h"
#include "Wifi_APList_Reader.h"
#include "Wifi_Connection_RecordReader.h"

#ifndef WIFI_IMPLEMENTATION
  #define WIFI_MANAGER_TEMP
  #define WIFI_IMPLEMENTATION
#endif

#include "Wifi_Device_Listener.h"
#include "Wifi_NMSignals_DeviceHandler.h"
#include "Wifi_NMSignals_ClientHandler.h"

#ifdef WIFI_MANAGER_TEMP
  #undef WIFI_MANAGER_TEMP
  #undef WIFI_IMPLEMENTATION
#endif

namespace Wifi { class Manager; }

class Wifi::Manager
{
public:
    /**
     * @brief  Initializes the LibNM thread resource, then creates and connects 
     *         all Wifi signal handlers.
     */
    Manager();

    /**
     * @brief  Destroys all Wifi signal handlers before the LibNM thread
     *         resource can be destroyed.
     */
    virtual ~Manager();

private:
    /* Ensures the LibNM thread resource is initialized. */
    LibNM::ThreadHandler nmThread;

    /* Ensures the access point list resource is initialized. */
    std::unique_ptr<APList::Reader> apListReader;

    /* Ensures the connection record resource is initialized. */
    std::unique_ptr<Connection::RecordReader> recordReader;

    /* Handles all signals from the LibNM thread resource's Client object. */
    std::unique_ptr<NMSignals::ClientHandler> clientSignalHandler;

    /* Handles all signals from the LibNM thread resource's DeviceWifi object.*/
    std::unique_ptr<NMSignals::DeviceHandler> deviceSignalHandler;
};
