// Arduino Flappy Bird
// Modified by hqwemail@163.com
// 开发板购买链接: http://item.taobao.com/item.htm?spm=686.1000925.0.0.YKvcne&id=520461611747
// Thanks to augustzf@gmail.com
// Dependencies: 
// 3. LedControl Library (http://playground.arduino.cc/Main/LedControl)
// 4. Timer Library (http://playground.arduino.cc/Code/Timer)

#include <Esplora.h>
#include "LedControl.h"
#include "Timer.h"
#include "types.h"

#define JUMP_KEY  SWITCH_1
#define DIN 16
#define CLK 15
#define CS 8
#define CHIPS 1

LedControl gMatrix = LedControl(DIN, CLK, CS, CHIPS);

// gravity
const float kG = 0.005;

// button gives this much lift
const float kLift = -0.05;

Game gGame;

// this bird is stuck on col 1 & 2
const byte kXpos = 1;

// the global timer
Timer gTimer;

// timer events 
int gUpdateEvent;
int gMoveWallOneEvent;
int gMoveWallTwoEvent;

byte gNumbers[][8] = {
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
    {0,230,246,214,206,206,198,0},/*"N",10*/
    {0,254,194,252,194,194,252,0},/*"B",11*/
};
byte gBirdLogo[] = {24,36,44,100,158,169,78,56};
byte gMaxScore[] = {24,60,126,255,126,24,24,60};

void setup() 
{
    gMatrix.shutdown(0, false);
    gMatrix.setIntensity(0, 3);
    gMatrix.clearDisplay(0);

    randomSeed(analogRead(0));
    
    gGame.state = STOPPED;
    gTimer.every(30, reactToUserInput);
    
    startGame(true);
}

void startGame(boolean start)
{
    if (start) {
        gGame.score = 0;
        gGame.state = STARTED;
        gGame.birdY = 0.5;
        gGame.wallOne.xpos = 7;
        gGame.wallOne.bricks = generateWall();
        gGame.wallTwo.xpos = 7;
        gGame.wallTwo.bricks = generateWall();

        gUpdateEvent = gTimer.every(50, updateBirdPosition);
        gTimer.after(2500, startWallOne);
        gTimer.after(3300, startWallTwo);
    } else {
        gGame.state = STOPPED;
        gTimer.stop(gUpdateEvent);
        gTimer.stop(gMoveWallOneEvent);
        gTimer.stop(gMoveWallTwoEvent);
    }
}  

void startWallOne()
{
    gMoveWallOneEvent = gTimer.every(200, moveWallOne);
}

void startWallTwo()
{
    gMoveWallTwoEvent = gTimer.every(200, moveWallTwo);
}

void moveWallOne()
{
    moveWall(&gGame.wallOne);
}

void moveWallTwo()
{
    moveWall(&gGame.wallTwo);
}

void moveWall(Wall *wall)
{
    if (wall->xpos == 255) {
        // wall has come past screen
        eraseWall(wall, 0);
        wall->bricks = generateWall();
        wall->xpos = 7;
    } else if (wall->xpos < 7) {
        eraseWall(wall, wall->xpos + 1);    
    }

    drawWall(wall, wall->xpos);

    // check if the wall just slammed into the bird.
    if (wall->xpos == 2) {
        byte ypos = 7 * gGame.birdY;  
        if (wall->bricks & (0x80 >> ypos)) {
            gameOver();
        }
        else {
            gGame.score++;
            //Esplora.tone( 1976, 35);
        }
    }
    wall->xpos = wall->xpos - 1;
}

void drawWall(Wall *wall, byte x) 
{
    for (byte row = 0; row < 8; row++) {
        if (wall->bricks & (0x80 >> row)) {
            gMatrix.setLed(0, row, x, HIGH);
        }
    }
}

void eraseWall(Wall *wall, byte x) 
{
    for (byte row = 0; row < 8; row++) {
        if (wall->bricks & (0x80 >> row)) {
            gMatrix.setLed(0, row, x, LOW);
        }
    }
}

byte generateWall()
{
    // size of the hole in the wall
    byte gap = random(4, 7);

    // the hole expressed as bits
    byte punch = (1 << gap) - 1;

    // the hole's offset
    byte slide = random(1, 8 - gap);

    // the wall without the hole
    return 0xff & ~(punch << slide);
}

void reactToUserInput()
{
    static boolean old = false;
    //static unsigned long lastMillis = 0;

    boolean buttonPressed = Esplora.readButton(JUMP_KEY) == 0; //digitalRead(BUTTON_PIN);

    if (buttonPressed) {
        if (gGame.state == STARTED) {
            //unsigned long dt = millis() - lastMillis;
            if (!old) {
                // button was not pressed last time we checked
                if (gGame.vy > 0) {
                // initial bounce
                    gGame.vy = kLift;  
                }
                else {
                // keep adding lift
                    gGame.vy += kLift;   
                }        
            }      
        } else {
            // game is not playing. start it.
            transition();
            startGame(true); 
        }
    }
    old = buttonPressed;
}

void updateBirdPosition()
{
    // initial position (simulated screen size 0..1)
    static float y = 0.5;

    // apply gravity
    gGame.vy = gGame.vy + kG;

    float oldY = gGame.birdY;

    // calculate new y position
    gGame.birdY = gGame.birdY + gGame.vy;

    // peg y to top or bottom
    if (gGame.birdY > 1) {
        gGame.birdY = 1;
        gGame.vy = 0;
    } else if (gGame.birdY < 0) {
        gGame.birdY = 0;
        gGame.vy = 0;
    }

    // convert to screen position
    byte ypos = 7 * gGame.birdY;  

    Direction direction;
    if (abs(oldY - gGame.birdY) < 0.01) {
        direction = STRAIGHT;
    } else if (oldY < gGame.birdY) {
        direction = UP;
    } else {
        direction = DOWN;
    }

    drawBird(direction, ypos);
}

void drawBird(Direction direction, byte yHead)
{
    // previous position of tail and head (one pixel each)
    static byte cTail, cHead;
    byte yTail;
    yTail = constrain(yHead - direction, 0, 7);

    // erase it from old position
    gMatrix.setLed(0, cTail, kXpos, LOW);
    gMatrix.setLed(0, cHead, kXpos + 1, LOW);

    // draw it in new position
    gMatrix.setLed(0, yTail, kXpos, HIGH);
    gMatrix.setLed(0, yHead, kXpos + 1, HIGH);

    // remember current position 
    cTail = yTail;
    cHead = yHead;
}

void gameOver()
{
  showScore(gGame.score);
  startGame(false);
}

void showScore(byte value)
{  
    int index0;
    int index1;
    
    if(value > 99) {
        index0 = 10;
        index1 = 11;
    } else {
        index0 = value / 10;
        index1 = value % 10;
    }
    int count = 0;
    
    while(count++ < 3) {
        for(int r = 0; r < 8; r++) {
            for(int i = 0; i < 8; i++) {
                gMatrix.setRow(0, i - r, gNumbers[index0][i]);
                gMatrix.setRow(0, i - r + 8, gNumbers[index1][i]);
            }
            delay(100);
        }
        delay(50);
    }
    
    drawBirdLogo();
}

void drawBirdLogo()
{
    for(int i = 0; i < 8; i++) {
        gMatrix.setRow(0, i, gBirdLogo[i]);
    }
}

void loop() 
{
    gTimer.update();
}

void transition()
{
    for (int step = 0; step < 8; step++) {
        for (int row = 0; row < 8; row++) {
            byte pixels = gGame.framebuffer[row];
            if (row % 2) {
                pixels = pixels >> 1;        
            } else {
                pixels = pixels << 1;      
            }
            updateFrameRow(row, pixels);
        }
        delay(50);
    }  
}

void updateFrameRow(byte row, byte pixels) 
{
    gMatrix.setRow(0, row, pixels);
    gGame.framebuffer[row] = pixels;
}



