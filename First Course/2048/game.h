#ifndef GAME_H
#define GAME_H
#include "graphics.h"

void game();
void play();
void create_field();
void delete_field();
void draw_field();
void draw_numbers();
void new_number();
int find_num(int);

void save_field();
void back_step();

bool can_move(int);
bool can_combine(int);
void move(int);
void combine(int);
bool check_over();
void game_over();

#endif