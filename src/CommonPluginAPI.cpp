#ifdef OS_WINDOWS
# include <windows.h>
#else
# include "winlnxdefs.h"
#endif // OS_WINDOWS

#include "PluginAPI.h"

extern "C" {

EXPORT BOOL CALL InitiateGFX (GFX_INFO Gfx_Info)
{
	return api().InitiateGFX(Gfx_Info);
}

EXPORT void CALL MoveScreen (int xpos, int ypos)
{
	api().MoveScreen(xpos, ypos);
}

EXPORT void CALL ProcessDList(void)
{
	api().ProcessDList();
}

EXPORT void CALL ProcessRDPList(void)
{
	api().ProcessRDPList();
}

EXPORT void CALL RomClosed (void)
{
	api().RomClosed();
}

EXPORT void CALL ShowCFB (void)
{
	api().ShowCFB();
}

EXPORT void CALL UpdateScreen (void)
{
	api().UpdateScreen();
}

EXPORT void CALL ViStatusChanged (void)
{
	api().ViStatusChanged();
}

EXPORT void CALL ViWidthChanged (void)
{
	api().ViWidthChanged();
}

EXPORT void CALL ChangeWindow(void)
{
	api().ChangeWindow();
}

EXPORT void CALL FBWrite(unsigned int addr, unsigned int size)
{
	api().FBWrite(addr, size);
}

EXPORT void CALL FBRead(unsigned int addr)
{
	api().FBRead(addr);
}

EXPORT void CALL FBGetFrameBufferInfo(void *pinfo)
{
	api().FBGetFrameBufferInfo(pinfo);
}

#ifndef MUPENPLUSAPI
EXPORT void CALL FBWList(FrameBufferModifyEntry *plist, unsigned int size)
{
	api().FBWList(plist, size);
}
#endif

char gPluginConfigDir[MAX_PATH]{};
namespace Zilmar
{
	short Set_RdramSize = 0;
    enum SettingLocation
    {
        SettingType_ConstString = 0,
        SettingType_ConstValue = 1,
        SettingType_CfgFile = 2,
        SettingType_Registry = 3,
        SettingType_RelativePath = 4,
        TemporarySetting = 5,
        SettingType_RomDatabase = 6,
        SettingType_CheatSetting = 7,
        SettingType_GameSetting = 8,
        SettingType_BoolVariable = 9,
        SettingType_NumberVariable = 10,
        SettingType_StringVariable = 11,
        SettingType_SelectedDirectory = 12,
        SettingType_RdbSetting = 13,
    };

    enum SettingDataType
    {
        Data_DWORD = 0, Data_String = 1, Data_CPUTYPE = 2, Data_SelfMod = 3, Data_OnOff = 4, Data_YesNo = 5, Data_SaveChip = 6
    };

    typedef struct
    {
        uint32_t dwSize;
        int DefaultStartRange;
        int SettingStartRange;
        int MaximumSettings;
        int NoDefault;
        int DefaultLocation;
        void* handle;

        unsigned int(CALL* GetSetting)      (void* handle, int ID);
        const char* (CALL* GetSettingSz)    (void* handle, int ID, char* Buffer, int BufferLen);
        void(CALL* SetSetting)      (void* handle, int ID, unsigned int Value);
        void(CALL* SetSettingSz)    (void* handle, int ID, const char* Value);
        void(CALL* RegisterSetting) (void* handle, int ID, int DefaultID, SettingDataType Type,
            SettingLocation Location, const char* Category, const char* DefaultStr, uint32_t Value);
        void(CALL* UseUnregisteredSetting) (int ID);
    } PLUGIN_SETTINGS;

    typedef struct
    {
        unsigned int(CALL* FindSystemSettingId) (void* handle, const char* Name);
    } PLUGIN_SETTINGS2;

    static PLUGIN_SETTINGS  g_PluginSettings;
    static PLUGIN_SETTINGS2 g_PluginSettings2;
    static inline unsigned int GetSystemSetting(short SettingID)
    {
        return g_PluginSettings.GetSetting(g_PluginSettings.handle, SettingID);
    }

    static inline short FindSystemSettingId(const char* Name)
    {
        if (g_PluginSettings2.FindSystemSettingId && g_PluginSettings.handle)
        {
            return (short)g_PluginSettings2.FindSystemSettingId(g_PluginSettings.handle, Name);
        }
        return 0;
    }
}

extern "C" EXPORT void CALL SetSettingInfo(Zilmar::PLUGIN_SETTINGS* info)
{
    Zilmar::g_PluginSettings = *info;
}

extern "C" EXPORT void CALL SetSettingInfo2(Zilmar::PLUGIN_SETTINGS2* info)
{
    Zilmar::g_PluginSettings2 = *info;
}

extern "C" EXPORT void CALL PluginLoaded(void)
{
    int pluginConfigDir = Zilmar::FindSystemSettingId("Config Base Dir");
    Zilmar::Set_RdramSize = Zilmar::FindSystemSettingId("RDRamSize");

    if (pluginConfigDir)
    {
        const char* cfg = Zilmar::g_PluginSettings.GetSettingSz(Zilmar::g_PluginSettings.handle, pluginConfigDir, gPluginConfigDir, sizeof(gPluginConfigDir));
        if (!cfg)
            *gPluginConfigDir = '\0';
    }
}
}

uint32_t QueryRdramSize()
{
    if (!Zilmar::Set_RdramSize)
        return 0;

    return Zilmar::GetSystemSetting(Zilmar::Set_RdramSize) - 1;
}
