#include "game.h"

void apply_card(struct entity * entity, int index_card, struct game_data * game_data) {
  if (entity->type == FRIEND_TYPE) {
    entity->card->apply(entity, entity->target, game_data);
    return;
  }

  // Chooses a random target
  int i;
  do {
    i = randint(0, game_data->n_players + game_data->n_friends);
  } while (game_data->entities[i] == entity);

  entity->hand[index_card]->apply(entity, game_data->entities[i], game_data);
}

void play_turn(struct entity * entity, struct game_data * game_data) {
  if (entity->type == PLAYER_TYPE) {
    shuffle_hand(entity);

    int index_card = select_card(entity);

    if (index_card == -1)
    {
      return;
    }

    apply_card(entity, index_card, game_data);
    entity->hand[index_card] = draw_card(entity->deck);
  } else if (entity->type == FRIEND_TYPE && entity->mana >= entity->card->cost) {
    apply_card(entity, -1, game_data);
  }
}

/**
 * The last living entity of the list takes the place of the dead one.
 */
void die(int index, struct game_data * game_data) {
  for (int i = 0; i < (game_data->n_players + game_data->n_friends); i++) {
    if (game_data->entities[i]->type == FRIEND_TYPE && game_data->entities[i]->target == game_data->entities[index]) {
      game_data->entities[i]->lifespan = 1;
      game_data->entities[i]->mana = 0;
      game_data->entities[i]->mana_regen = 0;
    }
  }

  if (game_data->entities[index]->type == PLAYER_TYPE) {
    game_data->n_players--;
  }
  if (game_data->entities[index]->type == FRIEND_TYPE) {
    game_data->n_friends--;
  }

  if (game_data->entities[index]->type == PLAYER_TYPE) {
    free(game_data->entities[index]->deck);
  }
  free(game_data->entities[index]);

  game_data->entities[index] = game_data->entities[game_data->n_players + game_data->n_friends];
}

void purge_the_dead(struct game_data * game_data) {
  for (int i = 0; i < game_data->n_players + game_data->n_friends; ++i)
  {
    char friend_lifespan_ended = game_data->entities[i]->type == FRIEND_TYPE && game_data->entities[i]->lifespan == 0;
    if (friend_lifespan_ended) {
      game_data->board[game_data->entities[i]->x][game_data->entities[i]->y] = 0;
    }

    if (friend_lifespan_ended || game_data->entities[i]->life <= 0) {
      die(i, game_data);
      i--;
    }
  }
}

void generate_mana(struct game_data * game_data) {
  for (int i = 0; i < game_data->n_players + game_data->n_friends; ++i)
  {
    game_data->entities[i]->mana += game_data->entities[i]->mana_regen;
  }
}

void decay_friends(struct game_data * game_data) {
  for (int i = 0; i < game_data->n_players + game_data->n_friends; i++) {
    if (game_data->entities[i]->type == FRIEND_TYPE) {
      game_data->entities[i]->lifespan--;
    }
  }
}

void play_game(int initial_n_players) {
  struct game_data _game_data;
  struct game_data * game_data = &_game_data;
  game_data->n_players = initial_n_players;
  game_data->n_friends = 0;

  for (int i = 0; i < game_data->n_players; ++i)
  {
    game_data->entities[i] = malloc(sizeof(struct entity));
    game_data->entities[i]->type = PLAYER_TYPE;
    game_data->entities[i]->life = 50;
    game_data->entities[i]->mana = 0;
    game_data->entities[i]->mana_regen = 1;
    sprintf(game_data->entities[i]->name, "player%d", i);
    game_data->entities[i]->deck = malloc(sizeof(struct deck));
    deal_deck(game_data->entities[i]->deck);
    deal_hand(game_data->entities[i]);
  }

  init_board(game_data);

  int n_turns = 0;
  while (game_data->n_players > 1) {
    move_entities(game_data);
    // move_entities can make players die
    if (game_data->n_players > 1) {
      for (int i = 0; i < game_data->n_players + game_data->n_friends; ++i)
      {
        play_turn(game_data->entities[i], game_data);
      }
      generate_mana(game_data);
      decay_friends(game_data);
      purge_the_dead(game_data);
    }

    printf("FIN DU TOUR %d\n\n", n_turns++);
    display_entities(game_data);
  }

  printf("Et le Vainqueur est ... ");

  if (game_data->n_players)
  {
    printf("%s !!!\n", game_data->entities[0]->name);
  } else {
    printf("personne oOo :choqué-déçu:\n");
  }

  for (int i = 0; i < game_data->n_players + game_data->n_friends; i++) {
    if (game_data->entities[i]->type == PLAYER_TYPE) {
      free(game_data->entities[i]->deck);
    }
    free(game_data->entities[i]);
  }
}
