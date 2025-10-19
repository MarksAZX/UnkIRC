#include "client.h"
#include "config.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <filesystem>

void show_splash() {
    std::cout << R"(
    ██╗   ██╗███╗   ██╗██╗  ██╗██╗██████╗ ██████╗ 
    ██║   ██║████╗  ██║██║ ██╔╝██║██╔══██╗██╔══██╗
    ██║   ██║██╔██╗ ██║█████╔╝ ██║██████╔╝██████╔╝
    ██║   ██║██║╚██╗██║██╔═██╗ ██║██╔══██╗██╔══██╗
    ╚██████╔╝██║ ╚████║██║  ██╗██║██║  ██║██║  ██║
     ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
          Secure Terminal IRC Client v1.0
    )" << std::endl;
}

void show_usage() {
    std::cout << "Usage: UnkIRC [server] [port] [username]" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  UnkIRC                         # Auto-config" << std::endl;
    std::cout << "  UnkIRC irc.example.com 6667    # Custom server" << std::endl;
    std::cout << "  UnkIRC localhost 6667 john     # Full custom" << std::endl;
}

int main(int argc, char* argv[]) {
    std::srand(std::time(nullptr));
    
    show_splash();
    
    // Configuração automática
    ConfigManager config;
    
    std::string host = config.get_server_host();
    int port = config.get_server_port();
    std::string username = config.get_username();
    
    // Sobrescrever com argumentos
    if (argc >= 2) host = argv[1];
    if (argc >= 3) port = std::atoi(argv[2]);
    if (argc >= 4) username = argv[3];
    
    if (argc == 2 && (std::string(argv[1]) == "--help" || 
                      std::string(argv[1]) == "-h")) {
        show_usage();
        return 0;
    }
    
    std::cout << "Connecting to " << host << ":" << port << " as " << username << std::endl;
    std::cout << "Initializing secure connection..." << std::endl;
    
    try {
        UnkIRCClient client(host, port, username);
        
        if (client.initialize()) {
            std::cout << "Connection established! Starting TUI..." << std::endl;
            client.run();
        } else {
            std::cerr << "Failed to initialize client" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
