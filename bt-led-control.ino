#include <QList.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
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
int sections[16][6] = { // {firstPixel, pixelCount, status, R, G, B}
  {0, 403, 0, 0, 0, 0},
  {0, 43, 0, 0, 0, 0},
  {43, 41, 0, 0, 0, 0},
  {84, 41, 0, 0, 0, 0},
  {125, 35, 0, 0, 0, 0},
  {160, 86, 0, 0, 0, 0},
  {246, 35, 0, 0, 0, 0},
  {281, 41, 0, 0, 0, 0},
  {322, 41, 0, 0, 0, 0},
  {363, 40, 0, 0, 0, 0},
  {125, 156, 0, 0, 0, 0},
  {322, 165, 0, 0, 0, 0},
  {203, 78, 0, 0, 0, 0},
  {110, 93, 0, 0, 0, 0},
  {84, 197, 0, 0, 0, 0},
  {281, 247, 0, 0, 0, 0}
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
  return (random(sections[section][0], sections[section][0] + sections[section][1]) % LED_COUNT);
}

bool isActive(int i) {
  return strap.getPixelColor(i) != strap.Color(0, 0, 0);
}

void fill(uint32_t color, int first, int count) {
  for(int i=first; i<first+count; i++) {
    strap.setPixelColor(i % LED_COUNT, color);
  }
}

// ---------------------------------------
// |            READ EEPROM              |
// ---------------------------------------

void readState() {
  for(int i=0; i<2; i++) {
    byte state = EEPROM.read(i);
    if(state == 0) {
      continue;
    }
    for(int j=0; j<8; j++) {
      sections[i*8+j][2] = bitRead(state, 7-j);
    }
  }
}

void readColor() {
  for(int i=0; i<16; i++) {
    sections[i][3] = EEPROM.read(i*3+2) ? EEPROM.read(i*3+2) : defaultColor[0];
    sections[i][4] = EEPROM.read(i*3+3) ? EEPROM.read(i*3+3) : defaultColor[1];
    sections[i][5] = EEPROM.read(i*3+4) ? EEPROM.read(i*3+4) : defaultColor[2];
  }
}

void readEEPROM() {
  readState();
  readColor();
}

void saveState() {
  for(int i=0; i<2; i++) {
    byte state = 0;
    for(int j=0; j<8; j++) {
      bitWrite(state, 7-j, (bool) sections[i*8+j][2]);
    }
    byte value = EEPROM.read(i);
    if(value != state) {
      EEPROM.write(i, state);
    }
  }
}

void saveColor() {
  for(int i=0; i<16; i++) {
    for(int j=0; j<3; j++) {
      byte color = EEPROM.read(i*3+j+2);
      if(color != sections[i][3+j]) {
        EEPROM.write(i*3+j+2, sections[i][3+j]);
      }
    }
  }
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
  for(int i=sections[section][0]; i<sections[section][0]+sections[section][1]; i++) {
    if(isActive(i % LED_COUNT)) disintegrationStatus[1]++;
  }
  if(disintegrationStatus[1] == 1) {
    fill(strap.Color(0, 0, 0), sections[section][0], sections[section][1]);
    strap.show();
    sections[section][2] = 0;
    saveState();
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
    saveState();
  }
  for(int i=sections[section][0]; i<sections[section][0]+sections[section][1]; i++) {
    setHalloweenColor(i % LED_COUNT);
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
  readEEPROM();
  for(int i=0; i<sizeof(sections)/sizeof(sections[0]); i++) {
    if(sections[i][2] == 1) {
      fill(strap.Color(sections[i][3], sections[i][4], sections[i][5]), sections[i][0], sections[i][1]);
      if(i == 0) {
        break;
      }
    }
  }
  strap.show();
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
        fill(strap.Color(sections[section][3], sections[section][4], sections[section][5]), sections[section][0], sections[section][1]);
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
        fill(strap.Color(0, 0, 0), sections[section][0], sections[section][1]);
        sections[section][2] = 0;
      }
      saveState();
      strap.show();
    } else if(cmd == 2) {
      byte color[3];
      bluetooth.readBytes(color, 3);
      if(sections[section][2] != 0) {
        if(animation[0] == section) {
          animation[0] = -1;
        }
        fill(strap.Color(color[0], color[1], color[2]), sections[section][0], sections[section][1]);
        strap.show();
      }
      for(int i=0; i<3; i++) {
        sections[section][i+3] = color[i];
      }
      saveColor();
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