#include "command.h"
#include "utils.h"
#include "state.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 200

void command_load(char * message, char * response) {
    if (!strlen(message))
    {
        printf(RED "  Command should be in the format 'save <filename>'" RESET);
        return;
    }

    FILE *fd = fopen(message, "r");
    if (fd == NULL) {
        printf(RED "  Error opening file." RESET);
        return;
    }

    char line[BUFFER_SIZE];
    int width, height = 0;

    if (fgets(line, BUFFER_SIZE, fd) != NULL)
    {
        unsigned params_length = 2;
        char * params[params_length];
        char * regex = "^([[:digit:]]+)x([[:digit:]]+)";

        if (regex_match(line, regex, params, params_length)) {
            width = atoi(params[0]);
            height = atoi(params[1]);
            free_params(params, params_length);
        } else {
            printf(RED "  First line should be in the format '<width>x<height>'" RESET);
            return;
        }
    }

    view_t * views = NULL;
    int views_n = 0;

    while (fgets(line, BUFFER_SIZE, fd) != NULL) {
        unsigned params_length = 5;
        char * params[params_length];
        char * regex = "^(N[[:digit:]]) ([[:digit:]]{1,4})x([[:digit:]]{1,4})\\+([[:digit:]]{1,4})\\+([[:digit:]]{1,4})";

        if (regex_match(line, regex, params, params_length)) {
            views = realloc(views, ++views_n * sizeof(view_t));
            views[views_n - 1].id = malloc(strlen(params[0]) + 1);
            memcpy(views[views_n - 1].id, params[0], strlen(params[0]) + 1);
            views[views_n - 1].x = atoi(params[1]);
            views[views_n - 1].y = atoi(params[2]);
            views[views_n - 1].width = atoi(params[3]);
            views[views_n - 1].height = atoi(params[4]);
            free_params(params, params_length);
        } else {
            printf(RED "  Line %d should be in the format 'N<number> <width>x<height>+<x>+<y>'" RESET, views_n);
            return;
        }
    }

    fclose(fd);

    init_aquarium(width, height);
    for (int i = 0; i < views_n; ++i) {
        add_view(views[i].id, views[i].x, views[i].y, views[i].width, views[i].height);
        free(views[i].id);
    }
    free(views);

    printf("    -> aquarium loaded ! (%d display view)", aquarium.nb_views);
}

void command_save(char * message, char * response) {
    if (!strlen(message))
    {
        printf(RED "  Command should be in the format 'save <filename>'" RESET);
        return;
    }

    FILE *fd = fopen(message, "w");
    if (fd == NULL) {
        printf(RED "  Error opening file." RESET);
        return;
    }

    fprintf(fd, "%dx%d\n", aquarium.width, aquarium.height);

    for (int i = 0; i < aquarium.nb_views; ++i) {
        view_t * view = aquarium.views + i;
        fprintf(fd, "%s %dx%d+%d+%d\n", view->id, view->x, view->y, view->width, view->height);
    }

    fclose(fd);

    printf("    -> Aquarium saved ! (%d display view)", aquarium.nb_views);
}

void command_show(char * message, char * response) {
    printf("  %dx%d\n", aquarium.width, aquarium.height);

    for (int i = 0; i < aquarium.nb_views; ++i) {
        view_t * view = aquarium.views + i;
        printf("  %s %dx%d+%d+%d\n", view->id, view->x, view->y, view->width, view->height);
    }
}

void command_add_view(char * message, char * response) {
    unsigned params_length = 5;
    char * params[params_length];
    char * regex = "^view (N[[:digit:]]) ([[:digit:]]{1,4})x([[:digit:]]{1,4})\\+([[:digit:]]{1,4})\\+([[:digit:]]{1,4})";

    if (regex_match(message, regex, params, params_length)) {
        if (!add_view(params[0], atoi(params[3]), atoi(params[4]), atoi(params[1]), atoi(params[2]))) {
            printf("    -> view added");
        } else {
            printf(RED "  View %s already exist." RESET, params[0]);
        }

        free_params(params, params_length);
    } else {
        printf(RED "  Command should be in the format 'add view N<number> <width>x<height>+<x>+<y>'" RESET);
    }
}

void command_del_view(char * message, char * response) {
    unsigned params_length = 1;
    char * params[params_length];
    char * regex = "^view (N[[[:digit:]])";

    if (regex_match(message, regex, params, params_length)) {
        if (!remove_view(params[0])) {
            printf("    -> view %s deleted", params[0]);
        } else {
            printf(RED "  View %s doesn't exist." RESET, params[0]);
        }

        free_params(params, params_length);
    } else {
        printf(RED "  Command should be in the format 'del view N<number>'" RESET);
    }
}

void command_execute(char * message, char * response)
{
    printf(BLEUE);

    int length = strlen(message);
    message[length - 1] = '\0';

    char verb[length];
    strcpy(verb, message);
    char * result = strtok(verb, " ");

    message += strlen(verb) + 1;

    if (strcmp(verb, "load") == 0) {
        command_load(message, response);
    }
    else if (strcmp(verb, "show") == 0) {
        command_show(message, response);
    }
    else if (strcmp(verb, "add") == 0) {
        command_add_view(message, response);
    }
    else if (strcmp(verb, "del") == 0) {
        command_del_view(message, response);
    }
    else if (strcmp(verb, "save") == 0) {
        command_save(message, response);
    }
    else {
        printf(RED "  Unknown command %s\n" RESET, verb);
    }

    printf(RESET);
}
