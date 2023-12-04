//#define DMD2PANEL
#define SSD1306PANEL

#include "pong.h"

#ifdef DMD2PANEL
  SPIDMD dmd(PANELWIDE, PANELHIGH);  // DMD controls the entire display
#endif

#ifdef SSD1306PANEL
  Adafruit_SSD1306 display(larghezza, altezza, &Wire, OLED_RESET);
#endif

void setup() {
  #ifdef DMD2PANEL
    dmd.setBrightness(20);
    dmd.selectFont(SystemFont5x7);
    dmd.begin();
  #endif

  #ifdef SSD1306PANEL
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //initialize with the I2C addr 0x3C (128x64)
    display.clearDisplay();
    display.display();
  #endif

  menu.player = 2;
  menu.play = 1;
  menu.level = 5;
  menuElements[0] = &menu.player;
  menuElements[1] = &menu.level;
  menuElements[2] = &menu.play;
  creaPadelle();
  creaPalline();
}

void loop() {
  actualTime = millis();
  if (config) {
    printMenu();
    configGame();
  }
  else if (menu.play) {
    ballRefresh();
    padelRefresh();
    scoreRefresh();
  }
  #ifdef SSD1306PANEL
    display.display();
  #endif
}

void configGame() {
  int configIndex = 0;

  while (!menu.play) {
    for (int i = 0; i < 3; i++) {
      printString(larghezza - String(*menuElements[i]).length()*6, 2 + i * 10, String(*menuElements[i]));
      if (i == 1 && *menuElements[i] < 10) {
        printString(larghezza - 12, 2 + configIndex * 10, " ");
        display.display();
      }
    }
    printString(larghezza - 18, 2 + configIndex * 10, ">");
    display.display();
    *menuElements[configIndex] = map(analogRead(A0), 0, 1023, 0, menuElementsLimits[configIndex]);
    if (*menuElements[configIndex] == menuElementsOld[configIndex]) {
      actualTimeConfig = millis();
      if (actualTimeConfig - oldTimeConfig1 > 5000) {
        printString(larghezza - 24, 2 + configIndex * 10, " ");
        configIndex = (1 + configIndex) % 3;
        oldTimeConfig1 = actualTimeConfig;
        display.display();
      }
    } else {
      oldTimeConfig1 = millis();
    }

    for (int i = 0; i < 3; i++) {
      *menuElements[i] = (*menuElements[i] < 0) ? 0 : (*menuElements[i] > menuElementsLimits[i]) ? menuElementsLimits[i]
                                                                                                 : *menuElements[i];
    }
    menuElementsOld[configIndex] = *menuElements[configIndex];
    
  }
  config = 0;
  clearPanel(0);
  countDown(5);
  clearPanel(0);
}

void countDown(int c){
  while(c) {
    printString(40, 20, String(c--));
    delay(1000);
  }
}

void printMenu() {
  String strGiocatori = "n# Player: ";
  printString(6, 2, strGiocatori);
  String strDifficolta = "Level: ";
  printString(6, 12, strDifficolta);
  String strPressToStart = "Select to Start: ";
  printString(6, 22, strPressToStart);
  display.display();
}

void scoreRefresh() {
  String score1 = String(pad[0].score);
  String score2 = String(pad[1].score);
  printString((larghezza >> 1) - (1 + score1.length()) * 6, 2, score1);
  printString((larghezza >> 1) + 6, 2, score2);
  for (int i = 0; i < PADELLE; i++) {
    if (pad[i].score >= MAXSCORE) {
      menu.play = 0;
      gameOver(i);
    }
  }
}

void gameOver(int winner) {
  clearPanel(0);
  String str1 = "GAME OVER";
  String str2 = "Vince il giocatore";
  printString((larghezza >> 1) - 3 * str1.length(), 10, str1);
  printString((larghezza >> 1) - 3 * str2.length(), 30, str2);
  printString(64, 40, String(winner));
}


void padelRefresh() {
  if (actualTime - oldTime2 > 10) {
    for (int i = 0; i < PADELLE; i++) {
      drawPadella(pad[i], GRAPHICS_OFF);
      pad[i].y = map(analogRead(pad[i].ch), 0, 1023, 0, altezza - pad[i].dim);
      pad[i] = checkConfiniPadelle(pad[i]);
      drawPadella(pad[i], GRAPHICS_ON);
    }

    oldTime2 = actualTime;
  }
}

void ballRefresh() {
  if (actualTime - oldTime > DIFFICOLTA) {
    for (int j = 0; j < PALLINE; j++) {
      drawPallina(pal[j].x, pal[j].y, pal[j].dim, GRAPHICS_OFF);
      for (int i = 0; i < PADELLE; i++) {
        drawPadella(pad[i], GRAPHICS_ON);
        pal[j] = checkCollisionePadella(pad[i], pal[j]);
      }

      pal[j].x += pal[j].xDir;
      pal[j].y += pal[j].yDir;

      pal[j] = checkConfiniCampo(pal[j]);

      drawPallina(pal[j].x, pal[j].y, pal[j].dim, GRAPHICS_ON);
    }
    oldTime = actualTime;
  }
}

void creaPalline() {
  randomSeed(analogRead(0));
  int xDir;
  int yDir;
  for (int n = 0; n < PALLINE; n++) {
    do {
      xDir = random(1, 7) - 4;
    } while (xDir == 0 || xDir == 1 || xDir == -1);
    do {
      yDir = random(1, 7) - 4;
    } while (yDir == 0);
    pal[n].x = random(10, larghezza - 10);
    pal[n].y = random(10, altezza - 10);
    pal[n].xDir = xDir;
    pal[n].yDir = yDir;
    pal[n].dim = 2;
  }
}

void creaPadelle() {
  for (int n = 0; n < menu.player; n++) {
    pad[n].dim = 10;
    pad[n].x = n % 2 ? PANELWIDE * PIXELWIDE - PADELPAD - (n - 1) * 10 : 10 + n * 10;
    pad[n].y = 0;
    pad[n].xDir = 0;
    pad[n].yDir = 1;
    pad[n].ch = n;
    pad[n].score = 0;
  }
}

pallina checkCollisionePadella(padel p, pallina b) {
  if (
    (b.x >= p.x - b.dim && b.x <= p.x + b.dim) && (b.y >= p.y - b.dim && b.y <= p.y + b.dim + p.dim)) {
    b.xDir *= -1;
    b.x += b.xDir;
  }
  return b;
}

padel checkConfiniPadelle(padel p) {
  if (p.y < 1) p.y = 0;
  if (p.y > altezza - p.dim - 2) p.y = altezza - p.dim - 1;
  return p;
}

void drawPadella(padel p, DMDGraphicsMode mode) {
  for (int i = 0; i < p.dim; i++) {
    setPixel(p.x, p.y + i, mode);
  }
}

pallina checkConfiniCampo(pallina p) {
  if (p.x > larghezza - 2 - p.dim) {
    p.xDir *= -1;
    pad[0].score++;
  } else {
    if (p.x < 1 + p.dim) {
      p.xDir *= -1;
      pad[1].score++;
    }
  }
  if (p.y > altezza - 2 - p.dim || p.y < 1 + p.dim) {
    p.yDir *= -1;
  }
  return p;
}

#ifdef DMD2PANEL
void printString(int x, int y, String str) {
  dmd.drawString(x, y, str);
}

void clearPanel(int c) {
  dmd.fillScreen(c);
}

void drawPallina(int x, int y, int d, DMDGraphicsMode mode) {
  dmd.drawCircle(x, y, d, mode);
}

void setPixel(int x, int y, DMDGraphicsMode mode) {
  dmd.setPixel(x, y, mode);
}
#endif

#ifdef SSD1306PANEL
void printString(int x, int y, String str) {
  display.setTextSize(1);
  //display.fillRect(x, y, 18, 7, BLACK);
  display.setCursor(x, y);
  display.setTextColor(WHITE);
  display.print(str);
}

void clearPanel(int c) {
  display.clearDisplay();
}

void drawPallina(int x, int y, int d, int mode) {
  display.drawCircle(x, y, d, mode); 
}

void setPixel(int x, int y, int mode) {
  display.drawPixel(x, y, mode);
}
#endif
