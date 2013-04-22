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
#include "fileparser.h"
#include "hazard.h"
#include "player.h"
#include "sys.h"

Enemy* enemies[ENEMY_MAX];
Enemy* enemy_stats;
EnemyWave* enemy_wave;
int enemy_total = 0;
int enemy_type_max = 0;
int enemy_level_max = 0;
int enemy_wave_timer = 0;
int enemy_wave_timer_max = 0;

void enemyInit() {
    FileParser* f;

    int i;
    for (i=0; i<ENEMY_MAX; i++) {
        enemyReset(i);
    }

    enemy_total = 0;
    enemy_type_max = 0;
    enemy_level_max = 0;

    // load enemy definitions
    int current_type = 0;
    f = fileOpen(PKGDATADIR "/configs/enemies.txt");
    while(fileNext(f)) {
        if (!strcmp("id",fileGetKey(f))) {
            current_type = atoi(fileGetVal(f));
            if (current_type > enemy_type_max) {
                enemy_type_max = current_type;
                enemy_stats = realloc(enemy_stats, sizeof(Enemy) * enemy_type_max);
                enemyInitEnemy(&enemy_stats[current_type-1]);
                enemy_stats[current_type-1].type = current_type;
            }
        } else if (current_type > 0) {
            if (!strcmp("speed_x",fileGetKey(f))) enemy_stats[current_type-1].speed_x = atoi(fileGetVal(f));
            else if (!strcmp("speed_y",fileGetKey(f))) enemy_stats[current_type-1].speed_y = atoi(fileGetVal(f));
            else if (!strcmp("shoot_time",fileGetKey(f))) enemy_stats[current_type-1].shoot_timer_max = atoi(fileGetVal(f));
            else if (!strcmp("move_time",fileGetKey(f))) enemy_stats[current_type-1].move_timer_max = atoi(fileGetVal(f));
            else if (!strcmp("homing",fileGetKey(f))) enemy_stats[current_type-1].homing = atoi(fileGetVal(f));
            else if (!strcmp("boss",fileGetKey(f))) enemy_stats[current_type-1].boss = atoi(fileGetVal(f));
            else if (!strcmp("bullet",fileGetKey(f))) {
                int x_offset = atoi(fileGetValNext(f));
                int y_offset = atoi(fileGetValNext(f));
                int angle = atoi(fileGetValNext(f));
                int speed = atoi(fileGetValNext(f));
                enemy_stats[current_type-1].bullet_count++;
                enemy_stats[current_type-1].bullets = hazardDefAdd(enemy_stats[current_type-1].bullet_count, enemy_stats[current_type-1].bullets, x_offset, y_offset, angle, speed);
            } else if (!strcmp("animation",fileGetKey(f))) enemy_stats[current_type-1].anim = animationAdd(enemy_stats[current_type-1].anim, &enemy_stats[current_type-1].anim_count, fileGetVal(f));
        }
    }
    fileClose(f);

    // load level layouts
    int current_level = 0;
    f = fileOpen(PKGDATADIR "/configs/waves.txt");
    while(fileNext(f)) {
        if (!strcmp("wave_time",fileGetKey(f))) {
            enemy_wave_timer_max = FPS * atoi(fileGetVal(f));
        } else if (!strcmp("level",fileGetKey(f))) {
            current_level = atoi(fileGetVal(f));
            if (current_level > enemy_level_max) {
                enemy_level_max = current_level;
                enemy_wave = realloc(enemy_wave, sizeof(EnemyWave) * enemy_level_max);
                enemyInitWave(&enemy_wave[current_level-1]);
            }
        } else if (current_level > 0 && current_level <= enemy_level_max) {
            if (!strcmp("wave",fileGetKey(f))) {
                for (i=0; i<8; i++) {
                    enemy_wave[current_level-1].sector[i] = atoi(fileGetValNext(f));
                }
            }
        }
    }
    fileClose(f);

    enemy_wave_timer = enemy_wave_timer_max/2;
}

void enemyInitEnemy(Enemy* e) {
    e->pos.x = e->pos.y = e->pos.w = e->pos.h = 0;
    e->speed_x = e->speed_y = 0;
    e->type = 0;
    e->gfx = NULL;
    e->alive = false;
    e->shoot_timer = e->shoot_timer_max = 0;
    e->move_timer = e->move_timer_max = 0;
    e->homing = 0;
    e->boss = 0;
    e->bullets = NULL;
    e->bullet_count = 0;
    e->anim = NULL;
    e->anim_count = 0;
    e->anim_current = 0;
}

void enemyInitWave(EnemyWave* wave) {
    int i;
    for (i=0; i<8; i++) {
        wave->sector[i] = -1;
    }
}

void enemyCleanup() {
    int i;
    if (enemy_stats != NULL) {
        for (i=0; i<enemy_type_max; i++) {
            if (enemy_stats[i].bullets != NULL) {
                free(enemy_stats[i].bullets);
            }
            animationCleanup(&enemy_stats[i].anim, &enemy_stats[i].anim_count);
        }
        free(enemy_stats);
        enemy_stats = NULL;
    }
    if (enemy_wave != NULL) free(enemy_wave);
    enemy_wave = NULL;

    for (i=0; i<ENEMY_MAX; i++) {
        enemyReset(i);
    }
}

void enemyLogic() {
    enemyCreateWave();

    int i;
    for (i=0; i<ENEMY_MAX; i++) {
        if (enemies[i] != NULL) {
            if (enemies[i]->anim_count > 0) animationAdvanceFrame(&enemies[i]->anim[enemies[i]->anim_current-1]);
            // printf("%d\n",enemies[i]->anim[enemies[i]->anim_current-1].frame_total);
            if (enemies[i]->alive) {
                if (enemies[i]->move_timer == 0) {
                    enemies[i]->move_timer = enemies[i]->move_timer_max;

                    // save our x position temporarily so it can be reset if needed
                    int old_x = enemies[i]->pos.x;

                    // move vertically
                    // bosses will stop once they are fully on screen
                    if (enemies[i]->boss == 0 || enemies[i]->pos.y <= 0) {
                        enemies[i]->pos.y += enemies[i]->speed_y;
                    }

                    // if enemy is homing type, move horizontally to match player
                    if (enemies[i]->homing == 1) {
                        if (sysGetXCenter(enemies[i]->pos) < sysGetXCenter(player.pos))
                            enemies[i]->pos.x += enemies[i]->speed_x;
                        if (sysGetXCenter(enemies[i]->pos) > sysGetXCenter(player.pos))
                            enemies[i]->pos.x -= enemies[i]->speed_x;
                    }

                    // if we're colliding with another enemy, reset the position
                    int j;
                    for (j=0; j<ENEMY_MAX; j++) {
                        if (enemies[j] != NULL) {
                            if (i != j && enemies[i]->alive && enemies[j]->alive) {
                                if (sysCollide(&enemies[i]->pos, &enemies[j]->pos)) enemies[i]->pos.x = old_x;
                            }
                        }
                    }
                } else enemies[i]->move_timer--;

                // shoot
                if (enemies[i]->shoot_timer == 0) {
                    enemies[i]->shoot_timer = sysRandBetween(enemies[i]->shoot_timer_max/2, enemies[i]->shoot_timer_max);

                    int k;
                    for (k=0; k<enemies[i]->bullet_count; k++) {
                        hazardAdd(HAZARD_ENEMY,
                                  HAZARD_GFX2,
                                  sysGetXCenter(enemies[i]->pos)+enemies[i]->bullets[k].x_offset-(HAZARD_SIZE/2),
                                  sysGetYCenter(enemies[i]->pos)+enemies[i]->bullets[k].y_offset-(HAZARD_SIZE/2),
                                  (float)enemies[i]->bullets[k].angle,
                                  enemies[i]->bullets[k].speed);
                    }
                } else enemies[i]->shoot_timer--;
            } else {
                if (enemyCheckAnimation(enemies[i], ANIM_DEATH)) {
                    enemyReset(i);
                    continue;
                }
            }
            if (enemies[i]->pos.y > SCREEN_HEIGHT) {
                enemyReset(i);
                continue;
            }
        }
    }
}

void enemyAdd(int type, int sector) {
    if (type == 0 || type > enemy_type_max) return;

    int i;
    for (i=0; i<ENEMY_MAX; i++) {
        if (enemies[i] == NULL) {
            enemies[i] = (Enemy*) malloc(sizeof(Enemy));
            if (enemies[i] == NULL) return;

            enemies[i]->alive = true;
            enemies[i]->type = type;
            enemies[i]->move_timer = 0;

            // set variables by enemy type
            enemies[i]->gfx = enemy_stats[type-1].gfx;
            enemies[i]->pos.w = enemy_stats[type-1].pos.w;
            enemies[i]->pos.h = enemy_stats[type-1].pos.h;
            enemies[i]->speed_x = enemy_stats[type-1].speed_x;
            enemies[i]->speed_y = enemy_stats[type-1].speed_y;
            enemies[i]->shoot_timer_max = enemy_stats[type-1].shoot_timer_max;
            enemies[i]->move_timer_max = enemy_stats[type-1].move_timer_max;
            enemies[i]->homing = enemy_stats[type-1].homing;
            enemies[i]->boss = enemy_stats[type-1].boss;
            enemies[i]->bullets = enemy_stats[type-1].bullets;
            enemies[i]->bullet_count = enemy_stats[type-1].bullet_count;

            enemies[i]->anim_count = enemy_stats[type-1].anim_count;
            enemies[i]->anim_current = enemy_stats[type-1].anim_current;
            enemies[i]->anim = animationCopy(enemy_stats[type-1].anim, enemy_stats[type-1].anim_count);
            enemySetAnimation(enemies[i], ANIM_DEFAULT);

            // randomize the shooting timer
            enemies[i]->shoot_timer = sysRandBetween(enemies[i]->shoot_timer_max/2, enemies[i]->shoot_timer_max);

            // we center the enemy on the given x pos
            enemies[i]->pos.x = (sector*(SCREEN_WIDTH/8)) + (SCREEN_WIDTH/16) - (enemies[i]->pos.w/2);
            // y will typically be 0, so this will start enemies off screen
            enemies[i]->pos.y = -enemies[i]->pos.h;

            enemy_total++;
            break;
        }
    }
}

void enemyReset(int i) {
    if (enemies[i] != NULL) {
        if (enemies[i]->anim != NULL) {
            free(enemies[i]->anim);
        }
        free(enemies[i]);
        enemies[i] = NULL;
    }

    if (enemy_total > 0) enemy_total--;
}

void enemyCreateWave() {
    if (enemy_wave_timer > 0) {
        enemy_wave_timer--;
        return;
    }

    if (enemy_total > 0) return;
    if (enemy_level_max == 0) return;

    enemy_wave_timer = enemy_wave_timer_max;

    int wave = level % enemy_level_max;
    int i;
    for (i=0; i<8; i++) {
        enemyAdd(enemy_wave[wave].sector[i],i);
    }

    if (enemy_total > 0) level++;
}

void enemyHit(int i) {
    if (enemies[i]->alive) {
        enemySetAnimation(enemies[i], ANIM_DEATH);
        enemies[i]->alive = false;
    }
}

bool enemyCheckAnimation(Enemy* e, int id) {
    if (id == ANIM_DEATH) {
        if (e->anim != NULL && e->anim_current == ANIM_DEATH) {
            if (e->anim[ANIM_DEATH-1].finished) return true;
        } else if (e->alive == false) {
            if (e->anim != NULL) e->anim[e->anim_current-1].finished = true;
            return true;
        }
    }
    return false;
}

void enemySetAnimation(Enemy* e, int id) {
    if (e->anim != NULL) {
        if (id <= e->anim_count) {
            e->anim_current = id;
            e->anim[id-1].frame_current = 0;
            e->pos.w = e->anim[id-1].frame_width;
            e->pos.h = e->anim[id-1].frame_height;
        }
    }
}
