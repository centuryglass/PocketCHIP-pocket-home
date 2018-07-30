/**
 * @file GLibThread.h
 * 
 * @brief GLibThread runs a GLib event loop on a separate thread. 
 * 
 * On creation, this starts up a GLib event thread to handle events associated
 * with a GMainContext provided on creation.  This GMainContext will be set as
 * the thread default context. This context will be available through the 
 * getContext method, so that it can be used to add signal sources and signal
 * handlers to this thread.
 * 
 * GLibThread also provides methods for synchronously or asynchronously
 * executing code within the thread.  These should be used to handle all
 * interactions with GLib object tied to the thread context, unless those
 * objects are explicitly guaranteed to be thread-safe.
 */

#pragma once

// Since the main UI/message thread isn't a Juce thread, standard C++ thread
// libraries need to be used to wait/notify
#include <mutex>
#include <condition_variable>

#include "gio/gio.h"
#include "WindowFocus.h"
#include "JuceHeader.h"

class GLibThread : private juce::Thread, private WindowFocus::Listener
{
public:
    /**
     * @param context  The event context that will be claimed by this thread.
     *                 This will be unreferenced when the thread is destroyed.
     */
    GLibThread(GMainContext* context);
    
    virtual ~GLibThread();
    
    /**
     * Returns true if it's being executed on this GLib event thread.
     */
    bool runningOnThread();
    
    /**
     * Run a function on this GLib event loop, waiting until the function
     * has finished.
     * 
     * @param fn  A function that needs to run on the GLib event loop.
     */
    void call(std::function<void()> fn);
    
    /**
     * Asynchronously run a function once on this GLib event loop.
     * 
     * @param fn   A function that needs to run on the GLib event loop.
     */
    void callAsync(std::function<void()> fn);
    
    /**
     * Returns the thread default context that was set when this thread was
     * created.
     */
    GMainContext* getContext();
    
private:
    /*
     * Holds all data needed to handle a function call passed in by the call()
     * or callAsync() methods.
     */
    struct CallData
    {
        std::function<void() > call;
        GSource* callSource;
        std::mutex* callerMutex;
        std::condition_variable* callPending;
    };
    
    /**
     * Adds a function to the GMainContext so it will execute on the event
     * thread.
     * 
     * @param call         The function to run.
     * 
     * @param callerMutex  If this value is non-null, the event thread will
     *                     lock it while running this call.
     * 
     * @param callPending  If this value is non-null, the event thread will
     *                     use it and callerMutex to wake up the calling
     *                     thread after running the call.
     */
    void addAndInitCall(std::function<void() > call,
            std::mutex* callerMutex = nullptr,
            std::condition_variable* callPending = nullptr);

    /**
     * Runs the GLib main loop.
     */
    void run() override;

    /**
     * Callback function used to execute arbitrary functions on the 
     * GMainLoop.
     */
    static gboolean runAsync(CallData* runData);

    /**
     * Pause the event loop whenever window focus is lost.
     */
    virtual void windowFocusLost() override;
    
    /*
     * Resume the event loop whenever window focus is regained.
     */
    virtual void windowFocusGained() override;
    
    /**
     * Starts the GLib thread, then waits until the thread is running and
     * the thread context and main loop are initialized. This function locks the
     * threadStateLock for writing.
     * 
     * @return  True if the thread started successfully, false if the
     *          GLibThread is being destroyed.
     */
    bool startGLibThread();
    
    /**
     * Terminates the GLib main loop and stops the thread. This function locks
     * the threadStateLock for writing.
     * 
     * @param unrefGLibVars  If true, the thread context and GLib main loop 
     *                       will be dereferenced and removed.  This should only
     *                       be set to true if the GLibThread is being
     *                       destroyed, as the thread cannot be started again
     *                       after the GLib variables are removed.
     */
    void stopGLibThread(bool unrefGLibVars = false);
    
    GMainContext* context = nullptr;
    GMainLoop* mainLoop = nullptr;
    
    //Prevent thread access while it is being started or stopped.
    juce::ReadWriteLock threadStateLock;
    
    //Used when waiting for the thread to start.
    std::mutex threadStartMutex;
    std::condition_variable threadStarting;
};
