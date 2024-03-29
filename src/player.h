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

#ifndef PLAYER_H
#define PLAYER_H

#include "animation.h"
#include "hazard.h"
#include "sys.h"

typedef struct Player{
    SDL_Rect pos;
    bool alive;
    int speed;
    int damage;
    HazardDef* bullets;
    int bullet_count;
    Animation* anim;
    int anim_count;
    int anim_current;
}Player;

Player player;

void playerInit();
void playerCleanup();
void playerAnimate();
void playerLogic();
void playerMove();
void playerShoot();
void playerHit();
bool playerCheckAnimation(int id);
void playerSetAnimation(int id);

#endif
