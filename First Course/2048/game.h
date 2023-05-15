#ifndef GAME_H
#define GAME_H

struct Num
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
void create_field();
void delete_field();

void open_record();
void save_record();

void draw_field();
void refresh_field();
void draw_numbers();
void new_number();
int find_num(int);

void save_field();
void back_step();

bool can_move(int);
bool can_combine(int);
void move(int);
void do_animation();

bool check_over();
bool check_win();

void game_over();
void draw_win();

#endif