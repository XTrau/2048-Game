#define HEIGHT 600
#define WIDTH 600
#include <iostream>
#include <ctime>
#include "graphics.h"

using namespace std;

const int DELAY_TIME = 200;
bool combined = false;
bool game_over = false;
bool win = false;
int field_size = 4, free_fields = field_size * field_size;
int **playing_field;

void create_field();
void draw_field();
void draw_numbers();
void new_number();
void up_move();
void down_move();
void left_move();
void right_move();
void up_combine();
void down_combine();
void left_combine();
void right_combine();

int main()
{
   create_field();
   srand(time(NULL));
   
   initwindow(WIDTH, HEIGHT, "2048 - Game of the Year Edition by Hismatullin Vildan mega game 2023");
   
   new_number();
   while(true)
   {
      int key;
      draw_field();
      draw_numbers();
      getch();
      key = getch();
      switch(key)
      {
         case KEY_UP:
            up_move();
            up_combine();
            break;
         case KEY_DOWN:
            down_move();
            down_combine();
            break;
         case KEY_LEFT:
            left_move();
            left_combine();
            break;
         case KEY_RIGHT:
            right_move();
            right_combine();
            break;
         default:
            getch();
            continue;
      }
      
      if(free_fields >= 1)
         new_number();
   }
   
   return 0;
}

void create_field()
{
   playing_field = new int*[field_size];
   
   for(int i = 0; i < field_size; i++)
   {
      playing_field[i] = new int[field_size];
   }
   
   for(int i = 0; i < field_size; i++)
   {
      for(int j = 0; j < field_size; j++)
      {
         playing_field[i][j] = 0;
      }
   }
}

void draw_field()
{
   setfillstyle(SOLID_FILL, BLACK);
   bar(0, 0, WIDTH, HEIGHT);
   setcolor(WHITE);
   for(int i = 1; i <= field_size - 1; i++)
   {      
      line(WIDTH * i / field_size, 0, WIDTH * i / field_size, HEIGHT);
      line(0, HEIGHT * i / field_size, WIDTH, HEIGHT * i / field_size);
   }
}

void draw_numbers()
{
   for(int i = 0; i < field_size; i++)
   {
      for(int j = 0; j < field_size; j++)
      {
         if(playing_field[i][j] != 0)
         {
            char txt[8];
            int t = playing_field[i][j];
            for(int k = 0; t > 0; k++)
            {
               txt[k] = t % 10 + 48;
               txt[k + 1] = '\0';
               t /= 10;
            }
            
            int p = 0;
            char txt2[8];
            for(int k = strlen(txt) - 1; k >= 0; k--)
            {
               txt2[p] = txt[k];
               txt2[p + 1] = '\0';
               p++;
            }
            
            outtextxy(WIDTH * j / field_size + (WIDTH / (field_size * 2)), HEIGHT * i / field_size + (WIDTH / (field_size * 2)), txt2);
         }
      }
   }
}

void new_number()
{
   int i, j;
   
   for(int k = 0; k < field_size / 4; k++)
   {
      i = rand() % field_size;
      j = rand() % field_size;
      while(playing_field[i][j] != 0)
      {
         i = rand() % field_size;
         j = rand() % field_size;
      }
      playing_field[i][j] = 2;
      free_fields--;
   }
}

void up_move()
{
   for(int i = 0; i < field_size; i++)
   {
      for(int j = 0; j < field_size; j++)
      {
         if(playing_field[j][i] == 0)
         {
            for(int k = j + 1; k < field_size; k++)
            {
               if(playing_field[k][i] != 0)
               {
                  playing_field[j][i] = playing_field[k][i];
                  playing_field[k][i] = 0;
                  break;
               }
            }
         }
      }
   }
   
   draw_field();
   draw_numbers();
   delay(DELAY_TIME);
}

void down_move()
{
   for(int i = 0; i < field_size; i++)
   {
      for(int j = field_size - 1; j >= 0; j--)
      {
         if(playing_field[j][i] == 0)
         {
            for(int k = j - 1; k >= 0; k--)
            {
               if(playing_field[k][i] != 0)
               {
                  playing_field[j][i] = playing_field[k][i];
                  playing_field[k][i] = 0;
                  break;
               }
            }
         }
      }
   }
   
   draw_field();
   draw_numbers();
   delay(DELAY_TIME);
}

void left_move()
{
   for(int i = 0; i < field_size; i++)
   {
      for(int j = 0; j < field_size; j++)
      {
         if(playing_field[i][j] == 0)
         {
            for(int k = j + 1; k < field_size; k++)
            {
               if(playing_field[i][k] != 0)
               {
                  playing_field[i][j] = playing_field[i][k];
                  playing_field[i][k] = 0;
                  break;
               }
            }
         }
      }
   }
   
   draw_field();
   draw_numbers();
   delay(DELAY_TIME);
}

void right_move()
{
   for(int i = 0; i < field_size; i++)
   {
      for(int j = field_size - 1; j >= 0; j--)
      {
         if(playing_field[i][j] == 0)
         {
            for(int k = j - 1; k >= 0; k--)
            {
               if(playing_field[i][k] != 0)
               {
                  playing_field[i][j] = playing_field[i][k];
                  playing_field[i][k] = 0;
                  break;
               }
            }
         }
      }
   }
   
   draw_field();
   draw_numbers();
   delay(DELAY_TIME);
}

void up_combine()
{
   for(int i = 0; i < field_size; i++)
   {
      for(int j = 0; j < field_size - 1; j++)
      {
         if(playing_field[j + 1][i] == playing_field[j][i] && playing_field[j][i] > 0)
         {
            playing_field[j][i] *= 2;
            playing_field[j + 1][i] = 0;
            free_fields++;
            combined = true;
         }
      }
   }
   if(combined == true)
   {
      combined = false;
      up_move();
   }
}

void down_combine()
{
   for(int i = 0; i < field_size; i++)
   {
      for(int j = field_size - 1; j > 0; j--)
      {
         if(playing_field[j - 1][i] == playing_field[j][i] && playing_field[j][i] > 0)
         {
            playing_field[j][i] *= 2;
            playing_field[j - 1][i] = 0;
            free_fields++;
            combined = true;
         }
      }
   }
   if(combined == true)
   {
      combined = false;
      down_move();
   }
}

void left_combine()
{
   for(int i = 0; i < field_size; i++)
   {
      for(int j = 0; j < field_size - 1; j++)
      {
         if(playing_field[i][j + 1] == playing_field[i][j] && playing_field[i][j] > 0)
         {
            playing_field[i][j] *= 2;
            playing_field[i][j + 1] = 0;
            free_fields++;
            combined = true;
         }
      }
   }
   if(combined == true)
   {
      combined = false;
      left_move();
   }
}

void right_combine()
{
   for(int i = 0; i < field_size; i++)
   {
      for(int j = field_size - 1; j > 0; j--)
      {
         if(playing_field[i][j - 1] == playing_field[i][j] && playing_field[i][j] > 0)
         {
            playing_field[i][j] *= 2;
            playing_field[i][j - 1] = 0;
            free_fields++;
            combined = true;
         }
      }
   }
   if(combined = true)
   {
      combined = false;
      right_move();
   }
}