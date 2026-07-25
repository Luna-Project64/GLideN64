#ifdef OS_WINDOWS
# include <windows.h>
#else
# include "winlnxdefs.h"
#endif // OS_WINDOWS
#include "RSP.h"
#include "PluginAPI.h"
#include "Config.h"
#include "GBI.h"
#include "wst.h"

void Config::resetFbSettings()
{
	frameBufferEmulation.copyDepthToRDRAM = cdSoftwareRender;
	frameBufferEmulation.copyFromRDRAM = 0;
	frameBufferEmulation.copyAuxToRDRAM = 0;
	frameBufferEmulation.copyToRDRAM = ctDoubleBuffer;
	frameBufferEmulation.forceDepthBufferClear = 0;
	frameBufferEmulation.aspect = a43;
	frameBufferEmulation.bufferSwapMode = bsOnVerticalInterrupt;
	frameBufferEmulation.nativeResFactor = 0;
	frameBufferEmulation.fbInfoReadColorChunk = 0;
	frameBufferEmulation.fbInfoReadDepthChunk = 1;
	frameBufferEmulation.fbInfoDisabled = 0;
	frameBufferEmulation.enableOverscan = 0;
}

bool Config::isFbSettingsDefault() const
{
	return frameBufferEmulation.copyDepthToRDRAM == cdSoftwareRender
		&& frameBufferEmulation.copyFromRDRAM == 0
		&& frameBufferEmulation.copyAuxToRDRAM == 0
		&& frameBufferEmulation.copyToRDRAM == ctDoubleBuffer
		&& frameBufferEmulation.forceDepthBufferClear == 0
		&& frameBufferEmulation.aspect == a43
		&& frameBufferEmulation.bufferSwapMode == bsOnVerticalInterrupt
		&& frameBufferEmulation.nativeResFactor == 0
		&& frameBufferEmulation.fbInfoReadColorChunk == 0
		&& frameBufferEmulation.fbInfoReadDepthChunk == 1
		&& frameBufferEmulation.fbInfoDisabled == 0
		&& frameBufferEmulation.enableOverscan == 0;
}

void Config::resetGenericSettings()
{
	generalEmulation.enableLOD = 1;
	generalEmulation.enableNoise = 1;
	generalEmulation.enableHWLighting = 0;
	generalEmulation.enableShadersStorage = 1;
	generalEmulation.enableLegacyBlending = 0;
	generalEmulation.enableFragmentDepthWrite = FragDepthWriteMode::adaptive;
	graphics2D.correctTexrectCoords = tcDisable;
	graphics2D.enableNativeResTexrects = 0;
	graphics2D.bgMode = BGMode::bgStripped;
}

bool Config::isGenericSettingsDefault() const
{
	return generalEmulation.enableLOD == 1
		&& generalEmulation.enableNoise == 1
		&& generalEmulation.enableHWLighting == 0
		&& generalEmulation.enableShadersStorage == 1
		&& generalEmulation.enableLegacyBlending == 0
		&& generalEmulation.enableFragmentDepthWrite == FragDepthWriteMode::adaptive
		&& graphics2D.correctTexrectCoords == tcDisable
		&& graphics2D.enableNativeResTexrects == 0
		&& graphics2D.bgMode == BGMode::bgStripped;
}

void Config::resetToDefaults()
{
	version = CONFIG_VERSION_CURRENT;

#if defined(PANDORA) || defined(VC)
	video.fullscreen = 1;
	video.fullscreenWidth = video.windowedWidth = 800;
#else
	video.fullscreen = 0;
	video.fullscreenWidth = video.windowedWidth = 640;
#endif
	video.fullscreenHeight = video.windowedHeight = 480;
	video.fullscreenRefresh = 60;
	video.fxaa = 0;
	video.multisampling = 0;
	video.verticalSync = 0;
	video.borderless = 0;

	texture.maxAnisotropy = 0;
	texture.bilinearMode = BILINEAR_STANDARD;
	texture.enableHalosRemoval = 0;
	texture.screenShotFormat = 0;

	generalEmulation.enableLOD = 1;
	generalEmulation.enableNoise = 1;
	generalEmulation.enableHWLighting = 0;
	generalEmulation.enableCustomSettings = 1;
	generalEmulation.enableShadersStorage = 1;
	generalEmulation.enableLegacyBlending = 0;
	generalEmulation.hacks = 0;
	generalEmulation.hacksBase = 0;
#if defined(OS_ANDROID) || defined(OS_IOS)
	generalEmulation.enableFragmentDepthWrite = FragDepthWriteMode::adaptive;
	generalEmulation.enableBlitScreenWorkaround = 0;
	generalEmulation.forcePolygonOffset = 0;
	generalEmulation.polygonOffsetFactor = 0.0f;
	generalEmulation.polygonOffsetUnits = 0.0f;
#else
	generalEmulation.enableFragmentDepthWrite = FragDepthWriteMode::adaptive;
#endif

	graphics2D.correctTexrectCoords = tcDisable;
	graphics2D.enableNativeResTexrects = 0;
	graphics2D.bgMode = BGMode::bgStripped;

	frameBufferEmulation.enable = 0;
	frameBufferEmulation.copyDepthToRDRAM = cdSoftwareRender;
	frameBufferEmulation.copyFromRDRAM = 0;
	frameBufferEmulation.copyAuxToRDRAM = 0;
	frameBufferEmulation.copyToRDRAM = ctDoubleBuffer;
	frameBufferEmulation.N64DepthCompare = 0;
	frameBufferEmulation.forceDepthBufferClear = 0;
	frameBufferEmulation.aspect = a43;
	frameBufferEmulation.bufferSwapMode = bsOnVerticalInterrupt;
	frameBufferEmulation.nativeResFactor = 0;
	frameBufferEmulation.fbInfoReadColorChunk = 0;
	frameBufferEmulation.fbInfoReadDepthChunk = 1;
#ifndef MUPENPLUSAPI
	frameBufferEmulation.fbInfoDisabled = 0;
#else
	frameBufferEmulation.fbInfoDisabled = 1;
#endif
	frameBufferEmulation.enableOverscan = 0;
	frameBufferEmulation.instantInput = 0;
	frameBufferEmulation.noForceGL = 0;
	frameBufferEmulation.removeBlackBars = 0;

	textureFilter.txFilterMode = 0;
	textureFilter.txEnhancementMode = 0;
	textureFilter.txDeposterize = 0;
	textureFilter.txFilterIgnoreBG = 0;
	textureFilter.txCacheSize = 100 * gc_uMegabyte;

	textureFilter.txHiresEnable = 0;
	textureFilter.txHiresFullAlphaChannel = 1;
	textureFilter.txHresAltCRC = 0;
	textureFilter.txDump = 0;
	textureFilter.txStrongCRC = 0;

	textureFilter.txForce16bpp = 0;
	textureFilter.txCacheCompression = 1;
	textureFilter.txSaveCache = 1;

	textureFilter.txEnhancedTextureFileStorage = 0;
	textureFilter.txHiresTextureFileStorage = 0;
	textureFilter.txNoTextureFileStorage = 0;

	api().GetUserDataPath(textureFilter.txPath);
	strcat(textureFilter.txPath, "/hires_texture");
	api().GetUserCachePath(textureFilter.txCachePath);
	strcat(textureFilter.txCachePath, "/cache");
	api().GetUserCachePath(textureFilter.txDumpPath);
	strcat(textureFilter.txDumpPath, "/texture_dump");

#ifdef OS_WINDOWS
	font.name.assign("arial.ttf");
#elif defined (OS_ANDROID)
	font.name.assign("DroidSans.ttf");
#elif defined (PANDORA)
	font.name.assign("LiberationMono-Regular.ttf");
#else
	font.name = "FreeSans.ttf";
#endif
	font.size = 18;
	font.color[0] = 0xB5;
	font.color[1] = 0xE6;
	font.color[2] = 0x1D;
	font.color[3] = 0xFF;
	for (int i = 0; i < 4; ++i)
		font.colorf[i] = font.color[i] / 255.0f;

	gammaCorrection.force = 0;
	gammaCorrection.level = 2.0f;

	onScreenDisplay.vis = 0;
	onScreenDisplay.fps = 0;
	onScreenDisplay.percent = 0;
	onScreenDisplay.pos = posBottomLeft;

	debug.dumpMode = 0;

#if notyet
	angle.renderer = arAdaptive;
#else
	angle.renderer = arDirectX11;
#endif
	angle.directComposition = 0;
	angle.shadowDelimiter = 120;
}

bool isHWLightingAllowed()
{
	if (config.generalEmulation.enableHWLighting == 0)
		return false;
	return GBI.isHWLSupported();
}

static bool isWine(void)
{
	HMODULE ntdll = GetModuleHandle(L"ntdll.dll");
	if (!ntdll)
		return false;

	return NULL != GetProcAddress(ntdll, "wine_get_version");
}

void Config::validate()
{
	config.angle.directComposition = false;

	// Only OpenGL is capable of FB emulation
	if (frameBufferEmulation.enable != 0)
	{
		if (!frameBufferEmulation.noForceGL)
		{
			config.angle.renderer = config.arOpenGL;
		}
	}

	bool wine = isWine();
	if (config.angle.renderer == config.arDirectX11 && wine)
	{
		config.angle.renderer = config.arOpenGL;
	}

	if (wine)
	{
		config.video.borderless = true;
	}

	if (frameBufferEmulation.enable != 0 && frameBufferEmulation.N64DepthCompare != 0)
		video.multisampling = 0;
	if (frameBufferEmulation.nativeResFactor == 1) {
		graphics2D.enableNativeResTexrects = 0;
		graphics2D.correctTexrectCoords = tcDisable;
	}
	else {
		if (graphics2D.enableNativeResTexrects != 0)
			graphics2D.correctTexrectCoords = tcDisable;
	}

	if (config.angle.renderer != arOpenGL)
	{
		// Does not work currently
		config.video.fxaa = 0;
	}

	if (!config.frameBufferEmulation.enable)
		config.frameBufferEmulation.N64DepthCompare = 0;

	if (config.angle.shadowDelimiter < 10)
		config.angle.shadowDelimiter = 10;

	if (config.angle.shadowDelimiter > 1000)
		config.angle.shadowDelimiter = 1000;
}
