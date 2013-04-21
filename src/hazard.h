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

#ifndef HAZARD_H
#define HAZARD_H

#include "sys.h"

#define HAZARD_PLAYER 0
#define HAZARD_ENEMY 1
#define HAZARD_GFX1 0
#define HAZARD_GFX2 1

#define HAZARD_MAX 256
#define HAZARD_SIZE 8

typedef struct Hazard{
    SDL_Rect pos;
    int speed_x, speed_y;
    int src;
    int gfx;
    bool active;
}Hazard;

typedef struct HazardDef{
    int x_offset;
    int y_offset;
    int angle;
    int speed;
}HazardDef;

Hazard* hazards[HAZARD_MAX];

void hazardInit();
void hazardLogic();
void hazardAdd(int src, int gfx, int x, int y, float angle, int speed);
void hazardReset(int i);
void hazardCleanup();
HazardDef* hazardDefAdd(int size, HazardDef* def, int x_offset, int y_offset, int angle, int speed);

#endif
