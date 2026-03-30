#include<filesystem>
#include <map>
#include "/home/klounadich/dev/musicplayertui/headers/data.h" //temo
#include <iostream>
#include <ncurses.h>
using namespace std;

const string path = "/home/klounadich/Music/";

Playlist playlist;


void RecursiveScanDir(const filesystem::path& directory_path , Playlist& playlist ) {
    playlist.Clear(); 
    for(const auto& file : filesystem::recursive_directory_iterator(directory_path)) {
        if(file.is_regular_file()) {
            filesystem::path relative_path = filesystem::relative(file.path() , directory_path);
            FileInfo info = GetInfo(file.path());
            playlist.AddTrack(info);
        }
    }
}

void Library() {
    RecursiveScanDir(path, playlist);
    
    if(playlist.GetSize() != 0) {
        auto selected = playlist.GetHead();
        int ch;
        
        keypad(stdscr, TRUE); 
        
        while(true) {
            clear();
            
            auto current = playlist.GetHead();
            int index = 1;
            
            while(current) {
                string name = current->data.filename;
                size_t dot = name.find_last_of(".");
                if(dot != string::npos) name = name.substr(0, dot);
                if(name.length() > 40) name = name.substr(0, 37) + "...";
                
                if(current == selected) {
                    attron(A_REVERSE);
                }
            string dur_str = FormatDuration(current->data.duration);
            string size_str = FormatSize(current->data.file_size);
                 printw("%-4d %-40s %-12s %-10s\n", 
                       index, 
                       name.c_str(),
                       dur_str.c_str(), 
                       size_str.c_str());
                
                if(current == selected) {
                    attroff(A_REVERSE);
                }
                
                current = current->pNext;
                index++;
            }
            
            printw("\n arrowUP/ArrowDown -  select , Enter for choice\n");
            refresh();
            
            ch = getch();
            
            
            switch(ch) {
                case KEY_UP:
                    if(selected->pPrev) selected = selected->pPrev;
                    break;
                case KEY_DOWN:
                    if(selected->pNext) selected = selected->pNext;
                    break;
                
                

                case 10:
                    clear();
                    bool playing =true ;
                    while (playing==true) {
                    
                    printw("Are Playing: %s\n", selected->data.filename.c_str());
                    printw("\n ArrowLeft - previous track | ArrowRight- next track");
                    ch = getch();
                    switch(ch) {
                            case KEY_LEFT:
                                if(selected->pPrev) {
                                    clear();
                                    selected = selected->pPrev;
                                }
                                break;
                                
                            case KEY_RIGHT:
                                if(selected->pNext) {
                                    clear();
                                    selected = selected->pNext;
                                }
                                break;
                                
                            case 'q':
                            case 'Q':
                                playing = false; 
                                break;
                        }
                    }
                    break;
            }
        }
    }
}