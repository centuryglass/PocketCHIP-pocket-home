#include "PocketHomeApplication.h"
#include "PocketHomeWindow.h"
#include "Hardware_Audio.h"
#include "Windows_XInterface.h"
#include "Util_ShutdownListener.h"
#include "Util_TempTimer.h"
#include "Debug_ScopeTimerRecords.h"

#ifdef JUCE_DEBUG
/* Print the full class name before all debug output: */
static const constexpr char* dbgPrefix = "PocketHomeApplication::";
#endif

/* Milliseconds to wait between window focus attempts: */
static const constexpr int focusWaitMs = 100;
/* Each attempt to set and check window focus status should wait a little longer
 * before the next test. Wait time multiplier: */
static const constexpr float focusWaitMultiplier = 1.3;
/* Milliseconds to wait before abandoning window focus attempts: */
static const constexpr int focusTimeout = 20000;

/* Sets if tests should run after the window is created and focused. */
static bool runTests = false;

/* Sets if tests should print all output. */
static bool verboseTesting = false;

/* Specific test categories to run: */
static juce::StringArray testCategories;


/*
 * Performs all required initialization when the application is first launched.
 */
void PocketHomeApplication::initialise(const juce::String &commandLine)
{
    juce::StringArray args;
    args.addTokens(commandLine, true);

    if (args.contains("--help"))
    {
        using std::cerr;
        cerr << "arguments:" << std::endl;
        cerr << "  --help	         Print usage help\n";
        cerr << "  --fakeWifi	     Use fake WifiStatus\n";
        cerr << "  --test 	         Run all program tests\n";
        cerr << "     -categories    Run tests within listed categories\n";
        cerr << "     -v 	         Verbose test output\n";
        quit();
        return;
    }

    if (!Hardware::Audio::chipAudioInit())
    {
        DBG("PocketHomeApplication::" << __func__
                << ": PocketC.H.I.P audio setup failed");
    }
    lookAndFeel.reset(new Theme::LookAndFeel);
    juce::LookAndFeel::setDefaultLookAndFeel(lookAndFeel.get());

    runTests = args.contains("--test");
    if(runTests)
    {
        verboseTesting = args.contains("-v");
        int categoryIndex = args.indexOf("-categories");
        if(categoryIndex != -1)
        {
            for(int i = categoryIndex + 1; i < args.size(); i++)
            {
                testCategories.add(args[i]);
            }
        }

        // Use an empty window when testing.
        homeWindow.reset(new juce::DocumentWindow(getApplicationName(), 
                juce::Colours::dimgrey, juce::DocumentWindow::allButtons));
        homeWindow->setBounds(0,0,50,50);
        homeWindow->setLookAndFeel(&juce::LookAndFeel::getDefaultLookAndFeel());
        homeWindow->setUsingNativeTitleBar(true);
        homeWindow->setResizable(true, false);
        homeWindow->setVisible(true);
        homeWindow->addToDesktop();
    }
    else
    {
        homeWindow.reset(new PocketHomeWindow(getApplicationName()));
    }           

    focusChecker.setCheckInterval(focusWaitMs, focusWaitMultiplier);
    const auto onFocus = [this]()
    {
        DBG(dbgPrefix << __func__ 
                << ": Main window focused, enabling focus tracking:");
        static_cast<Windows::MainWindow*>(homeWindow.get())
                ->startFocusTracking();
        if(runTests)
        {
            runApplicationTests();
        }
    };
#ifdef JUCE_DEBUG
    focusChecker.startCheck([this]() { return focusAppWindow(); },
            onFocus, focusTimeout, 
            []()
            {
                DBG(dbgPrefix << __func__ 
                        << ": Window focus attempts timed out!");
            });
#else
    focusChecker.startCheck([this]() { return focusAppWindow(); },
            onFocus, focusTimeout);
#endif
}

/*
 * Performs all necessary cleanup steps before the application can be safely 
 * closed.
 */
void PocketHomeApplication::shutdown()
{
    DBG(dbgPrefix << __func__ << ": Closing application resources.");
    Util::ShutdownBroadcaster::broadcastShutdown();
    homeWindow.reset(nullptr);
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    lookAndFeel.reset(nullptr);
    Debug::ScopeTimerRecords::printRecords();
}

/*
 * Gets the name of this application.
 */
const juce::String PocketHomeApplication::getApplicationName()
{
    return ProjectInfo::projectName;
}

/*
 * Gets the application's version string.
 */
const juce::String PocketHomeApplication::getApplicationVersion()
{
    return ProjectInfo::versionString;
}

/*
 * Checks if multiple versions of this application may run simultaneously.
 */
bool PocketHomeApplication::moreThanOneInstanceAllowed()
{
    return false;
}

/*
 * Runs application tests and shuts down the application.
 */
void PocketHomeApplication::runApplicationTests()
{
    juce::UnitTestRunner tester;
    tester.setPassesAreLogged(verboseTesting);
    if(testCategories.isEmpty())
    {
        DBG(dbgPrefix << __func__ << ": Running all pocket-home tests.");
        tester.runAllTests();
    }
    else
    {
        for(const juce::String& category : testCategories)
        {
            DBG(dbgPrefix << __func__ << ": Running test category: " 
                    << category);
            tester.runTestsInCategory(category);
        }
    }
    DBG(dbgPrefix << __func__ << ": Finished running application tests.");
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

/*
 * Attempts to activate the application window and grab keyboard focus.  
 */
bool PocketHomeApplication::focusAppWindow()
{
    if(homeWindow == nullptr)
    {
        return false;
    }
    Windows::XInterface xWindows;
    Window appWindow = xWindows.getMainAppWindow();
    if(appWindow == BadWindow)
    {
        return false;
    }
    xWindows.activateWindow(appWindow);
    homeWindow->grabKeyboardFocus();
    return xWindows.isActiveWindow(appWindow) 
            && homeWindow->hasKeyboardFocus(true);
}
