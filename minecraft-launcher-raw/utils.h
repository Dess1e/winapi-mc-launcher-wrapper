#pragma once
#include <Windows.h>
#include <string>

using namespace std;

bool dirExists(const wstring&);
bool fileExists(const wstring&);
void downloadFile(const wstring&, const wstring&);
void rmDirRecurse(const wstring& dir);
bool unzip2Folder(const wstring& zipFile, const wstring& folder);
