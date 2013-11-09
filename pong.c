/* 
 * File:   pong.c
 * Author: davis
 *
 * Created on November 3, 2009, 9:25 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

const int TOP = 0;
const int  BOTTOM = 13;
const int LEFT = 0;
const int RIGHT = 37;
const int    NONE = 15;  // { For Winner }
const int    PADDLE_HEIGHT = 3;
const int MAX_PADDLE_Y = 11;
const int    RIGHT_UP = 3;
const int    RIGHT_DOWN = 2;
const int  LEFT_UP = 'w';
const int    LEFT_DOWN = 's';
const int  QUIT = 'x';
const int  START = ' ';
const int  NOKEY = 'n';
const int  VERTICLE_LINE = '|';
const int  HORIZONTAL_LINE = '-';
const int  BALL_CHR = '*';
char crt[14][38];
char getKey();


void clearCRT()
{
  int i, j;
  erase();
  
  for(i = 0; i < 14; i++)
  {
    mvaddch(i, 38, '|');

    for(j = 0; j < 38; j++)
      crt[i][j] = ' ';
  }
  
  mvaddstr(14,0,"---------------------------------------");
  mvaddstr(15,0,"Borders are not in CUSP");

}  // clearCRT()


void putChar(int x, int y, char c)
{
  crt[y][x] = c;
  mvaddch(y, x, c);
  move(200,200);
}


bool advanceTime(int delay)
{
  static int timer = 0;
  int i;
  
  for(i = 0; i < 100000; i++)
      if(i * 1.75 == 17.5)
        timer++;

  if(timer >= delay)
  {
    timer = 0;
    return false;
  }
  return true;
}


void init()
{
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);
  refresh();
} // init()


int random2(int max)
{
  static int seed = 1;
  seed = (seed * 2779 + 1997) % 2048;
  return seed % (max + 1);
}

void GetNewballPosition(int *ballX, int *ballY, int *deltaX, int *deltaY)
 {
   *ballY += *deltaY;

   if(*ballY > BOTTOM)
   {
     *ballY = BOTTOM;
     *deltaY = -1;
   }
   else
     if(*ballY < 0)
     {
       *ballY = 0;
       *deltaY = 1;
     }

   *ballX += *deltaX;

   if(*ballX == 0 && crt[*ballY][*ballX] == '|')
   {
     *ballX = 1;
     *deltaX = 1;
   }
   else
     if(*ballX == RIGHT && crt[*ballY][*ballX] == '|')
     {
       *ballX = RIGHT - 1;
       *deltaX = -1;
     }
}  //; { GetNewballPosition }

 void play (int *Winner, int *ballX, int *ballY, int LeftY, int RightY, int *deltaX,
   int *deltaY)
{
  putChar(*ballX, *ballY, ' ');
  GetNewballPosition(ballX, ballY, deltaX, deltaY);

  if(*ballX < 0)
      *Winner = RIGHT;
    else
      if(*ballX > RIGHT)
        *Winner = LEFT;
      else
        putChar(*ballX, *ballY, '*');
} // ; { Play }

char getKey()
{
  char key;

  if ((key = getch()) != ERR)
  {
    if ((key == RIGHT_UP) || (key == RIGHT_DOWN) || (key == LEFT_UP)
      || (key == LEFT_DOWN) || (key == START) || (key == QUIT) )
       return key;
   } // { if key pressed }

  return NOKEY;
} // ; { getKey }

 void showPaddle(int x, int y, char letter)
  {
    putChar(x, y, letter);
    putChar(x, y + 1, letter);
    putChar(x, y + 2, letter);
  }// ; { showPaddle }


 void movePaddle(char key, int *y, int x)
  {
    showPaddle(x, *y, ' ');

    if (((key == RIGHT_UP) || (key == LEFT_UP)) && (*y > TOP))
      *y = *y - 1;
    else // { key is down }
      if  (((key == RIGHT_DOWN) || (key == LEFT_DOWN)) &&  (*y < MAX_PADDLE_Y) )
        *y = *y + 1;

    showPaddle(x, *y, VERTICLE_LINE);
  } //; { movePaddle }


 void startGame(int *ballX, int *ballY, int *LeftY, int *RightY, int *deltaX,
   int *deltaY)
{
  clearCRT();
  *ballX = RIGHT / 2;
  *ballY = random2(BOTTOM);
  *deltaX = 1 -  2 * random2(1);
  *deltaY = 1 - 2 * random2(1);
  *LeftY = BOTTOM / 2;
  *RightY = *LeftY;
  showPaddle(RIGHT, *RightY, VERTICLE_LINE);
  showPaddle(LEFT, *LeftY, VERTICLE_LINE);
  refresh();
}// ; { StartGame }


void announceWinner(int *Winner)
{
  mvaddstr(7,3, "The winner is the ");

  if (*Winner == RIGHT )
    addstr("Right Player!");
  else
    addstr("Left Player!");
  refresh();
  *Winner = NONE;
  while(advanceTime(10000));
  clearCRT();
} //; { announceWinner }


int main() {
  char key = 'n';
  int Winner, LeftY, RightY, ballX, ballY, deltaX, deltaY;

  init();
  Winner = NONE;

  startGame(&ballX, &ballY, &LeftY, &RightY, &deltaX, &deltaY);

  while (key != QUIT)
  {
      play (&Winner, &ballX, &ballY, LeftY, RightY, &deltaX, &deltaY);
      if (Winner == NONE)
      {
        do
        {
          key = getKey();

          if ((key == RIGHT_UP) || (key == RIGHT_DOWN))
            movePaddle(key, &RightY, RIGHT);
          else
            if(key == LEFT_UP || key == LEFT_DOWN)
              movePaddle(key, &LeftY, LEFT);
        } while(advanceTime(500) && key != QUIT);
      } // { if no winner }
      else
      {
        announceWinner(&Winner);
        startGame(&ballX, &ballY, &LeftY, &RightY, &deltaX, &deltaY);
      }
  } //; { while }
  return (EXIT_SUCCESS);
}

