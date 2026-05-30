#include  "../headers/center.h"
#include <sstream>

void center_text(WINDOW* win, int row, const std::string& text) {
    int max_y, max_x;
    getmaxyx(win ? win : stdscr, max_y, max_x);
    
    int text_length = text.length();
    if (text_length > max_x) {
        text_length = max_x;
    }
    
    int start_col = (max_x - text_length) / 2;
    mvwprintw(win, row, start_col, "%.*s", text_length, text.c_str());
}

void center_text_vertical(WINDOW* win, const std::string& text) {
    int max_y, max_x;
    getmaxyx(win ? win : stdscr, max_y, max_x);
    
    int text_length = text.length();
    int row = max_y / 2;
    int col = (max_x - text_length) / 2;
    
    mvwprintw(win, row, col, "%s", text.c_str());
}

void center_ascii_art(WINDOW* win, int start_y, const std::string& art) {
    std::istringstream stream(art);
    std::string line;
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);
    
    int line_num = 0;
    while (std::getline(stream, line)) {
        
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        int x_pos = (max_x - line.length()) / 2;
        if (x_pos < 0) x_pos = 0;
        mvwprintw(win, start_y + line_num, x_pos, "%s", line.c_str());
        line_num++;
    }
}