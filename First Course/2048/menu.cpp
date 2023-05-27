#include "menu.h"
#include "game.h"
#include "graphics.h"

using namespace std;

Button buttons[S_BUTTONS]; // массив кнопок

// Изображения главного меню
IMAGE *start_image, *background_image, 
            *menu_image, *settings_image, *about_image;
            
// Изображения настроек
IMAGE *four, *five, *six, *seven;

// Режим игры
int field_size = 4;

// Загрузка изображений и создание кнопок главного меню
void load()
{
   start_image = loadBMP("images/start.bmp");
   background_image = loadBMP("images/background.bmp");
   menu_image = loadBMP("images/menu.bmp");
   about_image = loadBMP("images/about.bmp");
   settings_image = loadBMP("images/settings.bmp");
   
   create_button(GAME,  540, 250, "buttons/game_button.bmp");
   create_button(SETTINGS,  515, 350, 
      "buttons/settings_button.bmp");
   create_button(ABOUT, 540, 450, "buttons/about_button.bmp");
   create_button(EXIT,  540, 550, "buttons/exit_button.bmp");
   create_button(FOUR, 320, 250, "buttons/4x4_button.bmp");
   create_button(FIVE, 760, 250, "buttons/5x5_button.bmp");
   create_button(SIX, 320, 500, "buttons/6x6_button.bmp");
   create_button(SEVEN, 760, 500, "buttons/7x7_button.bmp");
}

// Вывод начальной заставки игры
void start()
{
   putimage(0, 0, start_image, COPY_PUT);
   swapbuffers();
   getch();
}

// Запуск главного меню игры
void menu() 
{
   while (true)
   {
      int state = NONE;
      setfillstyle(SOLID_FILL, WHITE);
      while (state == NONE)
      {
         while (mousebuttons() != 1)
         {
            putimage(0, 0, menu_image, COPY_PUT);
            for (int i = 0; i < N_BUTTONS; i++)
            {
               putimage(buttons[i].left, buttons[i].top, 
                              buttons[i].image, TRANSPARENT_PUT);
               if (mousex() > buttons[i].left && 
                  mousex() < buttons[i].left + buttons[i].width &&
                  mousey() > buttons[i].top && 
                  mousey() < buttons[i].top + buttons[i].height)
               {
                  setfillstyle(SOLID_FILL, WHITE);
                  bar (buttons[i].left, 
                        buttons[i].top + 0.96 * buttons[i].height, 
                        buttons[i].left + buttons[i].width, 
                        buttons[i].top + buttons[i].height);
               }
            }
            swapbuffers();
            
            if(kbhit())
               getch();
            
            state = select_button(0);
         }
      }
      
      
      switch (state)
      {
         case GAME: game(); break;
         case SETTINGS: settings(); break;
         case ABOUT: about(); break;
         case EXIT: close(); return;
      }
   }
}

// Настройки игры
void settings()
{
   int state = NONE;
   while (state == NONE)
   {
      while (mousebuttons() != 1)
      {
         putimage(0, 0, settings_image, COPY_PUT);
         for (int i = N_BUTTONS + 1; i < S_BUTTONS; i++)
         {
            putimage(buttons[i].left, buttons[i].top, 
                           buttons[i].image, COPY_PUT);
            if (mousex() > buttons[i].left && 
            mousex() < buttons[i].left + buttons[i].width &&
            mousey() > buttons[i].top && 
            mousey() < buttons[i].top + buttons[i].height)
            {
               setfillstyle(SOLID_FILL, WHITE);
               bar (buttons[i].left, 
                     buttons[i].top + 0.96 * buttons[i].height, 
                     buttons[i].left + buttons[i].width, 
                     buttons[i].top + buttons[i].height);
            }
         }
         swapbuffers();
         
         if (kbhit())
         {
            int key = getch();
            if (key == KEY_ESC)
               return;
         }
         
         state = select_button(1);
      }
   }
   field_size = state - N_BUTTONS + 3;
}

// Отображение информации об игре
void about()
{
   putimage(0, 0, about_image, COPY_PUT);
   swapbuffers();
   getch();
}

// Закрытие игры, очистка памяти
void close()
{
   for (int i = 0; i < N_BUTTONS; i++)
      freeimage(buttons[i].image);
   
   for (int i = N_BUTTONS + 1; i < S_BUTTONS; i++)
      freeimage(buttons[i].image);
   
   freeimage(start_image);
   freeimage(background_image);
   freeimage(menu_image);
   freeimage(settings_image);
   freeimage(about_image);
}

// Функция создание кнопки в массиве Buttons
void create_button(int i, int left, int top, const char *file_name)
{
   buttons[i].image = loadBMP(file_name);
   buttons[i].left = left;
   buttons[i].top = top;
   buttons[i].width = imagewidth(buttons[i].image);
   buttons[i].height = imageheight(buttons[i].image);
}

// Выбор кнопки главного меню
int select_button(int choice)
{
   int start, end;
   if (choice == 0)
   {
      start = 0;
      end = N_BUTTONS;
   }
   else if (choice == 1)
   {
      start = N_BUTTONS + 1;
      end = S_BUTTONS;
   }
   
   int x, y;
   
   x = mousex();
   y = mousey();
   
   for (int i = start; i != end; i++)
   {
      if (x > buttons[i].left &&
          x < buttons[i].left + buttons[i].width &&
          y > buttons[i].top &&
          y < buttons[i].top + buttons[i].height)
      {
         return i;
      }
   }
   
   return NONE;
}