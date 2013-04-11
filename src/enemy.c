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

#include <math.h>
#include <stdlib.h>

#include "enemy.h"
#include "hazard.h"
#include "sys.h"

Enemy enemies[ENEMY_MAX];
int enemy_total = 0;

void enemyInit() {
    int i;
    for (i=0; i<ENEMY_MAX; i++) {
        enemyReset(i);
    }
}

void enemyLogic() {
    enemyCreateWave();

    int i;
    for (i=0; i<ENEMY_MAX; i++) {
        if (enemies[i].active) {
            if (enemies[i].logic == ENEMY_LOGIC1) {
                enemies[i].y += enemies[i].speed_y;
                if (enemies[i].shoot_timer == 0) {
                    hazardAdd(HAZARD_ENEMY, HAZARD_GFX2, enemies[i].x, enemies[i].y+enemies[i].h, 225, 4);
                    hazardAdd(HAZARD_ENEMY, HAZARD_GFX2, enemies[i].x+enemies[i].w-HAZARD_SIZE, enemies[i].y+enemies[i].h, 135, 4);
                    enemies[i].shoot_timer = 60;
                } else enemies[i].shoot_timer--;
            }
        }
        if (enemies[i].y > SCREEN_HEIGHT) {
            enemyReset(i);
        }
    }
}

void enemyAdd(int logic, int gfx, int x, int y) {
    int i;
    for (i=0; i<ENEMY_MAX; i++) {
        if (!enemies[i].active) {
            enemies[i].active = true;
            enemies[i].logic = logic;

            // set up graphics
            if (gfx == ENEMY_GFX1) {
                enemies[i].gfx = surface_enemy1;
                enemies[i].w = 64;
                enemies[i].h = 32;
            }

            // we center the enemy on the given x pos
            enemies[i].x = x - (enemies[i].w/2);
            // y will typically be 0, so this will start enemies off screen
            enemies[i].y = y - enemies[i].h;

            // set speed
            if (logic == ENEMY_LOGIC1) {
                enemies[i].speed_x = 0;
                enemies[i].speed_y = 1;
            }

            enemy_total++;
            break;
        }
    }
}

void enemyReset(int i) {
    enemies[i].x = 0;
    enemies[i].y = 0;
    enemies[i].w = 0;
    enemies[i].h = 0;
    enemies[i].speed_x = 0;
    enemies[i].speed_y = 0;
    enemies[i].logic = -1;
    enemies[i].gfx = NULL;
    enemies[i].active = false;
    enemies[i].shoot_timer = 0;

    if (enemy_total > 0) enemy_total--;
}

void enemyCreateWave() {
    if (enemy_total > 0) return;

    // TODO make this change based on level

    enemyAdd(ENEMY_LOGIC1, ENEMY_GFX1, 128, 0);
    enemyAdd(ENEMY_LOGIC1, ENEMY_GFX1, SCREEN_WIDTH-128, 0);
}
