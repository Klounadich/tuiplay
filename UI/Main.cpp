#include <ncurses.h>
#include <notcurses/notcurses.h>
#include "../headers/data.h"
#include "../headers/ApiManager.h"
#include <string>
#include <vector>

using namespace std;

void Library(); 

int main() {
    setlocale(LC_ALL, "");
    ApiManager api ;
    initscr();
    
    cbreak();
    noecho();
    curs_set(0);
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

    const char* menu[] = {
        "1. My Lib",
        "2. Find Tracks",
        "3. Exit"
    };
    int menu_num = 3;
    int current = 0;
    
    while (run) {
        clear();
        
        attron(A_BOLD);
        printw("%s\n", art);
        attroff(A_BOLD);
        
        printw("==========================\n");
        
        for(int i = 0; i < menu_num; i++) {
            if(i == current) {
                attron(A_REVERSE);
                printw("%s\n", menu[i]);
                attroff(A_REVERSE);
            } else {
                printw("%s\n", menu[i]);
            }
        }
        
        printw("==========================\n");
        printw("Use ArrUp/ArrDown to navigate, Enter to select, Q to quit\n");
        refresh(); 
        
        int ch = getch();
        
        switch (ch) {
            case KEY_UP:
                current--;
                if (current < 0)
                    current = menu_num - 1; 
                break;
                
            case KEY_DOWN:
                current++;
                if (current >= menu_num)
                    current = 0; 
                break;
                
            case 10: {  
                switch (current) {
                    case 0: { 
                        def_prog_mode();
                        endwin();
                        Library();  
                        reset_prog_mode();
                        refresh();
                        keypad(stdscr, TRUE);  
                        curs_set(0);          
                        break;
                    }
                    case 1: {  
                        clear();
                        attron(A_BOLD);
                        mvprintw(0, 0, "======= TYPE SONG NAME ========");
                        attroff(A_BOLD);
                        refresh();

                        char buffer[256];
                        echo(); 
                        move(1, 0);
                        getstr(buffer); 
                        noecho(); 
                        
                        string request(buffer);
                        vector<FindedTracks> tracks = api.GetTracks(request);
                        
                        if (tracks.empty()) {
                            clear();
                            mvprintw(0, 0, "No tracks found. Press any key to continue...");
                            refresh();
                            getch();
                            break;
                        }
                        
                        int selected_track = 0;
                        bool in_track_selection = true;
                        
                        start_color();
                        init_pair(1, COLOR_CYAN, COLOR_BLACK);
                        
                        while (in_track_selection) {
                            clear();
                            attron(A_BOLD);
                            mvprintw(0, 0, "=== Found Tracks ===");
                            attroff(A_BOLD);
                            
                            for (size_t i = 0; i < tracks.size(); i++) {
                                int y = i + 2;
                                if ((int)i == selected_track) {
                                    attron(A_REVERSE);
                                    mvprintw(y, 0, "%zu. %s - %s", i + 1, tracks[i].Tittle.c_str(), tracks[i].Artist.c_str() );
                                    attroff(A_REVERSE);
                                } else {
                                    mvprintw(y, 0, "%zu. %s - %s", i + 1, tracks[i].Tittle.c_str(), tracks[i].Artist.c_str());
                                }
                            }
                            
                            mvprintw(tracks.size() + 3, 0, "Use arrows to navigate, Enter to select track, Q to go back");
                            refresh();
                            
                            int track_ch = getch();
                            
                            switch (track_ch) {
                                case KEY_UP:
                                    selected_track--;
                                    if (selected_track < 0)
                                        selected_track = tracks.size() - 1;
                                    break;
                                    
                                case KEY_DOWN:
                                    selected_track++;
                                    if (selected_track >= (int)tracks.size())
                                        selected_track = 0;
                                    break;
                                    
                                case 10:
                                    clear();
                                    mvprintw(0, 0, "Selected: %s by %s", tracks[selected_track].Tittle.c_str(), tracks[selected_track].Artist.c_str());
                                    api.DownloadTrack(tracks[selected_track].Url);
                                    mvprintw(1, 0, "Press any key to continue...");
                                    refresh();
                                    getch();
                                    in_track_selection = false;
                                    break;
                                    
                                case 'q':
                                case 'Q':
                                    in_track_selection = false;
                                    break;
                            }
                        }
                        break;
                    }
                    case 2: {  
                        run = false;
                        break;
                    }
                }
                break;
            }
                
            case 'q':
            case 'Q':
                run = false;
                break;
        }
    }
    
    endwin();
    return 0;
}