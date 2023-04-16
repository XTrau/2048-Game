#ifndef MENU_H
#define MENU_H
#include "graphics.h"

enum button_values { NONE = -1, GAME, SETTINGS, ABOUT, EXIT, N_BUTTONS, 
                              FOUR, FIVE, SIX, SEVEN, S_BUTTONS, 
                              MENU, LAST_STEP, G_BUTTONS };

struct Button
{
   int left;
   int top;
   int width;
   int height;
   IMAGE *image;
};

void load();
void draw_background();
void start();
void menu();
void settings();
void about();
void close();

void create_button(int, int, int, const char*);
int  select_button();
int select_setting_button();

#endif