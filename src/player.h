#ifndef NUMBERS_STATION_PLAYER_H
#define NUMBERS_STATION_PLAYER_H

typedef struct Player {
    float x;
    float y;
    float width;
    float height;
    float movement_speed;
} Player;

void player_initialize(Player *player);
void player_update(Player *player, float delta_seconds);
void player_render(const Player *player);

#endif
