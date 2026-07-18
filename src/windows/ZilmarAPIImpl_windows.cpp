#include "GLideN64_Windows.h"
#include "../PluginAPI.h"
#include "../GLideN64.h"
#include "../GLideNUI/GLideNUI.h"
#include "../Config.h"
#include "../Revision.h"
#include <DisplayWindow.h>

void PluginAPI::DllAbout(/*HWND _hParent*/)
{
	Config_LoadConfig();
	char strIniFolderPath[PLUGIN_PATH_SIZE];
	api().FindPluginPath(strIniFolderPath);
	RunAbout(strIniFolderPath);
}

void PluginAPI::CaptureScreen(char * _Directory)
{
	dwnd().setCaptureScreen(_Directory);
}

void PluginAPI::DllConfig(HWND _hParent)
{
	Config_DoConfig(/*_hParent*/);
}

void PluginAPI::LunaSaveConfig(const void* data)
{
	SaveMiniConfig((const MiniConfig*)data);
}

void PluginAPI::LunaLoadConfig(void* data)
{
	LoadMiniConfig((MiniConfig*)data);
}

extern void registerRenderApi(void* api);
void PluginAPI::LunaRegisterRenderWindowApi(void* data)
{
	registerRenderApi(data);
}

void PluginAPI::GetDllInfo(PLUGIN_INFO * PluginInfo)
{
	PluginInfo->Version = 0x103;
	PluginInfo->Type = PLUGIN_TYPE_GFX;
	sprintf(PluginInfo->Name, "ANGLE %s v4.3.34", pluginName);
	PluginInfo->NormalMemory = FALSE;
	PluginInfo->MemoryBswaped = TRUE;
}
