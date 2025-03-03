#ifndef CCONF_H
#define CCONF_H

#define CCONF_MAX_KEY_SIZE 32
#define CCONF_MAX_TEXT_SIZE 128
#define CCONF_MAX_CONFIG_SIZE 64
#define CCONF_MAX_LINE_SIZE 256
#define CCONF_SKIP_EMPTY_LINES false

#define CCONF_VALUE_DELIM "="

typedef struct {
    char key[CCONF_MAX_KEY_SIZE+1];
    char text[CCONF_MAX_TEXT_SIZE+1];
} cconf_value;

typedef struct {
    cconf_value *values;
    int count;
} cconf_config;

extern cconf_config* CConf_LoadConfigFile(const char *path);
extern void Cconf_SaveConfigFile(const char *path, cconf_config *config);
extern void CConf_UnloadConfig(cconf_config *config);

extern void Cconf_SetValue(cconf_config *config, const char *key, const char*text);
extern const char* Cconf_GetValue(cconf_config *config, const char *key);
extern const char* Cconf_GetSetValue(cconf_config *config, const char *key, const char *defaultText);

extern void Cconf_SetValueInt(cconf_config *config, const char *key, int value);
extern int Cconf_GetValueInt(cconf_config *config, const char *key);
extern int Cconf_GetSetValueInt(cconf_config *config, const char *key, int defaultValue);

extern void Cconf_SetValueFloat(cconf_config *config, const char *key, float value);
extern float Cconf_GetValueFloat(cconf_config *config, const char *key);
extern float Cconf_GetSetValueFloat(cconf_config *config, const char *key, float defaultValue);

extern int Cconf_DeleteValue(cconf_config *config, const char *key);

#endif //CCONF_H

#if defined(CCONF_IMPLEMENTATION)

#include <stdio.h>   // FILE, NULL, fopen, fclose, fgets, printf, snprintf
#include <stdlib.h>  // calloc, free, strtof, strtol
#include <string.h>  // memmove, memset, strcmp, strncpy, strpbrk
#include <stdbool.h> // (true, false)

// ****************************
// FILE management functions
// ****************************

cconf_config* CConf_LoadConfigFile(const char *path){
    cconf_config *config = calloc(1, sizeof(cconf_config));
    config->values = calloc(CCONF_MAX_CONFIG_SIZE, sizeof(cconf_value));

    if (path != NULL){
        FILE *file = fopen(path, "r");
        if (file != NULL){
            char buf[CCONF_MAX_LINE_SIZE] = {0};
            for (int i = 0; i < CCONF_MAX_CONFIG_SIZE && fgets(buf, CCONF_MAX_LINE_SIZE, file); i++) {
                if (buf[0] == '\n' && CCONF_SKIP_EMPTY_LINES){
                    continue;
                }

                char *midPtr = strpbrk(buf, CCONF_VALUE_DELIM);
                char *endPtr = strpbrk(buf, "\n");
                if (midPtr == NULL || endPtr == NULL) {
                    printf("Invalid file or buffer overflow\n");
                    CConf_UnloadConfig(config);
                    break;
                }
                int keyLen = midPtr - buf;
                char *text = midPtr+1;
                int textLen = endPtr - text;
                if (keyLen >= CCONF_MAX_KEY_SIZE) {
                    printf("Warning: Line %d, key %d chars too long\n", i, keyLen - CCONF_MAX_KEY_SIZE);
                    continue;
                }
                if (textLen >= CCONF_MAX_TEXT_SIZE) {
                    printf("Warning: Line %d, text %d chars too long\n", i, textLen - CCONF_MAX_TEXT_SIZE);
                    continue;
                }
                strncpy(config->values[config->count].key, buf, keyLen);
                strncpy(config->values[config->count].text, text, textLen);
                config->count++;
            }
            fclose(file);
        }
    }

    return config;
}

void Cconf_SaveConfigFile(const char *path, cconf_config *config){
    FILE *file = fopen(path, "w");
    if (file != NULL){
        for (int i = 0; i < config->count; i++) {
            fprintf(file, "%s%s%s\n", config->values[i].key, CCONF_VALUE_DELIM, config->values[i].text);
        }
        fclose(file);
    }
}

void CConf_UnloadConfig(cconf_config *config){
    free(config->values);
    free(config);
}

// ****************************
// TEXT functions
// ****************************

void Cconf_SetValue(cconf_config *config, const char *key, const char*text){
    for (int i = 0; i < config->count; i++) {
        if (strcmp(config->values[i].key, key) == 0){
            memset(config->values[i].text, 0, CCONF_MAX_TEXT_SIZE);
            strncpy(config->values[i].text, text, CCONF_MAX_TEXT_SIZE);
            return;
        }
    }
    strncpy(config->values[config->count].key, key, CCONF_MAX_KEY_SIZE);
    strncpy(config->values[config->count].text, text, CCONF_MAX_TEXT_SIZE);
    config->count++;
}

// NOTE: returns NULL on not found
// WARNING: expires on next use (including internal use)
// WARNING: not multithreading safe
const char* Cconf_GetValue(cconf_config *config, const char *key){
    static char buffer[CCONF_MAX_TEXT_SIZE+1] = "";
    memset(buffer, 0, CCONF_MAX_TEXT_SIZE);

    for (int i = 0; i < config->count; i++) {
        if (strcmp(config->values[i].key, key) == 0){
            strncpy(buffer, config->values[i].text, CCONF_MAX_TEXT_SIZE);
            return buffer;
        }
    }
    return NULL;
}

// NOTE: returns calls Cconf_setValue and defualtText on not found
// WARNING: expires on next use (including internal use)
// WARNING: not multithreading safe
const char* Cconf_GetSetValue(cconf_config *config, const char *key, const char *defaultText){
    const char *buf = Cconf_GetValue(config, key);
    if (buf == NULL) {
        Cconf_SetValue(config, key, defaultText);
        return defaultText;
    }
    return buf;
}

// ****************************
// INT functions
// ****************************

void Cconf_SetValueInt(cconf_config *config, const char *key, int value){
    char buf[CCONF_MAX_TEXT_SIZE+1];
    snprintf(buf, CCONF_MAX_TEXT_SIZE, "%d", value);
    Cconf_SetValue(config, key, buf);
}

// WARNING: returns 0 if invalid or not found
int Cconf_GetValueInt(cconf_config *config, const char *key){
    return (int)strtol(Cconf_GetValue(config, key), NULL, 0);
}

int Cconf_GetSetValueInt(cconf_config *config, const char *key, int defaultValue){
    const char *buf = Cconf_GetValue(config, key);
    if (buf == NULL) {
        Cconf_SetValueInt(config, key, defaultValue);
        return defaultValue;
    }
    return (int)strtol(buf, NULL, 0);
}

// ****************************
// FLOAT functions
// ****************************

void Cconf_SetValueFloat(cconf_config *config, const char *key, float value){
    char buf[CCONF_MAX_TEXT_SIZE+1];
    snprintf(buf, CCONF_MAX_TEXT_SIZE, "%g", value);
    Cconf_SetValue(config, key, buf);
}

// WARNING: returns 0.0 if invalid or not found
float Cconf_GetValueFloat(cconf_config *config, const char *key){
    return strtof(Cconf_GetValue(config, key), NULL);
}

float Cconf_GetSetValueFloat(cconf_config *config, const char *key, float defaultValue){
    const char *buf = Cconf_GetValue(config, key);
    if (buf == NULL) {
        Cconf_SetValueFloat(config, key, defaultValue);
        return defaultValue;
    }
    return strtof(buf, NULL);
}

// ****************************
// General functions
// ****************************

// NOTE: returns 1 on not found
int Cconf_DeleteValue(cconf_config *config, const char *key){
    for (int i = 0; i < config->count; i++) {
        if (strcmp(config->values[i].key, key) == 0){
            memmove(&config->values[i], &config->values[i+1], (config->count - i) * sizeof(cconf_value));
            config->count--;
            return 0;
        }
    }
    return 1;
}

#endif
