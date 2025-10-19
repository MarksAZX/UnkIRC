#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

class ConfigManager {
private:
    std::string config_dir;
    std::string config_file;
    
    void create_default_config();
    std::string get_default_username();
    
public:
    ConfigManager();
    
    std::string get_server_host();
    int get_server_port();
    std::string get_username();
    std::string get_config_path();
    
    void set_server(const std::string& host, int port);
    void set_username(const std::string& username);
    
    bool save_config();
    bool load_config();
};

#endif
