#ifndef TUI_H
#define TUI_H

#include <ncurses.h>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include "message.h"

class SophisticatedTUI {
private:
    WINDOW* main_win;
    WINDOW* input_win;
    WINDOW* status_win;
    WINDOW* users_win;
    WINDOW* title_win;
    
    std::vector<std::string> message_history;
    std::vector<std::string> user_list;
    std::string input_buffer;
    
    int main_height, main_width;
    int input_height;
    int status_height;
    int users_width;
    int title_height;
    
    std::chrono::steady_clock::time_point start_time;
    
    void create_windows();
    void draw_btop_style_borders();
    void draw_messages();
    void draw_input();
    void draw_status();
    void draw_users();
    void draw_title();
    void draw_uptime();
    
    // Cores e temas
    void init_colors();
    int get_message_color(const std::string& message);
    
public:
    SophisticatedTUI();
    ~SophisticatedTUI();
    
    void initialize();
    void refresh_display();
    void add_message(const std::string& msg, int type = 0);
    void set_users(const std::vector<std::string>& users);
    void set_status(const std::string& status, int type = 0);
    std::string get_input();
    void clear_input();
    void show_help();
    
    // Callbacks
    std::function<void(const std::string&)> on_message_sent;
    std::function<void()> on_quit;
    std::function<void()> on_help;
};

#endif
