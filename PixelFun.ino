#include <M5Stack.h>
#include <M5StackSAM.h>
#include "EEPROM.h"
#include "M5StackUpdater.h"
#include "Mover.cpp"
#include "utility/MPU9250.h"
// test.ino
#define EEPROM_SIZE 64
M5SAM MyMenu;

#define MAIN_MENU 0
#define POINT_COUNT_MENU 1
#define BRIGHTNESS_MENU 2
#define PIXEL_SIZE_MENU 3
#define PROGRAM_RUNNING 4



int menuID = PROGRAM_RUNNING;

int getAccel(char axis);
void changePointCountMenu();
void pixelSizeMenu();

void brightnessChangeMenu();

int pixelSize = 2;

int brightness = 100;

void mainMenu();
void showMainMenu();

void mainProgram();
void programRunningMenu();
int stepSpeed = 2;

bool mainProgramRunning = true;

MPU9250 IMU;

int screen_width = 320;
int screen_height = 240;

#define MAX_POINTS 1000

int points = 200;

Mover movers[MAX_POINTS](&M5.Lcd);



int locX = 320 / 2, locY = 240 / 2;
bool buttonBPressed = false;

void setup() {
  M5.begin();
  Wire.begin();
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  // MyMenu.addList("Configuration")
}

void loop() {
  M5.update();


if(M5.BtnA.isPressed()||M5.BtnB.isPressed()||M5.BtnC.isPressed()){
  switch (menuID) {
    case PROGRAM_RUNNING:
      programRunningMenu();
      break;
    case MAIN_MENU:
      mainMenu();
      break;
    case POINT_COUNT_MENU:
      changePointCountMenu();
      break;
    case PIXEL_SIZE_MENU:
      pixelSizeMenu();
      break;
    case BRIGHTNESS_MENU:
      brightnessChangeMenu();
      break;
  }
}
 
  if (mainProgramRunning) {
    mainProgram();
  }

  // showMainMenu();
}

int getAccel(char axis) {
  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {
    IMU.readAccelData(IMU.accelCount);
    IMU.getAres();
    switch (axis) {
      case 'X':
        IMU.ax = (float)IMU.accelCount[0] * IMU.aRes * 1000;
        return IMU.ax;
      case 'Y':
        IMU.ax = (float)IMU.accelCount[1] * IMU.aRes * 1000;
        return IMU.ax;
      case 'Z':
        IMU.az = (float)IMU.accelCount[2] * IMU.aRes * 1000;
        return IMU.az;
    }
  }
  return 0;
}

void showMainMenu() {
  // M5.update();
  mainProgramRunning = false;
  MyMenu.clearList();
  MyMenu.addList("Change Number of Points");
  MyMenu.addList("Change Pixel Size");
  MyMenu.addList("Change Brightness");

  MyMenu.drawAppMenu("Menu", "Back", "Select", "Next");
  // MyMenu.show();
  MyMenu.showList();

  menuID = MAIN_MENU;
}

void mainProgram() {
  if (buttonBPressed) {
    M5.Lcd.drawFastVLine(locX, 0, 240, M5.Lcd.color565(0, 0, 0));
    M5.Lcd.drawFastHLine(0, locY, 320, M5.Lcd.color565(0, 0, 0));
    int x = getAccel('X');
    int y = getAccel('Y');
    if (x > 0) {
      locX -= stepSpeed;
    } else if (x < 0) {
      locX += stepSpeed;
    }
    if (y > 0) {
      locY += stepSpeed;
    } else if (y < 0) {
      locY -= stepSpeed;
    }

    M5.Lcd.drawFastVLine(locX, 0, 240, M5.Lcd.color565(255, 255, 255));
    M5.Lcd.drawFastHLine(0, locY, 320, M5.Lcd.color565(255, 255, 255));
    // M5.Lcd.drawFastVLine(20,20,10, M5.Lcd.color565(255,255,255));
    // M5.Lcd.drawLine(locX,0,locX,240, M5.Lcd.color565(255,255,255));
    // M5.Lcd.drawLine(0,locY,320,locY, M5.Lcd.color565(255,255,255));
  }
  for (int i = 0; i < points; i = i + 1) {
    movers[i].clear(pixelSize);
    movers[i].update(locX, locY);
    movers[i].checkEdges();
    movers[i].display(pixelSize);
  }
}

void programRunningMenu() {
  {
    if (M5.BtnB.wasPressed()) {
      if (buttonBPressed) {
        buttonBPressed = false;
        M5.Lcd.drawFastVLine(locX, 0, 240, M5.Lcd.color565(0, 0, 0));
        M5.Lcd.drawFastHLine(0, locY, 320, M5.Lcd.color565(0, 0, 0));

      } else {
        buttonBPressed = true;
      }
    }
    if (M5.BtnC.wasPressed()) {
      locX = 320 / 2;
      locY = 240 / 2;
    }
    if (M5.BtnA.wasPressed()) {
      menuID = MAIN_MENU;
      M5.Lcd.fillScreen(0xf0000);
      showMainMenu();
    }
  }
}

void mainMenu() {
  {
    if (M5.BtnA.wasPressed()) {
      M5.Lcd.fillScreen(0xf0000);
      menuID = PROGRAM_RUNNING;
      mainProgramRunning = true;
    }
    if (M5.BtnB.wasPressed()) {
      int id = MyMenu.getListID();
      if (id == 0) {
        MyMenu.drawAppMenu("Change Number of Points", "-", "Back", "+");
        menuID = POINT_COUNT_MENU;
        MyMenu.windowClr();
        String myString = String(points);
        M5.Lcd.drawCentreString("Points: "+myString, M5.Lcd.width() / 2,
                                M5.Lcd.height() / 2, 4);
      }
       if (id == 1) {
        MyMenu.drawAppMenu("Change Pixel Size", "-", "Back", "+");
        menuID = PIXEL_SIZE_MENU;
        MyMenu.windowClr();
        String myString = String(pixelSize);
        M5.Lcd.drawCentreString("Pixel Size: "+myString, M5.Lcd.width() / 2,
                                M5.Lcd.height() / 2, 4);
      }
      if (id == 2) {
        MyMenu.drawAppMenu("Change Brightness", "-", "Back", "+");
        menuID = BRIGHTNESS_MENU;
        MyMenu.windowClr();
        M5.Lcd.progressBar( 119, 112,100,20,brightness);
      }

    }
    if (M5.BtnC.wasPressed()) {
      MyMenu.nextList();
    }
  }
}

void changePointCountMenu() {
  if (M5.BtnA.wasPressed()) {
    if (points > 0) {
      points--;
      MyMenu.windowClr();
      String myString = String(points);
      M5.Lcd.drawCentreString("Points: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
                              4);
    }
  }
  if (M5.BtnB.wasPressed()) {
    showMainMenu();
  }
  if (M5.BtnC.wasPressed()) {
    if (points < MAX_POINTS) {
      points++;
      MyMenu.windowClr();
      String myString = String(points);
      M5.Lcd.drawCentreString("Points: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
                              4);
    }
  }

  if (M5.BtnA.pressedFor(4000)) {
    if (points > 0) {
      points--;
      MyMenu.windowClr();
      String myString = String(points);
      M5.Lcd.drawCentreString("Points: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
                              4);
    }
    delay(3);
  }
  if (M5.BtnC.pressedFor(4000)) {
    if (points < MAX_POINTS) {
      points++;
      MyMenu.windowClr();
      String myString = String(points);
      M5.Lcd.drawCentreString("Points: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
                              4);
    }
    delay(3);
  }

  if (M5.BtnA.pressedFor(1000)) {
    if (points > 0) {
      points--;
      MyMenu.windowClr();
      String myString = String(points);
      M5.Lcd.drawCentreString("Points: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
                              4);
    }
    delay(50);
  }
  if (M5.BtnC.pressedFor(1000)) {
    if (points < MAX_POINTS) {
      points++;
      MyMenu.windowClr();
      String myString = String(points);
      M5.Lcd.drawCentreString("Points: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
                              4);
    }
    delay(50);
  }

  if (M5.BtnA.pressedFor(200)) {
    if (points > 0) {
      points--;
      MyMenu.windowClr();
      String myString = String(points);
      M5.Lcd.drawCentreString("Points: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
                              4);
    }
    delay(100);
  }
  if (M5.BtnC.pressedFor(200)) {
    if (points < MAX_POINTS) {
      points++;
      MyMenu.windowClr();
      String myString = String(points);
      M5.Lcd.drawCentreString("Points: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
                              4);
    }
    delay(100);
  }
}

void pixelSizeMenu(){
 if (M5.BtnA.wasPressed()) {
    if (pixelSize > 1) {
      pixelSize--;
      MyMenu.windowClr();
      String myString = String(pixelSize);
      M5.Lcd.drawCentreString("Pixel Size: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
                              4);
    }
  }
  if (M5.BtnB.wasPressed()) {
    showMainMenu();
  }
  if (M5.BtnC.wasPressed()) {
    if (pixelSize <10) {
      pixelSize++;
      MyMenu.windowClr();
      String myString = String(pixelSize);
      M5.Lcd.drawCentreString("Pixel Size: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
                              4);
    }
  }
}


void brightnessChangeMenu(){

  if (M5.BtnA.wasPressed()) {
    if (brightness > 10) {
      brightness--;
      M5.Lcd.setBrightness(brightness);
      M5.Lcd.fillRect(119,112,100,20,M5.Lcd.color565(128,128,128));
      M5.Lcd.progressBar( 119, 112,100,20,brightness);
      // String myString = String(brightness);
      // M5.Lcd.drawCentreString("Brightness: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
      //                         4);
    }
  }
  if (M5.BtnB.wasPressed()) {
    showMainMenu();
  }
  if (M5.BtnC.wasPressed()) {
    if (brightness <100) {
      brightness++;
      M5.Lcd.setBrightness(brightness);
      M5.Lcd.fillRect(119,112,100,20,M5.Lcd.color565(128,128,128));
       M5.Lcd.progressBar( 119, 112,100,20,brightness);
      // String myString = String(brightness);
      // M5.Lcd.drawCentreString("Brightness: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
      //                         4);
    }
  }
  if (M5.BtnA.pressedFor(1000)) {
     if (brightness > 10) {
      brightness--;
      M5.Lcd.setBrightness(brightness);
      M5.Lcd.fillRect(119,112,100,20,M5.Lcd.color565(128,128,128));
      M5.Lcd.progressBar( 119, 112,100,20,brightness);
      // String myString = String(brightness);
      // M5.Lcd.drawCentreString("Brightness: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
      //                         4);
    }
delay(50);
  }
 if (M5.BtnC.pressedFor(1000)) {
   if (brightness <100) {
      brightness++;
      M5.Lcd.setBrightness(brightness);
      // MyMenu.windowClr();
       M5.Lcd.progressBar( 119, 112,100,20,brightness);
      // String myString = String(brightness);
      // M5.Lcd.drawCentreString("Brightness: "+myString, M5.Lcd.width() / 2, M5.Lcd.height() / 2,
      //                         4);
    }
    delay(50);
  }
}
