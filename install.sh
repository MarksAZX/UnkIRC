#!/bin/bash

# UnkIRC Installation Script
# GitHub: https://github.com/MarksAZX/UnkIRC

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
REPO_URL="https://github.com/MarksAZX/UnkIRC.git"
TEMP_DIR="/tmp/unkirc-install"
INSTALL_DIR="/usr/local"
CONFIG_DIR="$HOME/.config/UnkIRC"

# Print colored output
print_status() {
    echo -e "${BLUE}==>${NC} $1"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}!${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

# Check if running as root for system installation
check_privileges() {
    if [ "$EUID" -eq 0 ]; then
        print_warning "Running as root. System-wide installation will be performed."
    else
        print_status "User installation mode. Using ~/.local/bin for binary."
        INSTALL_DIR="$HOME/.local"
    fi
}

# Detect package manager and install dependencies
install_dependencies() {
    print_status "Detecting package manager and installing dependencies..."
    
    if command -v apt-get > /dev/null 2>&1; then
        # Debian/Ubuntu
        print_status "Detected apt package manager"
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            pkg-config \
            libncurses5-dev \
            libncursesw5-dev \
            libssl-dev \
            git \
            curl
        
    elif command -v yum > /dev/null 2>&1; then
        # RHEL/CentOS
        print_status "Detected yum package manager"
        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y \
            cmake \
            pkgconfig \
            ncurses-devel \
            openssl-devel \
            git \
            curl
        
    elif command -v dnf > /dev/null 2>&1; then
        # Fedora
        print_status "Detected dnf package manager"
        sudo dnf groupinstall -y "Development Tools"
        sudo dnf install -y \
            cmake \
            pkgconfig \
            ncurses-devel \
            openssl-devel \
            git \
            curl
        
    elif command -v pacman > /dev/null 2>&1; then
        # Arch Linux
        print_status "Detected pacman package manager"
        sudo pacman -Sy --noconfirm \
            base-devel \
            cmake \
            pkg-config \
            ncurses \
            openssl \
            git \
            curl
        
    elif command -v zypper > /dev/null 2>&1; then
        # openSUSE
        print_status "Detected zypper package manager"
        sudo zypper install -y \
            patterns-devel-base-devel_basis \
            cmake \
            pkg-config \
            ncurses-devel \
            libopenssl-devel \
            git \
            curl
    else
        print_error "Could not detect package manager. Please install dependencies manually:"
        echo "  - build-essential / base-devel"
        echo "  - cmake"
        echo "  - pkg-config"
        echo "  - libncurses5-dev / ncurses-devel"
        echo "  - libssl-dev / openssl-devel"
        echo "  - git"
        exit 1
    fi
    
    print_success "Dependencies installed successfully"
}

# Check if dependencies are available
check_dependencies() {
    local missing_deps=()
    
    for dep in cmake pkg-config git; do
        if ! command -v "$dep" > /dev/null 2>&1; then
            missing_deps+=("$dep")
        fi
    done
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_warning "Missing dependencies: ${missing_deps[*]}"
        install_dependencies
    else
        print_success "All build dependencies are available"
    fi
}

# Clone or update repository
setup_source() {
    print_status "Setting up UnkIRC source code..."
    
    # Check if we're already in the UnkIRC directory
    if [ -f "CMakeLists.txt" ] && [ -d "src" ]; then
        print_status "Using current directory as source"
        SOURCE_DIR=$(pwd)
    else
        # Create temporary directory
        rm -rf "$TEMP_DIR"
        mkdir -p "$TEMP_DIR"
        
        # Clone repository
        print_status "Cloning UnkIRC from GitHub..."
        if git clone "$REPO_URL" "$TEMP_DIR" 2>/dev/null; then
            print_success "Repository cloned successfully"
        else
            print_error "Failed to clone repository"
            exit 1
        fi
        
        SOURCE_DIR="$TEMP_DIR"
    fi
    
    cd "$SOURCE_DIR"
}

# Build UnkIRC
build_unkirc() {
    print_status "Building UnkIRC..."
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure with CMake
    print_status "Configuring build with CMake..."
    if [ "$INSTALL_DIR" = "$HOME/.local" ]; then
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" ..
    else
        cmake -DCMAKE_BUILD_TYPE=Release ..
    fi
    
    # Build
    print_status "Compiling UnkIRC..."
    local cpu_cores=$(nproc 2>/dev/null || echo 2)
    if make -j"$cpu_cores"; then
        print_success "UnkIRC compiled successfully"
    else
        print_error "Compilation failed"
        exit 1
    fi
}

# Install UnkIRC
install_unkirc() {
    print_status "Installing UnkIRC..."
    
    if [ "$INSTALL_DIR" = "$HOME/.local" ]; then
        # User installation
        make install
        # Ensure ~/.local/bin is in PATH
        if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
            print_warning "Adding ~/.local/bin to PATH in ~/.bashrc"
            echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
        fi
    else
        # System installation
        sudo make install
    fi
    
    print_success "UnkIRC installed to $INSTALL_DIR/bin"
}

# Create configuration directory
setup_config() {
    print_status "Setting up configuration..."
    
    mkdir -p "$CONFIG_DIR"
    
    # Create default config if it doesn't exist
    if [ ! -f "$CONFIG_DIR/config.cfg" ]; then
        cat > "$CONFIG_DIR/config.cfg" << 'EOF'
# UnkIRC Configuration
server_host = irc.libera.chat
server_port = 6667
username = $(whoami)
theme = dark
encryption_enabled = true
auto_connect = true
EOF
        print_success "Default configuration created"
    fi
    
    # Set proper permissions
    chmod 700 "$CONFIG_DIR"
    print_success "Configuration directory ready: $CONFIG_DIR"
}

# Verify installation
verify_installation() {
    print_status "Verifying installation..."
    
    if command -v UnkIRC > /dev/null 2>&1; then
        print_success "UnkIRC is now available as a system command!"
    else
        if [ "$INSTALL_DIR" = "$HOME/.local" ]; then
            print_warning "UnkIRC installed to ~/.local/bin"
            print_warning "Please restart your terminal or run: source ~/.bashrc"
        else
            print_error "Installation verification failed"
            exit 1
        fi
    fi
}

# Cleanup temporary files
cleanup() {
    if [ -d "$TEMP_DIR" ]; then
        print_status "Cleaning up temporary files..."
        rm -rf "$TEMP_DIR"
    fi
}

# Display success message
show_success() {
    echo
    echo -e "${GREEN}╔════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║          UnkIRC Installed!             ║${NC}"
    echo -e "${GREEN}╚════════════════════════════════════════╝${NC}"
    echo
    echo -e "${BLUE}🎉 Installation completed successfully!${NC}"
    echo
    echo -e "${YELLOW}Quick Start:${NC}"
    echo -e "  ${GREEN}UnkIRC${NC}                         # Auto-configure"
    echo -e "  ${GREEN}UnkIRC irc.libera.chat 6667${NC}    # Connect to Libera.Chat"
    echo -e "  ${GREEN}UnkIRC --help${NC}                  # Show all options"
    echo
    echo -e "${YELLOW}Configuration:${NC}"
    echo -e "  Edit: ${BLUE}$CONFIG_DIR/config.cfg${NC}"
    echo
    echo -e "${YELLOW}Need Help?${NC}"
    echo -e "  GitHub: ${BLUE}https://github.com/MarksAZX/UnkIRC${NC}"
    echo
}

# Main installation function
main() {
    clear
    echo -e "${BLUE}"
    cat << "EOF"
    ██╗   ██╗███╗   ██╗██╗  ██╗██╗██████╗ ██████╗ 
    ██║   ██║████╗  ██║██║ ██╔╝██║██╔══██╗██╔══██╗
    ██║   ██║██╔██╗ ██║█████╔╝ ██║██████╔╝██████╔╝
    ██║   ██║██║╚██╗██║██╔═██╗ ██║██╔══██╗██╔══██╗
    ╚██████╔╝██║ ╚████║██║  ██╗██║██║  ██║██║  ██║
     ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
            Secure Terminal IRC Client
EOF
    echo -e "${NC}"
    
    # Installation steps
    check_privileges
    check_dependencies
    setup_source
    build_unkirc
    install_unkirc
    setup_config
    verify_installation
    cleanup
    show_success
}

# Handle script interruption
trap cleanup EXIT
trap 'print_error "Installation interrupted"; exit 1' INT TERM

# Run main function
main "$@"
