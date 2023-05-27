#ifndef GAME_H
#define GAME_H
#include "graphics.h"

const int X_OFFSET = 340, Y_OFFSET = 150;
const int FIELD_WALL_WIDTH = 10;

struct Cell
{
   long long data;
   int pos_x;
   int pos_y;
   int size;
   
   int red;
   int green;
   int blue;
   
   int next_x;
   int next_y;
};

void game();
void play();
void load_game();
void create_field();
void delete_field();

void open_record();
void save_record();

void draw_field();
void draw_numbers();
void new_number();
int find_num(int);

void save_field();
void back_step();

bool can_move(int);
bool can_combine(int);
void move(int);
void do_animation(int, int, int);
void refresh_field();

bool check_lose();
bool check_win();

void draw_over(IMAGE*);

#endif