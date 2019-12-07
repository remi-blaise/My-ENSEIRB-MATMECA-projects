#include "state.h"

aquarium_t aquarium = {};

void init_aquarium(int width, int height) {
    aquarium.width = width;
    aquarium.height = height;

    aquarium.views_size = VIEW_INIT_LIST_LENGTH;
    aquarium.views = malloc(sizeof(view_t) * aquarium.views_size);
    aquarium.nb_views = 0;

    aquarium.fishes_size = FISH_INIT_LIST_LENGTH;
    aquarium.fishes = malloc(sizeof(fish_t) * aquarium.fishes_size);
    aquarium.nb_fishes = 0;

    srand(time(NULL));
}

void free_aquarium() {
    free(aquarium.views); aquarium.views = NULL;
    free(aquarium.fishes); aquarium.fishes = NULL;
}

view_t * get_view_by_id(char * id) {
    for (int i = 0; i < aquarium.nb_views; ++i)
        if (strcmp(aquarium.views[i].id, id) == 0)
            return aquarium.views + i;

    return NULL;
}

view_t * get_view_by_socket(int fd_socket) {
    for (int i = 0; i < aquarium.nb_views; ++i)
        if (aquarium.views[i].fd_socket == fd_socket)
            return aquarium.views + i;

    return NULL;
}

int has_view_by_id(char * id) {
    for (int i = 0; i < aquarium.nb_views; ++i)
        if (strcmp(aquarium.views[i].id, id) == 0)
            return 1;

    return 0;
}

int has_view_by_socket(int fd_socket) {
    for (int i = 0; i < aquarium.nb_views; ++i)
        if (aquarium.views[i].fd_socket == fd_socket)
            return 1;

    return 0;
}

int add_view(char * id, int x, int y, int width, int height) {
    if (has_view_by_id(id)) return 1;

    if (aquarium.nb_views + 1 == aquarium.views_size) {
        aquarium.views_size *= 2;
        aquarium.views = realloc(aquarium.views, sizeof(view_t) * aquarium.views_size);
    }

    aquarium.views[aquarium.nb_views].id = malloc(strlen(id) + 1);
    memcpy(aquarium.views[aquarium.nb_views].id, id, strlen(id) + 1);
    aquarium.views[aquarium.nb_views].x = x;
    aquarium.views[aquarium.nb_views].y = y;
    aquarium.views[aquarium.nb_views].width = width;
    aquarium.views[aquarium.nb_views].height = height;
    aquarium.views[aquarium.nb_views].fd_socket = -1;
    aquarium.views[aquarium.nb_views].last_ping_timestamp = (unsigned)time(NULL);
    ++aquarium.nb_views;

    return 0;
}

int remove_view(char * id) {
    for (int i = 0; i < aquarium.nb_views; ++i)
    {
        if (strcmp(aquarium.views[i].id, id) == 0)
        {
            aquarium.views[i] = aquarium.views[--aquarium.nb_views];
            return 0;
        }
    }

    return 1;
}


char * allocate_view(int fd_socket) {
    for (int i=0; i<aquarium.nb_views; i++) {
        if (aquarium.views[i].fd_socket == -1) {
            aquarium.views[i].last_ping_timestamp = (unsigned)time(NULL);
            aquarium.views[i].fd_socket = fd_socket;
            return aquarium.views[i].id;
        }
    }
    return NULL;
}

void free_view(int fd_socket) {
    for (int i=0; i<aquarium.nb_views; i++) {
        if (aquarium.views[i].fd_socket == fd_socket) {
            aquarium.views[i].last_ping_timestamp = INT_MAX;
            aquarium.views[i].fd_socket = -1;
        }
    }
}

char * allocate_view_extended(int fd_socket, char * id) {
    for (int i=0; i<aquarium.nb_views; i++) {
        if (strcmp(aquarium.views[i].id, id) == 0 && aquarium.views[i].fd_socket == -1) {
            aquarium.views[i].last_ping_timestamp = (unsigned)time(NULL);
            aquarium.views[i].fd_socket = fd_socket;
            return aquarium.views[i].id;
        }
    }
    return allocate_view(fd_socket);
}

int has_fish(char * name) {
    for (int i = 0; i < aquarium.nb_fishes; ++i)
    {
        if (strcmp(aquarium.fishes[i].name, name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int add_fish(char * name, int x, int y, int width, int height, char * strategy) {
    if (has_fish(name)) return 0;

    if (strcmp(strategy, "RandomWayPoint") != 0) return -1;

    if (aquarium.nb_fishes + 1 == aquarium.fishes_size) {
        aquarium.fishes_size *= 2;
        aquarium.fishes = realloc(aquarium.fishes, sizeof(fish_t) * aquarium.fishes_size);
    }

    aquarium.fishes[aquarium.nb_fishes].name = malloc(strlen(name) + 1);
    strcpy(aquarium.fishes[aquarium.nb_fishes].name, name);
    aquarium.fishes[aquarium.nb_fishes].old_x = x;
    aquarium.fishes[aquarium.nb_fishes].old_y = y;
    aquarium.fishes[aquarium.nb_fishes].x = x;
    aquarium.fishes[aquarium.nb_fishes].y = y;
    aquarium.fishes[aquarium.nb_fishes].width = width;
    aquarium.fishes[aquarium.nb_fishes].height = height;
    aquarium.fishes[aquarium.nb_fishes].strategy = malloc(strlen(strategy) + 1);
    strcpy(aquarium.fishes[aquarium.nb_fishes].strategy, strategy);
    aquarium.fishes[aquarium.nb_fishes].started = 0;
    ++aquarium.nb_fishes;

    return 1;
}

int remove_fish(char * name) {
    for (int i = 0; i < aquarium.nb_fishes; ++i)
    {
        if (strcmp(aquarium.fishes[i].name, name) == 0)
        {
            aquarium.fishes[i] = aquarium.fishes[--aquarium.nb_fishes];
            return 1;
        }
    }
    return 0;
}

int start_fish(char * name) {
    for (int i = 0; i < aquarium.nb_fishes; ++i)
    {
        if (strcmp(aquarium.fishes[i].name, name) == 0)
        {
            aquarium.fishes[i].started = 1;
            return 1;
        }
    }
    return 0;
}

fish_t * get_fishes() {
    return aquarium.fishes;
}

int get_nb_fishes() {  //TODO
    return aquarium.nb_fishes;
}

void refresh( ) {
    for (int i = 0; i < aquarium.nb_fishes; ++i)
    {
        fish_t * fish = aquarium.fishes + i;
        // switch (fish->strategy) {
        //     case RANDOM_WAY_POINT_STRATEGY:
        //         applyRandomWayPointStrategy(fish);
        // }

        if (strcmp(fish->strategy, "RandomWayPoint") == 0) {
            applyRandomWayPointStrategy(fish);
        } else {
            printf("strategy %s not found\n", fish->strategy);
        }
        //
    }
}

// To be optimized (make sure that the destination is not too far from the fish)
void applyRandomWayPointStrategy(fish_t * fish) {
    // int actual_time = time(NULL);
    // if (aquarium.last_refreshing_timestamp + 5 < actual_time)
    // {
    if (fish->started == 1) {
        fish->old_x = fish->x;
        fish->old_y = fish->y;
        fish->x = rand() % aquarium.width;
        fish->y = rand() % aquarium.height;
    }
    // }
}

void get_fishes_by_socket(int fd_socket, char * response) {
    view_t * view = get_view_by_socket(fd_socket);
    if (view == NULL) {
        strcpy(response, "Vous n'êtes pas lié à aucune vue");
    }


    int empty = 1;
    int offset = 5; //the response contains the string "list ", which has a length of 5
    for (int i = 0; i < aquarium.nb_fishes; ++i)
    {
        fish_t * fish = aquarium.fishes + i;
        if ((fish->x >= view->x && fish->x <= view->x + view->width &&
             fish->y >= view->y && fish->y <= view->y + view->height) ||
            (fish->old_x >= view->x && fish->old_x <= view->x + view->width &&
             fish->old_y >= view->y && fish->old_y <= view->y + view->height)) {

             empty = 0;
             sprintf(response + offset, "[%s at %dx%d,%dx%d,%d] ", fish->name, fish->x, fish->y, fish->width, fish->height, time_to_refresh());
             offset = strlen(response);
         }
    }

    if (empty) strcpy(response + offset, "(vide)");

    strcpy(response + strlen(response), "\n");
}

int time_to_refresh() {
    return 5;
}
