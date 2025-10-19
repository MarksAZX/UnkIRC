cmake_minimum_required(VERSION 3.12)
project(UnkIRC VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurações de compilação
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Encontrar dependências
find_package(PkgConfig REQUIRED)
pkg_check_modules(NCURSES REQUIRED ncurses)
pkg_check_modules(OPENSSL REQUIRED openssl)

# Arquivos fonte
set(SOURCES
    src/main.cpp
    src/tui.cpp
    src/crypto.cpp
    src/client.cpp
    src/config.cpp
)

# Executável principal
add_executable(UnkIRC ${SOURCES})

# Link com dependências
target_link_libraries(UnkIRC ${NCURSES_LIBRARIES} ${OPENSSL_LIBRARIES})
target_include_directories(UnkIRC PRIVATE ${NCURSES_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIRS})

# Instalação
install(TARGETS UnkIRC DESTINATION bin)

# Configuração pós-instalação
install(CODE "
    execute_process(
        COMMAND mkdir -p \$ENV{HOME}/.config/UnkIRC
    )
    message(\"UnkIRC installed successfully!\")
    message(\"Run with: UnkIRC\")
")
