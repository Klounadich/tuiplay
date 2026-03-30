
#include <cstdio>
#include <iostream>
#include<ncurses.h>
#include <string>
#include </home/klounadich/dev/musicplayertui/headers/data.h>
using namespace std;

int main() {
   
    initscr();
    cbreak();
    noecho();
keypad(stdscr, TRUE);
    bool run = true;
    
    const char* art = R"(
 _______ _    _  _____     _____  _           __     ________ _____  
|__   __| |  | ||_   _|   |  __ \| |        /\\ \   / /  ____|  __ \ 
   | |  | |  | |  | |     | |__) | |       /  \\ \_/ /| |__  | |__) |
   | |  | |  | |  | |     |  ___/| |      / /\ \\   / |  __| |  _  / 
   | |_ | |__| | _| |_    | |    | |____ / ____ \| |  | |____| | \ \ 
   |_(_) \____(_)_____|   |_|    |______/_/    \_\_|  |______|_|  \_\
                                                                      
)";

const char* track_text = R"(  /$$$$$$$$ /$$$$$$$   /$$$$$$   /$$$$$$  /$$   /$$  /$$$$$$ 
|__  $$__/| $$__  $$ /$$__  $$ /$$__  $$| $$  /$$/ /$$__  $$
   | $$   | $$  \ $$| $$  \ $$| $$  \__/| $$ /$$/ | $$  \__/
   | $$   | $$$$$$$/| $$$$$$$$| $$      | $$$$$/  |  $$$$$$ 
   | $$   | $$__  $$| $$__  $$| $$      | $$  $$   \____  $$
   | $$   | $$  \ $$| $$  | $$| $$    $$| $$\  $$  /$$  \ $$
   | $$   | $$  | $$| $$  | $$|  $$$$$$/| $$ \  $$|  $$$$$$/
   |__/   |__/  |__/|__/  |__/ \______/ |__/  \__/ \______/ 
                                                            
                                                            
                                                            )";
printw("%s\n", art);
    while (run) {
    
    
    
        printw("==========================\n");
        printw("1. My Lib\n");
        printw("2. Find Tracks\n");
        printw("3. Exit\n");
        printw("==========================\n");
        printw("Select option: ");
        refresh(); 
        int ch = getch();
        switch (ch) {
            case '1':{
                clear();
                printw("%s\n", track_text , "\n");
                Library();
                refresh();
                getch();
                break;
            }
            case '2': {
                clear();
                printw("\nconecting to API");
                refresh();
                getch();
                break;
            }
            case '3': {
                
                run = false;
                
                break;
            }
            default: {
                 clear();
                printw("\nno answer , try again");
                break;
            }

        }

    }
    endwin();
    return 0;
}