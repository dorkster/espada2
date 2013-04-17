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

#include "hazard.h"
#include "sys.h"

Hazard* hazards[HAZARD_MAX];

void hazardInit() {
    int i;
    for (i=0; i<HAZARD_MAX; i++) {
        hazardReset(i);
    }
}

void hazardLogic() {
    int i;
    for (i=0; i<HAZARD_MAX; i++) {
        if (hazards[i] != NULL) {
            if (hazards[i]->active) {
                hazards[i]->x += hazards[i]->speed_x;
                hazards[i]->y += hazards[i]->speed_y;
            }
            if (hazards[i]->x < 0-HAZARD_SIZE ||
                hazards[i]->x > SCREEN_WIDTH ||
                hazards[i]->y < 0-HAZARD_SIZE ||
                hazards[i]->y > SCREEN_HEIGHT) {
                hazardReset(i);
            }
        }
    }
}

void hazardAdd(int src, int gfx, int x, int y, float angle, int speed) {
	float pi = 3.1415926535898f;
    int i;
    for (i=0; i<HAZARD_MAX; i++) {
        if (hazards[i] == NULL) {
            hazards[i] = (Hazard*) malloc(sizeof(Hazard));
            if (hazards[i] == NULL) return;

            hazards[i]->active = true;
            hazards[i]->src = src;
            hazards[i]->gfx = gfx;
            hazards[i]->x = x;
            hazards[i]->y = y;

            // calcuate speed
            float theta = angle * pi / 180.0f;
            while (theta >= pi+pi) theta -= pi+pi;
            while (theta < 0.0) theta += pi+pi;
            hazards[i]->speed_x = speed * sin(theta);
            hazards[i]->speed_y = speed * -cos(theta);
            break;
        }
    }
}

void hazardReset(int i) {
    if (hazards[i] != NULL) {
        free(hazards[i]);
        hazards[i] = NULL;
    }
}
