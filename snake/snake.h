#ifndef __SNAKE_H__
#define __SNAKE_H__

#define DEBUG

#define DATA_IN   16
#define CLK      15
#define LOAD     8

#define UP_KEY     SWITCH_1
#define RIGHT_KEY  SWITCH_2
#define DOWN_KEY   SWITCH_3
#define LEFT_KEY   SWITCH_4

#define WIDTH    8
#define HEIGHT   8

#define SHOW_SCORE_TIMES     3    // 得分滚动显示次数

#define DEFAULT_SNAKE_LIFE   1    // 小蛇默认有1条命
#define DEFAULT_SNAKE_SPEED  450  // 小蛇初始速度标量
#define DEFAULT_SNAKE_LENGTH 3    // 小蛇初始长度

#define FOOD_FLASH_SPEED     300 // 食物闪烁速度标量

#define SOUND           137
#define SOUND_DURATION  4

#ifdef DEBUG
#define DEBUG_PRINT(ARGV) Serial.println(ARGV)
#else
#define DEBUG_PRINT(ARGV)
#endif

enum FoodType
{
    NORMAL = 0,     // 无特殊效果，增加一个蛇身长度
    DOUBLE,         // 增加2个蛇身长度
    SLOWDOWN,       // 减缓30%小蛇移动速度
    ADD_LIFE,       // 增加一个生命值
};

enum Direction
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

typedef struct _Point
{
    int x;
    int y;
} Point;

typedef struct _Snake
{
    int length;                 // 蛇身长度
    int life;                   // 剩余生命值
    int speed;                  // 小蛇移动速度
    Direction direction;        // 移动方向
    Point body[WIDTH * HEIGHT]; // 蛇身坐标
} Snake;

typedef struct _Food
{
    FoodType type;       // 食物类型
    int flashInterval;   // 闪烁频度
    bool visiable;
    Point position;      // 食物位置
} Food;


#endif
