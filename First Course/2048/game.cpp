#define DELAY_TIME 100
#define FONT_TEXT 3
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

int field_width = 600;

long long **play_field;
bool win;
bool last_win;
int free_fields;
int field_wall_width = 10;
int score, record;
int speed = field_size * 5;

int **move_pos;
int n_nums = 0;
int x_speed, y_speed;

long long **last_field;
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
      
      if (key == KEY_ESC)
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
      }
      else
         continue;
      
      if (free_fields > 0)
         new_number();
      
      if (check_win() == true)
      {
         draw_field();
         draw_numbers();
         
         win = true;
         draw_win();
         int choice = getch();
         if (choice == KEY_BACKSPACE)
            back_step();
         else if (choice == KEY_ESC)
            break;
      }
      
      if (check_over() == true)
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
   while (field_width % field_size != 0)
      field_width++;
   
   free_fields = field_size * field_size;
   score = 0;
   last_score = 0;
   
   win = false;
   last_win = false;
   
   nums = new Num*[field_size];
   play_field = new long long*[field_size];
   
   move_pos = new int*[field_size * field_size];
   last_field = new long long*[field_size];
   
   for (int i = 0; i < field_size; i++)
   {
      nums[i] = new Num[field_size];
      last_field[i] = new long long[field_size];
      play_field[i] = new long long[field_size];
      
      int offset = field_width / field_size / 2 + (field_wall_width / 2);
      for (int j = 0; j < field_size; j++)
      {
         play_field[i][j] = 0;
         last_field[i][j] = play_field[i][j];
         nums[i][j] = 
         {play_field[i][j], 
            x_offset + j * (field_width / field_size) + offset,
            y_offset + i * (field_width / field_size) + offset, 
            field_width / field_size / 2, 0, 0, 0, 
            x_offset + j * (field_width / field_size) + offset, 
            y_offset + i * (field_width / field_size) + offset};
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
   
   setusercharsize(6, 5, 4, 3);
   settextstyle(FONT_TEXT, HORIZ_DIR, USER_CHAR_SIZE);
   setbkcolor(imagegetpixel(background_image, 0, 0));
   setcolor(WHITE);
   
   int score_y = 80;
   int record_y = 120;
   
   settextjustify(LEFT_TEXT, BOTTOM_TEXT);
   outtextxy(x_offset, score_y, score_txt.c_str());
   outtextxy(x_offset, record_y, record_txt.c_str());
   
   setfillstyle(SOLID_FILL, COLOR(100, 100, 100));
   bar (x_offset, y_offset,
      x_offset + field_width + field_wall_width,
      y_offset + field_width + field_wall_width);
      
   setfillstyle(SOLID_FILL, COLOR(70, 70, 70));
   setcolor(WHITE);
   
   int field_down = 750;
   for (int i = 0; i <= field_size; i++) 
   {
      bar(x_offset + i * field_width / field_size, y_offset,
         x_offset + i * field_width / field_size + field_wall_width, 
         field_down + field_wall_width);
      bar(x_offset, y_offset + i * field_width / field_size, 
         WIDTH - x_offset + field_wall_width,
         y_offset + i * field_width / field_size + field_wall_width);
   }
}

void draw_numbers()
{
   settextjustify(CENTER_TEXT, BOTTOM_TEXT);
   for (int i = 0; i < field_size; i++)
      for (int j = 0; j < field_size; j++)
         if (nums[i][j].data > 0)
         {
            string text = to_string(nums[i][j].data);
            int num_background_color = 
               COLOR(nums[i][j].red, nums[i][j].green, nums[i][j].blue);
            setbkcolor(num_background_color);
            setfillstyle(SOLID_FILL, num_background_color);
            int coefficient = (text.length() + 1) * field_size;
            setusercharsize(31, coefficient + 2, 31, coefficient);
            settextstyle(FONT_TEXT, HORIZ_DIR, USER_CHAR_SIZE);
            
            int offset = nums[i][j].size - field_wall_width / 2;
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
   
   win = last_win;
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
   int j_start, end, step;
   x_speed = 0, y_speed = 0;
   switch (key)
   {
      case KEY_UP:
         j_start = 0; 
         step = 1;
         end = field_size;
         y_speed = -speed;
         break;
      case KEY_DOWN:
         j_start = field_size - 1;
         step = -1;
         end = -1;
         y_speed = speed;
         break;
      case KEY_LEFT:
         j_start = 0;
         step = 1;
         end = field_size;
         x_speed = -speed;
         break;
      case KEY_RIGHT:
         j_start = field_size - 1;
         step = -1;
         end = -1;
         x_speed = speed;
         break;
   }
   if (key == KEY_RIGHT || key == KEY_LEFT)
   {
      for (int i = 0; i < field_size; i++)
         for (int j = j_start; j != end; j += step)
            for (int k = j + step; k != end; k += step)
            {
               if (play_field[i][j] == 0 && play_field[i][k] != 0)
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
               }
               else if (play_field[i][j] != 0 && 
                        play_field[i][j] == play_field[i][k])
               {
                  if (nums[i][k].pos_x == nums[i][k].next_x)
                  {
                     move_pos[n_nums][0] = i;
                     move_pos[n_nums][1] = k;
                     n_nums++;
                  }
                  nums[i][k].next_x = nums[i][j].next_x;
                  play_field[i][j] *= 2;
                  play_field[i][k] = 0;
                  free_fields++;
                  score += play_field[i][j];
                  break;
               }
               else if (play_field[i][j] != 0 && 
                        play_field[i][j] != play_field[i][k] && 
                        play_field[i][k] != 0)
                  break;
            }
   }
   else if (key == KEY_UP || key == KEY_DOWN)
   {
      for (int i = 0; i < field_size; i++)
         for (int j = j_start; j != end; j += step)
            for (int k = j + step; k != end; k += step)
            {
               if (play_field[j][i] == 0 && play_field[k][i] != 0)
               {
                  if (nums[k][i].pos_y == nums[k][i].next_y)
                  {
                     move_pos[n_nums][0] = k;
                     move_pos[n_nums][1] = i;
                     n_nums++;
                  }
                  nums[k][i].next_y = nums[j][i].pos_y;
                  play_field[j][i] = play_field[k][i];
                  play_field[k][i] = 0;
               }
               else if (play_field[j][i] != 0 && 
                        play_field[j][i] == play_field[k][i])
               {
                  if (nums[k][i].pos_y == nums[k][i].next_y)
                  {
                     move_pos[n_nums][0] = k;
                     move_pos[n_nums][1] = i;
                     n_nums++;
                  }
                  nums[k][i].next_y = nums[j][i].pos_y;
                  play_field[j][i] *= 2;
                  play_field[k][i] = 0;
                  free_fields++;
                  score += play_field[j][i];
                  break;
               }
               else if (play_field[j][i] != 0 && 
                        play_field[j][i] != play_field[k][i] && 
                        play_field[k][i] != 0)
                  break;
            }
   }
   if (score > record)
      record = score;
}

void do_animation()
{
   bool moved;
   do
   {
      moved = false;
      for (int i = 0; i < n_nums; i++)
      {
         if (((x_speed >= 0 && y_speed >= 0) &&
            (nums[move_pos[i][0]][move_pos[i][1]].next_x >
            nums[move_pos[i][0]][move_pos[i][1]].pos_x ||
            nums[move_pos[i][0]][move_pos[i][1]].next_y >
            nums[move_pos[i][0]][move_pos[i][1]].pos_y)) 
            ||
            ((x_speed <= 0 && y_speed <= 0) &&
            (nums[move_pos[i][0]][move_pos[i][1]].next_x <
            nums[move_pos[i][0]][move_pos[i][1]].pos_x ||
            nums[move_pos[i][0]][move_pos[i][1]].next_y <
            nums[move_pos[i][0]][move_pos[i][1]].pos_y)))
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
         
         nums[i][j].pos_x = x_offset + j * (field_width / field_size) + 
                                    nums[i][j].size + field_wall_width / 2;
         nums[i][j].pos_y = y_offset + i * (field_width / field_size) + 
                                    nums[i][j].size + field_wall_width / 2;
         
         nums[i][j].next_x = nums[i][j].pos_x;
         nums[i][j].next_y = nums[i][j].pos_y;
         
         nums[i][j].red = 255 - (find_num(play_field[i][j]) * 15 + 50) % 255;
         nums[i][j].green = 255 - (find_num(play_field[i][j]) * 10 + 50) % 255;
         nums[i][j].blue = 255 - (find_num(play_field[i][j]) * 0 + 0) % 255;
      }
}

bool check_win()
{
   if (win == true)
      return false;
   
   for (int i = 0; i < field_size; i++)
      for (int j = 0; j < field_size; j++)
         if (play_field[i][j] == 2048)
            return true;
   return false;
}

bool check_over()
{
   if (free_fields > 0)
      return false;
   
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

void draw_win()
{
   string win_text = "Поздравляем!\nВы Победили!";
   settextjustify(CENTER_TEXT, BOTTOM_TEXT);
   setusercharsize(6, 5, 4, 3);
   outtextxy(WIDTH / 2, HEIGHT / 2 + 0.5 * textheight(win_text.c_str()), win_text.c_str());
   swapbuffers();
}

void game_over()
{
   string over_text = "Игра окончена!";
   string wait_text = "Нажмите ESC чтобы вернуться в главное меню...";
   
   setbkcolor(COLOR(70, 70, 70));
   settextjustify(CENTER_TEXT, BOTTOM_TEXT);
   setusercharsize(6, 5, 4, 3);
   settextstyle(FONT_TEXT, HORIZ_DIR, USER_CHAR_SIZE);
   
   outtextxy(WIDTH / 2,
      HEIGHT / 2 + 0.5 * textheight(over_text.c_str()), over_text.c_str());
   outtextxy(WIDTH / 2,
      HEIGHT / 2 + textheight(wait_text.c_str()) * 2, wait_text.c_str());
   swapbuffers();
}