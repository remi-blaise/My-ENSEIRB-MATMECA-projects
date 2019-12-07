#include "entity.h"

void display_player(struct entity * player) {
  printf(
    "%s : life=%d, mana=%d, mana_regen=%d, hand=(",
    player->name,
    player->life,
    player->mana,
    player->mana_regen
  );

  for (int i = 0; i < HAND_SIZE; ++i)
  {
    printf("%s", player->hand[i]->name);
    if (i != HAND_SIZE - 1)
    {
      printf(", ");
    }
  }
  printf("), position=(%d, %d)\n", player->x, player->y);
}

void display_puppy(struct entity * player) {
  printf(
    "%s : target=%s, life=%d, lifespan=%d, mana=%d, mana_regen=%d, card=%s, position=(%d, %d)\n",
    player->name,
    player->target->name,
    player->life,
    player->lifespan,
    player->mana,
    player->mana_regen,
    player->card->name,
    player->x, player->y
  );
}

void display_entities(struct game_data * game_data) {
  for (int i = 0; i < game_data->n_players + game_data->n_friends; ++i)
  {
    if (game_data->entities[i]->type == PLAYER_TYPE) {
      display_player(game_data->entities[i]);
    }
    if (game_data->entities[i]->type == FRIEND_TYPE) {
      display_puppy(game_data->entities[i]);
    }
  }
}

void deal_hand(struct entity * player) {
  for (int i = 0; i < HAND_SIZE; ++i)
  {
    player->hand[i] = draw_card(player->deck);
  }
}

void shuffle_hand(struct entity * player) {
  for(int i = 0; i < HAND_SIZE; i++) {
    int j = randint(0, HAND_SIZE);
    const struct card * tmp = player->hand[i];
    player->hand[i] = player->hand[j];
    player->hand[j] = tmp;
  }
}

/**
 * Chooses the first playable card
 * Returns its position in the hand or -1 if no card is playable
 */
int select_card(struct entity * player) {
  for (int i = 0; i < HAND_SIZE; ++i) {
    if (player->hand[i]->cost <= player->mana) {
      return i;
    }
  }
  return -1;
}
