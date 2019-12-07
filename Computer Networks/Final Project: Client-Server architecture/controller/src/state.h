#ifndef STATE_H
#define STATE_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <limits.h>

#define VIEW_INIT_LIST_LENGTH 10
#define FISH_INIT_LIST_LENGTH 20

#define RANDOM_WAY_POINT_STRATEGY 0


typedef struct view_s {
    char * id;
    int x;
    int y;
    int width;
    int height;

    int fd_socket; //socket relatif à la vue

    int last_ping_timestamp;
} view_t;

typedef struct fish_s {
    char * name;
    int old_x;
    int old_y;
    int x;
    int y;
    int width;
    int height;
    char * strategy;
    int started;
} fish_t;

typedef struct aquarium_s {
    int width;
    int height;

    view_t * views;
    int nb_views; // nombres de vues créées
    int views_size; // la capacité du tableau views

    fish_t * fishes;
    int nb_fishes; // nombre de poisson créés
    int fishes_size; // capacité du tableau fishes

    int last_refreshing_timestamp;
} aquarium_t;

extern aquarium_t aquarium;

void init_aquarium(int width, int height);

int has_view_by_id(char * id);
int has_view_by_socket(int fd_socket);
view_t * get_view_by_id(char * id);
view_t * get_view_by_socket(int fd_socket);

int add_view(char * id, int x, int y, int width, int height);
int remove_view(char * id);

char * allocate_view(int fd_socket);
char * allocate_view_extended(int fd_socket, char * id);
void free_view(int fd_socket);

int add_fish(char * name, int x, int y, int width, int height, char * strategy);
int remove_fish(char * name);
int start_fish(char * name);
fish_t * get_fishes();
int get_nb_fishes();

void refresh();
void applyRandomWayPointStrategy(fish_t * fish);
void get_fishes_by_socket(int fd_socket, char * response);

int time_to_refresh();
#endif
