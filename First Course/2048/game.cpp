#define WIDTH 1280
#define HEIGHT 800
#define DELAY_TIME 100
#include <ctime>
#include <string>
#include <iostream>
#include "game.h"
#include "menu.h"
#include "graphics.h"

using namespace std;

extern int field_size;
extern IMAGE *background_image;

bool combined = false;
int field_width = 10;
int free_fields;
int **playing_field;
int **last_field;
int score;

int last_score;
int last_free_fields;

void game()
{
   cleardevice();
   create_field();
   new_number();
   save_field();
   play();
}

void play()
{
   srand(time(NULL));
   settextjustify(CENTER_TEXT, BOTTOM_TEXT);
   while(true)
   {
      int key;
      draw_field();
      draw_numbers();
      
      if (check_over() == false && free_fields == 0)
      {
         game_over();
         if (check_over() == false && free_fields == 0)
            break;
      }
      swapbuffers();
      
      key = getch();
      
      if (key == 27)
         break;
      else if (key == 8)
      {
         back_step();
         continue;
      }         
      
      if (can_move(key) == true || can_combine(key) == true)
      {
         save_field();
         move(key);
         combine(key);
      }
      else
         continue;
      
      if (free_fields >= 1)
         new_number();
   }
   delete_field();
}

void create_field()
{
   free_fields = field_size * field_size;
   score = 0;
   last_score = 0;
   playing_field = new int*[field_size];
   last_field = new int*[field_size];
   for (int i = 0; i < field_size; i++)
   {
      playing_field[i] = new int[field_size];
      last_field[i] = new int[field_size];
   }
   for (int i = 0; i < field_size; i++)
   {
      for (int j = 0; j < field_size; j++)
      {
         playing_field[i][j] = 0;
         last_field[i][j] = 0;
      }
   }
}

void delete_field()
{
   for (int i = 0; i < field_size; i++)
   {
      delete playing_field[i];
      delete last_field[i];
   }
   delete playing_field;
   delete last_field;
}

void new_number()
{
   int i, j;
   int num;
   
   i = rand() % field_size;
   j = rand() % field_size;
   num = rand() % 6;
   while (playing_field[i][j] != 0)
   {
      i = rand() % field_size;
      j = rand() % field_size;
   }
   if (num == 0)
      playing_field[i][j] = 4;
   else
      playing_field[i][j] = 2;
   
   free_fields--;
}

void draw_field()
{
   setfillstyle(SOLID_FILL, WHITE);
   putimage(0, 0, background_image, COPY_PUT);
   string text = "Score: " + to_string(score);
   setusercharsize(1, 1, 5, 3);
   settextstyle(GOTHIC_FONT, HORIZ_DIR, USER_CHAR_SIZE);
   settextjustify(LEFT_TEXT, BOTTOM_TEXT);
   setbkcolor(COLOR(70, 70, 70));
   outtextxy(340, 80, text.c_str()); 
   setfillstyle(SOLID_FILL, COLOR(100, 100, 100));
   for (int i = 0; i <= field_size; i++)
   {
      bar(340 + i * 600 / field_size, 150, 340 + i * 600 / field_size + field_width, 750 + field_width);
      bar(340, 150 + i * 600 / field_size, WIDTH - 340 + field_width, 150 + i * 600 / field_size + field_width);
   }
}

void draw_numbers()
{
   for (int i = 0; i < field_size; i++)
   {
      for (int j = 0; j < field_size; j++)
      {
         if (playing_field[i][j] != 0)
         {
            int r = find_num(playing_field[i][j]);
            setfillstyle(SOLID_FILL, COLOR(r * 20, 0, 0));
            bar (340 + (j * 600/field_size) + (field_width + 1), 150 + (i * 600 / field_size) + (field_width + 1), 340 + (j * 600/field_size) + 600 / field_size, 150 + (i * 600 / field_size) + 600 / field_size);
            string text = to_string(playing_field[i][j]);
            settextjustify(CENTER_TEXT, BOTTOM_TEXT);
            setbkcolor(COLOR(r * 20, 0, 0));
            setusercharsize(600 - text.length() * 5, 600 + field_size * field_size * field_size * text.length() / 2, 800, 600 + field_size * field_size * text.length());
            settextstyle(GOTHIC_FONT, HORIZ_DIR, USER_CHAR_SIZE);
            outtextxy(340 + (j * (600 / field_size)) + (600 / field_size / 2) + field_width / 2, 150 + (i * 600 / field_size) + ((600 / field_size + field_width) / 2) + 0.5 * textheight(text.c_str()), text.c_str());
         }
         else
         {
            setfillstyle(SOLID_FILL, COLOR(120, 120, 120));
            bar (340 + (j * 600/field_size) + (field_width + 1), 150 + (i * 600 / field_size) + (field_width + 1), 340 + (j * 600/field_size) + 600 / field_size, 150 + (i * 600 / field_size) + 600 / field_size);   
         }
      }
   }
}

int find_num(int n)
{
   int res = 0;
   while(n != 1)
   {
      n /= 2;
      res++;
   }
   return res;
}

void save_field()
{
   for (int i = 0; i < field_size; i++)
   {
      for (int j = 0; j < field_size; j++)
      {
         last_field[i][j] = playing_field[i][j];
      }
   }
   last_score = score;
   last_free_fields = free_fields;
}

void back_step()
{
   for (int i = 0; i < field_size; i++)
   {
      for (int j = 0; j < field_size; j++)
      {
         playing_field[i][j] = last_field[i][j];
      }
   }
   score = last_score;
   free_fields = last_free_fields;
}

bool can_move(int key)
{
   if (key == KEY_UP)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = 0; j < field_size; j++)
         {
            if (playing_field[j][i] == 0)
            {
               for (int k = j + 1; k < field_size; k++)
               {
                  if (playing_field[k][i] != 0)
                  {
                     return true;
                  }
               }
            }
         }
      }
   }
   else if (key == KEY_DOWN)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = field_size - 1; j >= 0; j--)
         {
            if (playing_field[j][i] == 0)
            {
               for (int k = j - 1; k >= 0; k--)
               {
                  if (playing_field[k][i] != 0)
                  {
                     return true;
                  }
               }
            }
         }
      }
   }
   else if (key == KEY_LEFT)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = 0; j < field_size; j++)
         {
            if (playing_field[i][j] == 0)
            {
               for (int k = j + 1; k < field_size; k++)
               {
                  if (playing_field[i][k] != 0)
                  {
                     return true;
                  }
               }
            }
         }
      }
   }
   else if (key == KEY_RIGHT)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = field_size - 1; j >= 0; j--)
         {
            if (playing_field[i][j] == 0)
            {
               for (int k = j - 1; k >= 0; k--)
               {
                  if (playing_field[i][k] != 0)
                  {
                     return true;
                  }
               }
            }
         }
      }
   }
   return false;
}

bool can_combine(int key)
{
   if (key == KEY_UP)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = 0; j < field_size - 1; j++)
         {
            if (playing_field[j + 1][i] == playing_field[j][i] && playing_field[j][i] > 0)
            {
               return true;
            }
         }
      }
   }
   else if (key == KEY_DOWN)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = field_size - 1; j > 0; j--)
         {
            if (playing_field[j - 1][i] == playing_field[j][i] && playing_field[j][i] > 0)
            {
               return true;
            }
         }
      }
   }
   else if (key == KEY_LEFT)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = 0; j < field_size - 1; j++)
         {
            if (playing_field[i][j + 1] == playing_field[i][j] && playing_field[i][j] > 0)
            {
               return true;
            }
         }
      }
   }
   else if (key == KEY_RIGHT)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = field_size - 1; j > 0; j--)
         {
            if (playing_field[i][j - 1] == playing_field[i][j] && playing_field[i][j] > 0)
            {
               return true;
            }
         }
      }
   }
   else
   {
      return false;
   }
   return false;
}

void move(int key)
{
   if (key == KEY_UP)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = 0; j < field_size; j++)
         {
            if (playing_field[j][i] == 0)
            {
               for (int k = j + 1; k < field_size; k++)
               {
                  if (playing_field[k][i] != 0)
                  {
                     playing_field[j][i] = playing_field[k][i];
                     playing_field[k][i] = 0;
                     break;
                  }
               }
            }
         }
      }
   }
   else if (key == KEY_DOWN)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = field_size - 1; j >= 0; j--)
         {
            if (playing_field[j][i] == 0)
            {
               for (int k = j - 1; k >= 0; k--)
               {
                  if (playing_field[k][i] != 0)
                  {
                     playing_field[j][i] = playing_field[k][i];
                     playing_field[k][i] = 0;
                     break;
                  }
               }
            }
         }
      }
   }
   else if (key == KEY_LEFT)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = 0; j < field_size; j++)
         {
            if (playing_field[i][j] == 0)
            {
               for (int k = j + 1; k < field_size; k++)
               {
                  if (playing_field[i][k] != 0)
                  {
                     playing_field[i][j] = playing_field[i][k];
                     playing_field[i][k] = 0;
                     break;
                  }
               }
            }
         }
      }
   }
   else if (key == KEY_RIGHT)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = field_size - 1; j >= 0; j--)
         {
            if (playing_field[i][j] == 0)
            {
               for (int k = j - 1; k >= 0; k--)
               {
                  if (playing_field[i][k] != 0)
                  {
                     playing_field[i][j] = playing_field[i][k];
                     playing_field[i][k] = 0;
                     break;
                  }
               }
            }
         }
      }
   }
   else
   {
      return;
   }
   
   draw_field();
   draw_numbers();
   swapbuffers();
   delay(DELAY_TIME);
}

void combine(int key)
{
   if (key == KEY_UP)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = 0; j < field_size - 1; j++)
         {
            if (playing_field[j + 1][i] == playing_field[j][i] && playing_field[j][i] > 0)
            {
               playing_field[j][i] *= 2;
               score += playing_field[i][j];
               playing_field[j + 1][i] = 0;
               free_fields++;
               combined = true;
            }
         }
      }
   }
   else if (key == KEY_DOWN)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = field_size - 1; j > 0; j--)
         {
            if (playing_field[j - 1][i] == playing_field[j][i] && playing_field[j][i] > 0)
            {
               playing_field[j][i] *= 2;
               score += playing_field[i][j];
               playing_field[j - 1][i] = 0;
               free_fields++;
               combined = true;
            }
         }
      }
   }
   else if (key == KEY_LEFT)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = 0; j < field_size - 1; j++)
         {
            if (playing_field[i][j + 1] == playing_field[i][j] && playing_field[i][j] > 0)
            {
               playing_field[i][j] *= 2;
               score += playing_field[i][j];
               playing_field[i][j + 1] = 0;
               free_fields++;
               combined = true;
            }
         }
      }
   }
   else if (key == KEY_RIGHT)
   {
      for (int i = 0; i < field_size; i++)
      {
         for (int j = field_size - 1; j > 0; j--)
         {
            if (playing_field[i][j - 1] == playing_field[i][j] && playing_field[i][j] > 0)
            {
               playing_field[i][j] *= 2;
               score += playing_field[i][j];
               playing_field[i][j - 1] = 0;
               free_fields++;
               combined = true;
            }
         }
      }
   }
   else
   {
      return;
   }
   
   if (combined = true)
   {
      combined = false;
      move(key);
   }
}

bool check_over()
{
   for (int i = 0; i < field_size; i++)
   {
      for (int j = 0; j < field_size - 1; j++)
      {
         if (playing_field[j + 1][i] == playing_field[j][i] && playing_field[j][i] > 0)
         {
            return true;
         }
      }
   }
      
   for (int i = 0; i < field_size; i++)
   {
      for (int j = 0; j < field_size - 1; j++)
      {
         if (playing_field[i][j + 1] == playing_field[i][j] && playing_field[i][j] > 0)
         {
            return true;
         }
      }
   }
   return false;
}

void game_over()
{
   string over = "Game Over!";
   string s = "Нажмите ESC чтобы вернуться в главное меню...";
   settextjustify(CENTER_TEXT, BOTTOM_TEXT);
   setusercharsize(1, 1, 1, 1);
   outtextxy(WIDTH / 2, HEIGHT / 2 + 0.5 * textheight(over.c_str()), over.c_str());
   outtextxy(WIDTH / 2, HEIGHT / 2 + textheight(s.c_str()) * 2, s.c_str());
   swapbuffers();
   int choice = getch();
   while (choice != 27 && choice != 8)
   {
      choice = getch();
   }
   if (choice == 8)
      back_step();
}