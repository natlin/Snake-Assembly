/* 
 * File:   snake.cpp
 * Author: davis
 *
 * Created on May 9, 2012, 2:25 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>


const int TOP = 0;
const int BOTTOM = 13;
const int MAX_RIGHT = 37;
const int MIN_RIGHT = 1;
const int DOWN = 2;
const int UP = 3;
const int LEFT = 4;
const int RIGHT = 5;
const int MAX = 33;
const int  QUIT = 'x';
const int  START = '\n';
const int  NOKKEY = 'n';
const int  BODY = 'O';
const int NO_BODY = ' ';
const int FOOD = 'F';
const int Q_SIZE = 600;
const int PLAY_DELAY = 2500;
const int SCORE_DELAY= 50000;
const int COUNTDOWN_DELAY = 10000;

char crt[14][38];


bool advanceTime(int delay);
char getKey();
void init();
bool moveSnake(int *head, int *tail, int direction, int Q[], int *score);
void putChar(char c, int x, int y);
int random2(int max);
void showIntro();
void showScore(int score, int lives);
void startNewScreen();
void startSnake(int *head, int *tail, int *direction, int Q[]);
int speed;


bool advanceTime(int delay)
{
  static int timer = 0;
  int i;
  
  for(i = 0; i < speed; i++)
    if(i * 1.75 == 17.5)
      i = 11;
  
  timer++;

  if(timer >= delay)
  {
   timer = 0;
   return false;
  } // if reached the delay 
  
  return true;
} // advanceTime()


char getKey(int *direction)
{
  char key;

  if ((key = getch()) != ERR)
  {
    if ((key == RIGHT && *direction != LEFT) 
      || (key == LEFT && *direction != RIGHT) 
      || (key == UP && *direction != DOWN)
      || (key == DOWN && *direction != UP))
    {
       *direction = key;
       return key;
    }
    else if (key == QUIT || key == START)
       return key;
   } // { if key pressed }

  return NOKKEY;
} // getKey()


void init()  // not needed by CUSP
{
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);
  refresh();
} // init()


bool moveSnake(int *head, int *tail, int direction, int Q[], int *score)
{
  int newLoc = Q[*head], x, y;
  bool OKK, foodFlag;
  
  switch(direction)
  {
    case UP : newLoc -= 38; OKK = newLoc >= 0; break;
    case DOWN : newLoc += 38; OKK = newLoc <= 531; break;
    case RIGHT : newLoc += 1; OKK = newLoc % 38 != 0; break;
    case LEFT : newLoc -= 1; OKK = newLoc % 38 != 37; break;
  }  // switch
  
  if(OKK)
  {
    x = newLoc % 38;
    y = newLoc / 38;
    
    if(crt[y][x] == BODY)
      OKK = false;
    else  // OKK space to move into
    {
      foodFlag = crt[y][x] == FOOD;
      putChar(BODY, x, y);
      
      if(++(*head) >= Q_SIZE)
        *head = 0;
      
      Q[*head] = newLoc;
    
      if(foodFlag)
      {
        (*score)++;
        do
        {
          x = random2(37);
          y = random2(13);
        } while(crt[y][x] == BODY || crt[y][x] == FOOD);
        
        putChar(FOOD, x , y);
      }  // if ate a food
      else  // not food so keep snake the same size
      {
        x = Q[*tail] % 38;
        y = Q[*tail] / 38;
        putChar(' ', x, y);
        
        if(++(*tail) >= Q_SIZE)
          *tail = 0;
      } //else not a food
    } // else space OK
  } //if doesn't move off screen
    
  return OKK;
}  // moveSnake()


void putChar(char c, int x, int y)
{
  crt[y][x] = c;
  mvaddch(y + 1, x + 1, c);  // allow room for top and left border
  move(200,200);  // move cursor off screen
}  // putChar()


int random2(int max)
{
  static int seed = 1;
  seed = (seed * 2789 + 1997) % 2048;
  return seed % (max + 1);
}  // random2()


void showIntro()
{
  char key;
  int dummyDirection;
  
  mvaddstr(0, 5, "Snake Introduction");
  mvaddstr(2, 0, "This is the minimal version of");
  mvaddstr(3, 0, "snake.  The game starts when the");
  mvaddstr(4, 0, "enter key is pressed.  The game");
  mvaddstr(5, 0, "ends when either the user presses x,");
  mvaddstr(6, 0, "or their lives reaches zero.");
  mvaddstr(7, 0, "Instead of this introduction, students");
  mvaddstr(8, 0, "should place a description of their");
  mvaddstr(9, 0, "options here.");
  
  do
  {
    key = getKey(&dummyDirection);
  } while (key != START);
} // showIntro()


void showScore(int score, int lives, int *delay)
{
  int i;
  char str[20];
  sprintf(str, " Score: %04d  ", score);

  for(i = 0; str[i]; i++)
    putChar(str[i], i + 10, TOP);  // print score
  
  sprintf(str, " Lives: %d  ", lives);

  for(i = 0; str[i]; i++)
    putChar(str[i], i + 10, TOP + 1);  // print lives
  
  
  if(lives == 0)  
    mvaddstr(3,1, "           Game Over       ");
  
  refresh();
  
  *delay = SCORE_DELAY;
}  // showScore()




void startNewScreen() // just clear the screen in CUSP
{
  int x, y;
  erase();  // clear screen completely
  for(x = 0; x < 38; x++)
    for(y = 0; y < 14; y++)
      crt[y][x] = ' ';

  mvaddstr(0,0,"----------------------------------------");

  for(y = 1; y < 15; y++)
    mvaddstr(y, 0, "|                                      |");

  mvaddstr(15,0,"----------------------------------------");
  mvaddstr(16,0,"Borders are not in CUSP");
  refresh();
} // StartNewScreen()

void startSnake(int *head, int *tail, int *direction, int Q[], int *delay)
{
  int x, i, y, loc, x2, y2;
  putChar(' ', 17, 0);
  *direction = DOWN + random2(3);
  x = random2(28) + 5; // 5-33, so starts minimum of five from OKK
  y = random2(4) + 5;  // 5-9, so starts minimum of five from OKK
  loc = y * 38 + x;
  putChar(BODY, x, y);
  *head = 3;
  *tail = 0;
  Q[3] = loc;
  
  for(i = 2; i >= 0; i--)
  {
    switch(*direction)
    {
      case UP : Q[i] = Q[i + 1] + 38; break;
      case DOWN : Q[i] = Q[i + 1] - 38; break;
      case RIGHT : Q[i] = Q[i + 1] - 1; break;
      case LEFT : Q[i] = Q[i + 1] + 1; break;
    }  // switch on direction
    
    putChar(BODY, Q[i] % 38, Q[i] / 38);
  } // for each centipede body part
  
  for(i = 0; i < 5; i++)
  {
    do
    {
      x2 = random2(37);
    } while(abs(x2 - x) < 5);  // food not within 5 of head

    do
    {
      y2 = random2(12) + 1;
    } while(abs(y2 - y) < 5);  // food not within 5 of head

    if(crt[y2][x2] != 'F')
      putChar(FOOD, x2, y2);
    else
      i--;
  } // for i
  
  *delay = PLAY_DELAY;
}  // startSnake()

  
int main() {
  char key = 'n', countdown;
  int i, head, tail, score = 0, direction, lives = 3, seed, delay;
  int Q[Q_SIZE];
  
  printf("Speed for demo (1 = fast, 9 = slow): ");
  scanf("%d", &speed);
  speed *= 5000;
  printf("Seed: ");
  scanf("%d", &seed);
  srand(seed);
  init();  // not needed by CUSP
  showIntro();
  startNewScreen();  // just clear the screen in CUSP
  showScore(score, lives, &delay);

  while (key != QUIT)  // All but getKey() are the TimerISR.
  {
    switch(delay)
    {
      case SCORE_DELAY:  // start countdown
        if(lives == 0)
          return 0;  // DONE!
        
        startNewScreen();
        putChar('3', 17, 0);
        refresh();
        delay = COUNTDOWN_DELAY;
        break;
      case COUNTDOWN_DELAY:
        countdown = crt[0][17];
        
        if(countdown > '0')
          putChar(--countdown, 17, 0);
        else
          startSnake(&head, &tail, &direction, Q, &delay);
        
        break;
      case PLAY_DELAY:
        if(!moveSnake(&head, &tail, direction, Q, &score))
          showScore(score, --lives, &delay);
        
        break;
    }  // switch on delay
    while(advanceTime(delay));
    
    key = getKey(&direction);

    
  } //  while key != QUIT

  return 0;
} // main()

