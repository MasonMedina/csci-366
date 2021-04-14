//
// Created by carson on 5/20/20.
//

#include <stdlib.h>
#include <stdio.h>
#include "game.h"
#include <string.h>

// STEP 9 - Synchronization: the GAME structure will be accessed by both players interacting
// asynchronously with the server.  Therefore the data must be protected to avoid race conditions.
// Add the appropriate synchronization needed to ensure a clean battle.

static game * GAME = NULL;

void game_init() {
    if (GAME) {
        free(GAME);
    }
    GAME = malloc(sizeof(game));
    GAME->status = CREATED;
    game_init_player_info(&GAME->players[0]);
    game_init_player_info(&GAME->players[1]);
}

void game_init_player_info(player_info *player_info) {
    player_info->ships = 0;
    player_info->hits = 0;
    player_info->shots = 0;
}


// Step 5 - This is the crux of the game.  You are going to take a shot from the given player and
// update all the bit values that store our game state.
//
//  - You will need up update the players 'shots' value
//  - you You will need to see if the shot hits a ship in the opponents ships value.  If so, record a hit in the
//    current players hits field
//  - If the shot was a hit, you need to flip the ships value to 0 at that position for the opponents ships field
//
//  If the opponents ships value is 0, they have no remaining ships, and you should set the game state to
//  PLAYER_1_WINS or PLAYER_2_WINS depending on who won.
int game_fire(game *game, int player, int x, int y) {
    unsigned long long int mask = xy_to_bitval(x, y);
    int opponent = (player + 1) % 2;
    game->players[player].shots = game->players->shots | mask;

    if (game->players[opponent].ships & mask) {
        game->players[player].hits = game->players[player].hits | mask;
        game->players[opponent].ships = game->players[opponent].ships ^= mask;
    }

    else{
        return 0;
    }

    if (game->players[opponent].ships == 0) {
        if(player == 1){
            GAME->status = PLAYER_1_WINS;
        }

        if(player == 0){
            GAME->status = PLAYER_0_WINS;
        }

    } else{
        if(player == 1){
            GAME->status = PLAYER_0_TURN;

        }
        if(player == 0){
            GAME->status = PLAYER_1_TURN;
        }

        return 1;

    }

    return 1;
}

// Step 1 - implement this function.  We are taking an x, y position
// and using bitwise operators, converting that to an unsigned long long
// with a 1 in the position corresponding to that x, y
//
// x:0, y:0 == 0b00000...0001 (the one is in the first position)
// x:1, y: 0 == 0b00000...10 (the one is in the second position)
// ....
// x:0, y: 1 == 0b100000000 (the one is in the eighth position)
//
// you will need to use bitwise operators and some math to produce the right
// value.
unsigned long long int xy_to_bitval(int x, int y) {

    unsigned long long value = 1ull <<x;
    value = 1ull << (y*8+x);

    if (x == 8 || y == 8 || x == -1 || y == -1) {
        return 0;
    }
    else {
        return value;
    }
}

struct game * game_get_current() {
    return GAME;
}
// Step 2 - implement this function.  Here you are taking a C
// string that represents a layout of ships, then testing
// to see if it is a valid layout (no off-the-board positions
// and no overlapping ships)
//
// if it is valid, you should write the corresponding unsigned
// long long value into the Game->players[player].ships data
// slot and return 1
//
// if it is invalid, you should return -1
int game_load_board(struct game *game, int player, char * spec) {

    struct player_info *playerInfo = &game->players[player];
    int opponent = (player + 1) % 2;

    int carrier_used = 0;
    int battleship_used = 0;
    int destroyer_used = 0;
    int submarine_used = 0;
    int patrolBoat_used = 0;

    if (spec == NULL || strlen(spec) != 15) {
        return -1;
    }
    for (int i = 0; i < 15; i += 3) {
        char ship_type = spec[i];
        int x = spec[i + 1] - '0';
        int y = spec[i + 2] - '0';


        if (ship_type == 'C' || ship_type == 'c') {
            if (carrier_used == 1) {
                return -1;
            }
            carrier_used = 1;
            if (ship_type == 'C') {
                if (add_ship_horizontal(playerInfo, x, y, 5) == -1) {
                    return -1;
                }
            } else if (ship_type == 'c') {
                if (add_ship_vertical(playerInfo, x, y, 5) == -1) {
                    return -1;
                }
            }
        }else if (ship_type == 'B' || ship_type == 'b') {
            if (battleship_used == 1) {
                return -1;
            }
            battleship_used = 1;
            if (ship_type == 'B') {
                if (add_ship_horizontal(playerInfo, x, y, 4) == -1) {
                    return -1;
                }

            } else if (ship_type == 'b') {
                if (add_ship_vertical(playerInfo, x, y, 4) == -1) {
                    return -1;
                }
            }
        }else if (ship_type == 'D' || ship_type == 'd') {
            if (destroyer_used == 1) {
                return -1;
            }
            destroyer_used = 1;
            if (ship_type == 'D') {
                if (add_ship_horizontal(playerInfo, x, y, 3) == -1) {
                    return -1;
                }

            } else if (ship_type == 'd') {
                if (add_ship_vertical(playerInfo, x, y, 3) == -1) {
                    return -1;
                }
            }
        }else if (ship_type == 'S' || ship_type == 's') {
            if (submarine_used == 1) {
                return -1;
            }
            submarine_used = 1;
            if (ship_type == 'S') {
                if (add_ship_horizontal(playerInfo, x, y, 3) == -1) {
                    return -1;
                }

            } else if (ship_type== 's') {
                if (add_ship_vertical(playerInfo, x, y, 3) == -1) {
                    return -1;
                }
            }
        }else if (ship_type == 'P' || ship_type == 'p') {
            if (patrolBoat_used == 1) {
                return -1;
            }
            patrolBoat_used = 1;
            if (ship_type == 'P') {
                if (add_ship_horizontal(playerInfo, x, y, 2) == -1) {
                    return -1;
                }

            } else {
                if (add_ship_vertical(playerInfo, x, y, 2) == -1) {
                    return -1;
                }
            }
            return 1;

        } else {
            return -1;
        }
        if (game->players[player].ships != 0 && game->players[opponent].ships != 0) {
            GAME->status = PLAYER_0_TURN;
        }
    }
    return 1;
}
// implement this as part of Step 2
// returns 1 if the ship can be added, -1 if not
// hint: this can be defined recursively
int add_ship_horizontal(player_info *player, int x, int y, int length) {
    unsigned long long bitmask = xy_to_bitval(x,y);

    if (length <= 0) {
        return 1;
    }
    if (bitmask == 0) {
        return -1;
    }

    if(player->ships & bitmask) {
        return -1;
    }
    else {
        player->ships = player->ships | bitmask;
        add_ship_horizontal(player, x + 1, y, length - 1);
    }

}
// implement this as part of Step 2
// returns 1 if the ship can be added, -1 if not
// hint: this can be defined recursively
int add_ship_vertical(player_info *player, int x, int y, int length) {
    unsigned long long bitmask = xy_to_bitval(x,y);

    if (length == 0) {
        return 1;
    }
        if (bitmask == 0) {
            return -1;
        }

    else if(player->ships & bitmask) {
        return -1;
    }
    else {
        player->ships = player->ships | bitmask;
        add_ship_vertical(player, x, y + 1, length - 1);
    }
}