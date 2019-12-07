#include "cards.h"

void think(struct entity * caster, struct entity * target, struct game_data * game_data) {
  (void) target;
  (void) game_data;
  caster->mana_regen++;
  caster->mana -= 5;
}
const struct card _THINK = { "THINK", think, 5 };
const struct card * THINK = &_THINK;

void steal(struct entity * caster, struct entity * target, struct game_data * game_data) {
  (void) game_data;
  if (target->mana_regen > 1)
  {
    target->mana_regen--;
  }
  caster->mana_regen++;
  caster->mana -= 10;
}
const struct card _STEAL = { "STEAL", steal, 10 };
const struct card * STEAL = &_STEAL;

void panacea(struct entity * caster, struct entity * target, struct game_data * game_data) {
  (void) target;
  (void) game_data;
  caster->life += 10;
  caster->mana -= 2;
}
const struct card _PANACEA = { "PANACEA", panacea, 2 };
const struct card * PANACEA = &_PANACEA;

void razor(struct entity * caster, struct entity * target, struct game_data * game_data) {
  (void) game_data;
  target->life -= 10;
  caster->mana -= 2;
}
const struct card _RAZOR = { "RAZOR", razor, 2 };
const struct card * RAZOR = &_RAZOR;

void hell(struct entity * caster, struct entity * target, struct game_data * game_data) {
  (void) game_data;
  target->life = -50;
  caster->mana -= 100;
}
const struct card _HELL = { "HELL IS OTHERS", hell, 100 };
const struct card * HELL = &_HELL;

void stone(struct entity * caster, struct entity * target, struct game_data * game_data) {
  caster->mana -= 10;

  struct cell cells[4];
  int n_cells = 0;
  struct cell starting_cells[1] = {{ target->x, target->y }};
  int n_starting_cells = 1;
  find_reachable_cells(n_starting_cells, starting_cells, &n_cells, cells, 1, game_data);

  if (n_cells) {
    int i = randint(0, n_cells);
    game_data->board[cells[i].x][cells[i].y] = 1;
  }
}
const struct card _STONE = { "STONE", stone, 10 };
const struct card * STONE = &_STONE;

void puppy(struct entity * caster, struct entity * target, struct game_data * game_data) {
  caster->mana -= 5;

  struct cell cells[4];
  int n_cells = 0;
  struct cell starting_cells[1] = {{ caster->x, caster->y }};
  int n_starting_cells = 1;
  find_reachable_cells(n_starting_cells, starting_cells, &n_cells, cells, 1, game_data);

  if (n_cells) {
    game_data->entities[game_data->n_players + game_data->n_friends] = malloc(sizeof(struct entity));
    game_data->entities[game_data->n_players + game_data->n_friends]->type = FRIEND_TYPE;
    sprintf(game_data->entities[game_data->n_players + game_data->n_friends]->name, "Ami de %s", caster->name);
    game_data->entities[game_data->n_players + game_data->n_friends]->life = 1;
    game_data->entities[game_data->n_players + game_data->n_friends]->mana = 0;
    game_data->entities[game_data->n_players + game_data->n_friends]->mana_regen = 1;
    game_data->entities[game_data->n_players + game_data->n_friends]->target = target;
    game_data->entities[game_data->n_players + game_data->n_friends]->card = draw_random_card();
    game_data->entities[game_data->n_players + game_data->n_friends]->lifespan = 20;

    int i = randint(0, n_cells);
    game_data->board[cells[i].x][cells[i].y] = 1;
    game_data->entities[game_data->n_players + game_data->n_friends]->x = cells[i].x;
    game_data->entities[game_data->n_players + game_data->n_friends]->y = cells[i].y;

    game_data->n_friends++;
  }
}
const struct card _PUPPY = { "PUPPY", puppy, 5 };
const struct card * PUPPY = &_PUPPY;

const struct card * draw_random_card() {
  int i = randint(0, 161);

  if (i == 0)
  {
    return HELL;
  }
  else if (i <= 10)
  {
    return STEAL;
  }
  else if (i <= 30)
  {
    return THINK;
  }
  else if (i <= 80)
  {
    return PANACEA;
  }
  else if (i <= 130)
  {
    return RAZOR;
  }
  else if (i<= 140)
  {
    return STONE;
  }
  else
  {
    return PUPPY;
  }
}
