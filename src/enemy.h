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

#include "sys.h"

#define ENEMY_LOGIC1 0
#define ENEMY_GFX1 0

#define ENEMY_MAX 16
#define ENEMY_WAVE_TIMER FPS * 10

typedef struct Enemy{
    SDL_Rect pos;
    int speed_x, speed_y;
    int logic;
    SDL_Surface *gfx;
    bool active;
    int shoot_timer, shoot_timer_max;
    int move_timer, move_timer_max;
}Enemy;

Enemy* enemies[ENEMY_MAX];
int enemy_total;
int enemy_wave_timer;

void enemyInit();
void enemyLogic();
void enemyAdd(int logic, int gfx, int sector);
void enemyReset(int i);
void enemyCreateWave();
void enemyKill(int i);

#endif
