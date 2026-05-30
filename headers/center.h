#ifndef CENTER_H
#define CENTER_H

#include <ncurses.h>
#include <string>


void center_text(WINDOW* win, int row, const std::string& text);
void center_text_vertical(WINDOW* win, const std::string& text);
void center_ascii_art(WINDOW* win, int start_y, const std::string& art) ;

#endif 