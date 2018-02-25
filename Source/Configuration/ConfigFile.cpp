#include "../Utils.h"
#include "ConfigFile.h"
#include "Configurables/Configurable.h"

template<> std::map<String, int>& ConfigFile::getMapReference<int>()
{
    const ScopedLock lockFileMaps(configLock);
    return intValues[filename];
}

template<> std::map<String, String>& ConfigFile::getMapReference<String>()
{
    const ScopedLock lockFileMaps(configLock);
    return stringValues[filename];
}

template<> std::map<String, bool>& ConfigFile::getMapReference<bool>()
{
    const ScopedLock lockFileMaps(configLock);
    return boolValues[filename];
}

CriticalSection ConfigFile::configLock;
std::map<String, CriticalSection> ConfigFile::fileLocks;
std::map<String, bool> ConfigFile::openFileMap;
std::map<String, bool> ConfigFile::fileChangesPending;
std::map<String, std::map<String, int>> ConfigFile::intValues;
std::map<String, std::map<String, String>> ConfigFile::stringValues;
std::map<String, std::map<String, bool>> ConfigFile::boolValues;
std::map<String, std::map<String, Array<Configurable*>>>
ConfigFile::configured;

ConfigFile::ConfigFile(String configFilename) : filename(configFilename)
{
    const ScopedLock changeLock(configLock);
    if (fileChangesPending.count(filename) == 0)
    {
        fileChangesPending[filename] = false;
    }

    if (fileChangesPending.count(filename) == 0)
    {
        openFileMap[filename] = false;
    }
}

/**
 * Writes any pending changes to the file before destruction.
 */
ConfigFile::~ConfigFile()
{
    const ScopedLock writeLock(configLock);
    writeChanges();
}

/**
 * Register an object as tracking configuration changes. That object
 * is notified whenever any data it tracks is changed.
 */
void ConfigFile::registerConfigurable(Configurable * configurable,
        Array<String> keys)
{
    const ScopedLock changeLock(configLock);
    for (const String& key : keys)
    {
        configured[filename][key].add(configurable);
    }
}

/**
 * Removes an object from the list of objects to notify when configuration
 * changes.
 */
void ConfigFile::unregisterConfigurable(Configurable * configurable,
        Array<String> keys)
{
    const ScopedLock changeLock(configLock);
    for (const String& key : keys)
    {
        configured[filename][key].removeAllInstancesOf(configurable);
    }
}

/**
 * @return true iff this ConfigFile and rhs have the same filename.
 */
bool ConfigFile::operator==(const ConfigFile& rhs) const
{
    return filename == rhs.filename;
}

/**
 * @return the CriticalSection shared by all ConfigFile objects that
 * access the same file as this one.
 */
CriticalSection& ConfigFile::getFileLock()
{
    const ScopedLock readLock(configLock);
    return fileLocks[filename];
}

/**
 * Opens and reads data from this ConfigFile's json file.  This will mark
 * the file as opened, so that ConfigFiles can avoid reading in file data
 * more than once.
 */
var ConfigFile::openFile()
{
    const ScopedLock readLock(getFileLock());
    if (fileOpened())
    {
        return var::null;
    }
    openFileMap[filename] = true;
    File configFile = File(getHomePath() + String(CONFIG_PATH) + filename);
    return JSON::parse(configFile);
}

/**
 * Check to see if this ConfigFile has already read data from its json file.
 * @return true iff the file has been read.
 */
bool ConfigFile::fileOpened()
{
    const ScopedLock lockFileMaps(configLock);
    const ScopedLock lockFileData(getFileLock());
    return openFileMap[filename];
}

/**
 * Marks the ConfigFile as containing changes that need to be written
 * back to the object's json file.
 */
void ConfigFile::markPendingChanges()
{
    const ScopedLock lockFileMaps(configLock);
    const ScopedLock lockFileData(getFileLock());
    fileChangesPending[filename] = true;
}


//################################# File IO ####################################

/**
 * Read in this object's data from a json config object
 */
void ConfigFile::readDataFromJson(var& config, var& defaultConfig)
{
    std::vector<DataKey> dataKeys = getDataKeys();
    for (const DataKey& key : dataKeys)
    {
        switch (key.dataType)
        {
            case stringType:
                initMapProperty<String>(key.keyString, getProperty
                        (config, defaultConfig, key.keyString).toString());
                break;

            case intType:
                initMapProperty<int>(key.keyString,
                        getProperty(config, defaultConfig, key.keyString));
                break;

            case boolType:
                initMapProperty<bool>(key.keyString,
                        getProperty(config, defaultConfig, key.keyString));
                break;
        }
    }
}

/**
 * Copy all config data to a json object
 */
void ConfigFile::copyDataToJson(DynamicObject::Ptr jsonObj)
{

    std::vector<DataKey> dataKeys = getDataKeys();
    for (const DataKey& key : dataKeys)
    {
        switch (key.dataType)
        {
            case stringType:
                jsonObj->setProperty(key.keyString,
                        getMapReference<String>()[key.keyString]);
                break;

            case intType:
                jsonObj->setProperty(key.keyString,
                        getMapReference<int>()[key.keyString]);
                break;

            case boolType:
                jsonObj->setProperty(key.keyString,
                        getMapReference<bool>()[key.keyString]);
                break;
        }
    }
}

/**
 * Checks if a property exists in a config data object loaded from a json
 * file.
 */
bool ConfigFile::propertyExists(var& config, String propertyKey)
{

    var property = config.getProperty(propertyKey, var::null);
    return var::null != property;
}

/**
 * Gets a property from json configuration data, or from default
 * configuration data if necessary
 */
var ConfigFile::getProperty(var& config, var& defaultConfig, String key)
{
    bool exists = propertyExists(config, key);
    DBG(key + (exists ? String(" exists") : String(" doesn't exist")));
    if (propertyExists(config, key))
    {
        return config.getProperty(key, var::null);
    }
    else
    {
        if (defaultConfig == var::null)
        {

            defaultConfig = JSON::parse(assetFile(filename));
        }
        fileChangesPending[filename] = true;
        return defaultConfig.getProperty(key, var::null);
    }
}

/**
 * Re-writes all data back to the config file, as long as there are
 * changes to write.
 * 
 * 
 * @pre any code calling this function is expected to have already
 * acquired the ConfigFile's lock
 */
void ConfigFile::writeChanges()
{
    if (!fileChangesPending[filename])
    {
        return;
    }
    DynamicObject::Ptr jsonBuilder = new DynamicObject();
    copyDataToJson(jsonBuilder.get());

    //convert to JSON string, write to config.json
    String jsonText = JSON::toString(jsonBuilder.get());
    File configFile = File(getHomePath() + String(CONFIG_PATH) + filename);
    if (!configFile.exists())
    {
        configFile.create();
    }
    if (!configFile.replaceWithText(jsonText))
    {
        String message = String("Failed to save changes to ~")
                + String(CONFIG_PATH) + filename
                + String("\nMake sure you have permission to write to this file.");
        AlertWindow::showMessageBox(
                AlertWindow::AlertIconType::WarningIcon,
                "Error saving configuration:",
                message);
    }
    else
    {

        fileChangesPending[filename] = false;
    }
}

/**
 * Announce new changes to each object tracking a particular key.
 */
void ConfigFile::notifyConfigurables(String key)
{
    for (Configurable * tracking : configured[filename][key])
    {
        tracking->loadConfigProperties(this, key);
    }
}