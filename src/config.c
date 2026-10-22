// todo: переделать в нормальный вид
#include "../include/config.h"
#include "../include/cJSON.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//todo: переделать логи ошибок
char* read_config_to_string(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("не открылся конфиг");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long lenght = ftell(file);
    fseek(file, 0, SEEK_SET);

    //memory: buffer (alloc)
    char* buffer = (char*)malloc(lenght+1);
    if (!buffer) {
        printf("не выдалась память");
        return NULL;
    }

    size_t read_bytes = fread(buffer, 1, lenght, file);
    buffer[read_bytes] = '\0';
    fclose(file);

    return buffer;
}

char* cJSON_to_String(cJSON* value) {
    if (cJSON_IsString(value) && (value->valuestring != NULL)) {
        //memory: value_string (alloc)
        char* value_string = strdup(value->valuestring);
        return value_string;
    } else {
        printf("Не сделали строку");
        return NULL;
    }
}

int* cJSON_to_Int(cJSON* value) {
    if (cJSON_IsNumber(value)) {
        //memory: value_int (alloc)
        int* value_int = (int*)calloc(1, sizeof(int));
        *value_int = value->valueint;
        return value_int;
    } else {
        printf("не сделали число");
        return NULL;
    }
}

int is_uniq(Config** config_array, int size, char* name) {
    for (int i = 0; i < size; i++) {
        if (strcmp((config_array[i])->name, name) == 0) {
            return NULL;
        }
    }

    return 0;
}

int clear_config_array(Config** config_array) {
    int i = 0;
    while (config_array[i] != NULL) {
        //memory: value_string/value_int (free)
        free((config_array[i])->name);
        free((config_array[i])->interface);
        free((config_array[i])->destination_mac);
        free((config_array[i])->destination_ip);
        free((config_array[i])->source_mac);
        free((config_array[i])->source_ip);
        free((config_array[i])->timeout);
        //memory: dev_conf (free)
        free(config_array[i]);
    }
    //memory: config_array (free)
    free(config_array);
    return 0;
}

Config** read_my_config() {
    char* json_raw = read_config_to_string("config.json");
    if (!json_raw) {
        return NULL;
    }

    //todo: убедиться, что очистили выделенную память
    //memory: root_array (alloc)
    cJSON* root_array = cJSON_Parse(json_raw);

    //memory: buffer (free)
    free(json_raw);

    if (!root_array) {
        printf("не получилось спарсить");
        return NULL;
    }

    if (!cJSON_IsArray(root_array)) {
        printf("не является объектом");
        cJSON_Delete(root_array);
        return NULL;
    }

    int size = cJSON_GetArraySize(root_array);
    //memory: config_array (alloc)
    Config** config_array = (Config**)calloc(size, sizeof(Config*));

    for (int i = 0; i < size; i++) {
        cJSON* config_item = cJSON_GetArrayItem(root_array, i);

        if (cJSON_IsObject(config_item)) {
            //memory: dev_conf (alloc)
            Config* dev_conf = (Config*)calloc(1, sizeof(Config));
            if (!dev_conf) {
                //todo: сделать логику очистки полной в случае падения скрипта
                return NULL;
            }

            config_array[i] = dev_conf;

            cJSON* name = cJSON_GetObjectItemCaseSensitive(config_item, "name");
            char* name_string = cJSON_to_String(name);
            if(!name_string) {
                printf("Не получили знач");
                
                cJSON_Delete(root_array);
                clear_config_array(config_array);
                return NULL;
            }
            if (!is_uniq(config_array, i, name_string)) {
                
                printf("Конфиг с таким названием уже есть %s", name_string);
                cJSON_Delete(root_array);
                clear_config_array(config_array);
                return NULL;
            }
            dev_conf->name = name_string;

            cJSON* interface = cJSON_GetObjectItemCaseSensitive(config_item, "interface");
            char* interface_string = cJSON_to_String(interface);
            if(!interface_string) {
                printf("Не получили знач");

                cJSON_Delete(root_array);
                clear_config_array(config_array);
                return NULL;
            }
            dev_conf->interface = interface_string;

            cJSON* destination_mac = cJSON_GetObjectItemCaseSensitive(config_item, "destination_mac");
            char* destination_mac_string = cJSON_to_String(destination_mac);
            if(!destination_mac_string) {
                printf("Не получили знач");
                cJSON_Delete(root_array);
                clear_config_array(config_array);
                return NULL;
            }
            dev_conf->destination_mac = destination_mac_string;

            cJSON* destination_ip = cJSON_GetObjectItemCaseSensitive(config_item, "destination_ip");
            char* destination_ip_string = cJSON_to_String(destination_ip);
            if(!destination_ip_string) {
                printf("Не получили знач");
                cJSON_Delete(root_array);
                clear_config_array(config_array);
                return NULL;
            }
            dev_conf->destination_ip = destination_ip_string;

            cJSON* source_mac = cJSON_GetObjectItemCaseSensitive(config_item, "source_mac");
            char* source_mac_string = cJSON_to_String(source_mac);
            if(!source_mac_string) {
                printf("Не получили знач");
                cJSON_Delete(root_array);
                clear_config_array(config_array);
                return NULL;
            }
            dev_conf->source_mac = source_mac_string;

            cJSON* source_ip = cJSON_GetObjectItemCaseSensitive(config_item, "source_ip");
            char* source_ip_string = cJSON_to_String(source_ip);
            if(!source_ip_string) {
                printf("Не получили знач");
                cJSON_Delete(root_array);
                clear_config_array(config_array);
                return NULL;
            }
            dev_conf->source_ip = source_ip_string;

            cJSON* timeout = cJSON_GetObjectItemCaseSensitive(config_item, "timeout");
            int* timeout_int = cJSON_to_Int(timeout);
            if(!timeout_int) {
                printf("Не получили знач");
                cJSON_Delete(root_array);
                clear_config_array(config_array);
                return NULL;
            }
            dev_conf->timeout = timeout_int;
        }

    }
    cJSON_Delete(root_array);
    return config_array;
}   //todo: сделать что б при возвращении NULL падал скрипт