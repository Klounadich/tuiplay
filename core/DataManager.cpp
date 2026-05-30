#include <filesystem>
#include "../headers/data.h"
#include "../headers/audio.h"
#include  "../headers/center.h"
#include  "../headers/cava.h"
#include <ncurses.h>
#include <string>
#include <chrono>

using namespace std;

const string path = "/home/klounadich/Music/";
Playlist playlist;
bool manual_stop =false;
const long long SEEK_DELTA = 44100 * 10;
const char* track_text = R"(  /$$$$$$$$ /$$$$$$$   /$$$$$$   /$$$$$$  /$$   /$$  /$$$$$$ 
|__  $$__/| $$__  $$ /$$__  $$ /$$__  $$| $$  /$$/ /$$__  $$
   | $$   | $$  \ $$| $$  \ $$| $$  \__/| $$ /$$/ | $$  \__/
   | $$   | $$$$$$$/| $$$$$$$$| $$      | $$$$$/  |  $$$$$$ 
   | $$   | $$__  $$| $$__  $$| $$      | $$  $$   \____  $$
   | $$   | $$  \ $$| $$  | $$| $$    $$| $$\  $$  /$$  \ $$
   | $$   | $$  | $$| $$  | $$|  $$$$$$/| $$ \  $$|  $$$$$$/
   |__/   |__/  |__/|__/  |__/ \______/ |__/  \__/ \______/ 
)";

void RecursiveScanDir(const filesystem::path& directory_path, Playlist& playlist) {
    playlist.Clear(); 
    for(const auto& file : filesystem::recursive_directory_iterator(directory_path)) {
        if(file.is_regular_file()) {
            FileInfo info = GetInfo(file.path());
            playlist.AddTrack(info);
        }
    }
}

shared_ptr<FilesNode> FindTrackByName(Playlist& playlist, const string& search_query) {
    auto current = playlist.GetHead();
    while (current) {
        string name = current->data.filename;
        size_t dot = name.find_last_of(".");
        if(dot != string::npos) name = name.substr(0, dot);
        
        string query_lower = search_query;
        string name_lower = name;
        transform(query_lower.begin(), query_lower.end(), query_lower.begin(), ::tolower);
        transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
        
        if (name_lower.find(query_lower) != string::npos) {
            return current; 
        }
        current = current->pNext;
    }
    return nullptr;
}

// Функция для проверки, соответствует ли трек поисковому запросу
bool MatchesSearch(shared_ptr<FilesNode> node, const string& search_query) {
    if (search_query.empty()) return true;
    
    string name = node->data.filename;
    size_t dot = name.find_last_of(".");
    if(dot != string::npos) name = name.substr(0, dot);
    
    string query_lower = search_query;
    string name_lower = name;
    transform(query_lower.begin(), query_lower.end(), query_lower.begin(), ::tolower);
    transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
    
    return name_lower.find(query_lower) != string::npos;
}

void Library() {
    bool in_player_mode = false;
    static float def_volume = 0.5f;
    RecursiveScanDir(path, playlist);
    
    if(playlist.GetSize() == 0) {
        clear();
        printw("No music found in %s\n", path.c_str());
        printw("\nPress any key to return to main menu...");
        refresh();
        getch();
        return; 
    }
    
    auto selected = playlist.GetHead();
    int ch;
    keypad(stdscr, TRUE); 
    
    bool run = true;
    string search_query = "";
    bool search_mode = false;
    int selected_index = 0;
      
    while(run) {
        clear();
        
        // Собираем все треки, которые соответствуют поиску
        vector<shared_ptr<FilesNode>> filtered_tracks;
        auto current = playlist.GetHead();
        while(current) {
            if (MatchesSearch(current, search_query)) {
                filtered_tracks.push_back(current);
            }
            current = current->pNext;
        }
        
        // Корректируем выбранный индекс
        if (!filtered_tracks.empty()) {
            if (selected_index >= (int)filtered_tracks.size()) {
                selected_index = filtered_tracks.size() - 1;
            }
            if (selected_index < 0) {
                selected_index = 0;
            }
            selected = filtered_tracks[selected_index];
        } else {
            selected = nullptr;
            selected_index = 0;
        }
        
        attron(A_BOLD);
        printw("%s\n", track_text);
        
        // Показываем строку поиска
        if (search_mode) {
            attron(A_REVERSE);
            printw("Search: %s", search_query.c_str());
            attroff(A_REVERSE);
            printw(" [ESC to exit, Enter to select, arrows to navigate]");
        } else {
            printw("Search: [Press '/' to search]");
        }
        printw("\n\n");
                        
        printw("%-4s %-40s %-12s %-10s\n", "#", "Track Name", "Duration", "Size");
        attroff(A_BOLD);
        
        // Показываем отфильтрованный список
        for (size_t i = 0; i < filtered_tracks.size(); i++) {
            string name = filtered_tracks[i]->data.filename;
            size_t dot = name.find_last_of(".");
            if(dot != string::npos) name = name.substr(0, dot);
            if(name.length() > 40) name = name.substr(0, 37) + "...";
            
            // Подсвечиваем выбранный трек
            if ((int)i == selected_index) {
                attron(A_REVERSE);
            }
            
            string dur_str = FormatDuration(filtered_tracks[i]->data.duration);
            string size_str = FormatSize(filtered_tracks[i]->data.file_size);
            printw("%-4zu %-40s %-12s %-10s\n", 
                   i + 1, 
                   name.c_str(),
                   dur_str.c_str(), 
                   size_str.c_str());
            
            if ((int)i == selected_index) {
                attroff(A_REVERSE);
            }
        }
        
        if (filtered_tracks.empty() && !search_query.empty()) {
            printw("\n No tracks found matching: %s\n", search_query.c_str());
        }
        
        if (search_mode) {
            printw("\n [Type to search] [↑/↓ to navigate] [Enter to play] [ESC to cancel]");
        } else {
            printw("\n [↑/↓] Select  [Enter] Play  [/] Search  [Q] Quit");
        }
        refresh();
        
        ch = getch();
        
        if (search_mode) {
            // Режим поиска - обрабатываем навигацию
            switch(ch) {
                case 27: // ESC - выход из поиска
                    search_mode = false;
                    search_query = "";
                    selected_index = 0;
                    // Восстанавливаем выбранный трек из полного списка
                    if (!filtered_tracks.empty()) {
                        selected = filtered_tracks[0];
                        selected_index = 0;
                    } else {
                        selected = playlist.GetHead();
                        selected_index = 0;
                    }
                    break;
                    
                case 10: // Enter - выход из поиска и воспроизведение выбранного трека
                    if (!filtered_tracks.empty()) {
                        selected = filtered_tracks[selected_index];
                        search_mode = false;
                        search_query = "";
                        // Переходим к воспроизведению
                        goto play_track;
                    }
                    break;
                    
                case KEY_UP:
                    if (selected_index > 0) {
                        selected_index--;
                    }
                    break;
                    
                case KEY_DOWN:
                    if (selected_index < (int)filtered_tracks.size() - 1) {
                        selected_index++;
                    }
                    break;
                    
                case KEY_BACKSPACE:
                case 127: // Backspace
                case 8:
                    if (!search_query.empty()) {
                        search_query.pop_back();
                        selected_index = 0; // Сброс на первый трек при изменении поиска
                    }
                    break;
                    
                default:
                    if (ch >= 32 && ch <= 126) { // Печатные символы
                        search_query.push_back((char)ch);
                        selected_index = 0; // Сброс на первый трек при изменении поиска
                    }
                    break;
            }
        } else {
            // Обычный режим (без поиска)
            switch(ch) {
                default:
                case KEY_UP:
                    if (filtered_tracks.empty()) break;
                    if (selected_index > 0) {
                        selected_index--;
                        selected = filtered_tracks[selected_index];
                    }
                    break;
                    
                case KEY_DOWN:
                    if (filtered_tracks.empty()) break;
                    if (selected_index < (int)filtered_tracks.size() - 1) {
                        selected_index++;
                        selected = filtered_tracks[selected_index];
                    }
                    break;
                    
                case 's':
                case 'S':
                case '/':
                    search_mode = true;
                    search_query = "";
                    selected_index = 0;
                    break;
                
                case 'q':
                case 'Q':
                    clear();  
                    return;  
                    
                case 10: { // Enter - play
                    play_track:
                    if (filtered_tracks.empty()) break;
                    
                    in_player_mode = true;
                    clear();
                    static AudioPlayer player;
                    static CavaVisualizer viz;
                    bool playing = true;
                    
                    if (!player.load(selected->data.path)) {
                        printw("Error loading file: %s\n", selected->data.filename.c_str());
                        printw("Path: %s\n", selected->data.path.string().c_str());
                        printw("\nPress any key to continue...");
                        refresh();
                        getch();
                        break;
                    }
                    player.setVolume(def_volume);
                    viz.start();
                    player.play();
                    
                    long long paused_elapsed = 0;
                    bool is_paused = false;
                    auto start_time = std::chrono::steady_clock::now();
                    
                    nodelay(stdscr, TRUE);
                    
                    while(playing) {
                        clear();
                        
                        long long elapsed;
                        if (is_paused) {
                            elapsed = paused_elapsed;
                        } else {
                            elapsed = player.getPosition() / 44100;
                        }
                        
                        if (elapsed >= selected->data.duration && !is_paused && !manual_stop) {
                            player.stop();
                            selected = selected->pNext ? selected->pNext : playlist.GetHead();
                            if (player.load(selected->data.path)) {
                                player.setVolume(def_volume);
                                player.play();
                                paused_elapsed = 0;
                                start_time = std::chrono::steady_clock::now();
                                is_paused = false;
                                manual_stop = false;
                            }
                            continue;
                        }
                        
                        center_text(stdscr, 0, ("Now Playing: " + selected->data.filename).c_str());
                        center_text(stdscr, 1, ("Status: " + std::string(player.isPlaying() ? "Playing" : "Paused")).c_str());
                        center_text(stdscr, 2, ("Volume: " + std::to_string(static_cast<int>(player.getVolume() * 100)) + "%").c_str());
                        center_text(stdscr, 3, ("Playing: " + FormatDuration(elapsed) + "||" + FormatDuration(selected->data.duration)).c_str());
                        center_ascii_art(stdscr, 12, R"(                    -@                
                   .##@               
                  .####@              
                  @#####@             
                . *######@            
               .##@o@#####@           
              /############@          
             /##############@         
            @######@**%######@        
           @######`     %#####o       
          @######@       ######%      
        -@#######h       ######@.`    
       /#####h**``       `**%@####@   
      @H@*`                    `*%#@  
     *`                            `* )");
                        center_text(stdscr, 40, "[P] Pause/Resusume  [<-] Previous  [->] Next  [+/-] Volume  [Q] Quit");
                        
                        int max_y, max_x;
                        getmaxyx(stdscr, max_y, max_x);
                        
                        vector<int> bars = viz.getBars();
                        
                        if (!bars.empty()) {
                            int start_y = max_y - 8;  
                            int max_height = 6;        
                            
                            int max_bars = min((int)bars.size(), max_x / 2);
                            int offset_x = (max_x - (max_bars * 2)) / 2;
                            
                            for (int i = 0; i < max_bars; i++) {
                                for (int h = 0; h < max_height; h++) {
                                    int y = start_y + (max_height - 1 - h);
                                    int x = offset_x + (i * 2);
                                    if (y >= 0 && y < max_y && x >= 0 && x < max_x - 1) {
                                        mvprintw(y, x, "  ");
                                    }
                                }
                            }
                            
                            for (int i = 0; i < max_bars; i++) {
                                int height = bars[i] * max_height / 20;
                                
                                if (height > max_height * 2/3) {
                                    attron(A_BOLD);
                                } else if (height > max_height / 3) {
                                    attron(A_DIM);
                                }
                                
                                for (int h = 0; h < height; h++) {
                                    int y = start_y + (max_height - 1 - h);
                                    int x = offset_x + (i * 2);
                                    if (y >= 0 && y < max_y && x >= 0 && x < max_x - 1) {
                                        mvprintw(y, x, "##");
                                    }
                                }
                                
                                attroff(A_BOLD);
                                attroff(A_DIM);
                            }
                        }
                        refresh();
                        
                        if (!player.isPlaying() && !manual_stop && !is_paused) {
                            selected = selected->pNext ? selected->pNext : playlist.GetHead();
                            if (player.load(selected->data.path)) {
                                player.setVolume(def_volume);
                                player.play();
                                paused_elapsed = 0;
                                start_time = std::chrono::steady_clock::now();
                            }
                        }
                        
                        int player_ch = getch();
                        
                        switch(player_ch) {
                            case 'p':
                            case 'P':
                            case 32:
                                if (player.isPlaying()) {
                                    manual_stop = true;
                                    player.pause();
                                    is_paused = true;
                                    paused_elapsed = elapsed;
                                } else {
                                    manual_stop = false;
                                    player.resume();
                                    is_paused = false;
                                    start_time = std::chrono::steady_clock::now();
                                }
                                break;
                                
                            case '+':
                            case '=':
                                player.setVolume(min(1.0f, player.getVolume() + 0.1f));
                                break;
                                
                            case '-':
                            case '_':
                                player.setVolume(max(0.0f, player.getVolume() - 0.1f));
                                break;
                                
                            case KEY_LEFT:
                                player.stop();
                                selected = selected->pPrev ? selected->pPrev : playlist.GetTail();
                                if (player.load(selected->data.path)) {
                                    player.setVolume(def_volume);
                                    player.play();
                                    paused_elapsed = 0;
                                    start_time = std::chrono::steady_clock::now();
                                    is_paused = false;
                                    manual_stop = false;
                                }
                                break;
                                
                            case KEY_RIGHT:
                                player.stop();
                                selected = selected->pNext ? selected->pNext : playlist.GetHead();
                                if (player.load(selected->data.path)) {
                                    player.setVolume(def_volume);
                                    player.play();
                                    paused_elapsed = 0;
                                    start_time = std::chrono::steady_clock::now();
                                    is_paused = false;
                                    manual_stop = false;
                                }
                                break;
                                
                            case '.':
                            case '>':
                            {
                                long long new_pos = player.getPosition() + SEEK_DELTA;
                                long long duration_samples = selected->data.duration * 44100;
                                if (new_pos < 0) new_pos = 0;
                                if (new_pos > duration_samples) new_pos = duration_samples;
                                player.setPosition(new_pos);
                                if (is_paused) {
                                    paused_elapsed += 10;
                                    if (paused_elapsed > selected->data.duration) paused_elapsed = selected->data.duration;
                                }
                                break;
                            }
                            
                            case ',':
                            case '<':
                            {
                                long long new_pos = player.getPosition() - SEEK_DELTA;
                                if (new_pos < 0) new_pos = 0;
                                player.setPosition(new_pos);
                                if (is_paused) {
                                    paused_elapsed -= 10;
                                    if (paused_elapsed < 0) paused_elapsed = 0;
                                }
                                break;
                            }
                                
                            case 'q':
                            case 'Q':
                                player.stop();
                                playing = false;
                                break;
                        }
                        
                        napms(50);  
                    }
                    
                    nodelay(stdscr, FALSE);
                    clear();
                    break;
                }
            }
        }
    }
}