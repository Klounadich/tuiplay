#ifndef DATA_H
#define DATA_H
#include <cstddef>
#include <memory>
#include <string>
#include <filesystem>
#include <map>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
}

struct FileInfo {
    std::size_t file_size;
    std::string filename;
    double duration;
};

struct FilesNode {
    std::shared_ptr<FilesNode> pNext;
     std::shared_ptr<FilesNode> pPrev;
    FileInfo data;

    FilesNode(const FileInfo& info) 
        : data(info), pNext(nullptr), pPrev(nullptr) {}
};

class Playlist {
    private:
    std::shared_ptr<FilesNode> head;
    std::shared_ptr<FilesNode> tail;
    std::shared_ptr<FilesNode> current;
    size_t size;

    public:
    Playlist() : head(nullptr), tail(nullptr), current(nullptr), size(0) {}
    ~Playlist() = default;

    Playlist(const Playlist&) = delete;
    Playlist& operator=(const Playlist&) = delete;
    
    // РАЗОБРАТЬСЯ С СЕМАНТИКОЙ ПЕРЕМЕЩЕНИЙ
    Playlist(Playlist&& other) noexcept 
        : head(std::move(other.head)), 
          tail(std::move(other.tail)), 
          current(std::move(other.current)), 
          size(other.size) {
        other.size = 0;
    }
    
    Playlist& operator=(Playlist&& other) noexcept {
        if (this != &other) {
            head = std::move(other.head);
            tail = std::move(other.tail);
            current = std::move(other.current);
            size = other.size;
            other.size = 0;
        }
        return *this;
    }

    void AddTrack(FileInfo& track) {
        auto newNode = std::make_shared<FilesNode>(track);
        if (!head) {
            head = tail = current = newNode;
        }
        else {
            tail->pNext=newNode;
            newNode->pPrev = tail;
            tail= newNode;
        }
        size++;
    }

    void Clear() {
        head=nullptr;
        tail=nullptr;
        current=nullptr;
        size=0;
    }

    size_t GetSize() const {
        return size;
    }

     std::shared_ptr<FilesNode> GetHead() const {
        return head;
    }
};


inline double GetDuration(const std::filesystem::path& path) {
    AVFormatContext* context = nullptr;
    if (avformat_open_input(&context, path.string().c_str(), nullptr, nullptr) != 0) {
        return -1.0;
    }
    if (avformat_find_stream_info(context, nullptr) < 0) {
        avformat_close_input(&context);
        return -1.0;
    }
    int64_t duration_us = context->duration;
    avformat_close_input(&context);
    if (duration_us == AV_NOPTS_VALUE) return -1.0;
    return static_cast<double>(duration_us) / AV_TIME_BASE;
}


inline std::string FormatDuration(double seconds) {
    if (seconds < 0) return "--:--";
    
    int total_sec = static_cast<int>(seconds);
    int min = total_sec / 60;
    int sec = total_sec % 60;
    
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d:%02d", min, sec);
    return std::string(buffer);
}


inline std::string FormatSize(std::size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }
    
    char buffer[32];
    if (unit_index == 0) {
        snprintf(buffer, sizeof(buffer), "%zu %s", bytes, units[0]);
    } else {
        snprintf(buffer, sizeof(buffer), "%.1f %s", size, units[unit_index]);
    }
    return std::string(buffer);
}

inline FileInfo GetInfo(const std::filesystem::path& path) {
    return {
        std::filesystem::file_size(path),
        path.filename().string(),
        GetDuration(path)
    };
}


void Library();
#endif