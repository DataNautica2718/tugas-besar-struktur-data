#include "musik.h"
#include <iostream>
#include <limits>
#include <string>
#include <csignal>
using namespace std;

int main() {
    SongList library;
    createSongList(library);

    // Tambah dummy 15 lagu
    insertSong(library, createSong("S001","Lagu1","Pop","Artis1",18));
    insertSong(library, createSong("S002","Lagu2","Jazz","Artis2",20));
    insertSong(library, createSong("S003","Lagu3","Rock","Artis3",21));
    insertSong(library, createSong("S004","Lagu4","Pop","Artis4",19));
    insertSong(library, createSong("S005","Lagu5","Jazz","Artis5",23));
    insertSong(library, createSong("S006","Lagu6","Rock","Artis6",25));
    insertSong(library, createSong("S007","Lagu7","Pop","Artis7",17));
    insertSong(library, createSong("S008","Lagu8","Jazz","Artis8",22));
    insertSong(library, createSong("S009","Lagu9","Rock","Artis9",20));
    insertSong(library, createSong("S010","Lagu10","Pop","Artis10",24));
    insertSong(library, createSong("S011","Lagu11","Jazz","Artis11",21));
    insertSong(library, createSong("S012","Lagu12","Rock","Artis12",19));
    insertSong(library, createSong("S013","Lagu13","Pop","Artis13",18));
    insertSong(library, createSong("S014","Lagu14","Jazz","Artis14",23));
    insertSong(library, createSong("S015","Lagu15","Rock","Artis15",22));

    signal(SIGINT, handleExit);

    while(true){
        cout << "\n===== MENU AWAL =====\n";
        cout << "1. Admin\n";
        cout << "2. Pengguna\n";
        cout << "0. Exit\n";
        cout << "Pilihan: ";
        int mainChoice;
        if(!(cin >> mainChoice)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }

        if(mainChoice==0) handleExit(0);

        else if(mainChoice==1){ // ADMIN
            while(true){
                cout << "\n--- ADMIN MENU ---\n";
                cout << "1. Tambah lagu\n";
                cout << "2. Edit lagu\n";
                cout << "3. Hapus lagu\n";
                cout << "4. Lihat semua lagu\n";
                cout << "0. Back\nPilihan: ";
                int aChoice; cin >> aChoice;
                if(aChoice==0) break;
                else if(aChoice==1){
                    string id,title,genre,artist; int dur;
                    cout << "ID: "; cin >> id;
                    cout << "Judul: "; cin >> title;
                    cout << "Genre: "; cin >> genre;
                    cout << "Artis: "; cin >> artist;
                    cout << "Durasi (detik): "; cin >> dur;
                    insertSong(library, createSong(id,title,genre,artist,dur));
                } else if(aChoice==2){
                    editSong(library); // sesuaikan header & cpp
                } else if(aChoice==3){
                    string id; cout << "ID lagu yang dihapus: "; cin >> id;
                    deleteSong(library,id);
                } else if(aChoice==4){
                    printAllSongs(library);
                }
            }
        }

        else if(mainChoice==2){ // PENGGUNA
            string username;
            cout << "\nMasukkan username: "; cin >> username;
            User* u = findUser(username);
            if(!u){
                cout << "User tidak ditemukan, buat baru? (y/n): ";
                char yn; cin >> yn;
                if(yn=='y' || yn=='Y'){
                    string genres[maxGenre]; int genreCount;
                    cout << "Jumlah genre favorit (max "<< maxGenre <<"): "; cin >> genreCount;
                    for(int i=0;i<genreCount;i++){
                        cout << "Genre favorit "<<i+1<<": "; cin >> genres[i];
                    }
                    createUser(username, genres, genreCount);
                    u = findUser(username);
                } else continue;
            }

            while(true){
                cout << "\n===== MENU PENGGUNA =====\n";
                cout << "1. Library\n2. Search\n3. Playlist\n4. Favorite\n5. History\n6. User management\n0. Back\nPilihan: ";
                int uChoice; cin >> uChoice;
                if(uChoice==0) break;

                // LIBRARY
                if(uChoice==1){
                    printAllSongs(library);
                    int pilih; cout << "Pilih lagu (0 back): "; cin >> pilih;
                    if(pilih>0){
                        SongPtr p = library.first;
                        for(int i=1;i<pilih;i++) p=p->next;
                        playSongWithInput(*u,p,library);
                    }
                }
                // SEARCH
                else if(uChoice==2){
                    string keyword; cout << "Masukkan keyword: "; cin >> keyword;
                    SongPtr hasil[20]; int count = searchByTitle(library, keyword, hasil);
                    for(int i=0;i<count;i++)
                        cout << i+1 << ". " << hasil[i]->info.title << " - " << hasil[i]->info.artist << "\n";
                    int pilih; cout << "Pilih lagu (0 back): "; cin >> pilih;
                    if(pilih>0 && pilih<=count)
                        playSongWithInput(*u, hasil[pilih-1], library);
                }
                // PLAYLIST
                else if(uChoice==3){
                    while(true){
                        cout << "\n--- PLAYLIST MENU ---\n1. Buat playlist\n2. Lihat playlist\n0. Back\nPilihan: " << endl;
                        cout << endl;
                        int pChoice; cin >> pChoice;
                        if(pChoice==0) break;
                        else if(pChoice==1){
                            string name; cout << "Nama playlist: "; cin >> name;
                            createUserPlaylist(*u,name);
                        } else if(pChoice==2){
                            cout << "\n--- Playlist yang tersedia ---" << endl;
                            showAndSelectPlaylist(*u);
                            if(u->activePlaylist==-1) continue;
                            Playlist &pl = u->playlists[u->activePlaylist];
                            while(true){
                                cout << "\nPlaylist: " << pl.name << "\n";
                                printPlaylist(pl);
                                cout << "1. Play Playlist\n2. Tambah Lagu\n3. Hapus Lagu\n0. Back\nPilihan: ";
                                int plOpt; cin >> plOpt;
                                if(plOpt==0) break;
                                else if(plOpt==1){      
                                    if (pl.first){
                                        u->fromPlaylist = true;
                                        u->fromLibrary = false;
                                        u->fromSearch = false;

                                        u->currentPlaylistNode = pl.first;
                                        playSongWithInput(*u,pl.first->song,library);
                                    } else {
                                        cout << "Playlist kosong.\n";
                                    }
                                } else if(plOpt==2){
                                    printAllSongs(library);
                                    int pilihL; cout << "Pilih lagu (0 back): "; cin >> pilihL;
                                    if(pilihL>0){
                                        SongPtr p = library.first;
                                        for(int i=1;i<pilihL;i++) p=p->next;
                                        addSongToActivePlaylist(*u,p);
                                    }
                                } else if(plOpt==3){
                                    printPlaylist(pl);
                                    int pilihL; cout << "Pilih lagu hapus (0 back): "; cin >> pilihL;
                                    if(pilihL>0){
                                        adrPlaylist node = pl.first;
                                        for(int i=1;i<pilihL;i++) node=node->next;
                                        removeSongFromPlaylists(pl,node->song->info.id);
                                    }
                                }
                            }
                        }
                    }
                }
                // FAVORITE
                else if(uChoice==4){
                    if (u->favCount == 0){
                        cout << "Favorite kosong\n";
                        cout << "Ingin menambahkan lagu ke favorite? (y/n): ";
                        char yn; 
                        cin >> yn;
                        if(yn=='y' || yn=='Y'){
                            printAllSongs(library);
                            int pilihLagu; 
                            cout << "Pilih lagu (0 back): "; 
                            cin >> pilihLagu;
                            if(pilihLagu>0){
                                SongPtr p = library.first;
                                int counter = 1;
                                while (p && counter < pilihLagu){
                                    p = p->next;
                                    counter++;
                                }
                                if (p){
                                    addFavoriteToUser(*u, p);
                                } else {
                                    cout << "Lagu tidak ditemukan.\n";
                                }
                                
                            }
                        }
                    } else {
                        printUserFavorites(*u);
                        cout << "-----------------------\n";
                        cout << "1. Putar Lagu Favorite\n";
                        cout << "2. Tambah Favorite Baru\n";
                        cout << "3. Hapus Favorite\n";
                        cout << "0. Kembali\n";
                        cout << "pilihan: ";
                        int fChoice;
                        cin >> fChoice;
                        if(fChoice==0){
                            break;
                        } else if (fChoice==1){
                            int pilih; 
                            cout << "Pilih lagu favorite untuk diputar (0 back): "; 
                            cin >> pilih;
                            if(pilih>0){
                                playSongWithInput(*u,u->favorites[pilih-1],library);
                            }
                        } else if (fChoice==2){
                            printAllSongs(library);
                            int pilihLagu; 
                            cout << "Pilih lagu untuk ditambahkan ke favorite (0 back): "; 
                            cin >> pilihLagu;
                            if(pilihLagu>0){
                                SongPtr p = library.first;
                                int counter = 1;
                                while (p && counter < pilihLagu){
                                    p = p->next;
                                    counter++;
                                }
                                if (p){
                                    bool ada = false;
                                    for(int k = 0 ; k < u->favCount; k++){
                                        if(u->favorites[k]->info.id == p->info.id){
                                            ada = true;
                                            break;
                                        }
                                    }
                                    if (ada){
                                        cout << "Lagu sudah ada di favorite.\n";
                                    } else {
                                        addFavoriteToUser(*u, p);
                                    }
                                } else {
                                    cout << "Lagu tidak ditemukan.\n";
                                }
                            }
                        } else if (fChoice==3){
                            int hapus;
                            cout << "Pilih nomor lagu yang ingin di hapus (0 back): ";
                            cin >> hapus;
                            if(hapus > 0 && hapus <= u->favCount){
                                cout << "Lagu '" << u->favorites[hapus-1]->info.title << "' dihapus dari favorite.\n";
                                for(int k = hapus - 1; k < u->favCount - 1; k++){
                                    u->favorites[k] = u->favorites[k + 1];
                                }
                                u->favCount--;
                            } else if (hapus != 0){
                                cout << "Nomor Tidak Valid.\n";
                            }
                        }
                    }
                    
                }
                // HISTORY
                else if(uChoice==5){
                    printHistory(u->history);
                    int pilih; cout << "Pilih lagu play (0 back): "; cin >> pilih;
                    if(pilih>0) playSongWithInput(*u,getHistoryByIndex(u->history,pilih),library);
                }
                // USER MANAGEMENT
                else if(uChoice==6){
                    cout << "\n--- USER MANAGEMENT ---\n1. Hapus user\n0. Back\nPilihan: ";
                    int um; cin >> um;
                    if(um==1){
                        string uname; cout << "Username hapus: "; cin >> uname;
                        deleteUser(uname);
                        if(uname==u->username) break;
                    }
                }
            }
        }
    }

    return 0;
}
