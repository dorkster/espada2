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

#ifndef ANIMATION_H
#define ANIMATION_H

#include "sys.h"

#define ANIM_DEFAULT 1
#define ANIM_DEATH 2

typedef struct Animation{
    int id;
    SDL_Surface* gfx;
    int frame_current;
    int frame_total;
    int frame_duration;
    int frame_ticks;
    int frame_width;
    int frame_height;
    bool looped;
    bool finished;
}Animation;

void animationInit(Animation* anim);
Animation* animationAdd(Animation* anim, int* anim_count, char* filename);
void animationCleanup(Animation **anim, int* anim_count);
void animationAdvanceFrame(Animation *anim);

#endif
