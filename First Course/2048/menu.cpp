#include "menu.h"
#include "game.h"
#include "graphics.h"

Button buttons[S_BUTTONS];
IMAGE *start_image, *background_image;
IMAGE *menu_image, *settings_image, *about_image;
IMAGE *four, *five, *six, *seven;
int field_size = 4;
int n_level;

void load()
{
   start_image = loadBMP("images/start.bmp");
   background_image = loadBMP("images/background.bmp");
   menu_image = loadBMP("images/menu.bmp");
   about_image = loadBMP("images/about.bmp");
   settings_image = loadBMP("images/settings.bmp");
   
   create_button(GAME,  540, 250, "buttons/game.bmp");
   create_button(SETTINGS,  515, 350, "buttons/settings.bmp");
   create_button(ABOUT, 540, 450, "buttons/about.bmp");
   create_button(EXIT,  540, 550, "buttons/exit.bmp");
   create_button(FOUR, 320, 250, "buttons/4x4.bmp");
   create_button(FIVE, 760, 250, "buttons/5x5.bmp");
   create_button(SIX, 320, 500, "buttons/6x6.bmp");
   create_button(SEVEN, 760, 500, "buttons/7x7.bmp");
}

void start()
{
   putimage(0, 0, start_image, COPY_PUT);
   swapbuffers();
   getch();
}


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
            
            state = select_button();
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
         
         state = select_setting_button();
      }
   }
   field_size = state - N_BUTTONS + 3;
}

void about()
{
   putimage(0, 0, about_image, COPY_PUT);
   swapbuffers();
   getch();
   getch();
}

void close()
{
   freeimage(start_image);
   freeimage(background_image);
   freeimage(menu_image);
   freeimage(settings_image);
   freeimage(about_image);
   for (int i = 0; i < N_BUTTONS; i++)
   {
      freeimage(buttons[i].image);
   }
}

void create_button(int i, int left, int top, const char *file_name)
{
   buttons[i].image = loadBMP(file_name);
   buttons[i].left = left;
   buttons[i].top = top;
   buttons[i].width = imagewidth(buttons[i].image);
   buttons[i].height = imageheight(buttons[i].image);
}

int select_button()
{
   int x, y;
   
   x = mousex();
   y = mousey();
   
   for (int i = 0; i < N_BUTTONS; i++)
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

int select_setting_button()
{
   int x, y;
   
   x = mousex();
   y = mousey();
   
   for (int i = N_BUTTONS + 1; i < S_BUTTONS; i++)
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