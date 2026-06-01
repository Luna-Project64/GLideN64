#include "GLideN64_Windows.h"
#include "../Config.h"
#include "../N64.h"
#include "../Config.h"
#include "../RSP.h"
#include "../PluginAPI.h"
#include "../GLideNUI/GLideNUI.h"
#include <DisplayWindow.h>


Config config;

void Config_DoConfig(/*HWND hParent*/)
{
	char strIniFolderPath[PLUGIN_PATH_SIZE];
	api().FindPluginPath(strIniFolderPath);

	const bool bRestart = RunConfig(strIniFolderPath, api().isRomOpen() ? RSP.romname : nullptr, 8, 16);
	if (bRestart)
	{
		PluginAPI::get().Restart();
	}
}

void Config_LoadConfig()
{
	char strIniFolderPath[PLUGIN_PATH_SIZE];
	api().FindPluginPath(strIniFolderPath);
	LoadConfig(&config, strIniFolderPath);
	if (config.generalEmulation.enableCustomSettings != 0)
		LoadCustomRomSettings(&config, strIniFolderPath, RSP.romname);
	config.validate();

	dwnd().reset();
}

void LoadMiniConfig(MiniConfig* miniConfig)
{
	char strIniFolderPath[PLUGIN_PATH_SIZE];
	api().FindPluginPath(strIniFolderPath);

	LoadConfig(&config, strIniFolderPath);
	miniConfig->fb = config.frameBufferEmulation.enable != 0;
	miniConfig->fbDepthCompare = config.frameBufferEmulation.N64DepthCompare != 0;
	miniConfig->fbDefault = config.isFbSettingsDefault();
	miniConfig->emuDefault = config.isGenericSettingsDefault();
	miniConfig->removeBlackBars = config.frameBufferEmulation.removeBlackBars != 0;
	miniConfig->reduceInputDelay = config.frameBufferEmulation.instantInput != 0;
	miniConfig->enableZeldaHacks = config.generalEmulation.hacksBase == (hack_ZeldaMonochrome | hack_ZeldaMM);
}

void SaveMiniConfig(const MiniConfig* miniConfig)
{
	char strIniFolderPath[PLUGIN_PATH_SIZE];
	api().FindPluginPath(strIniFolderPath);

	LoadConfig(&config, strIniFolderPath);

	config.frameBufferEmulation.enable = miniConfig->fb ? 1 : 0;
	config.frameBufferEmulation.N64DepthCompare = miniConfig->fbDepthCompare ? 1 : 0;
	if (miniConfig->fbDefault)
		config.resetFbSettings();
	if (miniConfig->emuDefault)
		config.resetGenericSettings();

	config.frameBufferEmulation.removeBlackBars = miniConfig->removeBlackBars ? 1 : 0;
	config.frameBufferEmulation.instantInput = miniConfig->reduceInputDelay ? 1 : 0;
	if (miniConfig->enableZeldaHacks)
		config.generalEmulation.hacksBase = (hack_ZeldaMonochrome | hack_ZeldaMM);
	else
		config.generalEmulation.hacksBase = 0;

	config.validate();

	SaveConfig(&config, strIniFolderPath);

	// Assuming not needed because it is a private API called with rom not being running.
	// PluginAPI::get().Restart();
}
