/*
 * $Id$
 */
#include <CoreFoundation/CoreFoundation.h>
#include <SystemConfiguration/SystemConfiguration.h>

/*
 * Protocols
 */
#define        PROTOCOL_HTTP        "http"
#define        PROTOCOL_HTTPS        "https"
#define        PROTOCOL_FTP        "ftp"
#define        PROTOCOL_GOPHER        "gopher"
#define        PROTOCOL_SOCKS        "socks"

class SystemProxySettings {
    public:
        SystemProxySettings(SCDynamicStoreRef);
        /* Proxy Autodiscovery (WPAD) */
        bool autodiscoveryEnabled();
        /* Proxy Autoconfiguration (PAC) */
        bool autoconfigEnabled();
        char* autoconfigURL();

        /* Manual configuration */
        bool proxyEnabled();
        bool proxyEnabledFor(char*);
        char* proxyHostFor(char*);
        int proxyPortFor(char*);

        /* Exceptions List */
        char* exceptionsList();
        ~SystemProxySettings();
    private:
        CFDictionaryRef proxySettings;
        void* getProp(CFStringRef entityKey);
        bool getBoolProp(CFStringRef entityKey);
        int getIntProp(CFStringRef entityKey);
        char* getCharProp(CFStringRef entityKey);
};
