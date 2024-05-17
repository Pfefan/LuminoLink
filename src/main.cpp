#include <Arduino.h>

enum Effect {
  NONE,
  FADE
};

class RGBLedEffects {
  private:
    const int redPin = 4;
    const int greenPin = 3;
    const int bluePin = 2;
  
    int red_state = 255;
    int green_state = 255;
    int blue_state = 255;
    bool IsOn = false;
    Effect currentEffect = NONE;
  public:
    RGBLedEffects() {
      pinMode(redPin, OUTPUT);
      pinMode(greenPin, OUTPUT);
      pinMode(bluePin, OUTPUT);
    }
    void on() {
      if (IsOn) return;
      int temp_red = 0;
      int temp_green = 0;
      int temp_blue = 0;
      unsigned long startMillis = millis();
      unsigned long currentMillis;
      const int dimSpeed = 5;

      while (temp_red <= red_state || temp_green <= green_state || temp_blue <= blue_state) {
        currentMillis = millis();
        if (currentMillis - startMillis >= dimSpeed) {
          startMillis = currentMillis;
          temp_red = min(temp_red + 1, red_state);
          temp_green = min(temp_green + 1, green_state);
          temp_blue = min(temp_blue + 1, blue_state);
          analogWrite(redPin, temp_red);
          analogWrite(greenPin, temp_green);
          analogWrite(bluePin, temp_blue);
        }
        if (temp_red == red_state && temp_green == green_state && temp_blue == blue_state) {
          IsOn = true;
          break;
        }
      }
    }
    void off() {
      if (!IsOn) return;
      int temp_red = red_state;
      int temp_green = green_state;
      int temp_blue = blue_state;
      unsigned long startMillis = millis();
      unsigned long currentMillis;
      const int dimSpeed = 5;

      while (temp_red > 0 || temp_green > 0 || temp_blue > 0) {
        currentMillis = millis();
        if (currentMillis - startMillis >= dimSpeed) {
          startMillis = currentMillis;
          temp_red = max(temp_red - 1, 0);
          temp_green = max(temp_green - 1, 0);
          temp_blue = max(temp_blue - 1, 0);
          analogWrite(redPin, temp_red);
          analogWrite(greenPin, temp_green);
          analogWrite(bluePin, temp_blue);
        }
      }
      IsOn = false;
    }

    void setColor(int r, int g, int b) {
      if (IsOn) {
        red_state = r;
        green_state = g;
        blue_state = b;
        analogWrite(redPin, r);
        analogWrite(greenPin, g);
        analogWrite(bluePin, b);
      }
    }
    void fadeEffect() {
      static unsigned long lastUpdate = 0;
      static int i = 0;
      if (millis() - lastUpdate > 10) {
        lastUpdate = millis();
        if (i < 256) {
          setColor(255, i, 0);
        } else if (i < 512) {
          setColor(255 - (i - 256), 255, 0);
        } else if (i < 768) {
          setColor(0, 255, i - 512);
        } else if (i < 1024) {
          setColor(0, 255 - (i - 768), 255);
        } else if (i < 1280) {
          setColor(i - 1024, 0, 255);
        } else if (i < 1536) {
          setColor(255, 0, 255 - (i - 1280));
        }
        i = (i + 1) % 1536;
      }
    }
    void setCurrentEffect(Effect effect) {
      currentEffect = effect;
    }
    void runCurrentEffect() {
      if (IsOn) {
        switch (currentEffect) {
          case NONE:
            break;
          case FADE:
            fadeEffect();
            break;
        }
      }
    }
};

RGBLedEffects led;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');

    if (data == "on") {
      led.on();
    }
    if (data == "off") {
      led.off();
    }
    if (data.startsWith("setColor")) {
      data = data.substring(9);

      int commaIndex1 = data.indexOf(',');
      int commaIndex2 = data.lastIndexOf(',');
      int r = data.substring(0, commaIndex1).toInt();
      int g = data.substring(commaIndex1 + 1, commaIndex2).toInt();
      int b = data.substring(commaIndex2 + 1).toInt();

      led.setColor(r, g, b);
    }
    if (data.startsWith("setEffect")) {
      data = data.substring(10);
      if (data == "none") {
        led.setCurrentEffect(NONE);
      } else if (data == "fade") {
        led.setCurrentEffect(FADE);
      }
    }
  }
  led.runCurrentEffect();
}
