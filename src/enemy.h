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

#ifndef ENEMY_H
#define ENEMY_H

#include "animation.h"
#include "hazard.h"
#include "sys.h"

#define ENEMY_MAX 8

typedef struct Enemy{
    SDL_Rect pos;
    int hp;
    int score;
    int speed_x, speed_y;
    int type;
    SDL_Surface *gfx;
    bool alive;
    int shoot_timer, shoot_timer_max;
    int move_timer, move_timer_max;
    int homing;
    int boss;
    bool immunity;
    HazardDef *bullets;
    int bullet_count;
    Animation* anim;
    int anim_count;
    int anim_current;
}Enemy;

typedef struct EnemyWave{
    int sector[8];
}EnemyWave;

Enemy* enemies[ENEMY_MAX];
Enemy* enemy_stats;
EnemyWave* enemy_wave;
int enemy_total;
int enemy_type_max;
int enemy_level_max;
int enemy_wave_timer;
int enemy_wave_timer_max;
int enemy_immunity_buffer;

void enemyInit();
void enemyInitEnemy(Enemy* e);
void enemyInitWave(EnemyWave* wave);
void enemyCleanup();
void enemyLogic();
void enemyAdd(int type, int sector);
void enemyReset(int i);
void enemyCreateWave();
void enemyHit(int i, int dmg);
bool enemyCheckAnimation(Enemy* e, int id);
void enemySetAnimation(Enemy* e, int id);

#endif
