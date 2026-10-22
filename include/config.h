#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char* name;
    char* interface;
    char* destination_mac;
    char* destination_ip;
    char* source_mac;
    char* source_ip;
    int* timeout;
} Config;

Config** read_my_config();
int clear_config_array(Config** config_array);

#endif