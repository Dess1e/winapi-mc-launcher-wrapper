#pragma once
#include <string>
#include "defines.h"

using namespace std;

void startInstaller(LPVOID);


class Installer {
private:
	const wstring JAR_DIRECTORY_NAME = JAR_DIR_NAME_DEF;
	const wstring JRE_DIRECTORY_NAME = JRE_DIR_NAME_DEF;
	const wstring JAR_FILENAME = JAR_FNAME_DEF;
    const wstring JRE_URL = JRE_URL_DEF;
    const wstring JAR_URL = JAR_URL_DEF;
	HWND mainWindowHwnd;
	wstring mainDirectory, jarFilePath, jreDirectory, jreBinaryPath, tmpZipPath;

public:
	Installer(HWND hwnd);
	void initializePaths();
	bool checkLauncherExists();
	void initDirectories();
	void startLauncher();
};