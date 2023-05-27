#define FONT_TEXT 3
#include <ctime>
#include <fstream>
#include "game.h"
#include "menu.h"
#include "graphics.h"

using namespace std;

extern int field_size; // Режим игры
extern IMAGE *background_image; // Фон игры

IMAGE *win_image, *lose_image;

// Массив структуры клеток цифр
Cell **Cells;

// Массив индексов клеток которые двигаются
int **move_pos;


int speed; // Скорость цифр
int field_width; // Ширина игрового поля.

// Переменные состояния игры
long long **play_field;
int free_fields;
bool win; 
int score, record;

// Переменные предыдущего хода
long long **last_field;
int last_score, last_free_fields;
bool last_win;

/* 
Функция в которой выполняются все игровые функции
загрузка данных, игра и очистка памяти
*/
void game()
{
   cleardevice();
   swapbuffers();
   
   load_game();
   create_field();
   open_record();
   play();
   delete_field();
}

// Загрузка начальных данных состояния игры
void load_game()
{
   // Установка ширины игрвого поля
   field_width = 600;
   while (field_width % field_size != 0)
      field_width++;
   
   free_fields = field_size * field_size;
   score = 0;
   last_score = 0;
   
   speed = field_size * 5;
   
   win = false;
   last_win = false;
   
   // Загрузка изображений
   win_image = loadBMP("images/win.bmp");
   lose_image = loadBMP("images/over.bmp");
}

// Игровой процесс
void play()
{
   srand(time(NULL));
   
   while(true)
   {
      int key; // Переменная клавиши
      draw_field();
      draw_numbers();
      swapbuffers();
      
      key = getch();
      if (key == 0)
         continue;
      
      // Выход и ход назад
      if (key == KEY_ESC)
         break;
      else if (key == KEY_BACKSPACE)
      {
         back_step();
         continue;
      }
      
      // Проверка на возможность двигаться в выбранном направлении
      if (can_move(key) == true || 
         can_combine(key) == true)
      {
         save_field();
         move(key);
      }
      else
         continue;
      
      // Создание цифры на случайной позиции
      if (free_fields > 0)
         new_number();
      
      if (check_lose() == true) // проигрыш
      {
         draw_over(lose_image);
         
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
      else if (check_win() == true) // победа
      {
         draw_over(win_image);
         
         win = true;
         int choice = getch();
         while (choice != KEY_ENTER && 
                  choice != KEY_BACKSPACE &&
                  choice != KEY_ESC)
            choice = getch();
         
         if (choice == KEY_BACKSPACE)
            back_step();
         else if (choice == KEY_ESC)
            break;
      }
   }
   // Сохранение рекорда после игры
   save_record();
}

// выделение памяти под все игровые поля
void create_field()
{
   Cells = new Cell*[field_size];
   play_field = new long long*[field_size];
   
   move_pos = new int*[field_size * field_size];
   last_field = new long long*[field_size];
   
   for (int i = 0; i < field_size; i++)
   {
      Cells[i] = new Cell[field_size];
      last_field[i] = new long long[field_size];
      play_field[i] = new long long[field_size];

      
      int offset = field_width / field_size / 2 + 
                        (FIELD_WALL_WIDTH / 2);
      for (int j = 0; j < field_size; j++)
      {
         play_field[i][j] = 0;
         last_field[i][j] = play_field[i][j];
         Cells[i][j] = 
         {play_field[i][j], 
            X_OFFSET + j * (field_width / field_size) + offset,
            Y_OFFSET + i * (field_width / field_size) + offset, 
            field_width / field_size / 2, 0, 0, 0, 
            X_OFFSET + j * (field_width / field_size) + offset, 
            Y_OFFSET + i * (field_width / field_size) + offset};
         }
   }
   
   for (int i = 0; i < field_size * field_size; i++)
   {
      move_pos[i] = new int[2];
      for (int j = 0; j < 2; j++)
         move_pos[i][j] = 0;
   }
   
   // Создание двух цифр в начале игры
   new_number();
   new_number();
}

// Считывание рекорда с файла
void open_record()
{
   string path = "records/record_" + 
                     to_string(field_size) + ".txt";
   ifstream fin;
   fin.open(path);
   record = 0;
   if (!fin.eof())
      fin >> record;
   fin.close();
}

// Сохранение рекорда в файл
void save_record()
{
   string path = "records/record_" + 
                     to_string(field_size) + ".txt";
   ofstream fout;
   fout.open(path);
   fout << record;
   fout.close();
}

// Очистка памяти игры
void delete_field() 
{
   freeimage(win_image);
   freeimage(lose_image);
   
   for (int i = 0; i < field_size; i++)
   {
      delete[] last_field[i];
      delete[] Cells[i];
      delete[] play_field[i];
      delete[] move_pos[i];
   }
   delete[] move_pos;
   delete[] Cells;
   delete[] last_field;
   delete[] play_field;
}

// Создание цифры на случайной позиции поля
void new_number()
{
   int i, j;
   int num;
   int chance = 10;
   
   i = rand() % field_size;
   j = rand() % field_size;
   
   // Пока случайная позиция не пустая, ищем новую
   while (play_field[i][j] != 0)
   {
      i = rand() % field_size;
      j = rand() % field_size;
   }
   
   // Выбор цифры в зависимости от шанса
   num = rand() % chance;
   
   if (num == 0)
      play_field[i][j] = 4;
   else
      play_field[i][j] = 2;
   
   // Обновить игрвое поле
   refresh_field();
   free_fields--;
}

// Отрисовка игрового поля
void draw_field()
{
   // Фон
   putimage(0, 0, background_image, COPY_PUT);
   
   // Текст очков и рекорда
   string score_txt = "Score: " + to_string(score);
   string record_txt = "Record: " + to_string(record);
   
   // Настройка шрифта
   setusercharsize(6, 5, 4, 3);
   settextstyle(FONT_TEXT, HORIZ_DIR, USER_CHAR_SIZE);
   setbkcolor(imagegetpixel(background_image, 0, 0));
   setcolor(WHITE);
   
   // Смещения позиции выводимых значений
   int score_y = 80;
   int record_y = 120;
   
   settextjustify(LEFT_TEXT, BOTTOM_TEXT);
   outtextxy(X_OFFSET, score_y, score_txt.c_str());
   outtextxy(X_OFFSET, record_y, record_txt.c_str());
   
   
   // Отрисовка фона игрового поля
   setfillstyle(SOLID_FILL, COLOR(100, 100, 100));
   bar (X_OFFSET, Y_OFFSET,
      X_OFFSET + field_width + FIELD_WALL_WIDTH,
      Y_OFFSET + field_width + FIELD_WALL_WIDTH);
   
   setfillstyle(SOLID_FILL, COLOR(70, 70, 70));
   setcolor(WHITE);
   
   
   int field_down = 150 + field_width; // нижняя точка игрового поля
   
   // отрисока линий игрового поля
   for (int i = 0; i <= field_size; i++) 
   {
      int offset = i * field_width / field_size;
      
      bar(X_OFFSET + offset, Y_OFFSET,
         X_OFFSET + offset + FIELD_WALL_WIDTH, 
         field_down + FIELD_WALL_WIDTH);
      
      bar(X_OFFSET, Y_OFFSET + offset, 
      WIDTH - X_OFFSET + FIELD_WALL_WIDTH,
      Y_OFFSET + offset + FIELD_WALL_WIDTH);
   }
}

// Отрисовка клеток с цифрами игрового поля
void draw_numbers()
{
   // Установка вывода текста по центру
   settextjustify(CENTER_TEXT, BOTTOM_TEXT);
   for (int i = 0; i < field_size; i++)
      for (int j = 0; j < field_size; j++)
         if (Cells[i][j].data > 0) // Вывод при условии что есть цифра
         {
            // Текст который необходимо вывести
            string text = to_string(Cells[i][j].data);
            int Cell_background_color = 
            COLOR(Cells[i][j].red, 
                        Cells[i][j].green, 
                        Cells[i][j].blue);
            
            // Настройки вывода текста
            setbkcolor(Cell_background_color);
            setfillstyle(SOLID_FILL, Cell_background_color);
            
            // Коэффицент уменьшения шрифта
            int coefficient = (text.length() + 1) * field_size;
            setusercharsize(31, coefficient + 2, 31, coefficient);
            settextstyle(FONT_TEXT, HORIZ_DIR, USER_CHAR_SIZE);
            
            // Смещение расположения цифры
            int offset = Cells[i][j].size - FIELD_WALL_WIDTH / 2;
            // Отрисовка фона клетки
            bar (Cells[i][j].pos_x - offset, 
                  Cells[i][j].pos_y - offset, 
                  Cells[i][j].pos_x + offset, 
                  Cells[i][j].pos_y + offset);
            
            // Отрисовка цифры клетки
            outtextxy(Cells[i][j].pos_x, 
               Cells[i][j].pos_y + 0.5 * textheight(text.c_str()), 
               text.c_str());
         }
}

// Найти степень двойки для заданного числа
// Функция нужна для установки цвета фона клеток
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

// Сохранение игрового поля в переменные предыдущего хода
void save_field()
{
   for (int i = 0; i < field_size; i++)
      for (int j = 0; j < field_size; j++)
         last_field[i][j] = play_field[i][j];
   
   last_score = score;
   last_free_fields = free_fields;
}

// Ход назад
// Присвоение значениям текущего поля, значения предыдущего хода 
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

// Проверка возможности хода в выбранном направлении 
bool can_move(int key)
{
   // Переменные параметров перебора матрицы 
   int j_start, step, end;
  
   // При кнопках вверх и вниз перебор будет вертикальным
   // При кнопках влево и вправо - горизонтальным
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
   
   // Если не найдена возможность сделать ход, то вернуть false
   return false;
}

// Проверка возможности соединения клеток в выбранном направлении 
bool can_combine(int key)
{
   // При кнопках вверх и вниз перебор будет вертикальным
   // При кнопках влево и вправо - горизонтальным
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

// Движение цифр в выбранном направлении
void move(int key)
{
   // Переменные параметров перебора матрицы 
   int j_start, end, step;
   
   // Количество клеток которые необходимо двигать
   int n_moved_Cells = 0;

   // Скорость движения по вертикале и горизонтале
   int x_speed = 0, y_speed = 0;
   
   // Установка параметров  в зависимости от клавиши
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
      /* Если текущая клетка ноль и проверяемая не ноль,
         то передвигаем проверяемую клетку на текущую и 
         перебор продолжается.
                        
         Если же текущая и проверяемая равны, то соединяем их
         и заканчиваем перебор*/
      for (int i = 0; i < field_size; i++)
         for (int j = j_start; j != end; j += step)
            for (int k = j + step; k != end; k += step)
            {
               
               if ((play_field[i][j] == 0 && play_field[i][k] != 0) ||
                     play_field[i][j] == play_field[i][k] && play_field[i][j] > 0)
               {
                  move_pos[n_moved_Cells][0] = i;
                  move_pos[n_moved_Cells][1] = k;
                  n_moved_Cells++;
               }
               
               if (play_field[i][j] == 0 && play_field[i][k] != 0)
               {
                  Cells[i][k].next_x = Cells[i][j].pos_x;
                  play_field[i][j] = play_field[i][k];
                  play_field[i][k] = 0;
               }
               else if (play_field[i][j] != 0 && 
               play_field[i][j] == play_field[i][k])
               {
                  Cells[i][k].next_x = Cells[i][j].next_x;
                  play_field[i][j] *= 2;
                  play_field[i][k] = 0;
                  free_fields++; // Увеличиваем свободные клетки
                  score += play_field[i][j]; // Увеличиваем очки
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
      /* Перебор по вертикале точно такой же, но
       i и j позиции поменяны местами*/
      for (int i = 0; i < field_size; i++)
         for (int j = j_start; j != end; j += step)
            for (int k = j + step; k != end; k += step)
            {
               if ((play_field[j][i] == 0 && play_field[k][i] != 0) ||
                     play_field[j][i] == play_field[k][i] && play_field[j][i] > 0)
               {
                  move_pos[n_moved_Cells][0] = k;
                  move_pos[n_moved_Cells][1] = i;
                  n_moved_Cells++;
               }
               
               if (play_field[j][i] == 0 && play_field[k][i] != 0)
               {
                  Cells[k][i].next_y = Cells[j][i].pos_y;
                  play_field[j][i] = play_field[k][i];
                  play_field[k][i] = 0;
               }
               else if (play_field[j][i] != 0 && 
                        play_field[j][i] == play_field[k][i])
               {
                  Cells[k][i].next_y = Cells[j][i].pos_y;
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
   
   // Воспроизводим анимацию
   do_animation(x_speed, y_speed, n_moved_Cells);
}

// Анимация движения клеток
void do_animation(int x_speed, int y_speed, int n_moved_Cells)
{
   /* Анимация движения будет воспроизводится пока происходит
   движение, если его не было то прерывается*/
   
   bool moved;
   do
   {
      moved = false;
      for (int i = 0; i < n_moved_Cells; i++)
      {
         if (((x_speed >= 0 && y_speed >= 0) &&
            (Cells[move_pos[i][0]][move_pos[i][1]].next_x >
            Cells[move_pos[i][0]][move_pos[i][1]].pos_x ||
            Cells[move_pos[i][0]][move_pos[i][1]].next_y >
            Cells[move_pos[i][0]][move_pos[i][1]].pos_y)) 
            ||
            ((x_speed <= 0 && y_speed <= 0) &&
            (Cells[move_pos[i][0]][move_pos[i][1]].next_x <
            Cells[move_pos[i][0]][move_pos[i][1]].pos_x ||
            Cells[move_pos[i][0]][move_pos[i][1]].next_y <
            Cells[move_pos[i][0]][move_pos[i][1]].pos_y)))
         {
            Cells[move_pos[i][0]][move_pos[i][1]].pos_x += x_speed;
            Cells[move_pos[i][0]][move_pos[i][1]].pos_y += y_speed;
            moved = true;
         }
      }
      // Отрисовываем числа после каждого движения
      draw_field();
      draw_numbers();
      swapbuffers();
   }
   while (moved == true);
   
   // Обновляем поле
   refresh_field();
}

/* 
Обновление массива структур Cells
Приравнивание значений из playing_field в Cells
Нужно для нормальной работы анимации
*/
void refresh_field()
{
   int cell_width = field_width / field_size;
   for (int i = 0; i < field_size; i++)
      for (int j = 0; j < field_size; j++)
      {
         Cells[i][j].data = play_field[i][j];
         
         Cells[i][j].pos_x = X_OFFSET + j * cell_width + 
               Cells[i][j].size + FIELD_WALL_WIDTH / 2;
         Cells[i][j].pos_y = Y_OFFSET + i * cell_width + 
               Cells[i][j].size + FIELD_WALL_WIDTH / 2;
         
         Cells[i][j].next_x = Cells[i][j].pos_x;
         Cells[i][j].next_y = Cells[i][j].pos_y;
         
         Cells[i][j].red = 255 - 
            (find_num(play_field[i][j]) * 15 + 50) % 255;
         Cells[i][j].green = 255 - 
            (find_num(play_field[i][j]) * 10 + 50) % 255;
         Cells[i][j].blue = 255;
      }
}

// Проверка победы (если встречается 2048, то победа)
bool check_win()
{
   if (win == true)
      return false;
   
   for (int i = 0; i < field_size; i++)
      for (int j = 0; j < field_size; j++)
         if (play_field[i][j] >= 2048)
            return true;
   return false;
}

// Провека проигрыша
bool check_lose()
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

// Отрисовка окна победы или поражения
void draw_over(IMAGE* image)
{
   // Смещение окна
   int x_pos = 240, y_pos = 200;
  
   draw_field();
   draw_numbers();
   putimage(x_pos, y_pos, image, COPY_PUT);
   swapbuffers();
}