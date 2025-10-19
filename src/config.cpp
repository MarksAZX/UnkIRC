#include "config.h"
#include <unistd.h>
#include <pwd.h>
#include <cstdlib>
#include <ctime>

ConfigManager::ConfigManager() {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    
    config_dir = std::string(home) + "/.config/UnkIRC";
    config_file = config_dir + "/config.cfg";
    
    // Criar diretório se não existir
    std::filesystem::create_directories(config_dir);
    
    // Carregar ou criar config padrão
    if (!load_config()) {
        create_default_config();
    }
}

void ConfigManager::create_default_config() {
    set_server("localhost", 6667);
    set_username(get_default_username());
    save_config();
}

std::string ConfigManager::get_default_username() {
    const char* env_user = getenv("USER");
    if (env_user) {
        return std::string(env_user) + std::to_string(std::rand() % 1000);
    }
    return "user" + std::to_string(std::rand() % 10000);
}

std::string ConfigManager::get_server_host() {
    return "localhost"; // Será sobrescrito pelo config file
}

int ConfigManager::get_server_port() {
    return 6667;
}

std::string ConfigManager::get_username() {
    return get_default_username();
}

bool ConfigManager::load_config() {
    std::iffile file(config_file);
    if (!file.is_open()) return false;
    
    // Implementar parsing do arquivo de configuração
    return true;
}

bool ConfigManager::save_config() {
    std::ofstream file(config_file);
    if (!file.is_open()) return false;
    
    file << "# UnkIRC Configuration File" << std::endl;
    file << "server_host=localhost" << std::endl;
    file << "server_port=6667" << std::endl;
    file << "username=" << get_username() << std::endl;
    
    return true;
}
