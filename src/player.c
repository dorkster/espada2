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

#include "fileparser.h"
#include "hazard.h"
#include "player.h"
#include "sys.h"

void playerInit() {
    FileParser* f;
    player.speed = 4;
    player.bullets = NULL;
    player.bullet_count = 0;

    // load some stats from a config file
    f = fileOpen(PKGDATADIR "/configs/player.txt");
    while(fileNext(f)) {
        if (!strcmp("width",fileGetKey(f))) player.pos.w = atoi(fileGetVal(f));
        else if (!strcmp("height",fileGetKey(f))) player.pos.h = atoi(fileGetVal(f));
        else if (!strcmp("speed",fileGetKey(f))) player.speed = atoi(fileGetVal(f));
        else if (!strcmp("bullet",fileGetKey(f))) {
            int x_offset = atoi(fileGetValNext(f));
            int y_offset = atoi(fileGetValNext(f));
            int angle = atoi(fileGetValNext(f));
            int speed = atoi(fileGetValNext(f));
            player.bullet_count++;
            player.bullets = hazardDefAdd(player.bullet_count, player.bullets, x_offset, y_offset, angle, speed);
        }
    }
    fileClose(f);

    player.pos.x = (SCREEN_WIDTH/2) - (player.pos.w/2);
    player.pos.y = SCREEN_HEIGHT - player.pos.h - 64;
}

void playerCleanup() {
    if (player.bullets != NULL) {
        free(player.bullets);
        player.bullets = NULL;
    }
}

void playerLogic() {
    playerMove();
    playerShoot();
}

void playerMove() {
    if (action_left) player.pos.x -= player.speed;
    else if (action_right) player.pos.x += player.speed;
    if (action_up) player.pos.y -= player.speed;
    else if (action_down) player.pos.y += player.speed;

    if (player.pos.x < 0) player.pos.x = 0;
    else if (player.pos.x > SCREEN_WIDTH-player.pos.w) player.pos.x = SCREEN_WIDTH-player.pos.w;
    if (player.pos.y < 0) player.pos.y = 0;
    else if (player.pos.y > SCREEN_HEIGHT-player.pos.h-MENUBAR_SIZE) player.pos.y = SCREEN_HEIGHT-player.pos.h-MENUBAR_SIZE;
}

void playerShoot() {
    if (action_cooldown > 0) return;

    if (action_main1) {
        int i;
        for (i=0; i<player.bullet_count; i++) {
            hazardAdd(HAZARD_PLAYER,
                      HAZARD_GFX1,
                      sysGetXCenter(player.pos)+player.bullets[i].x_offset-(HAZARD_SIZE/2),
                      sysGetYCenter(player.pos)+player.bullets[i].y_offset-(HAZARD_SIZE/2),
                      (float)player.bullets[i].angle,
                      player.bullets[i].speed);
        }
    }

    action_cooldown = ACTION_COOLDOWN;
}

void playerKill() {
    // TODO switch to death animation and wait for it to complete before removing a life
    trigger_game_over = true;
}
