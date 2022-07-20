#include <urlmon.h>
#include <string>
#include <ShlObj.h>
#include <Shlwapi.h>

#include "installer.h"
#include "framework.h"
#include "utils.h"

using namespace std;


void startInstaller(LPVOID arg) {
	HWND hwnd = (HWND)arg;
	auto ins = new Installer(hwnd);
}

Installer::Installer(HWND hwnd) {
	mainWindowHwnd = hwnd;
	this->initializePaths();
	if (!this->checkLauncherExists()) {
		this->initDirectories();
		SendMessage(hwnd, WM_INSTALLER_UPDATE, (WPARAM)1, NULL);
		downloadFile(this->JRE_URL, tmpZipPath);
		SendMessage(hwnd, WM_INSTALLER_UPDATE, (WPARAM)2, NULL);
		unzip2Folder(tmpZipPath, mainDirectory);
		SendMessage(hwnd, WM_INSTALLER_UPDATE, (WPARAM)3, NULL);
		downloadFile(this->JAR_URL, jarFilePath);
	}
	startLauncher();
	SendMessage(hwnd, WM_INSTALLER_UPDATE, (WPARAM)4, NULL);
	exit(0);
}


void Installer::initializePaths() {
	wchar_t * tmpAppDataPath;
	wchar_t appDataPath[MAX_PATH] = { 0 };
	wchar_t mainDirectoryPath[MAX_PATH] = { 0 };
	wchar_t jarFilePath[MAX_PATH] = { 0 };
	wchar_t jreDirectory[MAX_PATH] = { 0 };
	wchar_t jreBinaryPath[MAX_PATH] = { 0 };

	wchar_t tmpZipPath[MAX_PATH] = { 0 };

	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &tmpAppDataPath))) {
		StrCpyW(appDataPath, tmpAppDataPath);
		CoTaskMemFree(tmpAppDataPath);
	}
	StrCpyW(mainDirectoryPath, appDataPath);
	PathAppendW(mainDirectoryPath, JAR_DIRECTORY_NAME.c_str());
	
	StrCpyW(jarFilePath, mainDirectoryPath);
	PathAppendW(jarFilePath, JAR_FILENAME.c_str());
	
	StrCpyW(jreDirectory, mainDirectoryPath);
	PathAppendW(jreDirectory, JRE_DIRECTORY_NAME.c_str());

	StrCpyW(jreBinaryPath, jreDirectory);
	PathAppendW(jreBinaryPath, L"bin\\java.exe");

	this->mainDirectory = wstring(mainDirectoryPath);
	this->jarFilePath = wstring(jarFilePath);
	this->jreDirectory = wstring(jreDirectory);
	this->jreBinaryPath = wstring(jreBinaryPath);
	
	GetTempPathW(MAX_PATH, tmpZipPath);
	PathAppendW(tmpZipPath, L"jre.zip");
	
	this->tmpZipPath = wstring(tmpZipPath);
}

bool Installer::checkLauncherExists()
{
	if (!dirExists(this->mainDirectory))
		return false;
	if (!dirExists(this->jreDirectory))
		return false;
	if (!fileExists(this->jarFilePath))
		return false;
	return true;
}

void Installer::initDirectories()
{
	rmDirRecurse(this->mainDirectory);
	CreateDirectoryW(this->mainDirectory.c_str(), 0);
}

void Installer::startLauncher()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	wstring cmd = (jreBinaryPath + L" -jar " + jarFilePath);

	CreateProcessW(NULL, &cmd[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); 
}
