#include <map>
#include "SharedResource.h"
#include "ScopedThreadWriteLock.h"

//Holds each SharedResource subclass's single object.
static std::map<juce::Identifier, juce::ScopedPointer<SharedResource>> 
resourceMap;
static juce::ReadWriteLock resourceMapLock;

/*
 * Creates the single object instance shared by a ResourceManager
 * subclass.
 */
SharedResource::SharedResource(const juce::Identifier& resourceKey) :
resourceKey(resourceKey)
{
    using namespace juce;
    const ScopedWriteLock resourceInitLock(resourceMapLock);
    jassert(resourceMap[resourceKey] == nullptr);
    resourceMap[resourceKey] = this;
}

/*
 * In debug builds, print an error message if the resource is destroyed
 * while its ResourceManager list is not empty.
 */
SharedResource::~SharedResource()
{
    using namespace juce;
    if(!resourceHandlers.isEmpty())
    {
        DBG("SharedResource::~SharedResource: Destroying resource while list of"
                << " ResourceManagers is not empty!");
    }    
}

/*
 * Creates a new Handler for a SharedResource, initializing the resource
 * if necessary.
 */
SharedResource::Handler::Handler(
        const juce::Identifier& resourceKey,
        std::function<SharedResource*()> createResource) :
resourceKey(resourceKey)
{
    using namespace juce;
    SharedResource* classResource = getClassResource();
    if(classResource == nullptr)
    {
        classResource = createResource();
    }
    ScopedThreadWriteLock addHandlerLock(getResourceLock());
    classResource->resourceHandlers.addIfNotAlreadyThere(this);
}


/*
 * Removes a handler from the handler list, destroying the resource if
 * no handlers remain.
 */
SharedResource::Handler::~Handler()
{
    using namespace juce;
    const ScopedThreadWriteLock cleanupLock(getResourceLock());
    SharedResource* classResource = getClassResource();
    classResource->resourceHandlers.removeAllInstancesOf(this);
    if(classResource->resourceHandlers.isEmpty())
    {
        const ScopedWriteLock removalLock(resourceMapLock);
        resourceMap[resourceKey] = nullptr;
    }

}

/*
 * Gets a reference to the SharedResource object shared by all
 * objects of this ResourceManager subclass.
 */
SharedResource*
SharedResource::ResourceManager::getClassResource()
{
    const juce::ScopedReadLock mapLock(resourceMapLock);
    return resourceMap[resourceKey].get();
}

/**
 * Gets a reference to the lock used to control access to the
 * shared resource.
 */
juce::ReadWriteLock& SharedResource::ResourceManager::getResourceLock()
{
    return getClassResource()->resourceLock;;
}

/*
 * Packages an asynchronous action so that it will check if the 
 * SharedResource instance that created it stil valid, and if so,
 * ensure it remains valid while the action is executed.
 */
std::function<void()> SharedResource::buildAsyncFunction(
        SharedResource::LockType lockType,
        std::function<void()> action,
        std::function<void()> ifDestroyed)
{
    using namespace juce;
    ScopedPointer<SharedResource>& resource = instanceHolder;
    ReadWriteLock& lock = resourceLock;
    return [this, lockType, &resource, &lock, action, ifDestroyed]()
    {
        ScopedPointer<ResourceManager> resourceProtector = nullptr;
        lock.enterWrite();
        if(resource == this)
        {
            resourceProtector = new ResourceManager(resource, lock);
        }
        lock.exitWrite();

        if(resourceProtector != nullptr)
        {
            if(lockType == SharedResource::LockType::write)
            {
                const ScopedWriteLock writeLock(lock);
                action();
            }
            else if(lockType == SharedResource::LockType::read)
            {
                const ScopedReadLock readLock(lock);
                action();
            }
        }
        else
        {
            ifDestroyed();
        }
    };
}


/*
 * Runs an arbitrary function on each Handler object connected to the
 * SharedResource.i
 */
void SharedResource::foreachHandler
(std::function<void(const Handler*)> handlerAction)
{
    ScopedThreadWriteLock handlerLock(resourceLock);
    const int handlerCount = resourceHandlers.size();
    for(int i = 0; i < handlerCount; i++)
    {
        //ensure nothing is messing with the handler list during the loop
        jassert(resourceHandlers.size() == handlerCount);
        handlerAction(resourceHandlers[i]);

    }
}
