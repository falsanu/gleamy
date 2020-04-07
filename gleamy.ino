
#include <Encoder.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#include <ESP8266WebServer.h>
ESP8266WebServer server(80); //creating the server at port 80

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

#define PIN D4
#define N_LEDS 60

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

Encoder myEnc(D6, D7);
Encoder myColorEnc(D8, D5);

#define DISPLAY_DELAY 200

// Stripe Configuration
long oldPosition  = -999;
long newPosition = -999;
long l = 1;
long len = 4;
long dir = 1;

// Button Configuration
int oldButtonState = 0;
int red = 255;
int green = 0;
int blue = 0;

int addr = 0;

int brightness = 255;


boolean isButtonPressed = false;
long lastUpdateMillis = 0;



int buttonState = 0;


/** WIFI Setup **/

/*Put your SSID & Password*/
const char* ssid = "XXX";    // Enter SSID here
const char* password = "XXX";  // Enter Password here
void wifiSetup() {
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}



String sendHTML() {


  String ptr = "";
  ptr +="<!DOCTYPE HTML>";
ptr +="<html>";
ptr +="<head>";
ptr +="  <title>Gleamy</title>";
ptr +="<meta name=\"apple-mobile-web-app-title\" content=\"Gleamy\">";
ptr +="<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">";
ptr +="  <style rel=\"text/stylesheet\" media=\"screen\">";
ptr +="    body{background-color:#000;color:#fff;font-family:Arial;font-size:12px}.container{display:flex;margin:auto;text-alignt:center;flex-wrap:wrap}.slider{width:100%;display:flex;flex-wrap:nowrap;margin:1em}input[type=range]{-webkit-appearance:none!important;width:100%;height:15px;background-color:#a3cd99;border:1px solid #97c68b;border-radius:10px;margin:auto;transition:all .3s ease}input[type=range]:hover{background-color:#b2d5aa}input[type=range]::-webkit-slider-thumb{-webkit-appearance:none!important;width:20px;height:20px;background-color:#579e81;border-radius:30px;box-shadow:0 0 3px #3c6d59;transition:all .5s ease}input[type=range]::-webkit-slider-thumb:hover{background-color:#457d66}input[type=range]::-webkit-slider-thumb:active{box-shadow:0 0 1px #3c6d59}output{text-align:center;font-family:Arial,sans-serif;display:block;margin:auto;width:3em;color:#579e81}label{width:100px;font-size:1em;margin:auto}input{flex-grow:1}h1{width:100%;text-align:center}";
ptr +="    body{margin: 0 1em; min-height: 100%; display: grid; grid-template-rows: 1fr auto;}";
ptr +="footer {grid-row-start: 2;grid-row-end: 3; text-align:center} a{color:white}";
ptr +="  </style>";
ptr +="  <script>";
ptr +="    function updateTextInput(val, field) {";
ptr +="      fetch(`http://";
ptr+=WiFi.localIP().toString();
ptr+="?submit=true&${field}=${val}`).then((response) => {";
ptr +="        return response;";
ptr +="      });";
ptr +="      document.getElementById(field + 'value').value = val;";
ptr +="    }";
ptr +="  </script>";
ptr +="  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
ptr +="</head>";
ptr +="<body>";
ptr +="  <h1>Gleamy</h1>";
ptr +="  <form action=\"/\" method=\"POST\">";
ptr +="    <div class=\"container\">";
ptr +="      <div class=\"slider\">";
ptr +="        <label for=\"pos\">Position</label>";
ptr +="        <input type=\"range\" id=\"pos\" name=\"pos\" min=\"0\" max=\"64\" onchange=\"updateTextInput(this.value, 'pos');\" value=\"";
ptr +=newPosition;
ptr +="\">";
ptr +="        <output id=\"posvalue\">";
ptr +=newPosition;
ptr +="</output>";
ptr +="      </div>";
ptr +="      <div class=\"slider\">";
ptr +="        <label for=\"len\">Length </label>";
ptr +="        <input type=\"range\" id=\"len\" name=\"len\" min=\"0\" max=\"64\" onchange=\"updateTextInput(this.value, 'len');\" value=\"";
ptr +=len;
ptr +="\">";

ptr +="        <output id=\"lenvalue\">";
ptr +=len;
ptr +="</output>";
ptr +="      </div>";
ptr +="      <div class=\"slider\">";
ptr +="        <label for=\"brightness\">Brightness</label>";
ptr +="        <input type=\"range\" id=\"brightness\" name=\"brightness\" min=\"0\" max=\"255\"onchange=\"updateTextInput(this.value, 'brightness');\" value=\"";
ptr +=brightness;
ptr +="\">";

ptr +="        <output id=\"brightnessvalue\">";
ptr +=brightness;
ptr +="</output>";
ptr +="      </div>";
ptr +="      <div class=\"slider\">";
ptr +="        <label for=\"red\">Red</label>";
ptr +="        <input type=\"range\" id=\"red\" name=\"red\" min=\"0\" max=\"255\" onchange=\"updateTextInput(this.value, 'red');\" value=\"";
ptr +=red;
ptr +="\">";
ptr +="        <output id=\"redvalue\">";
ptr +=red;
ptr +="</output>";
ptr +="      </div>";
ptr +="      <div class=\"slider\">";
ptr +="        <label for=\"green\">Green</label>";
ptr +="        <input type=\"range\" id=\"green\" name=\"green\" min=\"0\" max=\"255\" onchange=\"updateTextInput(this.value, 'green');\" value=\"";
ptr +=green;
ptr +="\">";

ptr +="        <output id=\"greenvalue\">";
ptr +=green;
ptr +="</output>";
ptr +="      </div>";
ptr +="      <div class=\"slider\">";
ptr +="        <label for=\"blue\">Blue</label>";
ptr +="        <input type=\"range\" id=\"blue\" name=\"blue\" min=\"0\" max=\"255\" onchange=\"updateTextInput(this.value, 'blue');\" value=\"";
ptr +=blue;
ptr +="\">";
ptr +="        <output id=\"bluevalue\">";
ptr +=blue;
ptr +="</output>";
ptr +="      </div>";
ptr +="    </div>";
ptr +="  </form>";
ptr +="<footer>made with heart <a href=\"https://janfanslau.com\" target=\"_blank\">@falsanu</a>";
ptr +="</body>";
ptr +="</html>";







  

  return ptr;
}

void handleRoot() {
  Serial.println("Request arrived");

  for (int i = 0; i < server.args(); i++) {
    Serial.print(String(i) + " ");  //print id
    Serial.print("\"" + String(server.argName(i)) + "\" ");  //print name
    Serial.println("\"" + String(server.arg(i)) + "\"");  //print value
  }

  if (server.hasArg("submit")) {
    if (server.hasArg("red")) {
      red =  server.arg("red").toInt();
    }
    if (server.hasArg("green")) {
      green =  server.arg("green").toInt();
    }
    if (server.hasArg("blue")) {
      blue =  server.arg("blue").toInt();
    }
    if (server.hasArg("len")) {
      len =  server.arg("len").toInt();
    }
    if (server.hasArg("brightness")) {
      brightness = server.arg("brightness").toInt();
    }
    if (server.hasArg("pos")) {
      newPosition =  server.arg("pos").toInt();
    }
    saveState();


  }

  server.send(200, "text/html", sendHTML()); //Send web page

}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void httpServerSetup() {
  server.on("/", HTTP_GET, handleRoot);              // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/", HTTP_POST, handleRoot);              // Call the 'handleRoot' function when a client requests URI "/"
  server.onNotFound(handleNotFound);
  server.begin();

}






/*
   handleButtonKey Press
   Iterate through differentStates
*/
void handleButtonKey() {
  isButtonPressed = true;
  if (buttonState < 5) {
    buttonState ++;
  } else {
    buttonState = 0;
  }
  displayState();
  saveState();
}


void displayState() {
  String text = "";
  switch (buttonState) {
    case 0: text = "Position"; break;
    case 1: text = "Length"; break;
    case 2: text = "Brightness"; break;
    case 3: text = "Red"; break;
    case 4: text = "Green"; break;
    case 5: text = "Blue"; break;
  }
  display.clearDisplay();
  display.display();

  // text display tests
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(text);
  display.display();
  delay(DISPLAY_DELAY);
}

/*
   Save the state within EPROM
   Save the state in Rotary
*/
void saveState() {
  myEnc.write(newPosition);

  // Store Data in EPROM

  EEPROM.write(1, newPosition);
  EEPROM.write(2, len);
  EEPROM.write(3, brightness);
  EEPROM.write(4, red);
  EEPROM.write(5, green);
  EEPROM.write(6, blue);

  EEPROM.commit();

  Serial.println("WRITE:");
  Serial.print("pos: ");
  Serial.print(newPosition);
  Serial.print("len: ");
  Serial.print(len);
  Serial.print("bri: ");
  Serial.print(brightness);
  Serial.print("r: ");
  Serial.print(red);
  Serial.print("g: ");
  Serial.print(green);
  Serial.print("b: ");
  Serial.println(blue);
}


void writeLightStripe() {
  strip.clear();

  for (int i = newPosition; i >= newPosition - (int)(len / 2); i--) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  for (int i = newPosition; i < newPosition + (int)(len / 2); i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.setBrightness(brightness);
  strip.show();
}
//bool written = false;
void readEncoder() {

  long pos = myEnc.read();
  long dir = 0;
  long valueToChange = 0;

  if (pos != oldPosition) {
    dir = pos - oldPosition;
    switch (buttonState) {
      case 0: //pos
        newPosition = pos;
        break;
      case 1: //len
        if (dir > 0) {
          len = len + 2;
        } else {
          if (len > 4) {
            len = len - 2;
          }
        }
        oldPosition = pos;
        break;
      case 2:  //bright

        if (dir < 0) {
          if (brightness <= 255) {
            brightness = brightness + 5;
          }
        } else {
          if (brightness >= 0) {
            brightness = brightness - 5;
          }
        }
        if (brightness > 255) {
          brightness = 255;
        }
        if (brightness < 0) {
          brightness = 0;
        }

        break;
      default:
        // read Value to Change
        switch (buttonState) {
          case 3: // red
            valueToChange = red;
            break;
          case 4: //green
            valueToChange = green;
            break;
          case 5: //blue
            valueToChange = blue;
            break;
        }
        // set Value to Change
        if (dir > 0) {
          if (valueToChange <= 255) {
            valueToChange = valueToChange - 10;
          }
        } else {
          if (valueToChange >= 0) {
            valueToChange = valueToChange + 10;
          }
        }
        if (valueToChange < 0 ) {
          valueToChange = 0;
        }
        if (valueToChange > 255) {
          valueToChange = 255;
        }
        // write Value to Change
        switch (buttonState) {
          case 3: // red
            red = valueToChange;
            break;
          case 4: //green
            green = valueToChange;
            break;
          case 5: //blue
            blue = valueToChange;
            break;
        }

        break;

    }
    oldPosition = pos;
    writeDisplay();
  }
}




void writeDisplay() {
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  // text display tests
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("GLEAMY");
  display.setTextSize(1);
  display.print("Len:");
  display.print(len);

  display.print(" Pos:");
  display.print(newPosition);

  display.print(" Bri:");
  display.println(brightness);


  display.print("R: ");
  display.print(red);
  display.print(" G: ");
  display.print(green);
  display.print(" B: ");
  display.print(blue);
  display.setCursor(0, 0);
  display.display(); // actually display all of the above
}

void restoreDataFromEprom() {
  Serial.println("READ:");
  // Read data from EEPROM
  newPosition = EEPROM.read(1);
  len = EEPROM.read(2);
  brightness = EEPROM.read(3);
  red = EEPROM.read(4);
  green = EEPROM.read(5);
  blue = EEPROM.read(6);
  Serial.print("pos: ");
  Serial.print(newPosition);
  Serial.print("len: ");
  Serial.print(len);
  Serial.print("bri: ");
  Serial.print(brightness);
  Serial.print("r: ");
  Serial.print(red);
  Serial.print("g: ");
  Serial.print(green);
  Serial.print("b: ");
  Serial.println(blue);
  myEnc.write(newPosition);
}

void setup() {

  Serial.begin(115200);
  EEPROM.begin(512);

  wifiSetup();
  httpServerSetup();
  /*DISPLAY */
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
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
  Serial.println("Basic Encoder Test:");
  pinMode(D3, INPUT_PULLUP);
  attachInterrupt(D3, handleButtonKey, RISING);
  restoreDataFromEprom();
  writeLightStripe();
}



void loop() {
  readEncoder();
  writeLightStripe();
  if (isButtonPressed && millis() - lastUpdateMillis > 20) {
    isButtonPressed = false;
    lastUpdateMillis = millis();
  }

  server.handleClient();

}
