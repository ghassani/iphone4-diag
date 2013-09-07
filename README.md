iPhone 4 CDMA DIAG Attempt
-----------------------------

This is incomplete and not working.

This is an attempt to be able to provision a CDMA iPhone 4 by using a dylib injection with the CommCenter to hijack the baseband file discriptor and making 
a TCP server to use with a client software

This is how all current paid solutions that I am aware of accomplish provisioning the phone.

Requirements
----------------------------
- Jailbroken iPhone 4 CDMA w/ SSH Access
- XCode
- Boost Libraries for iOS


Usage
-----------------------------
- Compile
- Copy the results to your iphone 4, I put it in /System/Library/Frameworks/commcenter.dylib
- Copy support/CommCenter.patched in your /System/Library/Frameworks/CoreTelephony.framework/Support/ directory (apply permissions)
- Stop the Comm Center with:
	launchctl unload -w /System/Library/LaunchDaemons/com.apple.CommCenter.plist
- Run Command:
	DYLD_INSERT_LIBRARIES=/System/Library/Frameworks/commcenter.dylib /System/Library/Frameworks/CoreTelephony.framework/Support/CommCenter.patched
- Once baseband is opened it will start a TCP Server



Issues
------------------------------
- baseband doesn't accept normal commands? I assume it's speaking QMI. 
- not stable
- logging needs work
- im new to c++, there is probably a lot of room for code improvements
- only tested on 6.1.3
   

