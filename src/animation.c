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

#include "animation.h"
#include "fileparser.h"

void animationInit(Animation* anim) {
    anim->id = 0;
    anim->gfx = NULL;
    anim->frame_current = 0;
    anim->frame_total = 1;
    anim->frame_duration = 0;
    anim->frame_ticks = 0;
    anim->frame_width = 0;
    anim->frame_height = 0;
    anim->looped = false;
    anim->finished = false;
}

Animation* animationAdd(Animation* anim, int* anim_count, char* filename) {
    FileParser* f = NULL;
    int current_animation = 0;
    SDL_Surface* shared_gfx = NULL;

    char* path1 = NULL;
    path1 = calloc(1,strlen(PKGDATADIR)+strlen(filename)+5);
    if (path1 != NULL) {
        strcat(path1,PKGDATADIR);
        strcat(path1,filename);
        f = fileOpen(path1);
        free(path1);
    } else return NULL;

    while(fileNext(f)) {
        if (!strcmp("gfx",fileGetKey(f))) {
            char *path2 = calloc(1,strlen(PKGDATADIR)+strlen(fileGetVal(f))+1);
            if (path2) {
                strcat(path2,PKGDATADIR);
                strcat(path2,fileGetVal(f));
                shared_gfx = sysLoadGraphic(path2);
                free(path2);
            }
        }
        else if (!strcmp("id",fileGetKey(f)) && shared_gfx != NULL) {
            current_animation = atoi(fileGetVal(f));
            if (current_animation > *anim_count && shared_gfx != NULL) {
                *anim_count = current_animation;
                anim = realloc(anim, sizeof(Animation) * (*anim_count));
                animationInit(&anim[current_animation-1]);
                anim[current_animation-1].id = current_animation;
                anim[current_animation-1].gfx = shared_gfx;
            }
        }
        if (current_animation > 0) {
            if (!strcmp("frame_total",fileGetKey(f))) anim[current_animation-1].frame_total = atoi(fileGetVal(f));
            else if (!strcmp("frame_duration",fileGetKey(f))) {
                anim[current_animation-1].frame_duration = anim[current_animation-1].frame_ticks = atoi(fileGetVal(f));
            }
            else if (!strcmp("frame_width",fileGetKey(f))) anim[current_animation-1].frame_width = atoi(fileGetVal(f));
            else if (!strcmp("frame_height",fileGetKey(f))) anim[current_animation-1].frame_height = atoi(fileGetVal(f));
            else if (!strcmp("looped",fileGetKey(f))) {
                if (atoi(fileGetVal(f)) == 1) anim[current_animation-1].looped = true;
            }
        }
    }
    fileClose(f);

    return anim;
}

Animation* animationCopy(Animation* anim, int anim_count) {
    if (anim_count < 1) return NULL;

    Animation* new_anim = malloc(sizeof(Animation)*anim_count);
    if (new_anim == NULL) return NULL;

    int i;
    for (i=0; i<anim_count; i++) {
        new_anim[i].id = anim[i].id;
        new_anim[i].gfx = anim[i].gfx;
        new_anim[i].frame_current = anim[i].frame_current;
        new_anim[i].frame_total = anim[i].frame_total;
        new_anim[i].frame_duration = anim[i].frame_duration;
        new_anim[i].frame_ticks = anim[i].frame_ticks;
        new_anim[i].frame_width = anim[i].frame_width;
        new_anim[i].frame_height = anim[i].frame_height;
        new_anim[i].looped = anim[i].looped;
        new_anim[i].finished = anim[i].finished;
    }
    return new_anim;
}


void animationCleanup(Animation **anim, int* anim_count) {
    if (*anim != NULL) {
        SDL_Surface* shared_gfx = NULL;
        int i;
        for (i=0; i<(*anim_count); i++) {
            if (anim[i]->gfx != shared_gfx) {
                shared_gfx = anim[i]->gfx;
                break;
            }
        }
        SDL_FreeSurface(shared_gfx);
        free(*anim);
        *anim = NULL;
    }
}

void animationAdvanceFrame(Animation *anim) {
    if (anim != NULL) {
        if (anim->finished) return;

        if (anim->frame_ticks > 0) {
            anim->frame_ticks--;
        } else {
            anim->frame_ticks = anim->frame_duration;
            anim->frame_current++;
            if (anim->frame_current >= anim->frame_total) {
                if (!anim->looped) anim->finished = true;
                else anim->frame_current = 0;
            }
        }
    }
}
