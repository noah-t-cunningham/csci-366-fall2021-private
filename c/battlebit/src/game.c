//
// Created by carson on 5/20/20.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "game.h"

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

int game_fire(game *game, int player, int x, int y) {
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
}

unsigned long long int xy_to_bitval(int x, int y) {
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
    if (x >= 0 && x < 8 && y >= 0 && y < 8)
    {
        unsigned long long int mask = 1ull;
        int z = x + (y * 8);
        mask = mask << z;
        return mask;
    }
    else {return 0;}
}

struct game * game_get_current() {
    return GAME;
}

int game_load_board(struct game *game, int player, char * spec) {
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

    /*create char lists to check if ship is valid */
    char seen_ship[6]= {'z','z','z','z','z','\0'};
    char valid_ship[11] = {'c', 'C', 'b', 'B', 'd', 'D', 's', 'S', 'p', 'P', '\0'};
    /*get player info*/
    player_info *playerInfo = &game->players[player];
    /*check if spec is null*/
    if(spec == NULL){return -1;}
    /*check if spec is correct length */
    if(strlen(spec) != 15){return -1;}
    /*set cur to spec*/
    char *cur = spec;
    /*for loop that itertas through spec*/
    for( int i = 0; i < 5; i++) {
        /*gets the ship from spec*/
        char ship = *(cur + (i * 3));
        /* checks if ship is a valid ship*/
        int ship_check = 0;
        for(int j = 0; j < valid_ship[j] != '\0'; j++){
            if (valid_ship[j]== ship){ship_check++;}
        }
        if (ship_check != 1){return -1;}
        /* checks if ship has already been placed*/
        for(int q = 0; q < seen_ship[q] != '\0'; q++){
            if (toupper(ship) == seen_ship[q]){ return -1;}
        }
        seen_ship[i] = toupper(ship);

        /*gets cordinates*/
        char x_cord = *(cur + ((i*3) + 1));
        char y_cord = *(cur + ((i*3) + 2));
        /*convert cord to ints*/
        int int_x_cord = x_cord - '0';
        int int_y_cord = y_cord - '0';
        /*sets length*/
        int length;
        if(ship == 'C' || ship == 'c'){length = 5;}
        if(ship == 'B' || ship == 'b'){length = 4;}
        if(ship == 'D' || ship == 'd'){length = 3;}
        if(ship == 'S' || ship == 's'){length = 3;}
        if(ship == 'P' || ship == 'p'){length = 2;}
        /*call add ship fuunctions*/
        if (ship >= 'A' && ship <= 'Z'){
            int ash_int = add_ship_horizontal(playerInfo, int_x_cord, int_y_cord, length);
            if (ash_int == -1){return -1;}
        }
        else{
            int asv_int = add_ship_vertical(playerInfo, int_x_cord, int_y_cord, length);
            if (asv_int == -1){return -1;}
        }
    }

    return 1;
}

int add_ship_horizontal(player_info *player, int x, int y, int length) {
    // implement this as part of Step 2
    // returns 1 if the ship can be added, -1 if not
    // hint: this can be defined recursively

    /* lengths 0 you dont need to add anymore values*/
    if (length == 0){return 1;}

    /*check if x, y are on the board*/
    if(x < 0 || x > 7){
        return -1;
    }
    if(y < 0 || y > 7){
        return -1;
    }
    /*check if ship already at x, y*/
    unsigned long long int mask = xy_to_bitval(x,y);
    unsigned long long int location = player->ships;
    if(mask & player->ships){
        return -1;
    }

    /*flip players ship bit to 1*/
    player->ships = player->ships | mask;

    /*recusrivley calls add_ship_horizontal(manipulate x and y , decrease length by 1)
    base case is when length = 0*/
    add_ship_horizontal(player, x+1, y, length-1);
}

int add_ship_vertical(player_info *player, int x, int y, int length) {
    // implement this as part of Step 2
    // returns 1 if the ship can be added, -1 if not
    // hint: this can be defined recursively

    /* lengths 0 you dont need to add anymore values*/
    if (length == 0){return 1;}

    /*check if x, y are on the board*/
    if(x < 0 || x > 7){
        return -1;
    }
    if(y < 0 || y > 7){
        return -1;
    }
    /*check if ship already at x, y*/
    unsigned long long int mask = xy_to_bitval(x,y);
    unsigned long long int location = player->ships;
    if(mask & player->ships){
        return -1;
    }

    /*flip players ship bit to 1*/
    player->ships = player->ships | mask;

    /*recusrivley call add_ship_vertical(manipulate x and y , decrease length by 1)
    base case is when length = 0*/
    add_ship_vertical(player, x, y+1, length-1);
}