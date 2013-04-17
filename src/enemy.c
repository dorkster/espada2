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

Enemy* enemies[ENEMY_MAX];
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
        if (enemies[i] != NULL) {
            if (enemies[i]->active) {
                if (enemies[i]->logic == ENEMY_LOGIC1) {
                    enemies[i]->pos.y += enemies[i]->speed_y;
                    if (enemies[i]->shoot_timer == 0) {
                        hazardAdd(HAZARD_ENEMY, HAZARD_GFX2, enemies[i]->pos.x, enemies[i]->pos.y+enemies[i]->pos.h, 225, 4);
                        hazardAdd(HAZARD_ENEMY, HAZARD_GFX2, enemies[i]->pos.x+enemies[i]->pos.w-HAZARD_SIZE, enemies[i]->pos.y+enemies[i]->pos.h, 135, 4);
                        enemies[i]->shoot_timer = 120;
                    } else enemies[i]->shoot_timer--;
                }
            }
            if (enemies[i]->pos.y > SCREEN_HEIGHT) {
                enemyReset(i);
            }
        }
    }
}

void enemyAdd(int logic, int gfx, int x, int y) {
    int i;
    for (i=0; i<ENEMY_MAX; i++) {
        if (enemies[i] == NULL) {
            enemies[i] = (Enemy*) malloc(sizeof(Enemy));
            if (enemies[i] == NULL) return;

            enemies[i]->active = true;
            enemies[i]->logic = logic;
            enemies[i]->shoot_timer = 0;

            // set up graphics
            if (gfx == ENEMY_GFX1) {
                enemies[i]->gfx = surface_enemy1;
                enemies[i]->pos.w = 64;
                enemies[i]->pos.h = 32;
            }

            // we center the enemy on the given x pos
            enemies[i]->pos.x = x - (enemies[i]->pos.w/2);
            // y will typically be 0, so this will start enemies off screen
            enemies[i]->pos.y = y - enemies[i]->pos.h;

            // set speed
            if (logic == ENEMY_LOGIC1) {
                enemies[i]->speed_x = 0;
                enemies[i]->speed_y = 1;
            }

            enemy_total++;
            break;
        }
    }
}

void enemyReset(int i) {
    if (enemies[i] != NULL) {
        free(enemies[i]);
        enemies[i] = NULL;
    }

    if (enemy_total > 0) enemy_total--;
}

void enemyCreateWave() {
    if (enemy_total > 0) return;

    // TODO make this change based on level

    enemyAdd(ENEMY_LOGIC1, ENEMY_GFX1, 128, 0);
    enemyAdd(ENEMY_LOGIC1, ENEMY_GFX1, SCREEN_WIDTH-128, 0);
}

void enemyKill(int i) {
    // TODO switch to death animation and wait for it to finish before removing the enemy
    enemyReset(i);
}
