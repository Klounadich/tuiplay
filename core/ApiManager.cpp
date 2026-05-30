#include <cstdlib>
#include <curl/curl.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "../headers/ApiManager.h"

using namespace std;
using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

ApiManager::ApiManager() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

ApiManager::~ApiManager() {
    curl_global_cleanup();
}

vector<FindedTracks> ApiManager::GetTracks(string trackname) {
vector<FindedTracks> tracks;
   /*if( system("cd /home/klounadich/dev/MP3PartyPars/MP3PartyPars && dotnet run &") < 0) {
    return tracks;
    
}*/
    
sleep(3); 
    CURL* curl = curl_easy_init();
    
    
    if (!curl) {
        return tracks;
    }
    
    
    char* encoded = curl_easy_escape(curl, trackname.c_str(), trackname.length());
    string url = "http://localhost:5026/api/find/" + string(encoded);
    curl_free(encoded);
    
    string response_string;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    if (res == CURLE_OK && !response_string.empty()) {
        try {
            
            json data = json::parse(response_string);
            
            if (data.contains("success") && data["success"] == true) {
                if (data.contains("tracks") && data["tracks"].is_array()) {
                    for (auto& item : data["tracks"]) {
                        FindedTracks track;
                        
                        track.Tittle = item.value("Title", "");
                        track.Artist = item.value("Artist", "");
                        track.FullTitle = item.value("FullTitle", "");
                        track.TrackId = item.value("TrackId", "");
                        track.Url = item.value("Url", "");
                        track.Duration = " ";
                        
                        
                        tracks.push_back(track);
                    }
                }
            }
        } catch (const exception& e) {
            fprintf(stderr, "JSON parse error: %s\n", e.what());
        }
    }
    
    return tracks;
}

void ApiManager::DownloadTrack(string Url) {
    CURL* curl = curl_easy_init();
    
    
    if (!curl) {
        return;
    }
    
    
    char* encoded = curl_easy_escape(curl, Url.c_str(), Url.length());
    string url = "http://localhost:5026/api/download/" + string(encoded);
    curl_free(encoded);
    
    string response_string;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}