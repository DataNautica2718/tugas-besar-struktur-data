// musik.h (diperbarui)
#ifndef MUSIK_H
#define MUSIK_H

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <conio.h>

using namespace std;

struct Song {
    string id, title, genre, artist;
    int duration;
};

struct adrSong {
    Song info;
    adrSong *prev;
    adrSong *next;
};
typedef adrSong* SongPtr;

struct SongList {
    SongPtr first;
    SongPtr last;
};

struct PlaylistNode {
    SongPtr song;
    PlaylistNode *prev;
    PlaylistNode *next;
};
typedef PlaylistNode* adrPlaylist;

struct Playlist {
    string name;
    adrPlaylist first;
    adrPlaylist last;
};

struct HistoryNode {
    SongPtr song;
    HistoryNode *next;
};
typedef HistoryNode* adrHistory;

struct HistoryStack {
    adrHistory top;
};

struct BSTNode {
    SongPtr song;
    BSTNode *left;
    BSTNode *right;
};
typedef BSTNode* adrBST;

const int maxUser = 10;
const int maxPlaylist = 10;
const int maxFav = 50;
const int maxGenre = 3;

struct User {
    string username;
    adrPlaylist currentPlaylistNode;  // posisi lagu di playlist
    Playlist playlists[maxPlaylist];
    int playlistCount;
    int activePlaylist;
    HistoryStack history;
    SongPtr currentSong;
    bool pauseFlag;
    SongPtr favorites[maxFav];
    int favCount;
    int currentFavGenreIndex;
    string favGenres[maxGenre];
    int favGenreCount;
    bool fromPlaylist, fromLibrary, fromSearch;
    bool isPlaying;
    bool stopFlag;
    bool inPlayerMode;
    int remainingTime;
    thread timerThread;
};

extern vector<User*> users;  // Ubah dari array ke vector
extern int userCount;  // Tetap ada untuk count

// Admin
void createSongList(SongList &L);
SongPtr createSong(string id, string title, string genre, string artist, int duration);
void insertSong(SongList &L, SongPtr p);
void printAllSongs(const SongList &L);
void editSong(SongList &L);
void deleteSong(SongList &L, string id);
void removeSongFromAllPlaylists(Playlist &Q, const string &id);
void removeSongFromAllUsersPlaylists(const string &id);

// BST
void insertBST(adrBST &root, SongPtr song);
SongPtr searchBST(adrBST root, string id);

// User / Playlist
void createUser(string username, string favGenres[], int count);
User* findUser(string username);
void deleteUser(string username);
void initPlaylist(Playlist &Q, const string &name);
void addSongToActivePlaylist(User &u, SongPtr song);
void createUserPlaylist(User &u, string name);
void removeSongFromPlaylists(Playlist &Q, string id);
void printPlaylist(const Playlist &Q);
void showAndSelectPlaylist(User &u);

// History / Play
void initHistory(HistoryStack &S);
void pushHistory(HistoryStack &S, SongPtr song);
void printHistory(const HistoryStack &H);
SongPtr getHistoryByIndex(const HistoryStack &H, int index);
SongPtr popHistory(HistoryStack &S);
void playFromHistory(User &u, int index, SongList &L);
void playNextUser(User &u, SongList &L);
void pauseSong(User &u);
void resumeSong(User &u);
void playFromLibrary(User &u, SongPtr p);
int searchByTitle(const SongList &L, string keyword, SongPtr hasil[]);
void playFromSearch(User &u, SongPtr s, SongList &L);
void playSong(User &u, SongPtr s, SongList &L);
void showNowPlaying(const User &u);
void nextSong(User &u, SongList &L);  // Ubah: hapus SongList &L
void prevSong(User &u, SongList &L);  // Ubah: hapus SongList &L
void startTimer(User &u, SongList &L);
void stopTimer(User &u);

// Favorite & Genre
void addFavoriteToUser(User &u, SongPtr s);
void printUserFavorites(const User &u);
void playFavorite(User &u, int index, SongList &L);
void updateGenreFavorite(User &u, const string &genre);
string getFavoriteGenre(User &u);
void showGenreRecommendation(const SongList &L, const string &genre);
void inputThread(User &u, SongList &L);
void playSongWithInput(User &u, SongPtr s, SongList &L);
SongPtr getPrevSongByGenre(const SongList &L, SongPtr current, const string &genre);
SongPtr getNextSongByGenre(const SongList &L, SongPtr current, const string &genre) ;

// Exit
void handleExit(int signal);

// Helper
void removeFromHistory(HistoryStack &S, const string &id);

#endif