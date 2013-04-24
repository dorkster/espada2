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

#include "SDL/SDL_ttf.h"

#include "draw.h"
#include "enemy.h"
#include "hazard.h"
#include "menu.h"
#include "player.h"
#include "sys.h"

int background_y = 0;

void drawEverything() {
    drawBackground();

    if (title_screen) {
        drawTitle();
    } else if (high_scores_screen) {
        drawHighScores();
    } else if (options_screen > -1) {
        drawOptions();
    } else {
        drawHazards();
        drawEnemies();
        drawPlayer();
        drawInfo();
    }
}

void drawBackground() {
    SDL_Rect dest, dest2;
    dest.x = dest2.x = 0;

    dest.y = background_y - SCREEN_HEIGHT;
    dest2.y = background_y;

    SDL_BlitSurface(surface_background,NULL,screen,&dest);
    SDL_BlitSurface(surface_background,NULL,screen,&dest2);

    if (!game_over && !paused) {
        background_y += 5;
        if (background_y > SCREEN_HEIGHT)
            background_y = 0;
    }
}

void drawMenu(int offset) {
    SDL_Surface *text;
    SDL_Color color = {217,217,217};
    SDL_Rect dest;
    int i;

    for (i=0;i<menu_size;i++) {
        dest.x = 0;
        dest.y = SCREEN_HEIGHT - ((menu_size-i) * surface_bar->h) - offset;
        if (i == menu_option) SDL_BlitSurface(surface_bar,NULL,screen,&dest);
        else SDL_BlitSurface(surface_bar_inactive,NULL,screen,&dest);

        text = TTF_RenderText_Blended(font,menu_items[i],color);
        if (text) {
            dest.x = SCREEN_WIDTH/2 - text->w/2;
            dest.y = SCREEN_HEIGHT - ((menu_size-i-1) * surface_bar->h) - MENUBAR_SIZE - offset;
            SDL_BlitSurface(text,NULL,screen,&dest);
            SDL_FreeSurface(text);
        }
    }
}

void drawInfo() {
    SDL_Surface *text_info;
    char text[256];
    SDL_Color color = {217,217,217};
    SDL_Rect dest;

    // statusbar background
    dest.x = 0;
    dest.y = SCREEN_HEIGHT - surface_bar->h;
    if (paused || game_over) SDL_BlitSurface(surface_bar_inactive,NULL,screen,&dest);
    else SDL_BlitSurface(surface_bar,NULL,screen,&dest);

    // statusbar text
    sprintf(text, "Score: %-10d  Level: %-3d",score,level);
    if (game_over) sprintf(text,"%s  Game Over!",text);
    else if (paused) sprintf(text,"%s  *Paused*",text);

    text_info = TTF_RenderText_Blended(font,text,color);
    if(text_info) {
        dest.x = 8;
        dest.y = SCREEN_HEIGHT-MENUBAR_SIZE;

        SDL_BlitSurface(text_info,NULL,screen,&dest);
        SDL_FreeSurface(text_info);
    }

    // menu
    if (paused || game_over) drawMenu(surface_bar->h);
}

void drawTitle() {
    SDL_Rect dest;

    // title logo
    dest.x = SCREEN_WIDTH/2 - surface_title->w/2;
    dest.y = SCREEN_HEIGHT/2 - surface_title->h/2 - surface_bar->h*2;
    SDL_BlitSurface(surface_title,NULL,screen,&dest);

    // menu
    drawMenu(0);
}

void drawHighScores() {
    SDL_Surface *text_header;
    SDL_Surface *text_score[10];
    char text[256];
    SDL_Color color = {217,217,217};
    SDL_Rect dest;

    // list background
    dest.x = SCREEN_WIDTH/2 - surface_highscores->w/2;
    dest.y = 0;
    SDL_BlitSurface(surface_highscores,NULL,screen,&dest);

    // "High Scores" text
    sprintf(text,"High Scores");
    text_header = TTF_RenderText_Blended(font,text,color);
    if (text_header) {
        dest.x = SCREEN_WIDTH/2 - text_header->w/2;
        dest.y = 8;

        SDL_BlitSurface(text_header,NULL,screen,&dest);
        SDL_FreeSurface(text_header);
    }

    // high score list
    for (int i=0; i<10; i++) {
        if (high_scores[i] > 0) sprintf(text,"%d. %d",i+1,high_scores[i]);
        else sprintf(text,"%d.",i+1);
        text_score[i] = TTF_RenderText_Blended(font,text,color);
        if (text_score[i]) {
            dest.x = 240;
            dest.y = (MENUBAR_SIZE*i) + (MENUBAR_SIZE*2);

            SDL_BlitSurface(text_score[i],NULL,screen,&dest);
            SDL_FreeSurface(text_score[i]);
        }
    }

    // "Return to title" text
    drawMenu(0);
}

void drawOptions() {
    // menu
    drawMenu(0);
}

void drawPlayer() {
    int id = player.anim_current-1;
    if (player.anim != NULL && id < player.anim_count) {
        if (!player.alive && player.anim[id].finished) return;

        SDL_Rect src,dest;

        src.w = player.pos.w;
        src.h = player.pos.h;

        dest.x = player.pos.x;
        dest.y = player.pos.y;

        src.x = id * src.w; // animation type
        src.y = player.anim[id].frame_current * src.h; // animation frame
        SDL_BlitSurface(player.anim[id].gfx,&src,screen,&dest);
    }
}

void drawHazards() {
    SDL_Rect src,dest;

    src.x = src.y = 0;
    src.w = src.h = HAZARD_SIZE;

    int i;
    for (i=0; i<HAZARD_MAX; i++) {
        if (hazards[i] != NULL) {
            src.x = hazards[i]->gfx * HAZARD_SIZE;
            dest.x = hazards[i]->pos.x;
            dest.y = hazards[i]->pos.y;

            SDL_BlitSurface(surface_hazards,&src,screen,&dest);
        }
    }
}

void drawEnemies() {
    int i;
    for (i=0; i<ENEMY_MAX; i++) {
        if (enemies[i] != NULL) {
            int id = enemies[i]->anim_current-1;
            if (enemies[i]->anim != NULL && id < enemies[i]->anim_count) {
                if (!enemies[i]->alive && enemies[i]->anim[id].finished) return;

                SDL_Rect src,dest;

                src.w = enemies[i]->pos.w;
                src.h = enemies[i]->pos.h;

                dest.x = enemies[i]->pos.x;
                dest.y = enemies[i]->pos.y;

                src.x = id * src.w; // animation type
                src.y = enemies[i]->anim[id].frame_current * src.h; // animation frame
                SDL_BlitSurface(enemies[i]->anim[id].gfx,&src,screen,&dest);
            }
        }
    }
}
