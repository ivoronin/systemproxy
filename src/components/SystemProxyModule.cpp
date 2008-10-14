/*
 * $Id$
 */
#include "SystemProxyModule.h"

NS_METHOD
RegisterSystemProxyService(nsIComponentManager* aCompMgr __unused,
    nsIFile* aPath __unused,
    const char* registryLocation __unused,
    const char* componentType __unused,
    const nsModuleComponentInfo* info __unused)
{
    nsresult rv;

    nsCOMPtr<nsICategoryManager> categoryManager(do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv));
    if (NS_SUCCEEDED(rv))
    {
        rv = categoryManager->AddCategoryEntry(APPSTARTUP_CATEGORY,
            SYSTEMPROXY_SERVICE_CLASSNAME,
            "service," SYSTEMPROXY_SERVICE_CONTRACTID,
            PR_TRUE, PR_TRUE, nsnull);
    }
    return rv;
}

NS_METHOD UnregisterSystemProxyService(nsIComponentManager* aCompMgr,
    nsIFile* aPath __unused,
    const char* registryLocation __unused,
    const nsModuleComponentInfo* info __unused)
{
    nsresult rv;

    nsCOMPtr<nsICategoryManager> categoryManager(do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv));
    if (NS_SUCCEEDED(rv))
    {
        rv = categoryManager->DeleteCategoryEntry(APPSTARTUP_CATEGORY,
            SYSTEMPROXY_SERVICE_CLASSNAME,
            PR_TRUE);
    }
    return rv;
}

NS_GENERIC_FACTORY_CONSTRUCTOR(SystemProxyService)
    static const nsModuleComponentInfo moduleComponents[] = {
        {
            SYSTEMPROXY_SERVICE_CLASSNAME,
            SYSTEMPROXY_SERVICE_CID,
            SYSTEMPROXY_SERVICE_CONTRACTID,
            SystemProxyServiceConstructor,
            RegisterSystemProxyService,
            UnregisterSystemProxyService,
        },
    };

NS_IMPL_NSGETMODULE("SystemProxyService", moduleComponents);
