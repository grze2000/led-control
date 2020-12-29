#include <QList.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#define LED_COUNT 403
#define DEBUG false

// ---------------------------------------
// |          CLASS INSTANCES            |
// ---------------------------------------

SoftwareSerial bluetooth(4, 3);
Adafruit_NeoPixel strap = Adafruit_NeoPixel(LED_COUNT, A0, NEO_GRB + NEO_KHZ800);

// ---------------------------------------
// |             VARIABLES               |
// ---------------------------------------

int defaultColor[3] = {255, 78, 0};
int sections[16][6] = { // {firstPixel, lastPixel, status, R, G, B}
  {0, 402, 0, 0, 0, 0},
  {0, 42, 0, 0, 0, 0},
  {43, 83, 0, 0, 0, 0},
  {84, 124, 0, 0, 0, 0},
  {125, 159, 0, 0, 0, 0},
  {160, 245, 0, 0, 0, 0},
  {246, 280, 0, 0, 0, 0},
  {281, 321, 0, 0, 0, 0},
  {322, 362, 0, 0, 0, 0},
  {363, 402, 0, 0, 0, 0},
  {125, 280, 0, 0, 0, 0},
  {322, 83, 0, 0, 0, 0},
  {203, 280, 0, 0, 0, 0},
  {110, 202, 0, 0, 0, 0},
  {84, 280, 0, 0, 0, 0},
  {281, 124, 0, 0, 0, 0}
};

// ---------------------------------------
// |       VARIABLES - ANIMATIONS        |
// ---------------------------------------

int animation[2] = {-1, 0}; // {section, animationNr}
unsigned long animationTimer = 0;
QList<int> q;

// ---------------------------------------
// |     VARIABLES - HALLOWEEN MODE      |
// ---------------------------------------

int halloweenColors[4][3] = {
  {255, 30, 0},
  {255, 50, 0},
  {20, 4, 0},
  {0, 0, 0}
};

// ---------------------------------------
// |      VARIABLES - THANOS MODE        |
// ---------------------------------------

int disintegrationStatus[2] = {0, 0}; // {pixelsToDisintegration, pixelCount}

// ---------------------------------------
// |         SHARED FUNCTIONS            |
// ---------------------------------------

int randLedFromSection(int section) {
  if(sections[section][0] > sections[section][1]) {
    int rangeEnd = LED_COUNT + sections[section][1];
    int randomLed = random(sections[section][0], rangeEnd);
    return randomLed >= LED_COUNT ? randomLed-LED_COUNT : randomLed;
  } else {
    return random(sections[section][0], sections[section][1]+1);
  }
}

bool isActive(int i) {
  return strap.getPixelColor(i) != strap.Color(0, 0, 0);
}

// ---------------------------------------
// |          HALLOWEEN MODE             |
// ---------------------------------------

void setHalloweenColor(int led) {
  int color = random(0, 4);
  strap.setPixelColor(led, halloweenColors[color][0], halloweenColors[color][1], halloweenColors[color][2]);
}

// ---------------------------------------
// |            THANOS MODE              |
// ---------------------------------------

void thanosMode(int section) {
  if(sections[section][2] == 0) {
    return;
  }
  animation[0] = section;
  animation[1] = 2;
  disintegrationStatus[1] = 0;
  if(sections[section][0] > sections[section][1]) {
    for(int i=sections[section][0]; i<LED_COUNT; i++) {
      if(isActive(i)) disintegrationStatus[1]++;
    }
    for(int i=0; i<=sections[section][1]; i++) {
      if(isActive(i)) disintegrationStatus[1]++;
    }
  } else {
    for(int i=sections[section][0]; i<=sections[section][1]; i++) {
      if(isActive(i)) disintegrationStatus[1]++;
    }
  }
  if(disintegrationStatus[1] == 1) {
    //to change: clear section with fill method
    disintegrationStatus[0] = 1;
    return;
  }
  disintegrationStatus[0] = disintegrationStatus[1] / 2;
}

void thanosModeLoop() {
  if(animation[1] == 2 && animation[0] != -1) {
    if(disintegrationStatus[0] == 0) {
      // End of animation, cleanup
      animation[0] = -1;
      animation[1] = 0;
      disintegrationStatus[1] = 0;
      return;
    }
    if(millis() - animationTimer > 3000 / (disintegrationStatus[1] / 2)) {
      int led = randLedFromSection(animation[0]);
      while(!isActive(led)) {
        led = randLedFromSection(animation[0]);
      }
      strap.setPixelColor(led, 0, 0, 0);
      strap.show();
      disintegrationStatus[0]--;
      animationTimer = millis();
    }
  }
}

// ---------------------------------------
// |           HALLOWEEN MODE            |
// ---------------------------------------

void halloweenMode(int section) {
  animation[0] = section;
  animation[1] = 1;
  if(sections[section][2] == 0) {
    sections[section][2] = 1;
  }
  if(sections[section][0] > sections[section][1]) {
    for(int i=sections[section][0]; i<LED_COUNT; i++) {
      setHalloweenColor(i);
    }
    for(int i=0; i<=sections[section][1]; i++) {
      setHalloweenColor(i);
    }
  } else {
    for(int i=sections[section][0]; i<=sections[section][1]; i++) {
      setHalloweenColor(i);
    }
  }
  strap.show();
}

void halloweenModeLoop() {
  if(animation[1] == 1 && animation[0] != -1) {
    if(millis() - animationTimer > 150) {
      if(q.size() == 0) {
        for(int i=0; i<4; i++) {
          int led = randLedFromSection(animation[0]);
          q.push_front(led);
        }
      } else {
        int led = randLedFromSection(animation[0]);
        q.pop_back();
        q.push_front(led);
      }
      for(int i=0; i<4; i++) {
        int led = q.get(i);
        uint32_t ledColor = strap.getPixelColor(led);
        if(ledColor == strap.Color(0, 0, 0)) {
          int color = random(0, 3);
          strap.setPixelColor(led, halloweenColors[color][0], halloweenColors[color][1], halloweenColors[color][2]);
        } else {
          strap.setPixelColor(led, 0, 0, 0);
        }
      }
      strap.show();
      animationTimer = millis();
    }
  }
}

// ---------------------------------------
// |        SETUP AND MAIN LOOP          |
// ---------------------------------------

void setup() {
  randomSeed(analogRead(A1));
  bluetooth.begin(9600);
  if(DEBUG) {
    Serial.begin(9600);
    Serial.println("Serial started");
  }
  strap.begin();
  strap.show();
  for(int i=0; i<sizeof(sections)/sizeof(sections[0]); i++) {
    sections[i][3] = defaultColor[0];
    sections[i][4] = defaultColor[1];
    sections[i][5] = defaultColor[2];
  }
}

void loop() {
  if(bluetooth.available()) {
    int data = bluetooth.read();
    int cmd = data >> 4;
    int section = data & B00001111;
    if(cmd == 1) {
      if(DEBUG) {
        Serial.println("Power");
      }
      if(sections[section][2] == 0) {
        if(sections[section][1] > sections[section][0]) {
          strap.fill(strap.Color(sections[section][3], sections[section][4], sections[section][5]), sections[section][0], sections[section][1]-sections[section][0]+1);
        } else {
          strap.fill(strap.Color(sections[section][3], sections[section][4], sections[section][5]), sections[section][0], LED_COUNT-sections[section][0]);
          strap.fill(strap.Color(sections[section][3], sections[section][4], sections[section][5]), 0, sections[section][1]+1);
        }
        if(section == 0) {
          for(int i=0; i<sizeof(sections)/sizeof(sections[0]); i++) {
            sections[i][2] = 1;
          }
        } else {
          sections[section][2] = 1;
        }
      } else if(section == 0) {
        animation[0] = -1;
        strap.clear();
        for(int i=0; i<sizeof(sections)/sizeof(sections[0]); i++) {
            sections[i][2] = 0;
          }
      } else {
        if(animation[0] == section) {
          animation[0] = -1;
        }
        if(sections[section][1] > sections[section][0]) {
          strap.fill(strap.Color(0, 0, 0), sections[section][0], sections[section][1]-sections[section][0]+1);
        } else {
          strap.fill(strap.Color(0, 0, 0), sections[section][0], LED_COUNT-sections[section][0]);
          strap.fill(strap.Color(0, 0, 0), 0, sections[section][1]+1);
        }
        sections[section][2] = 0;
      }
      strap.show();
    } else if(cmd == 2) {
      byte color[3];
      bluetooth.readBytes(color, 3);
      if(sections[section][2] != 0) {
        if(animation[0] == section) {
          animation[0] = -1;
        }
        if(sections[section][1] > sections[section][0]) {
          strap.fill(strap.Color(color[0], color[1], color[2]), sections[section][0], sections[section][1]-sections[section][0]+1);
        } else {
          strap.fill(strap.Color(color[0], color[1], color[2]), sections[section][0], LED_COUNT-sections[section][0]);
          strap.fill(strap.Color(color[0], color[1], color[2]), 0, sections[section][1]+1);
        }
        strap.show();
      }
      for(int i=0; i<3; i++) {
        sections[section][i+3] = color[i];
      }
    } else if(cmd == 3 && (section == 0 || section > 9)) {
      halloweenMode(section);
    } else if(cmd == 4) {
      thanosMode(section);
    }
    //Serial.write(data);
  }
  if(Serial.available() && DEBUG) {
    bluetooth.write(Serial.read());
  }

  halloweenModeLoop();
  thanosModeLoop();
}