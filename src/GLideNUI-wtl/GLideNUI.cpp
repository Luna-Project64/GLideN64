#include "About.h"

#include <thread>

#include "GLideNUI.h"
#include "Settings.h"
#include "ConfigDlg.h"
#include "UIConfig.h"

#ifdef QT_STATICPLUGIN
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QICOPlugin)
#endif

//#define RUN_DIALOG_IN_THREAD

static
int openConfigDialog(const char* _strFileName, const char * _romName, unsigned int _maxMSAALevel, unsigned int _maxAnisotropy, bool & _accepted)
{
	loadSettings(config, _strFileName);
	if (config.generalEmulation.enableCustomSettings != 0 && _romName != nullptr && strlen(_romName) != 0)
		loadCustomRomSettings(config, _strFileName, _romName);

	LoadCurrentStrings(_strFileName, config.translationFile);

	CConfigDlg Dlg;
	Dlg.setIniPath(_strFileName);
	Dlg.setRomName(_romName);
	Dlg.setMSAALevel(_maxMSAALevel);
	Dlg.setMaxAnisotropy(_maxAnisotropy);
	auto err = Dlg.DoModal();
	auto errVal = GetLastError();
	_accepted = Dlg.Saved();
	return 0;
}

static bool runConfigThread(const char* _strFileName, const char * _romName, unsigned int _maxMSAALevel, unsigned int _maxAnisotropy)
{
	bool accepted = false;
#ifdef RUN_DIALOG_IN_THREAD
	std::thread configThread(openConfigDialog, _strFileName, _maxMSAALevel, std::ref(accepted));
	configThread.join();
#else
	openConfigDialog(_strFileName, _romName, _maxMSAALevel, _maxAnisotropy, accepted);
#endif
	return accepted;

}

bool RunConfig(const char* _strFileName, const char * _romName, unsigned int _maxMSAALevel, unsigned int _maxAnisotropy)
{
	return runConfigThread(_strFileName, _romName, _maxMSAALevel, _maxAnisotropy);
}

int RunAbout(const char * _strFileName)
{
	LoadCurrentStrings(_strFileName, config.translationFile);

	CAboutDlg Dlg;
	Dlg.DoModal();
	return 0;
}

void LoadConfig(Config* cfg, const char* _strFileName)
{
	loadSettings(*cfg, _strFileName);
}

void SaveConfig(const Config* cfg, const char* _strFileName)
{
	// TODO: Linker weirdness - why this cast is a must have?
	writeSettings(*(Config*) cfg, _strFileName);
}

void LoadCustomRomSettings(Config* cfg, const char* _strFileName, const char * _romName)
{
	loadCustomRomSettings(*cfg, _strFileName, _romName);
}
