#include "tui.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

SophisticatedTUI::SophisticatedTUI() : main_win(nullptr), input_win(nullptr), 
             status_win(nullptr), users_win(nullptr), title_win(nullptr),
             main_height(0), main_width(0),
             input_height(4), status_height(3),
             users_width(25), title_height(3) {
    start_time = std::chrono::steady_clock::now();
}

SophisticatedTUI::~SophisticatedTUI() {
    if (main_win) delwin(main_win);
    if (input_win) delwin(input_win);
    if (status_win) delwin(status_win);
    if (users_win) delwin(users_win);
    if (title_win) delwin(title_win);
    endwin();
}

void SophisticatedTUI::init_colors() {
    if (has_colors()) {
        start_color();
        
        // Cores estilo btop
        init_pair(1, COLOR_GREEN, COLOR_BLACK);    // Status OK
        init_pair(2, COLOR_RED, COLOR_BLACK);      // Status Error
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);   // Warning
        init_pair(4, COLOR_BLUE, COLOR_BLACK);     // Info
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);  // Private
        init_pair(6, COLOR_CYAN, COLOR_BLACK);     // System
        init_pair(7, COLOR_WHITE, COLOR_BLUE);     // Title bar
        init_pair(8, COLOR_BLACK, COLOR_WHITE);    // Input bar
        init_pair(9, COLOR_BLACK, COLOR_CYAN);     // Active user
    }
}

void SophisticatedTUI::initialize() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);
    
    init_colors();
    create_windows();
    refresh();
    
    // Mostrar ajuda inicial
    show_help();
}

void SophisticatedTUI::create_windows() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Ajustar dimensões para estilo btop
    title_height = 3;
    main_height = max_y - input_height - status_height - title_height;
    main_width = max_x - users_width;
    
    // Criar janelas com sobreposição estilosa
    title_win = newwin(title_height, max_x, 0, 0);
    main_win = newwin(main_height, main_width, title_height, 0);
    input_win = newwin(input_height, main_width, title_height + main_height, 0);
    status_win = newwin(status_height, main_width, title_height + main_height + input_height, 0);
    users_win = newwin(max_y - title_height, users_width, title_height, main_width);
    
    // Habilitar scroll e keypad
    scrollok(main_win, TRUE);
    keypad(input_win, TRUE);
}

void SophisticatedTUI::draw_btop_style_borders() {
    // Bordas arredondadas estilo btop
    const char* border_chars = "┌─┐││└─┘";
    
    // Title window
    wattron(title_win, COLOR_PAIR(7));
    wborder(title_win, border_chars[3], border_chars[3], border_chars[0], 
            border_chars[2], border_chars[0], border_chars[2], border_chars[0], border_chars[2]);
    wattroff(title_win, COLOR_PAIR(7));
    
    // Main window
    wborder(main_win, border_chars[3], border_chars[3], border_chars[0], 
            border_chars[2], border_chars[0], border_chars[2], border_chars[0], border_chars[2]);
    
    // Input window  
    wattron(input_win, COLOR_PAIR(8));
    wborder(input_win, border_chars[3], border_chars[3], border_chars[0], 
            border_chars[2], border_chars[0], border_chars[2], border_chars[0], border_chars[2]);
    wattroff(input_win, COLOR_PAIR(8));
    
    // Status window
    wborder(status_win, border_chars[3], border_chars[3], border_chars[0], 
            border_chars[2], border_chars[0], border_chars[2], border_chars[0], border_chars[2]);
    
    // Users window
    wborder(users_win, border_chars[3], border_chars[3], border_chars[0], 
            border_chars[2], border_chars[0], border_chars[2], border_chars[0], border_chars[2]);
}

void SophisticatedTUI::draw_title() {
    werase(title_win);
    draw_btop_style_borders();
    
    wattron(title_win, COLOR_PAIR(7) | A_BOLD);
    
    // Título centralizado
    std::string title = " UnkIRC - Secure Terminal Chat ";
    int title_x = (getmaxx(title_win) - title.length()) / 2;
    mvwprintw(title_win, 1, std::max(1, title_x), "%s", title.c_str());
    
    // Informações do sistema
    draw_uptime();
    
    wattroff(title_win, COLOR_PAIR(7) | A_BOLD);
    wrefresh(title_win);
}

void SophisticatedTUI::draw_uptime() {
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
    
    int hours = uptime.count() / 3600;
    int minutes = (uptime.count() % 3600) / 60;
    int seconds = uptime.count() % 60;
    
    std::stringstream ss;
    ss << "Uptime: " << std::setfill('0') << std::setw(2) << hours << ":"
       << std::setfill('0') << std::setw(2) << minutes << ":"
       << std::setfill('0') << std::setw(2) << seconds;
    
    mvwprintw(title_win, 1, 2, "%s", ss.str().c_str());
}

void SophisticatedTUI::draw_messages() {
    werase(main_win);
    draw_btop_style_borders();
    
    // Título da janela de mensagens
    wattron(main_win, A_BOLD);
    mvwprintw(main_win, 0, 2, " Messages ");
    wattroff(main_win, A_BOLD);
    
    int y = 1;
    int max_lines = main_height - 2;
    int start_idx = std::max(0, (int)message_history.size() - max_lines);
    
    for (int i = start_idx; i < message_history.size() && y < main_height - 1; i++) {
        const auto& msg = message_history[i];
        int color = get_message_color(msg);
        
        if (color > 0) wattron(main_win, COLOR_PAIR(color));
        
        // Formatar mensagem com timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time_t);
        
        char time_buf[9];
        std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S", &tm);
        
        std::string formatted_msg = "[" + std::string(time_buf) + "] " + msg;
        mvwprintw(main_win, y++, 1, "%.*s", main_width - 2, formatted_msg.c_str());
        
        if (color > 0) wattroff(main_win, COLOR_PAIR(color));
    }
    
    wrefresh(main_win);
}

int SophisticatedTUI::get_message_color(const std::string& message) {
    if (message.find("ERROR") != std::string::npos) return 2;
    if (message.find("WARNING") != std::string::npos) return 3;
    if (message.find("PRIVATE") != std::string::npos) return 5;
    if (message.find("joined") != std::string::npos || 
        message.find("left") != std::string::npos) return 6;
    return 0; // Default
}

void SophisticatedTUI::draw_input() {
    werase(input_win);
    draw_btop_style_borders();
    
    wattron(input_win, COLOR_PAIR(8) | A_BOLD);
    mvwprintw(input_win, 0, 2, " Input ");
    wattroff(input_win, COLOR_PAIR(8) | A_BOLD);
    
    // Prompt elegante
    wattron(input_win, A_BOLD);
    mvwprintw(input_win, 1, 1, "➤ ");
    wattroff(input_win, A_BOLD);
    
    mvwprintw(input_win, 1, 3, "%.*s", main_width - 4, input_buffer.c_str());
    
    // Dica de comandos
    wattron(input_win, COLOR_PAIR(4) | A_DIM);
    mvwprintw(input_win, 2, 1, "Type /help for commands");
    wattroff(input_win, COLOR_PAIR(4) | A_DIM);
    
    wrefresh(input_win);
}

void SophisticatedTUI::draw_status() {
    werase(status_win);
    draw_btop_style_borders();
    
    wattron(status_win, A_BOLD);
    mvwprintw(status_win, 0, 2, " Status ");
    wattroff(status_win, A_BOLD);
    
    wrefresh(status_win);
}

void SophisticatedTUI::draw_users() {
    werase(users_win);
    draw_btop_style_borders();
    
    wattron(users_win, A_BOLD);
    mvwprintw(users_win, 0, 2, " Online Users (%d) ", (int)user_list.size());
    wattroff(users_win, A_BOLD);
    
    int y = 1;
    for (const auto& user : user_list) {
        if (y < getmaxy(users_win) - 1) {
            // Destaque para usuário atual
            if (user.find("(you)") != std::string::npos) {
                wattron(users_win, COLOR_PAIR(9) | A_BOLD);
            } else {
                wattron(users_win, COLOR_PAIR(3));
            }
            
            mvwprintw(users_win, y++, 1, " %-*s", users_width - 2, user.c_str());
            
            if (user.find("(you)") != std::string::npos) {
                wattroff(users_win, COLOR_PAIR(9) | A_BOLD);
            } else {
                wattroff(users_win, COLOR_PAIR(3));
            }
        }
    }
    
    wrefresh(users_win);
}

void SophisticatedTUI::refresh_display() {
    draw_title();
    draw_messages();
    draw_input();
    draw_status();
    draw_users();
}

void SophisticatedTUI::add_message(const std::string& msg, int type) {
    message_history.push_back(msg);
    if (message_history.size() > 1000) {
        message_history.erase(message_history.begin());
    }
    draw_messages();
}

void SophisticatedTUI::set_users(const std::vector<std::string>& users) {
    user_list = users;
    draw_users();
}

void SophisticatedTUI::set_status(const std::string& status, int type) {
    werase(status_win);
    draw_btop_style_borders();
    
    wattron(status_win, COLOR_PAIR(type) | A_BOLD);
    mvwprintw(status_win, 1, 1, " %-*s", main_width - 2, status.c_str());
    wattroff(status_win, COLOR_PAIR(type) | A_BOLD);
    
    wrefresh(status_win);
}

void SophisticatedTUI::show_help() {
    add_message("=== UnkIRC Help ===", 6);
    add_message("/join <channel>  - Join a channel", 4);
    add_message("/msg <user> <message> - Private message", 4);
    add_message("/users - List online users", 4);
    add_message("/key <user> - Exchange encryption keys", 4);
    add_message("/help - Show this help", 4);
    add_message("/quit - Exit UnkIRC", 4);
}

std::string SophisticatedTUI::get_input() {
    echo();
    curs_set(2); // Cursor mais visível
    
    wattron(input_win, A_BOLD);
    mvwprintw(input_win, 1, 3, "%-*s", main_width - 4, ""); // Clear line
    wattroff(input_win, A_BOLD);
    
    wrefresh(input_win);
    
    char buffer[1024];
    mvwgetnstr(input_win, 1, 3, buffer, sizeof(buffer) - 1);
    
    noecho();
    curs_set(1);
    std::string input(buffer);
    
    return input;
}
