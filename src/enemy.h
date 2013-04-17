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

typedef struct Enemy{
    int x,y;
    int w,h;
    int speed_x, speed_y;
    int logic;
    SDL_Surface *gfx;
    bool active;
    int shoot_timer;
}Enemy;

Enemy* enemies[ENEMY_MAX];
int enemy_total;

void enemyInit();
void enemyLogic();
void enemyAdd(int logic, int gfx, int x, int y);
void enemyReset(int i);
void enemyCreateWave();

#endif
