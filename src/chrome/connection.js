/*
 * $Id$
 */
var systemProxy =
{
    updatePrefs: function()
    {
        var systemProxyEnabled = document.getElementById("extensions.systemproxy.enabled");
        var prefs = Components.classes["@mozilla.org/preferences-service;1"].getService(Components.interfaces.nsIPrefBranch);
        prefs.setBoolPref("extensions.systemproxy.enabled", systemProxyEnabled.value);
    },

    disablePrefs: function()
    {
        var systemProxyEnabled = document.getElementById("extensions.systemproxy.enabled");

        var networkProxyPrefs = ["http", "ssl", "ftp", "socks", "gopher"];
        for (var i=0; i<networkProxyPrefs.length; ++i)
        {
            var networkProxyHost = document.getElementById("network.proxy." + networkProxyPrefs[i]);
            var networkProxyPort = document.getElementById("network.proxy." + networkProxyPrefs[i] + "_port");
            if ( ! networkProxyHost || ! networkProxyPort )
                continue;
            networkProxyHost.disabled = systemProxyEnabled.value;
            networkProxyPort.disabled = systemProxyEnabled.value;
        }

        var proxyTypePref = document.getElementById("network.proxy.type");
        var shareAllProxiesPref = document.getElementById("network.proxy.share_proxy_settings");
        var networkProxySOCKSVersionPref = document.getElementById("network.proxy.socks_version");
        var networkProxyNonePref = document.getElementById("network.proxy.no_proxies_on");
        var networkProxyAutoconfigURLPref = document.getElementById("network.proxy.autoconfig_url");

        proxyTypePref.disabled = systemProxyEnabled.value;
        shareAllProxiesPref.disabled = systemProxyEnabled.value;
        networkProxySOCKSVersionPref.disabled = systemProxyEnabled.value;
        networkProxyNonePref.disabled = systemProxyEnabled.value;
        networkProxyAutoconfigURLPref.disabled = systemProxyEnabled.value;
   }
};
