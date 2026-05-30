#pragma once
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <iostream>
#include <fcntl.h>      
#include <sys/types.h>
#include <algorithm>    

class CavaVisualizer {
private:
    int fifo_fd = -1;
    FILE* cava_process = nullptr;
    std::string fifo_path = "/tmp/cava_fifo";
    std::string config_path;
    bool is_running = false;
    std::string leftover;  
    
public:
    bool start() {
        system("pkill cava 2>/dev/null");
        unlink(fifo_path.c_str());
        
        if (mkfifo(fifo_path.c_str(), 0666) == -1) {
            return false;
        }
        
        
        fifo_fd = open(fifo_path.c_str(), O_RDONLY | O_NONBLOCK);
        if (fifo_fd == -1) return false;
        
        
        config_path = std::string(getenv("HOME")) + "/.config/cava/player.conf";
        std::string mkdir_cmd = "mkdir -p " + std::string(getenv("HOME")) + "/.config/cava";
        system(mkdir_cmd.c_str());
        
        FILE* config = fopen(config_path.c_str(), "w");
        if (!config) return false;
        
        fprintf(config, "[general]\n");
        fprintf(config, "bars = 30\n");          
        fprintf(config, "framerate = 60\n");
        fprintf(config, "autosens = 1\n");
        fprintf(config, "sensitivity = 100\n");
        fprintf(config, "monstercat = 0\n");
        fprintf(config, "sleep_timer = 0\n");
        fprintf(config, "\n[eq]\n");
        fprintf(config, "enabled = 1\n");         
        fprintf(config, "\n[input]\n");
        fprintf(config, "method = pulse\n");
        fprintf(config, "source = auto\n");
        fprintf(config, "\n[output]\n");
        fprintf(config, "method = raw\n");
        fprintf(config, "raw_target = %s\n", fifo_path.c_str());
        fprintf(config, "data_format = ascii\n");
        fprintf(config, "ascii_max_range = 20\n");
        fprintf(config, "bar_delimiter = 32\n");  
        fclose(config);
        
        std::string cmd = "cava -p " + config_path + " &";
        system(cmd.c_str());
        
        
        usleep(100000);
        
        is_running = true;
        return true;
    }
    
    std::vector<int> getBars() {
        if (!is_running || fifo_fd == -1) return {};
        
        char buffer[8192];
        ssize_t bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            std::string data = leftover + std::string(buffer);
            leftover.clear();
            
            std::vector<int> all_bars;
            
           
            size_t pos = 0;
            size_t last_newline_pos = std::string::npos;
            
            while ((pos = data.find('\n', pos)) != std::string::npos) {
                last_newline_pos = pos;
                pos++;
            }
            
            if (last_newline_pos != std::string::npos) {
                
                size_t start = data.rfind('\n', last_newline_pos - 1);
                if (start == std::string::npos) start = 0;
                else start++;
                
                std::string line = data.substr(start, last_newline_pos - start);
                
              
                std::istringstream iss(line);
                int value;
                while (iss >> value) {
                    value = std::min(20, std::max(0, value));
                    all_bars.push_back(value);
                }
                if (last_newline_pos + 1 < data.length()) {
                    leftover = data.substr(last_newline_pos + 1);
                }
                
                return all_bars;
            } else {
            
                leftover = data;
            }
        }
        
        return {};
    }
    
    void stop() {
        is_running = false;
        if (fifo_fd != -1) {
            close(fifo_fd);
            fifo_fd = -1;
        }
        system("pkill cava 2>/dev/null");
        unlink(fifo_path.c_str());
    }
    
    ~CavaVisualizer() {
        stop();
    }
};