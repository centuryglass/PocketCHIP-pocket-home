#pragma once
#include "ComponentConfigFile.h"
#include "PageComponent.h"
#include "JuceHeader.h"

/**
 * @file PageFactory.h
 * 
 * PageFactory creates all new PageComponent objects, and initializes all
 * shared resources needed by pages.
 */

class PageFactory : public PageComponent::PageFactoryInterface
{
public:
    PageFactory() { }

    virtual ~PageFactory() { }

    /**
     * Initializes a HomePage instance to use as the root page of the page 
     * stack.
     */
    PageComponent* createHomePage();

    /**
     * Initializes a login page instance.
     * 
     * @param loginCallback  A callback function for the page to run when the 
     *                        user successfully logs in.
     */
    PageComponent* createLoginPage(std::function<void () > loginCallback);

private:
    /**
     * Create a new page to push on top of the page stack.
     */
    PageComponent* createPage(PageComponent::PageType type) override;
};