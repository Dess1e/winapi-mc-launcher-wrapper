#include "utils.h"
#include <ShlObj.h>
#include <Shlwapi.h>


bool dirExists(const std::wstring& dirName_in)
{
	DWORD ftyp = GetFileAttributesW(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}

bool fileExists(const std::wstring& szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath.c_str());

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void rmDirRecurse(const wstring& dir)
{
	size_t len = wcslen(dir.c_str()) + 2;
	wchar_t * tempdir = (wchar_t *) malloc(len * sizeof(wchar_t));
	if (!tempdir)
		return;
	memset(tempdir, 0, len);
	wcscpy_s(tempdir, len, dir.c_str());

	SHFILEOPSTRUCT file_op = {
		NULL,
		FO_DELETE,
		tempdir,
		NULL,
		FOF_NOCONFIRMATION |
		FOF_NOERRORUI |
		FOF_SILENT,
		false,
		0,
		L""
	};
	int ret = SHFileOperationW(&file_op);
	free(tempdir);
}

void downloadFile(const wstring& url, const wstring& dest) {
	URLDownloadToFileW(0, url.c_str(), dest.c_str(), 0, 0);
}

bool unzip2Folder(const wstring& zipFile, const wstring& folder)
{
	IShellDispatch *pISD;

	Folder  *pZippedFile = 0L;
	Folder  *pDestination = 0L;

	long FilesCount = 0;
	IDispatch* pItem = 0L;
	FolderItems *pFilesInside = 0L;
	VARIANT Options, OutFolder, InZipFile, Item;
	CoInitialize( NULL);
	__try{
		if (CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD) != S_OK)
			return 1;

		InZipFile.vt = VT_BSTR;
		InZipFile.bstrVal = SysAllocStringLen(zipFile.data(), zipFile.size());
		pISD->NameSpace( InZipFile, &pZippedFile);
		if (!pZippedFile)
		{
			pISD->Release();
			return 1;
		}

		OutFolder.vt = VT_BSTR;
		OutFolder.bstrVal = SysAllocStringLen(folder.data(), folder.size());
		pISD->NameSpace( OutFolder, &pDestination);
		if(!pDestination)
		{
			pZippedFile->Release();
			pISD->Release();
			return 1;
		}
    
		pZippedFile->Items(&pFilesInside);
		if(!pFilesInside)
		{
			pDestination->Release();
			pZippedFile->Release();
			pISD->Release();
			return 1;
		}
	    
		pFilesInside->get_Count( &FilesCount);
		if( FilesCount < 1)
		{
			pFilesInside->Release();
			pDestination->Release();
			pZippedFile->Release();
			pISD->Release();
			return 0;
		}

		pFilesInside->QueryInterface(IID_IDispatch,(void**)&pItem);

		Item.vt = VT_DISPATCH;
		Item.pdispVal = pItem;

		Options.vt = VT_I4;
		Options.lVal = 1024 | 512 | 16 | 4;

		bool retval = pDestination->CopyHere( Item, Options) == S_OK;

		pItem->Release();pItem = 0L;
		pFilesInside->Release();pFilesInside = 0L;
		pDestination->Release();pDestination = 0L;
		pZippedFile->Release();pZippedFile = 0L;
		pISD->Release();pISD = 0L;

		return retval;

	}__finally    
	{
		CoUninitialize();
	}
}

