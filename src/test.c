#include <stdio.h>
#define CCONF_IMPLEMENTATION
#include "cconf.h"

int main(void){
    cconf_config *config = CConf_LoadConfigFile("init.txt");
    Cconf_DeleteValue(config, "b");

    printf("test %s\n", Cconf_GetValue(config, "b"));
    Cconf_SaveConfigFile("init.txt", config);
    CConf_UnloadConfig(config);
    return 0;
}
