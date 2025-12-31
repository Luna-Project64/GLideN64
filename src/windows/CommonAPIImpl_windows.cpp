#include <algorithm>
#include <string>
#include "GLideN64_Windows.h"
#include "../RSP.h"

#include "../GLideNUI-wtl/resource.h"
#include "../PluginAPI.h"

#include <fcntl.h>
#include <io.h>
#include <commctrl.h>
#include <Shlobj.h>
#include <Shlobj_core.h>
#include <shlwapi.h>

#ifdef OS_WINDOWS
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#endif

BOOL CALLBACK FindToolBarProc( HWND _hWnd, LPARAM lParam )
{
	if (GetWindowLong( _hWnd, GWL_STYLE ) & RBS_VARHEIGHT) {
		hToolBar = _hWnd;
		return FALSE;
	}
	return TRUE;
}

int PluginAPI::InitiateGFX(const GFX_INFO & _gfxInfo)
{
	_initiateGFX(_gfxInfo);

	hWnd = _gfxInfo.hWnd;
	hStatusBar = _gfxInfo.hStatusBar;
	hToolBar = NULL;

	EnumChildWindows( hWnd, FindToolBarProc, 0 );
	return TRUE;
}

extern "C" char gPluginConfigDir[MAX_PATH];
void PluginAPI::FindPluginPath(char * _strPath)
{
	if (_strPath == NULL)
		return;

	if (*gPluginConfigDir)
	{
		strcpy_s(_strPath, MAX_PATH, gPluginConfigDir);
	}
	else
	{
		SHGetFolderPathA(NULL,
						 CSIDL_APPDATA,
						 NULL,
						 0,
						 _strPath);
	}

	{
		PathAppendA(_strPath, "GLideN64");
		CreateDirectoryA(_strPath, nullptr); // can fail, ignore errors
		size_t length = strlen(_strPath);

		PathAppendA(_strPath, "GLideN64.custom.ini");
		int fd = _open(_strPath, _O_BINARY | _O_WRONLY | _O_CREAT | _O_EXCL, 0666);
		if (-1 != fd)
		{
			auto rc = FindResource(hInstance, MAKEINTRESOURCE(IDR_RCDATA_CUSTOM_DEFAULT), RT_RCDATA);
			auto res = LoadResource(hInstance, rc);
			void* data = LockResource(res);
			size_t size = SizeofResource(hInstance, rc);
			_write(fd, data, size);
			_close(fd);
		}

		char* namePos = strstr(_strPath + length, "GLideN64");
		strcpy(namePos, "GLideN64.ini");
		fd = _open(_strPath, _O_BINARY | _O_WRONLY | _O_CREAT | _O_EXCL, 0666);
		if (-1 != fd)
		{
			auto rc = FindResource(hInstance, MAKEINTRESOURCE(IDR_RCDATA_DEFAULT), RT_RCDATA);
			auto res = LoadResource(hInstance, rc);
			void* data = LockResource(res);
			size_t size = SizeofResource(hInstance, rc);
			_write(fd, data, size);
			_close(fd);
		}
	}

	std::string pluginPath(_strPath);
	std::replace(pluginPath.begin(), pluginPath.end(), '\\', '/');
	std::string::size_type pos = pluginPath.find_last_of("/");
	strcpy(_strPath, pluginPath.substr(0, pos).c_str());
}

void PluginAPI::GetUserDataPath(char * _strPath)
{
	FindPluginPath(_strPath);
}

void PluginAPI::GetUserCachePath(char* _strPath)
{
	FindPluginPath(_strPath);
}
