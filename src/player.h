#ifndef NUMBERS_STATION_PLAYER_H
#define NUMBERS_STATION_PLAYER_H

typedef struct Player {
    int x;
    int y;
    int movement_speed;
} Player;

void player_initialize(Player *player);
void player_update(Player *player);
void player_render(const Player *player);

#endif
