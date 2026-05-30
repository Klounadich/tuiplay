#pragma once
#include <string>
#include <vector>
using namespace std;
struct FindedTracks {
    public:
    string Tittle;
    string Artist;
    string FullTitle;
    string TrackId;
    string Url;
    string Duration;
};

class ApiManager {
    public:
    ApiManager();
    ~ApiManager();
vector<FindedTracks> GetTracks(string trackname) ;

void DownloadTrack(string Url);
};
