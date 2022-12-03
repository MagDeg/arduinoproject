#include "LiquidCrystal_I2C.h"
#include "display.h"

Display::Display(int x, int y, int z) : LiquidCrystal_I2C(x, y, z) {
}

void Display::init() {
  LiquidCrystal_I2C::init();
  createChar(0, customCharBackslash);
  backlight();
};

void Display::print(int row, int col, const char* msg, bool clean = false) {
  if (clean == true) {
    clear();
  }
  setCursor(col, row);
  LiquidCrystal_I2C::print(msg);
  timeLastClean = millis();
}

void Display::printWait() {
  if ((millis() - lastTimeWaitIndicator) >= 500) {
    lastTimeWaitIndicator = millis();
    setCursor(15, 1);
    LiquidCrystal_I2C::print(strWait[curWaitIndicator++]);

    if (curWaitIndicator > (sizeof(strWait) - 1)) {
      curWaitIndicator = 0;
    }
  }
}

void Display::progressIndicator() {
  int progressPos = 3;


  for (int i = 0; i < 100; i++) {
    setCursor(0, 0);
    LiquidCrystal_I2C::print("Fortschritt: " + String(i) + "%");

    setCursor(2, 1);
    LiquidCrystal_I2C::print("[");
    setCursor(13, 1);
    LiquidCrystal_I2C::print("]");

    delay(25);

    if (((i % 10) == 0) && (i != 0)) {
      setCursor(progressPos, 1);
      LiquidCrystal_I2C::print("=");
      if (progressPos <= 12) {
        progressPos += 1;
      }
    }
  }
  clear();
  setCursor(0, 0);
  LiquidCrystal_I2C::print("Abgeschlossen!");
  delay(1000);
  clear();
}

void Display::displayClear() {
  if ((millis() - timeLastClean) >= DISPLAY_CLEAN) {
  	clear();
    timeLastClean = millis();
  }
}
