#ifndef GLIDENUII_H
#define GLIDENUII_H

class Config;

bool RunConfig(const char * _strFileName, const char * _romName, unsigned int _maxMSAALevel, unsigned int _maxAnisotropy);
int RunAbout(const char * _strFileName);
void LoadConfig(Config* cfg, const char * _strFileName);
void LoadCustomRomSettings(Config* cfg, const char * _strFileName, const char * _romName);

#endif // GLIDENUII_H
