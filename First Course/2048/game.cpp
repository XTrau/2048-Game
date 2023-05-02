#define DELAY_TIME 100
#include <ctime>
#include <fstream>
#include "game.h"
#include "menu.h"
#include "graphics.h"

using namespace std;

extern int field_size;
extern IMAGE *background_image;

Num **nums;

int x_offset = 340, y_offset = 150;

int **play_field;
int free_fields;
int field_width = 10;
int score, record;

int **move_pos;
int n_nums = 0;
int x_speed, y_speed;

int **last_field;
int last_score, last_free_fields;

void game()
{
   cleardevice();
   swapbuffers();
   
   create_field();
   open_record();
   play();
   delete_field();
}

void play()
{
   srand(time(NULL));
   
   while(true)
   {
      int key;
      draw_field();
      draw_numbers();
      swapbuffers();
      
      key = getch();
      if (key == 0)
         continue;
      else if (key == KEY_ESC)
         break;
      else if (key == KEY_BACKSPACE)
      {
         back_step();
         continue;
      }
      
      if (can_move(key) == true || 
         can_combine(key) == true)
      {
         save_field();
         move(key);
         do_animation();
         combine(key);
         do_animation();
      }
      else
         continue;
         
      if (free_fields > 0)
         new_number();
      
      if (check_over() == true && 
         free_fields == 0)
      {
         draw_field();
         draw_numbers();
         game_over();
         
         int choice = getch();
         while (choice != KEY_ESC && choice != KEY_BACKSPACE)
            choice = getch();
         
         if (choice == KEY_BACKSPACE)
         {
            back_step();
            continue;
         }
         break;
      }
   }
   save_record();
}

void create_field()
{
   free_fields = field_size * field_size;
   score = 0;
   last_score = 0;
   
   nums = new Num*[field_size];
   play_field = new int*[field_size];
   
   move_pos = new int*[field_size * field_size];
   last_field = new int*[field_size];
   
   for (int i = 0; i < field_size; i++)
   {
      nums[i] = new Num[field_size];
      last_field[i] = new int[field_size];
      play_field[i] = new int[field_size];
      
      int offset = 600 / field_size / 2 + (field_width / 2);
      for (int j = 0; j < field_size; j++)
      {
         nums[i][j] = 
         {0, x_offset + j * (600 / field_size) + offset,
            y_offset + i * (600 / field_size) + offset, 
            600 / field_size / 2, 0, 
            x_offset + j * (600 / field_size) + offset, 
            y_offset + i * (600 / field_size) + offset};
         
         last_field[i][j] = 0;
         play_field[i][j] = 0;
      }
   }
   
   for (int i = 0; i < field_size * field_size; i++)
   {
      move_pos[i] = new int[2];
      for (int j = 0; j < 2; j++)
         move_pos[i][j] = 0;
   }
   
   new_number();
   new_number();
}

void open_record()
{
   string path = "records/record_" + to_string(field_size) + ".txt";
   ifstream fin;
   fin.open(path);
   record = 0;
   if (!fin.eof())
      fin >> record;
   fin.close();
}

void save_record()
{
   string path = "records/record_" + to_string(field_size) + ".txt";
   ofstream fout;
   fout.open(path);
   fout << record;
   fout.close();
}

void delete_field() 
{
   for (int i = 0; i < field_size; i++)
   {
      delete[] last_field[i];
      delete[] nums[i];
      delete[] play_field[i];
      delete[] move_pos[i];
   }
   
   delete[] move_pos;
   delete[] nums;
   delete[] last_field;
   delete[] play_field;
}

void new_number()
{
   int i, j;
   int num;
   int chance = 10;
   
   i = rand() % field_size;
   j = rand() % field_size;
   
   num = rand() % chance;
   
   while (play_field[i][j] != 0)
   {
      i = rand() % field_size;
      j = rand() % field_size;
   }
   
   if (num == 0)
      play_field[i][j] = 4;
   else
      play_field[i][j] = 2;
   
   refresh_field();
   free_fields--;
}

void draw_field()
{
   putimage(0, 0, background_image, COPY_PUT);
   
   string score_txt = "Score: " + to_string(score);
   string record_txt = "Record: " + to_string(record);
   
   setusercharsize(2, 3, 3, 3);
   settextstyle(GOTHIC_FONT, HORIZ_DIR, USER_CHAR_SIZE);
   setbkcolor(COLOR(70, 70, 70));
   
   settextjustify(LEFT_TEXT, BOTTOM_TEXT);
   outtextxy(x_offset, 80, score_txt.c_str());
   outtextxy(x_offset, 120, record_txt.c_str());
   
   setfillstyle(SOLID_FILL, COLOR(100, 100, 100));
   
   for (int i = 0; i <= field_size; i++) 
   {
      bar(x_offset + i * 600 / field_size, y_offset,
         x_offset + i * 600 / field_size + field_width, 
         750 + field_width);
      bar(x_offset, y_offset + i * 600 / field_size, 
         WIDTH - x_offset + field_width,
         y_offset + i * 600 / field_size + field_width);
   }
}

void draw_numbers()
{
   int coefficient = field_size * field_size;
   settextjustify(CENTER_TEXT, BOTTOM_TEXT);
   for (int i = 0; i < field_size; i++)
      for (int j = 0; j < field_size; j++)
         if (nums[i][j].data > 0)
         {
            string text = to_string(nums[i][j].data);
            
            setbkcolor(COLOR(nums[i][j].red, 0, 0));
            setfillstyle(SOLID_FILL, COLOR(nums[i][j].red, 0, 0));
            setusercharsize(600 - text.length() * 5, 
            600 + field_size * coefficient * text.length() / 2, 800, 
            600 + coefficient * text.length());
            settextstyle(GOTHIC_FONT, HORIZ_DIR, USER_CHAR_SIZE);
            
            int offset = nums[i][j].size - field_width / 2;
            bar (nums[i][j].pos_x - offset, 
                  nums[i][j].pos_y - offset, 
                  nums[i][j].pos_x + offset, 
                  nums[i][j].pos_y + offset);
            
            outtextxy(nums[i][j].pos_x, 
               nums[i][j].pos_y + 0.5 * textheight(text.c_str()), 
               text.c_str());
         }
}

int find_num(int n) 
{
   if (n < 1)
      return 0;
   
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
      for (int j = 0; j < field_size; j++)
         last_field[i][j] = play_field[i][j];
   
   last_score = score;
   last_free_fields = free_fields;
}

void back_step()
{
   for (int i = 0; i < field_size; i++)
      for (int j = 0; j < field_size; j++)
         play_field[i][j] = last_field[i][j];
   
   score = last_score;
   free_fields = last_free_fields;
   refresh_field();
}

bool can_move(int key)
{
   int j_start, step, end;
  
   if (key == KEY_UP || key == KEY_LEFT)
   {
      j_start = 0; 
      step = 1;
      end = field_size;
   }
   else if (key == KEY_DOWN || key == KEY_RIGHT)
   {
      j_start = field_size - 1; 
      step = -1;
      end = -1;
   }
   
   if (key == KEY_UP || key == KEY_DOWN)
   {
      for (int i = 0; i < field_size; i++)
         for (int j = j_start; j != end; j += step)
            if (play_field[j][i] == 0)
            {
               int k_start = j + step;
               for (int k = k_start; k != end; k += step)
                  if (play_field[k][i] != 0)
                     return true;
            }
   }
   else if (key == KEY_RIGHT || key == KEY_LEFT)
   {
      for (int i = 0; i < field_size; i++)
         for (int j = j_start; j != end; j += step)
            if (play_field[i][j] == 0)
            {
               int k_start = j + step;
               for (int k = k_start; k != end; k += step)
                  if (play_field[i][k] != 0)
                     return true;
            }
   }
   return false;
}

bool can_combine(int key)
{
   if (key == KEY_UP || key == KEY_DOWN)
   {
      for (int i = 0; i < field_size; i++)
         for (int j = 0; j < field_size - 1; j++)
            if (play_field[j][i] != 0 &&
            play_field[j][i] == play_field[j + 1][i])
               return true;
   }
   else if (key == KEY_RIGHT || key == KEY_LEFT)
   {
      for (int i = 0; i < field_size; i++)
         for (int j = 0; j < field_size - 1; j++)
            if (play_field[i][j] != 0 &&
            play_field[i][j] == play_field[i][j + 1])
               return true;
   }
   return false;
}

void move(int key) 
{
   x_speed = 0, y_speed = 0;
   int j_start, end, step;
   switch (key)
   {
      case KEY_UP:
         j_start = 0; 
         step = 1;
         end = field_size;
         y_speed = -15;
         break;
      case KEY_DOWN:
         j_start = field_size - 1;
         step = -1;
         end = -1;
         y_speed = 15;
         break;
      case KEY_LEFT:
         j_start = 0;
         step = 1;
         end = field_size;
         x_speed = -15;
         break;
      case KEY_RIGHT:
         j_start = field_size - 1;
         step = -1;
         end = -1;
         x_speed = 15;
         break;
   }

   if (key == KEY_RIGHT || key == KEY_LEFT)
   {
      for (int i = 0; i < field_size; i++)
         for (int j = j_start; j != end; j += step)
            if (play_field[i][j] == 0)
               for (int k = j + step; k != end; k += step)
                  if (play_field[i][k] != 0)
                  {
                     if (nums[i][k].pos_x == nums[i][k].next_x)
                     {
                        move_pos[n_nums][0] = i;
                        move_pos[n_nums][1] = k;
                        n_nums++;
                     }
                     nums[i][k].next_x = nums[i][j].pos_x;
                     play_field[i][j] = play_field[i][k];
                     play_field[i][k] = 0;
                     break;
                  }
   }
   else if (key == KEY_UP || key == KEY_DOWN)
   {
      for (int i = 0; i < field_size; i++)
         for (int j = j_start; j != end; j += step)
            if (play_field[j][i] == 0)
               for (int k = j + step; k != end; k += step)
                  if (play_field[k][i] != 0)
                  {
                     if (nums[i][k].pos_x == nums[i][k].next_x)
                     {
                        move_pos[n_nums][0] = k;
                        move_pos[n_nums][1] = i;
                        n_nums++;
                     }
                     nums[k][i].next_y = nums[j][i].pos_y;
                     play_field[j][i] = play_field[k][i];
                     play_field[k][i] = 0;
                     break;
                  }
   }
}

void combine(int key)
{
   bool combined = false;
   int j_start, end, step;
   
   switch (key)
   {
      case KEY_UP:
         j_start = 0;
         step = 1;
         end = field_size - 1;
         break;
      case KEY_DOWN:
         j_start = field_size - 1;
         step = -1;
         end = 0;
         break;
      case KEY_LEFT:
         j_start = 0;
         step = 1;
         end = field_size - 1;
         break;
      case KEY_RIGHT:
         j_start = field_size - 1;
         step = -1;
         end = 0;
         break;
   }
   
   if (key == KEY_RIGHT || key == KEY_LEFT)
   {
      for (int i = 0; i < field_size; i++)
         for (int j = j_start; j != end; j += step)
            if (play_field[i][j] != 0 && 
               play_field[i][j] == play_field[i][j + step])
            {
               play_field[i][j] *= 2;
               score += play_field[i][j];
               play_field[i][j + step] = 0;
               
               if (nums[i][j + step].pos_x == 
                  nums[i][j + step].next_x)
               {
                  move_pos[n_nums][0] = i;
                  move_pos[n_nums][1] = j + step;
                  n_nums++;
               }
               
               nums[i][j + step].next_x = nums[i][j].next_x;
               free_fields++;
               combined = true;
            }
   }
   else if (key == KEY_UP || key == KEY_DOWN)
   {
      for (int i = 0; i < field_size; i++)
         for (int j = j_start; j != end; j += step)
            if (play_field[j][i] != 0 &&
            play_field[j][i] == play_field[j + step][i])
            {
               play_field[j][i] *= 2;
               score += play_field[j][i];
               play_field[j + step][i] = 0;
               
               if (nums[j + step][i].pos_x == 
                  nums[j + step][i].next_x)
               {
                  move_pos[n_nums][0] = j + step;
                  move_pos[n_nums][1] = i;
                  n_nums++;
               }
               nums[j + step][i].next_y = nums[j][i].next_y;
               free_fields++;
               combined = true;
            }
   }
   
   if (score > record)
      record = score;
   
   if (combined == true)
      move(key);
}

void do_animation()
{
   bool moved;
   do
   {
      moved = false;
      for (int i = 0; i < n_nums; i++)
      {
         if (nums[move_pos[i][0]][move_pos[i][1]].next_x !=
            nums[move_pos[i][0]][move_pos[i][1]].pos_x ||
            nums[move_pos[i][0]][move_pos[i][1]].next_y !=
            nums[move_pos[i][0]][move_pos[i][1]].pos_y)
         {
            nums[move_pos[i][0]][move_pos[i][1]].pos_x += x_speed;
            nums[move_pos[i][0]][move_pos[i][1]].pos_y += y_speed;
            moved = true;
         }
      }
      draw_field();
      draw_numbers();
      swapbuffers();
   }
   while (moved == true);
   
   refresh_field();
   n_nums = 0;
}

void refresh_field()
{
   for (int i = 0; i < field_size; i++)
      for (int j = 0; j < field_size; j++)
      {
         nums[i][j].data = play_field[i][j];
         nums[i][j].red = find_num(play_field[i][j]) * 20;
         nums[i][j].pos_x = x_offset + j * (600 / field_size) + 
                           nums[i][j].size + (field_width / 2);
         nums[i][j].next_x = nums[i][j].pos_x;
         nums[i][j].pos_y = y_offset + i * (600 / field_size) + 
                           nums[i][j].size + (field_width / 2);
         nums[i][j].next_y = nums[i][j].pos_y;
      }
}

bool check_over()
{
   for (int i = 0; i < field_size; i++)
      for (int j = 0; j < field_size - 1; j++)
         if (play_field[i][j] == play_field[i][j + 1])
            return false;
   for (int i = 0; i < field_size - 1; i++)
      for (int j = 0; j < field_size; j++)
         if (play_field[i][j] == play_field[i + 1][j])
            return false;
         
   return true;
}

void game_over()
{
   string over = "Game Over!";
   string s = "Нажмите ESC чтобы вернуться в главное меню...";
   
   setbkcolor(COLOR(70, 70, 70));
   settextjustify(CENTER_TEXT, BOTTOM_TEXT);
   setusercharsize(1, 1, 3, 2);
   
   outtextxy(WIDTH / 2,
      HEIGHT / 2 + 0.5 * textheight(over.c_str()), over.c_str());
   outtextxy(WIDTH / 2,
      HEIGHT / 2 + textheight(s.c_str()) * 2, s.c_str());
   swapbuffers();
}