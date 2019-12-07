#include "protocol.h"

pthread_t broadcast_thread;
pthread_t timeout_thread;
int * observers;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void protocol_hello(char * response, struct pollfd * fd_array, int ind_socket) {
    if (has_view_by_socket(fd_array[ind_socket].fd)) {
        sprintf(response, "You are already connected to a view\n");
        return;
    }

    char * result = allocate_view(fd_array[ind_socket].fd);
    if (result != NULL) {
        sprintf(response, "greeting %s\n", result);
        printf("    -> [client %d] linked to view %s", ind_socket, result);
    }
    else {
        sprintf(response, "no greeting\n");
        printf(RED "    -> command hello failed from [client %d]: not enough views" RESET, ind_socket);
    }
}

void protocol_hello_extended(char * message, char * response, struct pollfd * fd_array, int ind_socket) {
    if (has_view_by_socket(fd_array[ind_socket].fd)) {
        sprintf(response, "You are already connected to a view\n");
        return;
    }

    int params_length = 3;
    char * params[params_length];
    char * regex = "([[:alpha:]]{2}) ([[:alpha:]]{2}) (N[[[:digit:]]+)";

    if (regex_match(message, regex, params, params_length)) {
        char * result = allocate_view_extended(fd_array[ind_socket].fd, params[2]);

        if (result != NULL) {
            sprintf(response, "greeting %s\n", result);
            printf("    -> client %d linked to view %s", ind_socket, result);
        }
        else {
            strcpy(response, "no greeting\n");
            printf(RED "    -> command hello failed : not enough views" RESET);
        }

        free_params(params, params_length);
    } else {
        printf(RED "    -> command hello failed : wrong syntax'" RESET);
        strcpy(response, "NOK\n");
    }
}

void protocol_ping(char * response, struct pollfd * fd_array, int ind_socket) {
    view_t * view = get_view_by_socket(fd_array[ind_socket].fd);

    if (view) {
        view->last_ping_timestamp = (unsigned)time(NULL);
    }

    strcpy(response, "pong\n");
}

void protocol_log_out(char * response) {
    strcpy(response, "bye\n");
}


//addFish PoissonRouge at 10x10, 4x3, Random
void protocol_add_fish(char * message, char * response) {
    int params_length = 7;
    char * params[params_length];
    char * regex = "([[:alpha:]]+) ([[:alpha:]]{2}) ([[:digit:]]{1,4})x([[:digit:]]{1,4}), ([[:digit:]]{1,4})x([[:digit:]]{1,4}), ([[:alpha:]]+)";

    if (regex_match(message, regex, params, params_length)) {
        int resp_add_fish = add_fish(params[0], atoi(params[2]), atoi(params[3]), atoi(params[4]), atoi(params[5]), params[6]);
        switch (resp_add_fish) {
            case 1:
                printf("    -> fish %s added", params[0]);
                strcpy(response, "OK\n");
                break;
            case 0:
                printf(RED "    -> command add fish failed : fish %s already exist" RESET, params[0]);
                strcpy(response, "NOK : poisson déja existant\n");
                break;
            case -1:
                printf(RED "    -> command add fish failed : strategy non supported" RESET);
                strcpy(response, "NOK : modèle de mobilité non supporté\n");
                break;
        }

    free_params(params, params_length);
    } else {
        printf(RED "    -> command add fish failed : wrong syntax" RESET);
        strcpy(response, "NOK\n");
    }
}

//delFish PoissonRouge
void protocol_del_fish(char * message, char * response) {
    unsigned params_length = 1;
    char * params[params_length];
    char * regex = "([[:alpha:]]+)";

    if (regex_match(message, regex, params, params_length)) {
        if (remove_fish(params[0])) {
            printf("    -> Fish %s deleted", params[0]);
            strcpy(response, "OK\n");
        } else {
            printf(RED "    -> command del fish failed : Fish %s doesn't exist." RESET, params[0]);
            strcpy(response, "NOK : poisson inexistant\n");
        }

        free_params(params, params_length);
    } else {
        printf(RED "  command del fish failed : wrong syntax" RESET);
        strcpy(response, "NOK\n");
    }

}

void protocol_start_fish(char * message, char * response) {
    if (start_fish(message))
        strcpy(response, "OK\n");
    else
        strcpy(response, "poisson inexistant\n");
}


void protocol_status(char * response) {
    fish_t * fishes = get_fishes();
    int nb_fishes = get_nb_fishes();
    int offset = 0;

    sprintf(response + offset, "OK : Connecté au contrôleur, %d poissons trouvés\n", nb_fishes);
    offset = strlen(response);

    for(int i=0; i<nb_fishes; i++) {
        sprintf(response + offset, "Fish %s at %dx%d,%dx%d\n", fishes[i].name, fishes[i].x, fishes[i].y, fishes[i].width, fishes[i].height);
        offset = strlen(response);
    }

    printf("    -> status");

}

void protocol_get_fishes(char * response, struct pollfd * fd_array, int ind_socket) {

    //make sure that client is linked to a view
    if (!has_view_by_socket(fd_array[ind_socket].fd)) {
        strcpy(response, "Vous n'êtes pas lié à aucune vue\n");
        return;
    }

    sprintf(response, "list ");
    get_fishes_by_socket(fd_array[ind_socket].fd, response);
}

void protocol_get_fishes_continously(char * response, int ind_socket) {

    int i=0;
    while(observers[i] != -1) i++;

    pthread_mutex_lock(&mutex);

    observers[i] = ind_socket;
    observers[i+1] = -1;

    pthread_mutex_unlock(&mutex);

    strcpy(response, "les poissons seront envoyés chaque 5 secondes\n");
}


void close_socket(int ind_socket) {
    int ind_end=0;  //indice de la dernière case du tableau observers
    int ind_sock=0;  //indice de la case qui comporte ind_socket

    pthread_mutex_lock(&mutex);

    while (observers[ind_end] != -1) {
        if (observers[ind_end] == ind_socket) ind_sock = ind_end;
        ind_end++;
    }

    // Shift vers la gauche
    for (int i = ind_sock; i < ind_end; i++) {
        observers[i] = observers[i+1];
    }

    pthread_mutex_unlock(&mutex);
}

void * routine_broadcast_thread(void * fd_array) {
    //refresh the positions of the fishes
    //Send a signal to all the thread waiting for cond every 5 seconds

    char buffer[BUFF_SIZE];
    struct pollfd * _fd_array = (struct pollfd *) fd_array;

    for(;;)
    {
        refresh();
        // printf("ping\n");
        int i=0;
        pthread_mutex_lock(&mutex);

        while (observers[i] != -1) {
            strcpy(buffer, "list ");
            get_fishes_by_socket(_fd_array[observers[i]].fd, buffer);

            int n = write(_fd_array[observers[i]].fd, buffer, BUFF_SIZE);
            if(n < 0)
                printf(RED "[client %d] : ERROR writing to socket\n\n" RESET, observers[i]);
                printf(GRN "$ " RESET);
            bzero(buffer, BUFF_SIZE);
            i++;
        }

        pthread_mutex_unlock(&mutex);

        sleep(5);
    }
}

// addFish PoissonRouge at 20x20, 2x2, RandomWayPoint
// addFish PoissonVert at 70x70, 2x2, RandomWayPoint
// addFish PoissonBleu at 75x75, 2x2, RandomWayPoint
// addFish PoissonRose at 5x5, 2x2, RandomWayPoint

void broadcast_thread_init(struct pollfd * fd_array) {
    observers = malloc(sizeof(int) * BUFF_SIZE);
    for (int i=0; i<BUFF_SIZE; i++) observers[i] = 0;
    observers[0] = -1;
    pthread_create(&broadcast_thread, NULL, routine_broadcast_thread, fd_array);
}


void * routine_timeout_thread(void * args) {
    config_t config = get_config();
    int TIMESTAMP = config.display_timeout_value;

    elem_t * arg = (elem_t *) args;
    struct pollfd * _fd_array = (struct pollfd *) arg->value;
    int * pollsize = arg->next->value;

    for(;;)
    {
        for (int i=2; i<(*pollsize); i++){
            if (_fd_array[i].fd != -1) {
                view_t * view = get_view_by_socket(_fd_array[i].fd);
                if (view) {
                    int timestamp = (unsigned)time(NULL);
                    if (timestamp - view->last_ping_timestamp > TIMESTAMP) {
                        int n = write(_fd_array[i].fd, "bye\n", 4);
                        if (n < 0)
                            printf("ERROR writing to socket\n");
                        free_view(_fd_array[i].fd);
                        close_socket(i);
                        close(_fd_array[i].fd);
                        _fd_array[i].fd = -1;
                        printf(BLEUE "    -> client %d disconnected due to timeout\n\n" RESET, i);
                        printf(GRN "$ " RESET);
                        fflush(stdout);
                    }
                }
            }
        }

        sleep(1);
    }
}


void timeout_thread_init(struct pollfd * fd_array, int * pollsize) {
    elem_t * head = malloc(sizeof(elem_t));
    elem_t * tail = malloc(sizeof(elem_t));
    head->value = fd_array;
    tail->value = pollsize;
    head->next = tail;
    tail->next = NULL;

    pthread_create(&timeout_thread, NULL, routine_timeout_thread, head);
}


void protocol_execute(char * message, char * response, struct pollfd* fd_array, int ind_socket) {

    printf(BLEUE);

    int length = strlen(message);
    message[length - 1] = '\0'; //change the \n with \0

    char verb[length];
    strcpy(verb, message);
    strtok(verb, " ");

    if (strcmp(message, "hello") == 0) {
        protocol_hello(response, fd_array, ind_socket);
    }
    else if (strcmp(verb, "hello") == 0) {
        message += strlen(verb) + 1;
        protocol_hello_extended(message, response, fd_array, ind_socket);
    }
    else if (strcmp(verb, "addFish") == 0) {
        message += strlen(verb) + 1;
        protocol_add_fish(message, response);
    }
    else if (strcmp(verb, "delFish") == 0) {
        message += strlen(verb) + 1;
        protocol_del_fish(message, response);
    }
    else if (strcmp(verb, "startFish") == 0) {
        message += strlen(verb) + 1;
        protocol_start_fish(message, response);
    }
    else if (strcmp(message, "status") == 0) {
        protocol_status(response);
    }
    else if (strcmp(verb, "log") == 0) {
        protocol_log_out(response);
    }
    else if (strcmp(message, "getFishes") == 0) {
        protocol_get_fishes(response, fd_array, ind_socket);
    }
    else if (strcmp(message, "getFishesContinously") == 0) {
        protocol_get_fishes_continously(response, ind_socket);
    }
    else if (strcmp(message, "ping") == 0) {
        protocol_ping(response, fd_array, ind_socket);
    }
    else {
        strcpy(response, "NOK : commande introuvable\n");
        printf(RED "  Unknown command \"%s\" from [client %d]" RESET, verb, ind_socket);
    }

    printf(RESET);
}
