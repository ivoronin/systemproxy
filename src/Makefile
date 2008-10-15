#
# $Id$
#
VERSION=1.1

# ftp://ftp.mozilla.org/pub/mozilla.org/xulrunner/releases/1.9.0.3/sdk/xulrunner-1.9.0.3.en-US.mac-i386.sdk.tar.bz2
XULRUNNER_SDK_I386=/Users/ivoronin/xulrunner-sdk-i386/
# ftp://ftp.mozilla.org/pub/mozilla.org/xulrunner/releases/1.9.0.3/sdk/xulrunner-1.9.0.3.en-US.mac-powerpc.sdk.tar.bz2
XULRUNNER_SDK_PPC=/Users/ivoronin/xulrunner-sdk-ppc/
XULRUNNER_SDK=$(XULRUNNER_SDK_I386)
XPIDL=$(XULRUNNER_SDK)/bin/xpidl

CXXFLAGS=-Wall -O2 -I$(XULRUNNER_SDK)/sdk/include -Wno-non-virtual-dtor -DFORCE_PR_LOG
CXXFLAGS_I386=$(CXXFLAGS) -Wl,-executable_path,$(XULRUNNER_SDK_I386)/bin
CXXFLAGS_PPC=$(CXXFLAGS) -Wl,-executable_path,$(XULRUNNER_SDK_PPC)/bin

LIBS=-lxpcom -lnspr4 -lxpcomglue_s
FRAMEWORKS=-framework SystemConfiguration -framework CoreFoundation
LDFLAGS=$(LIBS) $(FRAMEWORKS) -dynamiclib -install_name libsystemproxy.dylib
LDFLAGS_I386=$(LDFLAGS) -L$(XULRUNNER_SDK_I386)/sdk/lib -arch i386
LDFLAGS_PPC=$(LDFLAGS) -L$(XULRUNNER_SDK_PPC)/sdk/lib -arch ppc

SOURCES=components/SystemProxyModule.cpp \
	components/SystemProxyService.cpp \
	components/SystemProxySettings.cpp

all: systemproxy-$(VERSION).xpi

components/libsystemproxy.xpt: components/SystemProxyInterface.idl
	$(XPIDL) -I $(XULRUNNER_SDK)/idl -m typelib -e $@ components/SystemProxyInterface.idl

components/libsystemproxy.dylib: components/libsystemproxy.ppc.dylib components/libsystemproxy.i386.dylib
	lipo -create -output $@ -arch ppc components/libsystemproxy.ppc.dylib \
				-arch i386 components/libsystemproxy.i386.dylib

components/libsystemproxy.i386.dylib: $(SOURCES) components/SystemProxyInterface.h
	g++ $(CXXFLAGS_I386) $(LDFLAGS_I386) -o $@  $(SOURCES)

components/libsystemproxy.ppc.dylib:  $(SOURCES) components/SystemProxyInterface.h
	g++ $(CXXFLAGS_PPC) $(LDFLAGS_PPC) -o $@ $(SOURCES)

components/SystemProxyInterface.h: components/SystemProxyInterface.idl
	$(XPIDL) -I $(XULRUNNER_SDK)/idl -m header -e $@ components/SystemProxyInterface.idl

systemproxy-$(VERSION).xpi: components/libsystemproxy.dylib components/libsystemproxy.xpt
	zip -r $@ install.rdf chrome.manifest defaults/ chrome/ \
		components/libsystemproxy.xpt components/libsystemproxy.dylib -x "*CVS*"

check:
	xmllint --noout install.rdf
clean:
	rm -f components/libsystemproxy.dylib
	rm -f components/libsystemproxy.ppc.dylib
	rm -f components/libsystemproxy.i386.dylib
	rm -f components/libsystemproxy.xpt
	rm -f components/SystemProxyInterface.h
	rm -f systemproxy-*.xpi
