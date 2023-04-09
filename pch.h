#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN	
#define titre "Assistant de Configuration pour WDSCapture"
#define WINVER  0x0600 //requiere  windows ver 6.xx et +

#include <windows.h> //entetes requises par l'application
#include <windowsx.h>
#include <wingdi.h>
#include <commctrl.h>
#include <Winuser.h>
#include <Commdlg.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <stdlib.h>
#include <strsafe.h>
#include <ShellAPI.h>
#include <io.h>	
#include <Shobjidl.h>
#include <Objbase.h>
#include <tchar.h>

#pragma comment (lib,"comctl32.lib") 
#pragma comment (lib,"kernel32.lib") 
#pragma comment (lib,"user32.lib") 
#pragma comment (lib,"gdi32.lib") 
#pragma comment (lib,"winspool.lib") 
#pragma comment (lib,"comdlg32.lib") 
#pragma comment (lib,"advapi32.lib") 
#pragma comment (lib,"shell32.lib") 
#pragma comment (lib,"ole32.lib") 
#pragma comment (lib,"oleaut32.lib") 
#pragma comment (lib,"uuid.lib") 
#pragma comment (lib,"odbc32.lib") 
#pragma comment (lib,"odbccp32.lib") 
#pragma comment (lib,"shlwapi.lib") 
