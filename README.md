# cconf - c config manager
a single header library

simply:
```
#define CCONF_IMPLEMENTATION
#include "cconf.h"
```
> `CCONF_IMPLEMENTATION` should only be defined once in a project

init.txt is an example config file

available functions are:
```
cconf_config* CConf_LoadConfigFile(const char *path);
void Cconf_SaveConfigFile(const char *path, cconf_config *config);
void CConf_UnloadConfig(cconf_config *config);

void Cconf_SetValue(cconf_config *config, const char *key, const char*text);
const char* Cconf_GetValue(cconf_config *config, const char *key);
const char* Cconf_GetSetValue(cconf_config *config, const char *key, const char *defaultText);

void Cconf_SetValueInt(cconf_config *config, const char *key, int value);
int Cconf_GetValueInt(cconf_config *config, const char *key);
int Cconf_GetSetValueInt(cconf_config *config, const char *key, int defaultValue);

void Cconf_SetValueFloat(cconf_config *config, const char *key, float value);
float Cconf_GetValueFloat(cconf_config *config, const char *key);
float Cconf_GetSetValueFloat(cconf_config *config, const char *key, float defaultValue);

int Cconf_DeleteValue(cconf_config *config, const char *key);
```
