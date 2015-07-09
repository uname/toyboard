/* Snake Game for My toybaord
 * Version: 1.0Beta
 * Author:  uname(hqwemail#163.com , replace # with @)
 * Attention: We need a 3rd party lib: LedControl
*/

#include <Esplora.h>
#include <LedControl.h>
#include "snake.h"

LedControl lc = LedControl(DATA_IN, CLK, LOAD, 1);
Snake snake;
Food food;
unsigned long ticks;
byte numbers[][8] = {
    {0, 60, 102, 66, 66, 102, 60, 0},/*"0",0*/
    {0, 8, 120, 8, 8, 8, 8, 0},/*"1",1*/
    {0, 60, 66, 6, 56, 96, 126, 0},/*"2",2*/
    {0, 60, 66, 28, 2, 66, 60, 0},/*"3",3*/
    {0, 12, 52, 68, 254, 4, 4, 0},/*"4",4*/
    {0, 62, 32, 124, 66, 66, 60, 0},/*"5",5*/
    {0, 60, 66, 124, 66, 66, 60, 0},/*"6",6*/
    {0, 126, 4, 8, 16, 16, 32, 0},/*"7",7*/
    {0, 60, 66, 60, 66, 66, 60, 0},/*"8",8*/
    {0, 60, 66, 66, 62, 6, 124, 0},/*"9",9*/
};

void initLedMatrix()
{
    lc.shutdown(0, false);
    lc.setIntensity(0, 5);
    lc.clearDisplay(0);
}

void initSnake()
{
    snake.direction = LEFT;
    snake.length = DEFAULT_SNAKE_LENGTH;
    snake.speed = DEFAULT_SNAKE_SPEED;
    snake.life = DEFAULT_SNAKE_LIFE;
    for(int i = 0; i < snake.length ; i++) {
        snake.body[i].x = HEIGHT >> 1;
        snake.body[i].y =  i + (WIDTH >> 1);
        lc.setLed(0, snake.body[i].x, snake.body[i].y, true);
    }
}

void getRandomFoodPosition()
{
    int x, y, i;
    bool baspos;
    
    while(1) {
        x = random(0, WIDTH);
        y = random(0, HEIGHT);
        baspos = false;
        for(i = 0; i < snake.length; i++) {
            if(x == snake.body[i].x && y == snake.body[y].y) {
                baspos = true;
                break;
            }
        }
        if(!baspos) {
            break;
        }
    }
    
    food.position.x = x;
    food.position.y = y;
}

void initFood()
{
    food.type = NORMAL;
    food.visiable = true;
    food.flashInterval = FOOD_FLASH_SPEED;
    getRandomFoodPosition();
}

void initGame()
{
    ticks = 0;
    initLedMatrix();
    initSnake();
    initFood();
}

void updateSnakeDirection()
{
    unsigned int keyVal = (!Esplora.readButton(UP_KEY) << 3) 
        + (!Esplora.readButton(DOWN_KEY) << 2) 
        + (!Esplora.readButton(RIGHT_KEY) << 1)
        + !Esplora.readButton(LEFT_KEY);
    
    switch(keyVal) {
    case 8:
        if(snake.direction == LEFT || snake.direction == RIGHT) {
            snake.direction = UP;
        }
        break;
    
    case 4:
        if(snake.direction == LEFT || snake.direction == RIGHT) {
            snake.direction = DOWN;
        }
        break;
    
    case 2:
        if(snake.direction == UP || snake.direction == DOWN) {
            snake.direction = RIGHT;
        }
        break;
    
    case 1:
        if(snake.direction == UP || snake.direction == DOWN) {
            snake.direction = LEFT;
        }
        break;   
    }
}

// 判断小蛇是否撞墙或者撞到自己
bool hurt(int x, int y)
{
    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return true;
    }
    
    for(int i = 0; i < snake.length - 1; i++) {
        if(x == snake.body[i].x && y == snake.body[i].y) {
            return true;
        }
    }
    
    return false;
}

void updateSnake()
{   
    updateSnakeDirection();
    
    if(ticks % snake.speed != 0) {
        return;
    }
    
    int dx, dy;
    switch(snake.direction) {
    case UP:
        dx = -1;
        dy = 0;
        break;
        
    case DOWN:
        dx = 1;
        dy = 0;
        break;
        
    case LEFT:
        dx = 0;
        dy = -1;
        break;
        
    case RIGHT:
        dx = 0;
        dy = 1;
        break;
    
    default:
        // TODO REPORT ERROR
        break;
    }
    
    int newX = snake.body[0].x + dx;
    int newY = snake.body[0].y + dy;
    
    if(hurt(newX, newY)) {
        gameOver();
        return;
    }

    bool eat = false;
    if(newX == food.position.x && newY == food.position.y) {
        snake.length++;
        eat = true;
        Esplora.tone(SOUND, SOUND_DURATION);
    }
    else {
        lc.setLed(0, snake.body[snake.length - 1].x, snake.body[snake.length - 1].y, false);
    }
    
    for(int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0].x = newX;
    snake.body[0].y = newY;
    
    for(int i = 0; i < snake.length; i++) {
        lc.setLed(0, snake.body[i].x, snake.body[i].y, true);
    }
    
    if(eat) {
        getRandomFoodPosition();
    }
}

void gameOver()
{
    showScore();
    initGame();
    DEBUG_PRINT("GAME OVER");
}

//显示得分, 按右键开始新一局游戏
void showScore()
{
    int tensNum = snake.length / 10;
    int bitsNum = snake.length % 10;
    int count = 0;
    
    while(++count <= SHOW_SCORE_TIMES) {
        for(int r = 0; r < 8; r++) {
            for(int i = 0; i < 8; i++) {
                lc.setRow(0, i - r, numbers[tensNum][i]);
                lc.setRow(0, i - r + 8, numbers[bitsNum][i]);
            }
            delay(100);
        }
        delay(50);
    }
}

void update()
{
    if(food.flashInterval != 0) {
        if(ticks % food.flashInterval == 0) {
            lc.setLed(0, food.position.x, food.position.y, food.visiable);
            food.visiable = !food.visiable;
        }
    } else {
        food.visiable = true;
        lc.setLed(0, food.position.x, food.position.y, food.visiable);
    }
    
    updateSnake();
    
    ticks++;
}

void setup()
{
#ifdef DEBUG
    Serial.begin(9600);
#endif
    initGame();
    
    while(1)
    {
        update();
    }
}

void loop()
{
}


