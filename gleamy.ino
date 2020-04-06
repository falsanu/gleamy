
#include <Encoder.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

#define PIN D4
#define N_LEDS 60

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

Encoder myEnc(D6, D7);
Encoder myColorEnc(D8, D5);




// Stripe Configuration
long oldPosition  = -999;
long oldColorPosition  = -999;
long newPosition = -999;
boolean lightSize = false;
long l = 1;
long len = 4;
long dir = 1;

// Button Configuration
//int button = D8; //D2(gpio4)
//int buttonState = 0;
int oldButtonState = 0;
int colorState = 0; // r
int red = 255;
int green = 0;
int blue = 0;

int addr = 0;

// Poti Configuration
const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0

int sensorValue = 0;  // value read from the pot
int outputValue = 0;  // value to output to a PWM pin



boolean isButtonPressed = false;
boolean isColorButtonPressed = false;
long lastUpdateMillis = 0;
long lastColorUpdateMillis = 0;

void handleKey() {
  isButtonPressed = true;
  lightSize = ! lightSize ;
  myEnc.write(newPosition);
  if (lightSize) {
    Serial.println("Light Sizing Enabled");
  } else {
    Serial.println("Light Sizing Disabled");
  }
}

void handleColorKey() {
  isColorButtonPressed = true;
  if (colorState < 2 ) {
    colorState ++;
  } else {
    colorState = 0;
  }
  Serial.print("ColorState: ");
  Serial.println(colorState);
}

void writeLightStripe() {
  strip.clear();

  for (int i = newPosition; i >= newPosition - (int)(len / 2); i--) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  for (int i = newPosition; i < newPosition + (int)(len / 2); i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.show();
}
//bool written = false;
void readEncoder() {

  long pos = myEnc.read();
  
  if (pos != oldPosition) {
    //written = false;
    if (lightSize) {
      dir = pos - oldPosition;

      // logging ColorLength
       char buf[10];
       sprintf(buf, "ColorLength: %d ", len);
       Serial.println(buf);

      if (dir > 0) {
        len = len + 2;
      } else {
        if (len > 4) {
          len = len - 2;
        }
      }
      //EEPROM.write(1,len);
      //EEPROM.commit();
      oldPosition = pos;

    } else {
      newPosition = pos;
    }
    oldPosition = pos;
    writeDisplay();
  }
  /*if(!written){
    EEPROM.write(1,len);
    EEPROM.commit();
    written=true;
  }*/

}

void readColorEncoder() {
  long p = myColorEnc.read();
  long dir = 0;
  long valueToChange = 0;
  if (p != oldColorPosition) {
    if (colorState == 0) {
      valueToChange = red;

    }
    if (colorState == 1) {
      valueToChange = green;

    }
    if (colorState == 2) {
      valueToChange = blue;

    }
    
    dir = p - oldColorPosition;


    if (dir > 0) {
      if (valueToChange <= 255) {
        valueToChange = valueToChange - 2;
      }
    } else {
      if (valueToChange >= 0) {
        valueToChange = valueToChange + 2;
      }
    }
    if(valueToChange < 0 ){
      valueToChange = 0;
    }
    if(valueToChange > 255) {
      valueToChange = 255;
    }

    if (colorState == 0) {
      red = valueToChange;

    }
    if (colorState == 1) {
      green = valueToChange;

    }
    if (colorState == 2) {
      blue = valueToChange;

    }
    Serial.print("valueToChange: ");
    Serial.println(valueToChange);
    writeDisplay();
    oldColorPosition = p;



  }
}


void writeDisplay() {
  // Clear the buffer.
  display.clearDisplay();
  display.display();

   // text display tests
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("GLEAMY");
  display.setTextSize(1);
  display.print("Length: ");
  display.print(len);

  display.print(" Pos: ");
  display.println(newPosition);
  
  
  display.print("R: ");
  display.print(red);
  display.print(" G: ");
  display.print(green);
  display.print(" B: ");
  display.print(blue);
  display.setCursor(0,0);
  display.display(); // actually display all of the above
}
void setup() {

  /*DISPLAY */
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();
  /*DISPLAY END */
  strip.begin();
  Serial.begin(115200);
//  EEPROM.begin(512);
//  len = EEPROM.read(1);
  Serial.println("Basic Encoder Test:");
  pinMode(D3, INPUT_PULLUP);
  pinMode(D10, INPUT_PULLUP);
//  myEnc.write(len);

  attachInterrupt(D3, handleKey, RISING);
  attachInterrupt(D10, handleColorKey, RISING);
   delay(1000);

  
}



void loop() {

  readEncoder();
  readColorEncoder();
  //readButton();
  //readPoti();
  writeLightStripe();
 
  if (isButtonPressed && millis() - lastUpdateMillis > 50) {
    isButtonPressed = false;
    lastUpdateMillis = millis();
  }
    if (isColorButtonPressed && millis() - lastColorUpdateMillis > 50) {
    isColorButtonPressed = false;
    lastColorUpdateMillis = millis();
  }

}
