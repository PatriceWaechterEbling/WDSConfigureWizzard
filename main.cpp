#include "pch.h"
/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author Patrice Waechter-Ebling
 * \date   March 2023
 *********************************************************************/
/* config WDSCapture
* 
[Capture] 
Unattended=No
VolumeToCapture=
SystemRoot=
ImageName=
ImageDescription=
DestinationFile=
Overwrite=No

[ExclusionList]
$ntfs.log
hiberfil.sys
pagefile.sys
"System Volume Information"
RECYCLER
winpepge.sys
%SYSTEMROOT%\CSC

[WDS]


*/

using namespace winrt;
using namespace Windows::Foundation;
using namespace std;

struct configuration {
    struct Capture {
        bool Unattended = true;
        char VolumeToCapture[260];
        char SystemRoot[5];
        char ImageName[80];
        char ImageDescription[80];
        char DestinationFile[260];
        bool Overwrite = false;
    }capture;
    struct WDS {
        bool UploadToWDSServer = false;
        char WDSServerName[100];
        char WDSImageGroup[64];
        char Username[64];
        char Password[64];
        bool DeleteLocalWimOnSuccess = false;
    }wds;
}*config;

TCHAR volumeName[MAX_PATH + 1];
TCHAR fileSysName[MAX_PATH + 1];
TCHAR driveType[MAX_PATH];
TCHAR driveLetter[16];
unsigned __int64 i64NumberOfBytesUsed;
INT mnu = 1;
UINT iDrvType = 0;
HWND hWnd;
HMODULE hMainMod;

void EnumerationLecteurs();
DWORD GetVolumeInfo(LPCTSTR pDriveLetter, LPTSTR pDriveInfoBuffer, DWORD nDriveInfoSize);
VOID GetDiskSpaces(LPCTSTR pDriveLetter, LPTSTR pSpaceInfoBuffer, DWORD nSpaceInfoBufferSize);

int main()
{
    init_apartment();
    hWnd = GetConsoleWindow();
    InitCommonControls();
    hMainMod = GetModuleHandle(0);
    HICON ico = LoadIcon(hMainMod, (LPCSTR)101);
    typedef BOOL(WINAPI* PSetConsoleIcon)(HICON);
    static PSetConsoleIcon pSetConsoleIcon = NULL;
    if (pSetConsoleIcon == NULL)	pSetConsoleIcon = (PSetConsoleIcon)GetProcAddress(GetModuleHandle("kernel32"), "SetConsoleIcon");
    pSetConsoleIcon(ico);
    SetConsoleTitle(titre);
    RECT rc;
    GetClientRect(hWnd, &rc);
    DrawIcon(GetDC(hWnd), rc.right - 33, 0, LoadIcon(hMainMod, (LPCSTR)0x66));
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),  FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("Bienvenue a l'%s\tPatrice Waechter-Ebling 2023\tversion: 1.0\n", titre);
    EnumerationLecteurs();
    cout << (mnu-1) << " entrees ont ete ajoutees au menu \n\nSelectionnez le lecteur a capturer: ";
    char x[3];
    cin.getline(x, 1);
    cout << x;
    MessageBox(hWnd, &x[0], titre, 0);
 
}
DWORD GetVolumeInfo(LPCTSTR pDriveLetter, LPTSTR pDriveInfoBuffer, DWORD nDriveInfoSize) {
    DWORD serialNumber = 0;
    DWORD maxCompLength = 0;
    DWORD fileSysFlags = 0;
    DWORD lastError = 0;
    BOOL bGetVolInf = FALSE;
    typedef enum tagGetVolumeInfoResult { RESULTS_SUCCESS = 0, RESULTS_GETVOLUMEINFORMATION_FAILED = 1 };
    size_t size = sizeof(driveType) / sizeof(TCHAR);
    bGetVolInf = GetVolumeInformation(pDriveLetter, volumeName, sizeof(volumeName) / sizeof(TCHAR), &serialNumber, &maxCompLength, &fileSysFlags, fileSysName, sizeof(fileSysName) / sizeof(TCHAR));
    if (bGetVolInf == 0) {
        lastError = GetLastError();
        _stprintf_s(pDriveInfoBuffer, nDriveInfoSize, TEXT(" ** Erreur %d lors de la requete d'information sur le lecteur %.1s **"), lastError, pDriveLetter);
        return RESULTS_GETVOLUMEINFORMATION_FAILED;
    }
    iDrvType = GetDriveType(pDriveLetter);
    if (_tcslen(volumeName) == 0) { _stprintf_s(volumeName, sizeof(volumeName) / sizeof(TCHAR), TEXT("%s"), TEXT("Nom indefini")); }
    if (iDrvType == DRIVE_RAMDISK) { _stprintf_s(driveType, size, TEXT("%s"), TEXT("RAM-Disk")); }
    if (iDrvType == DRIVE_CDROM) { _stprintf_s(driveType, size, TEXT("%s"), TEXT("CD-ROM")); return 0; }
    if (iDrvType == DRIVE_FIXED || iDrvType == DRIVE_REMOTE) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C);
        std::cout << " " << volumeName;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0B);
        _stprintf_s(driveType, size, TEXT("%s"), TEXT("disque dur"));
        std::cout << " " << driveType;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
        std::cout << " " << fileSysName;
    }
    return RESULTS_SUCCESS;
}
VOID GetDiskSpaces(LPCTSTR pDriveLetter, LPTSTR pSpaceInfoBuffer, DWORD nSpaceInfoBufferSize) {
    unsigned __int64 i64TotalNumberOfBytes, i64TotalNumberOfFreeBytes, i64FreeBytesAvailableToCaller;
    BOOL bGetDiskFreeSpaceEx = FALSE;
    bGetDiskFreeSpaceEx = GetDiskFreeSpaceEx(pDriveLetter, (PULARGE_INTEGER)&i64FreeBytesAvailableToCaller, (PULARGE_INTEGER)&i64TotalNumberOfBytes, (PULARGE_INTEGER)&i64TotalNumberOfFreeBytes);
    if (bGetDiskFreeSpaceEx == TRUE) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY );
        _stprintf_s(pSpaceInfoBuffer, nSpaceInfoBufferSize, TEXT(" Disponibilite: %I64u/%I64uGo"), i64TotalNumberOfFreeBytes / (1024 * 1024 * 1024), i64TotalNumberOfBytes / (1024 * 1024 * 1024));
        i64NumberOfBytesUsed = (i64TotalNumberOfBytes - i64TotalNumberOfFreeBytes) / (static_cast<unsigned long long>(1024 * 1024) * 1024);
    }
}
void EnumerationLecteurs(){
    INT i = 0;
    TCHAR driveInfo[MAX_PATH + 1];
    TCHAR spaceInfo[MAX_PATH + 1];
    DWORD dwGetVolInf = 0;
    DWORD drives = GetLogicalDrives();
    CHAR windir[100];
    GetWindowsDirectory(windir, 100);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x9F);
    std::cout << "Analyse des lecteurs possibles\n";
    for (i = 0; i < 26; i++) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
        if ((drives & (1 << i)) != 0) {
            _stprintf_s(driveLetter, sizeof(driveLetter) / sizeof(TCHAR), TEXT("%c:\\"), TEXT('A') + i);
            if (driveLetter[0] == windir[0]) {SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x8E);}
            if (GetDriveType(driveLetter) == DRIVE_FIXED) {
                std::cout << mnu << ": " << driveLetter;
                GetDiskSpaces(driveLetter, spaceInfo, MAX_PATH + 1);
                dwGetVolInf = GetVolumeInfo(driveLetter, driveInfo, MAX_PATH + 1);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0E);
                std::cout << " " << spaceInfo << std::endl;
                mnu++;
            }
        }
    }
}
