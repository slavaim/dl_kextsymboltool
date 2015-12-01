# dl_kextsymboltool
A tool for proxy kernel extensions generation

The tool is used to generate proxy drivers for Mac OS X to export kernel symbols/functions that is not available via Apple's provided proxy drivers com.apple.kpi.bsd , com.apple.kpi.dsep , com.apple.kpi.iokit , com.apple.kpi.libkern , com.apple.kpi.mach , com.apple.kpi.private , com.apple.kpi.unsupported (13.4.0) .

For example to get access to _mac_proc_set_enforce , _proc_iterate , _proc_lock , _proc_unlock run the following script. The generated binary file should be places in kext bundle's Contents/MacOS folder, you also need Contents/Info.plist as for any regular kernel extension( for example look Info.plist in any of the Apple's proxy kernel extensions ). This kext bundle should be declared as a dependency in OSBundleLibraries for a kernel extension using the exported functions, if you use kextload/kextutil use -d flag to declare dependency on load.

 #!/bin/bash
     NAME=/work/MyProxyKernelExtension
     nm -gj /mach_kernel > allsymbols
     echo "_mac_proc_set_enforce" > ${NAME}.exports
     echo "_proc_iterate" >> ${NAME}.exports
     echo "_proc_lock" >> ${NAME}.exports
     echo "_proc_unlock" >> ${NAME}.exports
     # include any more symbols needed.
     
     kextsymboltool -arch i386 -import allsymbols  -export ${NAME}.exports -output ${NAME}_32
     # make a universal kext
     kextsymboltool -arch x86_64 -import allsymbols -export ${NAME}.exports -output ${NAME}_64
     lipo -create ${NAME}_32 ${NAME}_64 -output ${NAME}
