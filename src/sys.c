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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys/stat.h"
#include "sys/types.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "fileparser.h"
#include "sys.h"

SDL_Surface* screen = NULL;
TTF_Font* font = NULL;
SDL_Surface* surface_bar = NULL;
SDL_Surface* surface_bar_inactive = NULL;
SDL_Surface* surface_background = NULL;
SDL_Surface* surface_title = NULL;
SDL_Surface* surface_highscores = NULL;
SDL_Surface* surface_player = NULL;
SDL_Surface* surface_hazards = NULL;
SDL_Surface* surface_enemy1 = NULL;
SDL_Surface* surface_enemy2 = NULL;
Mix_Music* music = NULL;
Mix_Chunk* sound_menu = NULL;
Mix_Chunk* sound_switch = NULL;
SDL_Joystick* joy = NULL;

int score = 0;
int level = 0;
bool title_screen = true;
bool high_scores_screen = false;
int options_screen = -1;
bool game_over = false;
bool trigger_game_over = false;
bool paused = false;
bool quit = false;

int action_cooldown = 0;
bool action_left = false;
bool action_right = false;
bool action_up = false;
bool action_down = false;
bool action_main1 = false;
bool action_pause = false;

char* config_folder = NULL;
int option_joystick = -1;
int option_sound = 8;
int option_music = 8;
int option_fullscreen = 0;

bool sysInit() {
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return false;
    
    screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_HWSURFACE|SDL_DOUBLEBUF);

    if(screen == NULL) return false;
    
    if(TTF_Init() == -1) return false;

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1 ) return false;
    
    SDL_WM_SetCaption("Espada 2",NULL);

    sysConfigSetFolder();
    sysConfigLoad();

    sysHighScoresClear();
    sysHighScoresLoad();

    return true;
}

SDL_Surface* sysLoadGraphic(char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        fprintf(stderr,"Could not load image: %s\n", filename);
        return NULL;
    } else {
        SDL_Surface *cleanup = surface;
        surface = SDL_DisplayFormatAlpha(surface);
        SDL_FreeSurface(cleanup);
    }
    return surface;
}

bool sysLoadFiles() {
    // font
    font = TTF_OpenFont(PKGDATADIR "/fonts/VDS_Italic_New.ttf",18);
    if(!font) return false;
    else TTF_SetFontHinting(font, TTF_HINTING_LIGHT);

    // graphics
    surface_bar = sysLoadGraphic(PKGDATADIR "/graphics/bar.png");
    if (!surface_bar) return false;

    surface_bar_inactive = sysLoadGraphic(PKGDATADIR "/graphics/bar_inactive.png");
    if (!surface_bar_inactive) return false;

    surface_background = sysLoadGraphic(PKGDATADIR "/graphics/background.png");
    if (!surface_background) return false;

    surface_title = sysLoadGraphic(PKGDATADIR "/graphics/title.png");
    if (!surface_title) return false;

    surface_highscores = sysLoadGraphic(PKGDATADIR "/graphics/highscores.png");
    if (!surface_highscores) return false;

    surface_player = sysLoadGraphic(PKGDATADIR "/graphics/player.png");
    if (!surface_player) return false;

    surface_hazards = sysLoadGraphic(PKGDATADIR "/graphics/hazards.png");
    if (!surface_hazards) return false;

    surface_enemy1 = sysLoadGraphic(PKGDATADIR "/graphics/enemy1.png");
    if (!surface_enemy1) return false;

    surface_enemy2 = sysLoadGraphic(PKGDATADIR "/graphics/enemy2.png");
    if (!surface_enemy2) return false;

    // background music
    // music = Mix_LoadMUS(PKGDATADIR "/sounds/music.ogg");
    // if (!music) return false;

    // sound effects
    // sound_menu = Mix_LoadWAV(PKGDATADIR "/sounds/menu.wav");
    // if (!sound_menu) return false;

    // sound_switch = Mix_LoadWAV(PKGDATADIR "/sounds/switch.wav");
    // if (!sound_switch) return false;

    return true;
}

void sysCleanup() {
    sysConfigSave();
    if (config_folder) free(config_folder);

    Mix_HaltMusic();

    TTF_CloseFont(font);
    SDL_FreeSurface(surface_bar);
    SDL_FreeSurface(surface_background);
    SDL_FreeSurface(surface_title);
    SDL_FreeSurface(surface_highscores);
    SDL_FreeSurface(surface_player);
    SDL_FreeSurface(surface_hazards);
    SDL_FreeSurface(surface_enemy1);
    SDL_FreeSurface(surface_enemy2);
    Mix_FreeMusic(music);
    Mix_FreeChunk(sound_menu);
    Mix_FreeChunk(sound_switch);
    SDL_Quit();
}

void sysInput() {
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.sym == SDLK_LEFT)
                action_left = true;
            if(event.key.keysym.sym == SDLK_RIGHT)
                action_right = true;
            if(event.key.keysym.sym == SDLK_UP)
                action_up = true;
            if(event.key.keysym.sym == SDLK_DOWN)
                action_down = true;
            if(event.key.keysym.sym == 'z')
                action_main1 = true;
            if(event.key.keysym.sym == SDLK_ESCAPE)
                action_pause = true;
        }

        else if(event.type == SDL_KEYUP) {
            if(event.key.keysym.sym == SDLK_LEFT)
                action_left = false;
            if(event.key.keysym.sym == SDLK_RIGHT)
                action_right = false;
            if(event.key.keysym.sym == SDLK_UP)
                action_up = false;
            if(event.key.keysym.sym == SDLK_DOWN)
                action_down = false;
            if(event.key.keysym.sym == 'z')
                action_main1 = false;
            if(event.key.keysym.sym == SDLK_ESCAPE)
                action_pause = false;
        }

        else if(event.type == SDL_JOYBUTTONDOWN) {
            if(event.jbutton.which == 0) {
                if (event.jbutton.button == 0)
                    action_main1 = true;
                if (event.jbutton.button == 9)
                    action_pause = true;
            }
        }

        else if(event.type == SDL_JOYBUTTONUP) {
            if(event.jbutton.which == 0) {
                if (event.jbutton.button == 0)
                    action_main1 = false;
                if (event.jbutton.button == 9)
                    action_pause = false;
            }
        }

        else if(event.type == SDL_QUIT) {
            quit = true;
        }
    }

    if (joy) {
        int joy_x = SDL_JoystickGetAxis(joy, 0);
        int joy_y = SDL_JoystickGetAxis(joy, 1);

        // x axis
        if (joy_x < -JOY_DEADZONE) {
            action_left = true;
            action_right = false;
        } else if (joy_x > JOY_DEADZONE) {
            action_left = false;
            action_right = true;
        } else {
            action_left = false;
            action_right = false;
        }

        // y axis
        if (joy_y < -JOY_DEADZONE) {
            action_up = true;
            action_down = false;
        } else if (joy_y > JOY_DEADZONE) {
            action_up = false;
            action_down = true;
        } else {
            action_up = false;
            action_down = false;
        }
    }
}

void sysInputClear() {
    action_left = false;
    action_right = false;
    action_up = false;
    action_down = false;
    action_main1 = false;
    action_pause = false;
}

void sysConfigSetFolder() {
    char *home = malloc(strlen(getenv("HOME"))+1);
    strcpy(home,getenv("HOME"));

    config_folder = malloc(strlen(home)+strlen("/.espada2")+1);
    sprintf(config_folder,"%s/.espada2",home);

    free(home);
}

void sysConfigLoad() {
    mkdir(config_folder, S_IRWXU | S_IRWXG | S_IRWXO);
    char *config_path = malloc(strlen(config_folder)+strlen("/config")+1);

    if (config_path) {
        sprintf(config_path,"%s/config",config_folder);

        if (fileOpen(config_path)) {
            while(fileNext()) {
                if (!strcmp("joystick",fileGetKey())) option_joystick = atoi(fileGetVal());
                else if (!strcmp("sound",fileGetKey())) option_sound = atoi(fileGetVal());
                else if (!strcmp("music",fileGetKey())) option_music = atoi(fileGetVal());
                else if (!strcmp("fullscreen",fileGetKey())) option_fullscreen = atoi(fileGetVal());
            }
            fileClose();
            sysConfigApply();
        } else {
            printf ("Error: Couldn't load config file. Creating new config...\n");
            sysConfigSave();
        }

        free(config_path);
    }
}

void sysConfigSave() {
    FILE *config_file;

    mkdir(config_folder, S_IRWXU | S_IRWXG | S_IRWXO);
    char *config_path = malloc(strlen(config_folder)+strlen("/config")+1);

    if (config_path) {
        sprintf(config_path,"%s/config",config_folder);
        config_file = fopen(config_path,"w+");

        if (config_file) {
            fprintf(config_file,"joystick=%d\n",option_joystick);
            fprintf(config_file,"sound=%d\n",option_sound);
            fprintf(config_file,"music=%d\n",option_music);
            fprintf(config_file,"fullscreen=%d\n",option_fullscreen);
            fclose(config_file);
        } else printf("Error: Couldn't write to config file.\n");

        free(config_path);
    }

    sysConfigApply();
}

void sysConfigApply() {
    if (joy) SDL_JoystickClose(joy);
    if (SDL_NumJoysticks() > 0 && option_joystick > -1) joy = SDL_JoystickOpen(option_joystick);
    else {
        option_joystick = -1;
        joy = NULL;
    }

    Mix_Volume(-1,option_sound*16);
    Mix_VolumeMusic(option_music*16);

    if (option_fullscreen == 1) {
        screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_HWSURFACE|SDL_FULLSCREEN);
    }
    if (!screen || option_fullscreen != 1) {
        screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_HWSURFACE);
    }
}

void sysHighScoresLoad() {
    int i = 0;

    mkdir(config_folder, S_IRWXU | S_IRWXG | S_IRWXO);
    char *path = malloc(strlen(config_folder)+strlen("/highscores")+1);

    if (path) {
        sprintf(path,"%s/highscores",config_folder);

        if (fileOpen(path)) {
            while (fileNext()) {
                if (i < 10) high_scores[i] = atoi(fileGetLine());
                else break;
                i++;
            }
            fileClose();
        } else {
            printf ("Error: Couldn't load high scores.\n");
            sysHighScoresSave();
        }

        free(path);
    }
}

void sysHighScoresSave() {
    FILE *file;
    int i = 0;

    mkdir(config_folder, S_IRWXU | S_IRWXG | S_IRWXO);
    char *path = malloc(strlen(config_folder)+strlen("/highscores")+1);

    if (path) {
        sprintf(path,"%s/highscores",config_folder);
        file = fopen(path,"w+");

        if (file) {
            for (i=0;i<10;i++) {
                fprintf(file,"%d\n",high_scores[i]);
            }
            fclose(file);
        } else printf("Error: Couldn't save high scores.\n");

        free(path);
    }
}

void sysHighScoresClear() {
    for (int i=0; i<10; i++) {
        high_scores[i] = 0;
    }
}

bool sysCollide(SDL_Rect* A, SDL_Rect* B) {
    if (A == NULL || B == NULL) return false;

    //If any of the sides from A are outside of B
    if ((A->y + A->h) <= B->y) return false;
    if (A->y >= (B->y + B->h)) return false;
    if ((A->x + A->w) <= B->x) return false;
    if (A->x >= (B->x + B->w)) return false;

    //If none of the sides from A are outside B
    return true;
}

int sysGetXCenter(SDL_Rect A) {
    return A.x + (A.w/2);
}

int sysRandBetween(int low, int high) {
    return rand() % (high - low + 1) + low;
}

