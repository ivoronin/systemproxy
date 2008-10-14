/*
 * $Id$
 */
#include "SystemProxySettings.h"

SystemProxySettings::SystemProxySettings(SCDynamicStoreRef dynamicStore)
{
    if (!(proxySettings = SCDynamicStoreCopyProxies(dynamicStore)))
        throw "SCDynamicStoreCopyProxies returned NULL value";
    if (!CFDictionaryGetCount(proxySettings))
        throw "SCDynamicStoreCopyProxies returted empty dictionary";
}

SystemProxySettings::~SystemProxySettings()
{
    if (!proxySettings)
        CFRelease(proxySettings);
}

void* SystemProxySettings::getProp(CFStringRef entityKey)
{
    void* value;

    if (!CFDictionaryGetValueIfPresent(proxySettings, entityKey, (const void**) &value))
        value = NULL;

    return value;
}

bool SystemProxySettings::getBoolProp(CFStringRef entityKey)
{
    return (getIntProp(entityKey));
}

int SystemProxySettings::getIntProp(CFStringRef entityKey)
{
    CFNumberRef n;
    int i;

    if (!(n = (CFNumberRef) getProp(entityKey)))
        return 0;

    if (CFNumberGetValue(n, kCFNumberIntType, &i) == false)
        return 0;

    return i;
}

char* SystemProxySettings::getCharProp(CFStringRef entityKey)
{
    CFStringRef s;

    if (!(s = (CFStringRef) getProp(entityKey)))
        return "";

    return (char*) CFStringGetCStringPtr(s, CFStringGetSystemEncoding());
}

/* Proxy Autodiscovery (WPAD) */
bool SystemProxySettings::autodiscoveryEnabled()
{
     return getBoolProp(kSCPropNetProxiesProxyAutoDiscoveryEnable);
}

/* Proxy Autoconfiguration (PAC) */
bool SystemProxySettings::autoconfigEnabled()
{
    return getBoolProp(kSCPropNetProxiesProxyAutoConfigEnable);
}

char* SystemProxySettings::autoconfigURL()
{
    return getCharProp(kSCPropNetProxiesProxyAutoConfigURLString);
}

bool SystemProxySettings::proxyEnabled()
{
    return (proxyEnabledFor(PROTOCOL_HTTP) ||
        proxyEnabledFor(PROTOCOL_HTTPS) ||
        proxyEnabledFor(PROTOCOL_FTP) ||
        proxyEnabledFor(PROTOCOL_GOPHER) ||
        proxyEnabledFor(PROTOCOL_SOCKS));
}

bool SystemProxySettings::proxyEnabledFor(char* protocol)
{
    if (!strcmp(protocol, PROTOCOL_HTTP))
        return getBoolProp(kSCPropNetProxiesHTTPEnable);
    else if (!strcmp(protocol, PROTOCOL_HTTPS))
        return getBoolProp(kSCPropNetProxiesHTTPSEnable);
    else if (!strcmp(protocol, PROTOCOL_FTP))
        return getBoolProp(kSCPropNetProxiesFTPEnable);
    else if (!strcmp(protocol, PROTOCOL_GOPHER))
        return getBoolProp(kSCPropNetProxiesGopherEnable);
    else if (!strcmp(protocol, PROTOCOL_SOCKS))
        return getBoolProp(kSCPropNetProxiesSOCKSEnable);
    else
        return false;
}

char* SystemProxySettings::proxyHostFor(char* protocol)
{
    if (!strcmp(protocol, PROTOCOL_HTTP))
        return getCharProp(kSCPropNetProxiesHTTPProxy);
    else if (!strcmp(protocol, PROTOCOL_HTTPS))
        return getCharProp(kSCPropNetProxiesHTTPSProxy);
    else if (!strcmp(protocol, PROTOCOL_FTP))
        return getCharProp(kSCPropNetProxiesFTPProxy);
    else if (!strcmp(protocol, PROTOCOL_GOPHER))
        return getCharProp(kSCPropNetProxiesGopherProxy);
    else if (!strcmp(protocol, PROTOCOL_SOCKS))
        return getCharProp(kSCPropNetProxiesSOCKSProxy);
    return "";
}

int SystemProxySettings::proxyPortFor(char* protocol)
{
    if (!strcmp(protocol, PROTOCOL_HTTP))
        return getIntProp(kSCPropNetProxiesHTTPPort);
    else if (!strcmp(protocol, PROTOCOL_HTTPS))
        return getIntProp(kSCPropNetProxiesHTTPSPort);
    else if (!strcmp(protocol, PROTOCOL_FTP))
        return getIntProp(kSCPropNetProxiesFTPPort);
    else if (!strcmp(protocol, PROTOCOL_GOPHER))
        return getIntProp(kSCPropNetProxiesGopherPort);
    else if (!strcmp(protocol, PROTOCOL_SOCKS))
        return getIntProp(kSCPropNetProxiesSOCKSPort);
    else
        return 0;
}

char* SystemProxySettings::exceptionsList()
{
    CFArrayRef exceptionsArray;

    if ((exceptionsArray = (CFArrayRef) getProp(kSCPropNetProxiesExceptionsList)) == NULL)
        return "";

    return (char*) CFStringGetCStringPtr(CFStringCreateByCombiningStrings(NULL,
        exceptionsArray,
        CFSTR(", ")),
        CFStringGetSystemEncoding());
}
