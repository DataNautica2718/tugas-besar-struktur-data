#include "musik.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <atomic>
#include <conio.h>

using namespace std;

vector<User*> users;
int userCount = 0;
atomic<bool> isPaused(false);
atomic<bool> stopPlaying(false);

// ADMIN
void createSongList(SongList &L) { L.first = L.last = nullptr; }

SongPtr createSong(string id, string title, string genre, string artist, int duration) {
    SongPtr p = new adrSong;
    p->info = {id, title, genre, artist, duration};
    p->prev = p->next = nullptr;
    return p;
}

void insertSong(SongList &L, SongPtr p) {
    SongPtr check = L.first;
    while (check) {
        if (check->info.id == p->info.id) {
            cout << "ID lagu sudah ada, tidak bisa ditambahkan\n";
            delete p;
            return;
        }
        check = check->next;
    }
    if (L.first == nullptr) { L.first = L.last = p; return; }
    L.last->next = p;
    p->prev = L.last;
    L.last = p;
}

void printAllSongs(const SongList &L) {
    SongPtr p = L.first; 
    if (!p) { cout << "Library kosong.\n"; return; }
    int i = 1;
    cout << "================== Library ==================\n";
    while (p) { 
        cout << i++ << ". " << p->info.id << " | " << p->info.title << " | " << p->info.genre << " | " << p->info.artist << " | " << p->info.duration << "s\n"; 
        p = p->next;
    }
    cout << "---------------------------------------------\n";
}

void editSong(SongList &L) {
    string id; 
    cout << "Masukkan ID lagu yang diedit: "; 
    cin >> id;

    SongPtr s = L.first;
    while (s && s->info.id != id) {
        s = s->next;
    }

    if (!s) { 
        cout << "Lagu tidak ditemukan\n"; 
        return; 
    }

    cout << "Judul baru (" << s->info.title << "): "; cin >> s->info.title;
    cout << "Genre baru (" << s->info.genre << "): "; cin >> s->info.genre;
    cout << "Artis baru (" << s->info.artist << "): "; cin >> s->info.artist;
    cout << "Durasi baru (" << s->info.duration << "): "; cin >> s->info.duration;
    cout << "Lagu berhasil diedit\n";
}

void deleteSong(SongList &L, string id) {
    SongPtr p = L.first;
    while (p && p->info.id != id) p = p->next;
    if (!p) { cout << "Lagu tidak ditemukan\n"; return; }
    removeSongFromAllUsersPlaylists(id);
    if (p == L.first && p == L.last) L.first = L.last = nullptr;
    else if (p == L.first) { L.first = p->next; L.first->prev = nullptr; }
    else if (p == L.last) { L.last = p->prev; L.last->next = nullptr; }
    else { p->prev->next = p->next; p->next->prev = p->prev; }
    delete p; cout << "Lagu berhasil dihapus\n";
}

void removeSongFromAllPlaylists(Playlist &Q, const string &id) {
    adrPlaylist p = Q.first;
    while (p) {
        if (p->song->info.id == id) {
            adrPlaylist del = p;

            if (p->prev)
                p->prev->next = p->next;
            else
                Q.first = p->next;

            if (p->next)
                p->next->prev = p->prev;
            else
                Q.last = p->prev;

            p = p->next;
            delete del;
        } else {
            p = p->next;
        }
    }
}

void removeSongFromAllUsersPlaylists(const string &id) {
    for (int i = 0; i < userCount; i++) {
        for (int j = 0; j < users[i]->playlistCount; j++)
            removeSongFromPlaylists(users[i]->playlists[j], id);
        for (int f = 0; f < users[i]->favCount; f++) {
            if (users[i]->favorites[f] && users[i]->favorites[f]->info.id == id) {
                for (int k = f; k < users[i]->favCount - 1; k++) users[i]->favorites[k] = users[i]->favorites[k + 1];
                users[i]->favCount--; f--;
            }
        }
        if (users[i]->currentSong && users[i]->currentSong->info.id == id)
            users[i]->currentSong = nullptr;
        removeFromHistory(users[i]->history, id);
    }
}

// USER
void createUser(string username, string favGenres[], int count) {
    if (userCount >= maxUser) { cout << "User penuh\n"; return; }
    User* newUser = new User;
    newUser->username = username;
    newUser->playlistCount = 0; newUser->activePlaylist = -1;
    initHistory(newUser->history);
    newUser->currentSong = nullptr;
    newUser->currentPlaylistNode = nullptr;
    newUser->pauseFlag = false;
    newUser->currentFavGenreIndex = 0;
    newUser->favCount = 0;
    newUser->favGenreCount = 0;
    newUser->inPlayerMode = false;
    newUser->fromPlaylist = newUser->fromLibrary = false;
    newUser->fromSearch = false; newUser->isPlaying = false;
    newUser->stopFlag = false; newUser->remainingTime = 0;
    for (int g = 0; g < maxGenre; g++) newUser->favGenres[g] = "";
    for (int i = 0; i < count && i < maxGenre; i++) {
        newUser->favGenres[i] = favGenres[i];
        newUser->favGenreCount++;
    }
    users.push_back(newUser);
    userCount++;
    cout << "User '" << username << "' berhasil didaftarkan\n";
}

User* findUser(string username) {
    for (int i = 0; i < userCount; i++) if (users[i]->username == username) return users[i];
    return nullptr;
}

void deleteUser(string username) {
    for (int i = 0; i < userCount; i++) {
        if (users[i]->username == username) {
            for (int j = 0; j < users[i]->playlistCount; j++) {
                adrPlaylist p = users[i]->playlists[j].first;
                while (p) {
                    adrPlaylist temp = p;
                    p = p->next;
                    delete temp;
                }
            }
            adrHistory h = users[i]->history.top;
            while (h) {
                adrHistory temp = h;
                h = h->next;
                delete temp;
            }
            delete users[i];
            users.erase(users.begin() + i);
            userCount--;
            cout << "User '" << username << "' dihapus\n";
            return;
        }
    }
    cout << "User tidak ditemukan\n";
}

void initPlaylist(Playlist &Q, const string &name) { Q.name = name; Q.first = Q.last = nullptr; }
void addSongToActivePlaylist(User &u, SongPtr song) {
    if (u.activePlaylist == -1 || u.activePlaylist >= u.playlistCount) {
        cout << "Belum ada playlist aktif atau pilihan tidak valid.\n";
        return;
    }

    Playlist &active = u.playlists[u.activePlaylist];

    adrPlaylist node = new PlaylistNode{song, nullptr, nullptr};

    if (!active.first) {
        active.first = active.last = node;
    } else {
        node->prev = active.last;
        active.last->next = node;
        active.last = node;
    }

    cout << "Lagu berhasil ditambahkan ke playlist '" << active.name << "'\n";
}

void createUserPlaylist(User &u, string name) {
    if (u.playlistCount >= maxPlaylist) { cout << "Maksimal playlist tercapai\n"; return; }
    initPlaylist(u.playlists[u.playlistCount], name);
    cout << "Playlist '" << name << "' dibuat\n";
    if (u.activePlaylist == -1) u.activePlaylist = 0;
    u.playlistCount++;
}

void removeSongFromPlaylists(Playlist &Q, string id) {
    adrPlaylist p = Q.first;
    while (p && p->song->info.id != id)
        p = p->next;

    if (!p) return;

    if (p->prev)
        p->prev->next = p->next;
    else
        Q.first = p->next;

    if (p->next)
        p->next->prev = p->prev;
    else
        Q.last = p->prev;

    delete p;
}

void showAndSelectPlaylist(User &u) {
    if(u.playlistCount==0){
        cout << "Belum ada playlist.\n";
        u.activePlaylist = -1;
        return;
    }
    for(int i=0;i<u.playlistCount;i++)
        cout << i+1 << ". " << u.playlists[i].name << "\n";

    int pilih; cout << "Pilih playlist (0 back): "; cin >> pilih;
    if(pilih>0 && pilih <= u.playlistCount)
        u.activePlaylist = pilih-1;
    else
        u.activePlaylist = -1;
}

void printPlaylist(const Playlist &Q) {
    adrPlaylist p = Q.first; if (!p) { cout << "Playlist kosong\n"; return; }
    cout << "===== Playlist: " << Q.name << " =====\n"; int i = 1;
    while (p) { cout << i++ << ". " << p->song->info.id << " | " << p->song->info.title << " - " << p->song->info.artist << "\n"; p = p->next; }
}

// HISTORY
void initHistory(HistoryStack &S) { S.top = nullptr; }

void pushHistory(HistoryStack &S, SongPtr s) { S.top = new HistoryNode{s, S.top}; }
SongPtr popHistory(HistoryStack &S) {
    if (!S.top) return nullptr;
    SongPtr s = S.top->song; adrHistory n = S.top; S.top = S.top->next; delete n; return s;
}

int searchByTitle(const SongList &L, string keyword, SongPtr hasil[]) {
    int count = 0; SongPtr p = L.first;
    while (p && count < 20) { if (p->info.title.find(keyword) != string::npos) hasil[count++] = p; p = p->next; }
    return count;
}

// FAVORITE
void addFavoriteToUser(User &u, SongPtr s) {
    if (u.favCount >= maxFav) {
        cout << "Favorite penuh\n";
        return;
    }
    u.favorites[u.favCount++] = s;
    cout << "Ditambahkan ke favorite\n";
}

void printUserFavorites(const User &u) {
    if (u.favCount == 0) {
        cout << "Favorite kosong\n";
        return;
    }
    cout << "\n===== FAVORITE =====\n";
    for (int i = 0; i < u.favCount; i++) {
        cout << i + 1 << ". "
             << u.favorites[i]->info.title
             << " - " << u.favorites[i]->info.artist << "\n";
    }
}


// HISTORY
void printHistory(const HistoryStack &H) {
    if (!H.top) {
        cout << "History kosong\n";
        return;
    }

    cout << "===== HISTORY =====\n";
    adrHistory p = H.top;
    int i = 1;
    while (p) {
        cout << i++ << ". "
             << p->song->info.title << " - "
             << p->song->info.artist << "\n";
        p = p->next;
    }
}

SongPtr getHistoryByIndex(const HistoryStack &H, int index) {
    adrHistory p = H.top;
    int i = 1;
    while (p) {
        if (i == index)
            return p->song;
        p = p->next;
        i++;
    }
    return nullptr;
}

void removeFromHistory(HistoryStack &H, const string &id) {
    adrHistory cur = H.top, prev = nullptr;
    while (cur) {
        if (cur->song && cur->song->info.id == id) {
            adrHistory del = cur;
            if (!prev) {
                H.top = cur->next;
                cur = H.top;
            } else {
                prev->next = cur->next;
                cur = prev->next;
            }
            delete del;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

// START PEMUTARAN
void playSongWithInput(User &user, SongPtr song, SongList &library) {
    if (!song) return;

    user.currentSong = song;
    user.isPlaying = true;
    user.stopFlag = false;
    user.pauseFlag = false;
    user.remainingTime = song->info.duration;

    pushHistory(user.history, song);
    cout << "\nNow playing: " << song->info.title
         << " - " << song->info.artist << "\n";
    cout << "Controls: [n] Next, [p] Prev, [s] Pause, [r] Resume, [b] Back\n";

    while (!user.stopFlag) {
        if (!user.pauseFlag && user.remainingTime > 0) {
            this_thread::sleep_for(chrono::seconds(1));
            user.remainingTime--;
            cout << "\rSisa waktu: " << user.remainingTime << " detik  " << flush;

            if (user.remainingTime == 0) {
                if (user.fromPlaylist && user.currentPlaylistNode && user.currentPlaylistNode->next) {
                    user.currentPlaylistNode = user.currentPlaylistNode->next;
                    user.currentSong = user.currentPlaylistNode->song;
                    pushHistory(user.history, user.currentSong);
                    user.remainingTime = user.currentSong->info.duration;
                    cout << "\nAuto Next: " << user.currentSong->info.title
                         << " - " << user.currentSong->info.artist << "\n";
                } else { 
                    SongPtr next = getNextSongByGenre(library, user.currentSong, user.currentSong->info.genre);
                    if (next) {
                        user.currentSong = next;
                        pushHistory(user.history, next);
                        user.remainingTime = next->info.duration;
                        cout << "\nAuto Next: " << next->info.title
                             << " - " << next->info.artist << "\n";
                    } else {
                        cout << "\nLagu selesai, tidak ada next\n";
                        break;
                    }
                }
            }
        }

        if (_kbhit()) {
            char c = _getch();
            switch(c) {
                case 's': case 'S':
                    user.pauseFlag = true;
                    cout << "\nPaused\n"; break;
                case 'r': case 'R':
                    user.pauseFlag = false;
                    cout << "\nResumed\n"; break;
                case 'n': case 'N':
                    if (user.fromPlaylist && user.currentPlaylistNode && user.currentPlaylistNode->next) {
                        user.currentPlaylistNode = user.currentPlaylistNode->next;
                        user.currentSong = user.currentPlaylistNode->song;
                        pushHistory(user.history, user.currentSong);
                        user.remainingTime = user.currentSong->info.duration;
                        cout << "\nNext: " << user.currentSong->info.title
                             << " - " << user.currentSong->info.artist << "\n";
                    } else {
                        SongPtr next = getNextSongByGenre(library, user.currentSong, user.currentSong->info.genre);
                        if (next) {
                            user.currentSong = next;
                            pushHistory(user.history, next);
                            user.remainingTime = next->info.duration;
                            cout << "\nNext: " << next->info.title
                                 << " - " << next->info.artist << "\n";
                        } else cout << "\nTidak ada next\n";
                    }
                    break;
                case 'p': case 'P':
                    if (user.fromPlaylist && user.currentPlaylistNode && user.currentPlaylistNode->prev) {
                        user.currentPlaylistNode = user.currentPlaylistNode->prev;
                        user.currentSong = user.currentPlaylistNode->song;
                        pushHistory(user.history, user.currentSong);
                        user.remainingTime = user.currentSong->info.duration;
                        cout << "\nPrev: " << user.currentSong->info.title
                             << " - " << user.currentSong->info.artist << "\n";
                    } else {
                        SongPtr prev = getPrevSongByGenre(library, user.currentSong, user.currentSong->info.genre);
                        if (prev) {
                            user.currentSong = prev;
                            pushHistory(user.history, prev);
                            user.remainingTime = prev->info.duration;
                            cout << "\nPrev: " << prev->info.title
                                 << " - " << prev->info.artist << "\n";
                        } else cout << "\nTidak ada prev\n";
                    }
                    break;
                case 'b': case 'B':
                    user.stopFlag = true;
                    cout << "\nBack to menu\n"; break;
            }
        }
    }
    user.isPlaying = false;
    user.pauseFlag = false;
    user.stopFlag = false;
}

SongPtr getNextSongByGenre(const SongList &L, SongPtr current, const string &genre) {
    SongPtr p = current->next;
    while (p) {
        if (p->info.genre == genre) return p;
        p = p->next;
    }
    p = L.first;
    while (p && p != current) {
        if (p->info.genre == genre) return p;
        p = p->next;
    }
    return nullptr;
}

SongPtr getPrevSongByGenre(const SongList &L, SongPtr current, const string &genre) {
    SongPtr p = current->prev;
    while (p) {
        if (p->info.genre == genre) return p;
        p = p->prev;
    }
    p = L.last;
    while (p && p != current) {
        if (p->info.genre == genre) return p;
        p = p->prev;
    }
    return nullptr;
}

// EXIT
void handleExit(int) {
    cout << "\nProgram ditutup\n";
    exit(0);
}
