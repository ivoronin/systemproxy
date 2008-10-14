/*
 * $Id$
 */
#ifndef _SYSTEMPROXYSERVICE_H_
#define _SYSTEMPROXYSERVICE_H_

#include <CoreFoundation/CoreFoundation.h>
#include <SystemConfiguration/SystemConfiguration.h>

#include <nsIObserver.h>
#include <nsIPrefService.h>
#include <nsIPrefBranch2.h>
#include <nsIObserverService.h>
#include <nsServiceManagerUtils.h>
#include <nsCOMPtr.h>
#include <prlog.h>

#include "SystemProxyInterface.h"
#include "SystemProxySettings.h"

#define SYSTEMPROXY_SERVICE_CONTRACTID "@curthread.org/systemproxy;1"
#define SYSTEMPROXY_SERVICE_CLASSNAME "Mac OS X Proxy Settings Component"
#define SYSTEMPROXY_SERVICE_CID { 0x0fb783db, 0x3167, 0x11dc, \
    { 0x83, 0x14, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 } }

/*
 * Logging macros
 */
#define LOG(module, level, format, ...) \
    PR_LOG(module, level, ( "[%s] (%i): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define LOG_DEBUG(module, format, ...) \
    LOG(module, PR_LOG_DEBUG, format, ##__VA_ARGS__)
#define LOG_ERROR(module, format, ...) \
    LOG(module, PR_LOG_ERROR, format, ##__VA_ARGS__)

/* Network proxy type (see chrome://browser/preferences/connection.xul) */
#define NETWORK_PROXY_TYPE                 "network.proxy.type"
#define NETWORK_PROXY_TYPE_DIRECT          0
#define NETWORK_PROXY_TYPE_MANUAL          1
#define NETWORK_PROXY_TYPE_AUTO            2
#define NETWORK_PROXY_TYPE_WPAD            4
/* Proxy Auto Configuration (PAC) */
#define NETWORK_PROXY_AUTOCONFIG_URL       "network.proxy.autoconfig_url"
/* Manual Proxy Configuration */
#define NETWORK_PROXY_HTTP                 "network.proxy.http"
#define NETWORK_PROXY_HTTP_PORT            "network.proxy.http_port"
#define NETWORK_PROXY_SSL                  "network.proxy.ssl"
#define NETWORK_PROXY_SSL_PORT             "network.proxy.ssl_port"
#define NETWORK_PROXY_FTP                  "network.proxy.ftp"
#define NETWORK_PROXY_FTP_PORT             "network.proxy.ftp_port"
#define NETWORK_PROXY_GOPHER               "network.proxy.gopher"
#define NETWORK_PROXY_GOPHER_PORT          "network.proxy.gopher_port"
#define NETWORK_PROXY_SOCKS                "network.proxy.socks"
#define NETWORK_PROXY_SOCKS_PORT           "network.proxy.socks_port"
/* Other prefs */
#define NETWORK_PROXY_SHARE_PROXY_SETTINGS "network.proxy.share_proxy_settings"
#define NETWORK_PROXY_NO_PROXIES_IN        "network.proxy.no_proxies_on"
#define NETWORK_PROXY_SOCKS_VERSION        "network.proxy.socks_version"
/* Extension prefs */
#define EXTENSIONS_SYSTEMPROXY_ENABLED     "extensions.systemproxy.enabled"
#define EXTENSIONS_SYSTEMPROXY_UPDATED     "extensions.systemproxy.updated"


class SystemProxyService: public SystemProxyInterface, public nsIObserver
{
    public:
        NS_DECL_ISUPPORTS
        NS_DECL_SYSTEMPROXYINTERFACE
        NS_DECL_NSIOBSERVER

        SystemProxyService();
        void updateSettings();
    private:
        virtual ~SystemProxyService();
        nsCOMPtr <nsIPrefBranch> prefs;
        SCDynamicStoreRef dynamicStore;
        PRLogModuleInfo* logModule;
};

#endif /* _SYSTEMPROXYSERVICE_H_ */
