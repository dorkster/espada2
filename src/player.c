/*
    Espada 2
    Copyright (C) 2013 Justin Jacobs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>

#include "hazard.h"
#include "player.h"
#include "sys.h"

void playerInit() {
    player.x = (SCREEN_WIDTH/2) - (PLAYERW/2);
    player.y = SCREEN_HEIGHT - PLAYERH - 64;
}

void playerLogic() {
    playerMove();
    playerShoot();
}

void playerMove() {
    int speed = 4;

    if (action_left) player.x -= speed;
    else if (action_right) player.x += speed;
    if (action_up) player.y -= speed;
    else if (action_down) player.y += speed;

    if (player.x < 0) player.x = 0;
    else if (player.x > SCREEN_WIDTH-PLAYERW) player.x = SCREEN_WIDTH-PLAYERW;
    if (player.y < 0) player.y = 0;
    else if (player.y > SCREEN_HEIGHT-PLAYERH-32) player.y = SCREEN_HEIGHT-PLAYERH-32;
}

void playerShoot() {
    if (action_cooldown > 0) return;

    if (action_main1) {
        hazardAdd(HAZARD_PLAYER, HAZARD_GFX1, player.x, player.y, 10, 8);
        hazardAdd(HAZARD_PLAYER, HAZARD_GFX1, player.x+PLAYERW-HAZARD_SIZE, player.y, -10, 8);
    }

    action_cooldown = ACTION_COOLDOWN;
}
