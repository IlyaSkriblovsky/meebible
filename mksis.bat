set EPOCROOT=C:\QtSDK\Symbian\SDKs\Symbian3Qt474\
set PATH=C:\QtSDK\Symbian\tools\gcce4\bin;C:\QtSDK\Symbian\tools\sbs\bin;C:\QtSDK\Symbian\tools\perl\bin;C:\QtSDK\Symbian\SDKs\Symbian3Qt474\epoc32\gcc\bin;C:\WINDOWS\system32;C:\QtSDK\Symbian\SDKs\Symbian3Qt474\epoc32\tools;C:\QtSDK\Symbian\SDKs\Symbian3Qt474\lib;C:\QtSDK\Symbian\SDKs\Symbian3Qt474\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem
set QTDIR=C:\QtSDK\Symbian\SDKs\Symbian3Qt474\
set QT_GCCE_VERSION=4.4.1
set SBS_GCCE441BIN=C:\QtSDK\Symbian\tools\gcce4\bin
set SBS_HOME=C:\QtSDK\Symbian\tools\sbs

C:\QtSDK\Symbian\SDKs\Symbian3Qt474\epoc32\tools\make release-gcce -w
C:\QtSDK\Symbian\SDKs\Symbian3Qt474\epoc32\tools\make unsigned_sis
C:\QtSDK\Symbian\SDKs\Symbian3Qt474\epoc32\tools\make unsigned_installer_sis
