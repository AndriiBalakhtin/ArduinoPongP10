#ifndef PONG_H
#define PONG_H

#ifdef DMD2PANEL
    #include <SPI.h>
    #include <DMD2.h>
    #include <fonts/SystemFont5x7.h>
#endif

#ifdef SSD1306PANEL
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>

  #define OLED_RESET 4
  #define GRAPHICS_ON WHITE
  #define GRAPHICS_OFF BLACK
  #define DMDGraphicsMode int
#endif

#define PANELWIDE 4
#define PANELHIGH 4
#define PIXELWIDE 32
#define PIXELHIGH 16
#define PADELPAD 10
#define PADELLE 2
#define PALLINE 1
#define MAXSCORE 41
#define DIFFICOLTA 20

typedef struct {
  int x, y;
  int xDir, yDir;
  int dim;
  int ch;
  int score;
} padel;

typedef struct {
  int x, y;
  int xDir, yDir;
  int dim;
} pallina;

struct menuConfig {
  int player;
  int level;
  int play;
};


void creaPadelle(void);
void configGame(void);
void creaPalline(void);
void countDown(int);
void printMenu(void);
void scoreRefresh(void);
void gameOver(int);
void padelRefresh(void);
void ballRefresh(void);
void creaPalline(void);
void creaPadelle(void);
pallina checkCollisionePadella(padel, pallina);
padel checkConfiniPadelle(padel);
pallina checkConfiniCampo(pallina);
void printString(int, int, String);
void clearPannel(int);

void drawPallina(int, int, int, DMDGraphicsMode);
void setPixel(int, int, DMDGraphicsMode);
void drawPadella(padel, DMDGraphicsMode);


struct menuConfig menu;
int *menuElements[3];
int menuElementsOld[3];
int menuElementsLimits[3] = { 4, 10, 1 };

padel pad[PADELLE];
pallina pal[PALLINE];
int larghezza = PANELWIDE * PIXELWIDE;
int altezza = PANELHIGH * PIXELHIGH;
int config = 1;
int padelleConfig = 2;
unsigned long int actualTime;
unsigned long int actualTimeConfig;
unsigned long int oldTime = 0;
unsigned long int oldTime2 = 0;
unsigned long int oldTimeConfig1 = 0;

#endif