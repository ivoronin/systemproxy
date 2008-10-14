/*
 * $Id$
 */
#include "SystemProxyService.h"

NS_IMPL_ISUPPORTS2(SystemProxyService, SystemProxyInterface, nsIObserver);

void SystemProxyServiceCallout(SCDynamicStoreRef store __unused,
    CFArrayRef changedKeys __unused,
    void* info)
{
    static_cast<SystemProxyService*>(info)->updateSettings();
}

SystemProxyService::SystemProxyService()
{
    nsresult rv;

    /*
      Logging
      Compile with -DFORCE_PR_LOG or -DDEBUG then:
      export NSPR_LOG_MODULES=SystemProxy:4
      export NSPR_LOG_FILE=/tmp/SystemProxy.log
    */
    logModule = PR_NewLogModule("SystemProxy");
    LOG_DEBUG(logModule, "SystemProxy is started");

    /* Create dynamic store session */
    SCDynamicStoreContext calloutContext = {0, this, NULL, NULL, NULL};

    dynamicStore = SCDynamicStoreCreate(NULL,
         CFSTR("Mozilla"),
         SystemProxyServiceCallout,
         &calloutContext);

    if (dynamicStore == NULL)
    {
        LOG_ERROR(logModule, "Unable to create SCDynamicStore session");
        return;
    }

    /* Proxy keys names array */
    CFStringRef proxyKeys[] = {SCDynamicStoreKeyCreateProxies(NULL)};
    CFArrayRef proxyKeysArray;

    proxyKeysArray = CFArrayCreate(NULL, (const void**)proxyKeys, 1, &kCFTypeArrayCallBacks);

    /* Setup notifications */
    if (!SCDynamicStoreSetNotificationKeys(dynamicStore, proxyKeysArray, NULL))
    {
        LOG_ERROR(logModule, "Unable to set notifications keys");
        return;
    }

    CFRunLoopAddSource(CFRunLoopGetCurrent(),
        SCDynamicStoreCreateRunLoopSource(NULL, dynamicStore, 0),
        kCFRunLoopDefaultMode);

    prefs = do_GetService(NS_PREFSERVICE_CONTRACTID, &rv);
    if (NS_FAILED(rv))
    {
        LOG_ERROR(logModule, "Error getting handle to the preference service");
        return;
    }
}

SystemProxyService::~SystemProxyService()
{
    if (dynamicStore != NULL)
        CFRelease(dynamicStore);
}

void SystemProxyService::updateSettings()
{
    SystemProxySettings* proxySettings;
    PRBool extensionEnabled;
    nsresult rv;

    rv = prefs->GetBoolPref(EXTENSIONS_SYSTEMPROXY_ENABLED, &extensionEnabled);
    if (NS_FAILED(rv))
    {
        LOG_ERROR(logModule, "Error getting preferences");
        return;
    }
    if (!extensionEnabled)
        return;

    try
    {
        proxySettings = new SystemProxySettings(dynamicStore);
    } catch(const char* e)
    {
        LOG_ERROR(logModule, "exception '%s', aborting", e);
        return;
    }

    /* Direct Connection */
    prefs->SetIntPref(NETWORK_PROXY_TYPE, NETWORK_PROXY_TYPE_DIRECT);

    prefs->SetIntPref(NETWORK_PROXY_SOCKS_VERSION, 5);
    prefs->SetBoolPref(NETWORK_PROXY_SHARE_PROXY_SETTINGS, false);

    if (proxySettings->proxyEnabled())
    {
        prefs->SetIntPref(NETWORK_PROXY_TYPE, NETWORK_PROXY_TYPE_MANUAL);
        /* HTTP */
        prefs->SetCharPref(NETWORK_PROXY_HTTP, proxySettings->proxyHostFor(PROTOCOL_HTTP));
        prefs->SetIntPref(NETWORK_PROXY_HTTP_PORT, proxySettings->proxyPortFor(PROTOCOL_HTTP));
        /* HTTPS */
        prefs->SetCharPref(NETWORK_PROXY_SSL, proxySettings->proxyHostFor(PROTOCOL_HTTPS));
        prefs->SetIntPref(NETWORK_PROXY_SSL_PORT, proxySettings->proxyPortFor(PROTOCOL_HTTPS));
        /* FTP */
        prefs->SetCharPref(NETWORK_PROXY_FTP, proxySettings->proxyHostFor(PROTOCOL_FTP));
        prefs->SetIntPref(NETWORK_PROXY_FTP_PORT, proxySettings->proxyPortFor(PROTOCOL_FTP));
        /* GOPHER */
        prefs->SetCharPref(NETWORK_PROXY_GOPHER, proxySettings->proxyHostFor(PROTOCOL_GOPHER));
        prefs->SetIntPref(NETWORK_PROXY_GOPHER_PORT, proxySettings->proxyPortFor(PROTOCOL_GOPHER));
        /* SOCKS */
        prefs->SetCharPref(NETWORK_PROXY_SOCKS, proxySettings->proxyHostFor(PROTOCOL_SOCKS));
        prefs->SetIntPref(NETWORK_PROXY_SOCKS_PORT, proxySettings->proxyPortFor(PROTOCOL_SOCKS));
        /* Exceptions */
        prefs->SetCharPref(NETWORK_PROXY_NO_PROXIES_IN, proxySettings->exceptionsList());

    }
    else
    {
        prefs->SetCharPref(NETWORK_PROXY_AUTOCONFIG_URL, "");
        /* HTTP */
        prefs->SetCharPref(NETWORK_PROXY_HTTP, "");
        prefs->SetIntPref(NETWORK_PROXY_HTTP_PORT, 0);
        /* HTTPS */
        prefs->SetCharPref(NETWORK_PROXY_SSL, "");
        prefs->SetIntPref(NETWORK_PROXY_SSL_PORT, 0);
        /* FTP */
        prefs->SetCharPref(NETWORK_PROXY_FTP, "");
        prefs->SetIntPref(NETWORK_PROXY_FTP_PORT, 0);
        /* GOPHER */
        prefs->SetCharPref(NETWORK_PROXY_GOPHER, "");
        prefs->SetIntPref(NETWORK_PROXY_GOPHER_PORT, 0);
        /* SOCKS */
        prefs->SetCharPref(NETWORK_PROXY_SOCKS, "");
        prefs->SetIntPref(NETWORK_PROXY_SOCKS_PORT, 0);
        /* Exceptions */
        prefs->SetCharPref(NETWORK_PROXY_NO_PROXIES_IN, "");
    }

    /* Web Proxy Auto Discovery (WPAD) */
    if (proxySettings->autodiscoveryEnabled())
    {
        prefs->SetIntPref(NETWORK_PROXY_TYPE, NETWORK_PROXY_TYPE_WPAD);
    }

    /* Web Proxy Auto Configuration (PAC) */
    if (proxySettings->autoconfigEnabled())
    {
        prefs->SetIntPref(NETWORK_PROXY_TYPE, NETWORK_PROXY_TYPE_AUTO);
        prefs->SetCharPref(NETWORK_PROXY_AUTOCONFIG_URL, proxySettings->autoconfigURL());
    }
    else
    {
        prefs->SetCharPref(NETWORK_PROXY_AUTOCONFIG_URL, "");
    }

    prefs->SetIntPref(EXTENSIONS_SYSTEMPROXY_UPDATED, (int) random());

    /* Delete objects and return */
    delete proxySettings;

    return;
}

NS_IMETHODIMP SystemProxyService::Observe(nsISupports* subject __unused,
    const char* topic,
    const PRUnichar* data __unused)
{
    nsresult rv;

    if (!strcmp(topic, "app-startup"))
    {
        nsCOMPtr<nsIObserverService> observerService = do_GetService(NS_OBSERVERSERVICE_CONTRACTID, &rv);
        if (NS_FAILED(rv))
        {
            LOG_ERROR(logModule, "Error getting observer service handle");
            return rv;
        }
        rv = observerService->AddObserver(this, "profile-after-change", PR_FALSE);
        if (NS_FAILED(rv))
        {
            LOG_ERROR(logModule, "Error adding observer");
            return rv;
        }
        nsCOMPtr<nsIPrefBranch2> prefService = do_GetService(NS_PREFSERVICE_CONTRACTID, &rv);
        if (NS_FAILED(rv))
        {
            LOG_ERROR(logModule, "Error getting preference service handle");
            return rv;
        }
        rv = prefService->AddObserver(EXTENSIONS_SYSTEMPROXY_ENABLED, this, PR_FALSE);
        if (NS_FAILED(rv))
        {
            LOG_ERROR(logModule, "Error adding preferences observer");
            return rv;
        }
    }
    else if (!strcmp(topic, "profile-after-change") || !strcmp(topic, NS_PREFBRANCH_PREFCHANGE_TOPIC_ID))
        updateSettings();

    return NS_OK;
}
