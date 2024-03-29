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

#ifndef SYS_H
#define SYS_H

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

typedef enum { false = 0, true = 1 } bool;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 32
#define FPS 60
#define JOY_DEADZONE 100
#define ACTION_COOLDOWN 10 / (60/FPS)
#define MENUBAR_SIZE 24

#define OPTIONS_MAIN 0
#define OPTIONS_JOYSTICK 1
#define OPTIONS_SOUND 2
#define OPTIONS_MUSIC 3

#ifndef PKGDATADIR
#define PKGDATADIR "./data"
#endif

SDL_Surface* screen;
TTF_Font* font;

int score;
int level;
int high_scores[10];
bool title_screen;
bool high_scores_screen;
int options_screen;
bool game_over;
bool trigger_game_over;
bool paused;
bool quit;

int action_cooldown;
bool action_left;
bool action_right;
bool action_up;
bool action_down;
bool action_main1;
bool action_pause;

char* config_folder;
int option_joystick;
int option_sound;
int option_music;
int option_fullscreen;

SDL_Event event;

// Timers
unsigned int startTimer;
unsigned int endTimer;
unsigned int deltaTimer;

// Functions
bool sysInit();
SDL_Surface* sysLoadGraphic(char* filename);
bool sysLoadFiles();
void sysCleanup();
void sysInput();
void sysInputClear();
void sysConfigSetFolder();
void sysConfigLoad();
void sysConfigSave();
void sysConfigApply();
void sysHighScoresLoad();
void sysHighScoresSave();
void sysHighScoresClear();
bool sysCollide(SDL_Rect* A, SDL_Rect* B);
int sysGetXCenter(SDL_Rect A);
int sysGetYCenter(SDL_Rect A);
int sysRandBetween(int low, int high);

// Images
SDL_Surface* surface_bar;
SDL_Surface* surface_bar_inactive;
SDL_Surface* surface_background;
SDL_Surface* surface_title;
SDL_Surface* surface_highscores;
SDL_Surface* surface_hazards;

// Music and Sounds
Mix_Music* music;
Mix_Chunk* sound_menu;
Mix_Chunk* sound_player_fire;
Mix_Chunk* sound_enemy_fire;
Mix_Chunk* sound_enemy_hit;
Mix_Chunk* sound_explosion;

// Joystick
SDL_Joystick* joy;

#endif
