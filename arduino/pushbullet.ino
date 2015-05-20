/*
 * Pushbullet Client
 * Author: Jacob Klinker
 */

#include <SPI.h>
#include <Wire.h>
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_CC3000.h>
#include <avr/pgmspace.h>
#include <ccspi.h>
#include <debug.h>
#include "pushbullet.h"
#include "menu.h"

// definitions for OLED screen
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Definitions for WiFi module
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                         SPI_CLOCK_DIVIDER);
                         
// WiFi security declarations
#define WLAN_SSID       ""                 // This needs to be your WiFi access point name, 32 character max
#define WLAN_PASS       ""                 // WiFi access point password
#define WLAN_SECURITY   WLAN_SEC_WPA2      // WiFi security type
#define IDLE_TIMEOUT_MS 3000

// Definitions for menu buttons
const int SWITCH_BUTTON = 2;
const int SELECT_BUTTON = 6;

// Pushbullet stuff
#define BASE_URL         "173.17.168.19"
#define URL_PROFILE      "/pushbullet/v2/users/me"
#define URL_PUSHES       "/pushbullet/v2/pushes"
#define URL_CONTACTS     "/pushbullet/v2/contacts"
#define URL_DEVICES      "/pushbullet/v2/devices"
#define API_TOKEN        ""

// States
#define STATE_MENU        0
#define STATE_ACCOUNTS    1
#define STATE_PUSHES      2
#define STATE_CONTACTS    3
#define STATE_DEVICES     4
#define STATE_SEND        5
#define STATE_DISCONNECT  6

// Global variables
uint32_t ip;
char jsonBuffer[6] = {' ', ' ', ' ', ' ', ' ', ' '};
int jsonBufferSize = 0;
int selection = 0;
int state = 0;
int prevButton = 0;
char buffer[30];

/**************************************************************************

    Set up all of the connections

 **************************************************************************/
void setup() {
  // set up display and serial communication
  initialize();
  displayProgress(0,21);
  pinMode(SWITCH_BUTTON, INPUT);
  pinMode(SELECT_BUTTON, INPUT);
  
  if (!cc3000.begin()) {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while (1);
  }
  
  displayProgress(22,43);
  // Connect to WiFi access point
  Serial.print(F("\nAttempting to connect to "));
  Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while (1);
  }

  Serial.println(F("Connected!"));
  displayProgress(44,65);

  // Wait for DHCP to complete
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP()) {
    delay(100);
  }

  displayProgress(66,87);
  // Display the IP address DNS, Gateway, etc.
  while (!displayConnectionDetails()) {
    delay(1000);
  }

  ip = 0;
  // Try looking up the website's IP address
  Serial.print(BASE_URL);
  Serial.print(F(" -> "));
  while (ip == 0) {
    if (!cc3000.getHostByName(BASE_URL, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }

    delay(500);
  }

  cc3000.printIPdotsRev(ip);
  displayProgress(88,110);

  // Actually make request to webpage
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 8181);
  if (!requestEndpoint(URL_PROFILE, www)) {
    return;
  }

  displayProgress(110,128);
  // parse the JSON and display the user's name on the screen
  displayUsernameResponse(www);
  www.close();
  
  // show the user their options on screen
  delay(1000);
  displayOptions();
}

/**************************************************************************

    Initializes the serial communication and displays pushbullet
    splash screen on the display

 **************************************************************************/
void initialize() {
  // Set up serial communication with 9600 baud rate and display available RAM
  Serial.begin(9600);
  Serial.println(F("Hello!\n"));
  Serial.print(F("Free RAM: "));
  Serial.println(getFreeRam(), DEC);

  // Activate S2C display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with a Pushbullet splashscreen
  // internally, this will display the splashscreen.
  display.drawBitmap(0,0,pushbullet_data,128,32,0,1);
  display.display();

  // Ensure that WiFi module is correctly wired
  Serial.println(F("\nInitializing..."));
}

/**************************************************************************

    Displays initial options on the screen, along with the current
    selection.
    
    > My Account  |   My Account    |
    My Pushes     |   > My Pushes   |   etc...
    My Contacts   |   My Contacts   |
    My Devices    |   My Devices    |

 **************************************************************************/
void displayOptions() {
  display.clearDisplay();
  
  if (state == STATE_MENU) {
    display.setCursor(0, 0);
    if (selection < 4) {
      if (selection == 0) {
        displayString(0, 2);
      }
      
      displayString(1, 10);
      
      display.setCursor(0, 1 * 8);
      if (selection == 1) {
        displayString(0, 2);
      }
      
      displayString(2, 9);
      
      display.setCursor(0, 2 * 8);
      if (selection == 2) {
        displayString(0, 2);
      }
      
      displayString(3, 11);
      
      display.setCursor(0, 3 * 8);
      if (selection == 3) {
        displayString(0, 2);
      }
      
      displayString(4, 10);
    } else {
      if (selection == 4) {
        displayString(0, 2);
      }
      
      displayString(5, 9);
      
      display.setCursor(0, 1 * 8);
      if (selection == 5) {
        displayString(0, 2);
      }
      
      displayString(6, 10);
    }
  } else if (state == STATE_ACCOUNTS) {
    display.setCursor(0,1);
    displayString(1, 10);
    displayString(":", 1);
  } else if (state == STATE_PUSHES) {
    display.setCursor(0,1);
    displayString(2, 9);
    displayString(":", 1);
  } else if (state == STATE_CONTACTS) {
    display.setCursor(0,1);
    displayString(3, 11); 
    displayString(":", 1);
  } else if (state == STATE_DEVICES) {
    display.setCursor(0,1);
    displayString(4, 10);
    displayString(":", 1);
  } else if (state == STATE_SEND) {
    display.setCursor(0,1);
    displayString(7, 15);
  } else if (state == STATE_DISCONNECT) {
    display.setCursor(0,1);
    displayString(8, 12);
    display.setCursor(0, 14);
    displayString(9, 29); 
  }
  
  display.display();
}

/**************************************************************************

    Read buttons and switch states accordingly

 **************************************************************************/
void loop() {
  int button;
  if (prevButton == 0) {
    button = digitalRead(SWITCH_BUTTON);
    if (button == HIGH) {
      delay(50);
      button = digitalRead(SWITCH_BUTTON);
      
      if (button == HIGH) {
        prevButton = SWITCH_BUTTON;
        switchPressed();
      }
    } else {
      button = digitalRead(SELECT_BUTTON);
      if (button == HIGH) {
        delay(50);
        button = digitalRead(SELECT_BUTTON);
        
        if (button == HIGH) {
          prevButton = SELECT_BUTTON;
          selectPressed();
        }
      }
    }
  } else if (prevButton == SWITCH_BUTTON) {
    button = digitalRead(SWITCH_BUTTON);
    if (button == LOW) {
      prevButton = 0;
    }
  } else if (prevButton == SELECT_BUTTON) {
    button = digitalRead(SELECT_BUTTON);
    if (button == LOW) {
      prevButton = 0;
    }
  }
}

/**************************************************************************

    Switch button has been pressed, so act accordingly and redraw 
    the menu options

 **************************************************************************/
void switchPressed() {
  if (state == STATE_MENU) {
    selection++;
    
    if (state == STATE_MENU && selection > 5) {
      selection = 0;
    }
  } else {
    returnToMenu();
  }
  
  displayOptions();
}

/**************************************************************************

    Select button has been pressed, so change the state

 **************************************************************************/
void selectPressed() {
  if (state == STATE_MENU) {
    if (selection == 0) {
      state = STATE_ACCOUNTS;
    } else if (selection == 1) {
      state = STATE_PUSHES;
    } else if (selection == 2) {
      state = STATE_CONTACTS;
    } else if (selection == 3) {
      state = STATE_DEVICES;
    } else if (selection == 4) {
      state = STATE_SEND;
    } else if (selection == 5) {
      state = STATE_DISCONNECT;
    }
  } else {
    returnToMenu();
  }
  
  displayOptions();
  actOnSelection();
}

/**************************************************************************

    Return to the main menu and ensure the selection is correct from
    the previous state

 **************************************************************************/
void returnToMenu() {
  if (state == STATE_ACCOUNTS) {
    selection = 0;
    state = STATE_MENU;
  } else if (state == STATE_PUSHES) {
    selection = 1;
    state = STATE_MENU;
  } else if (state == STATE_CONTACTS) {
    selection = 2;
    state = STATE_MENU;
  } else if (state == STATE_DEVICES) {
    selection = 3;
    state = STATE_MENU;
  } else if (state == STATE_SEND) {
    selection = 4;
    state = STATE_MENU;
  } else if (state == STATE_DISCONNECT) {
    // Don't do anything here, requires restart of uC
  }
}

/**************************************************************************

    Act accordingly when option from menu is selected, for example: 
    query and display account information on the screen

 **************************************************************************/
void actOnSelection() {
  if (state != STATE_MENU) {
    if (state == STATE_ACCOUNTS) {
      // find my profile and display name and email
      Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 8181);
      displayProgress(0,43);
      if (!requestEndpoint(URL_PROFILE, www)) {
        return;
      }
      
      displayProgress(44,88);
      displayMyAccountResponse(www);
      displayProgress(88,128);
      clearProgress();
      www.close();
      
    } else if (state == STATE_PUSHES) {
      // find my pushes and display the first 3 titles. Note, some pushes
      // do not have titles, they will just show as a blank line. These are
      // pushes that have something attached to them, but without parsing
      // the entire json instead of just looking for the title, it isn't
      // possible to grab other text or would be too processor/memory intensive
      Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 8181);
      displayProgress(0,43);
      if (!requestEndpoint(URL_PUSHES, www)) {
        return;
      }
      
      displayProgress(44,88);
      displayMyPushesResponse(www);
      displayProgress(88,128);
      clearProgress();
      www.close();
      
    } else if (state == STATE_CONTACTS) {
      // find all of my contacts and display names on screen
      Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 8181);
      displayProgress(0,43);
      if (!requestEndpoint(URL_CONTACTS, www)) {
        return;
      }
      
      displayProgress(44,88);
      displayMyContactsResponse(www);
      displayProgress(88,128);
      clearProgress();
      www.close();
      
    } else if (state == STATE_DEVICES) {
      // find all of my devices and display names on screen
      Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 8181);
      displayProgress(0,43);
      if (!requestEndpoint(URL_DEVICES, www)) {
        return;
      }
      
      displayProgress(44,88);
      displayMyDevicesResponse(www);
      displayProgress(88,128);
      clearProgress();
      www.close();
      
    } else if (state == STATE_SEND) {
      // send a post request to pushbullet to send a push out to all my devices
      Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 8181);
      displayProgress(0,44);
      if (!requestPostEndpoint(URL_PUSHES, www)) {
        return;
      }
      
      displayProgress(45,88);
      responseToSerial(www);
      displayProgress(89,128);
      clearProgress();
      www.close();
      display.clearDisplay();
      display.setCursor(0,1);
      displayString(13, 13);
      display.display();
      
    } else if (state == STATE_DISCONNECT) {
      disconnectWiFi();
    }
  }
}

/**************************************************************************

    Tries to read the IP address and other connection details

 **************************************************************************/
bool displayConnectionDetails(void) {
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;

  if (!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv)) {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  } else {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}

/**************************************************************************

    Creates request to BASE_URL plus the endpoint to hit

 **************************************************************************/
bool requestEndpoint(const char* endpoint, Adafruit_CC3000_Client& www) {
  if (www.connected()) {
    www.fastrprint(F("GET "));
    www.fastrprint(endpoint);
    www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: "));
    www.fastrprint(BASE_URL);
    www.fastrprint(F("\r\n"));
    www.fastrprint(F("User-Agent: Arduino\r\n"));
    www.fastrprint(F("Content-Type: application/json\r\n"));
    www.fastrprint(F("Authorization: Bearer "));
    www.fastrprint(API_TOKEN);
    www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    www.println();
    return true;
  } else {
    Serial.println(F("Connection failed"));
    return false;
  }
}

/**************************************************************************

    Creates post request to BASE_URL plus the endpoint to hit

 **************************************************************************/
bool requestPostEndpoint(const char* endpoint, Adafruit_CC3000_Client& www) {
  
  if (www.connected()) {
    www.fastrprint(F("POST "));
    www.fastrprint(endpoint);
    www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: "));
    www.fastrprint(BASE_URL);
    www.fastrprint(F("\r\n"));
    www.fastrprint(F("Accept: application/json"));
    www.fastrprint(F("User-Agent: Arduino\r\n"));
    www.fastrprint(F("Content-Type: application/json\r\n"));
    www.fastrprint(F("Authorization: Bearer "));
    www.fastrprint(API_TOKEN);
    www.fastrprint(F("\r\n"));
    www.fastrprint(F("Content-Length: 107\r\n"));
    www.fastrprint(F("Connection: close\r\n")); 
    www.println();
    www.fastrprint(F("{\"type\":\"note\",\"title\":\"Hey from Arduino\",\"body\":\"This push comes to your from your Arduino, its working!\"}"));
    www.fastrprint(F("\r\n"));
    return true;
  } else {
    Serial.println(F("Connection failed"));
    return false;
  }
}

/**************************************************************************

    Parse the response and output it to serial

 **************************************************************************/
void responseToSerial(Adafruit_CC3000_Client& www) {
  Serial.println();
  Serial.println(F("-------------------------------------"));
  unsigned long lastRead = millis();
  
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (www.available()) {
      char c = www.read();
      Serial.print(c);
      lastRead = millis();
    }
  }
  
  Serial.println();
  Serial.println(F("-------------------------------------"));
}

/**************************************************************************

    Parse the response to the user's profile and display their name
    on OLED

 **************************************************************************/
void displayUsernameResponse(Adafruit_CC3000_Client& www) {
  Serial.println();
  Serial.println(F("-------------------------------------"));

  // Read data until either the connection is closed, or the idle timeout is reached.
  //
  // Here, we con't save the entire response, JSON, headers, etc because it would take
  // too much storage space. So, instead we'll use a 6 character buffer and look
  // for the information that we want from the past 6 characters. for example, we'll
  // first be looking for ame":" which are the six characters before your name is
  // in the JSON response. We'll then grab that name and display it on the OLED screen.
  unsigned long lastRead = millis();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
    
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    bool found = false;
    bool needed = true;
    while (www.available()) {
      char c = www.read();
      Serial.print(c);
      
      // shift our buffer left
      shiftBufferLeft(c);
      
      if (found && c == '"') {
        found = false;
      }
      
      if (found) {
        display.write(c);
        display.display();
      }
      
      // Look for username
      if (jsonBuffer[0] == 'a' && jsonBuffer[1] == 'm' &&
          jsonBuffer[2] == 'e' && jsonBuffer[3] == '"' &&
          jsonBuffer[4] == ':' && jsonBuffer[5] == '"' && needed) {
        found = true;
        needed = false;
        display.clearDisplay();
        displayString(10, 4);
        display.display();
      }
      
      lastRead = millis();
    }
    
    if (found) {
      break;
    }
  }
  
  Serial.println();
  Serial.println(F("-------------------------------------"));
}

/**************************************************************************

    Parse the response to the user's profile and display their name
    and email on the screen

 **************************************************************************/
void displayMyAccountResponse(Adafruit_CC3000_Client& www) {
  Serial.println();
  Serial.println(F("-------------------------------------"));

  unsigned long lastRead = millis();
  display.setCursor(0,13);
    
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    int found = 0;
    bool needed = true;
    int currentNumChars = 0;
    while (www.available()) {
      char c = www.read();
      Serial.print(c);
      
      // shift our buffer left
      shiftBufferLeft(c);
      
      if (found == 1 && c == '"') {
        found++;
        display.setCursor(0,22);
      } else if (found == 3 && c == '"') {
        found++;
      }
      
      if (found == 1 || found == 3) {
        if (currentNumChars < 21) {
          display.write(c);
          display.display();
        }
        currentNumChars++;
      }
      
      // Look for name
      if (jsonBuffer[0] == 'a' && jsonBuffer[1] == 'm' &&
          jsonBuffer[2] == 'e' && jsonBuffer[3] == '"' &&
          jsonBuffer[4] == ':' && jsonBuffer[5] == '"' && needed) {
        found++;
        
        if (found == 3) {
          needed = false;
        }
        
        displayString(11, 6);
        currentNumChars = 6;
        display.display();
      } else if (jsonBuffer[0] == 'a' && jsonBuffer[1] == 'i' &&
          jsonBuffer[2] == 'l' && jsonBuffer[3] == '"' &&
          jsonBuffer[4] == ':' && jsonBuffer[5] == '"' && needed) {
        found++;
        
        if (found == 3) {
          needed = false;
        }
        
        displayString(12, 7);
        currentNumChars = 7;
        display.display();
      }
      
      lastRead = millis();
    }
    
    if (found) {
      break;
    }
  }
  
  Serial.println();
  Serial.println(F("-------------------------------------"));
}

/**************************************************************************

    Parse the response to the user's pushes and display them
    on the screen

 **************************************************************************/
void displayMyPushesResponse(Adafruit_CC3000_Client& www) {
  displayGenericThreeLineResponse(www, 't', 'l', 'e');
}

/**************************************************************************

    Parse the response to the user's contacts and display them
    on the screen

 **************************************************************************/
void displayMyContactsResponse(Adafruit_CC3000_Client& www) {
  displayGenericThreeLineResponse(www, 'a', 'm', 'e');
}

/**************************************************************************

    Parse the response to the user's devices and display them
    on the screen

 **************************************************************************/
void displayMyDevicesResponse(Adafruit_CC3000_Client& www) {
  displayGenericThreeLineResponse(www, 'a', 'm', 'e');
}

/**************************************************************************

    Parse the response and display the text on three lines. Matches to
    the JSON will be checked against the first three characters, plus ":"

 **************************************************************************/
void displayGenericThreeLineResponse(Adafruit_CC3000_Client& www, char char1, char char2, char char3) {
  Serial.println();
  Serial.println(F("-------------------------------------"));

  unsigned long lastRead = millis();
  display.setCursor(0,8);
    
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    int found = 0;
    bool needed = true;
    int currentNumChars = 0;
    while (www.available()) {
      char c = www.read();
      Serial.print(c);
      
      // shift our buffer left
      shiftBufferLeft(c);
      
      if ((found == 1 || found == 3 || found == 5) && c == '"') {
        found++;
      }
      
      if (found == 1 || found == 3 || found == 5) {
        if (currentNumChars < 21) {
          display.write(c);
          display.display();
        }
        
        currentNumChars++;
      }
      
      // Look for name
      if (jsonBuffer[0] == char1 && jsonBuffer[1] == char2 &&
          jsonBuffer[2] == char3 && jsonBuffer[3] == '"' &&
          jsonBuffer[4] == ':' && jsonBuffer[5] == '"' && needed) {
        found++;
        
        if (found == 1) {
          display.setCursor(0,9);
        } else if (found == 3) {
          display.setCursor(0,17);
        } else if  (found == 5) {
          display.setCursor(0,25);
          needed = false;
        }
        
        currentNumChars = 0;
      }
      
      lastRead = millis();
    }
  }
  
  Serial.println();
  Serial.println(F("-------------------------------------"));
}

/**************************************************************************

    Shifts JSON buffer to the left by one, allows for processing
    the JSON without storing the whole thing which is very important
    due to memory limitations on the device.

 **************************************************************************/
void shiftBufferLeft(char c) {
  if (jsonBufferSize == 6) {
    jsonBuffer[0] = jsonBuffer[1];
    jsonBuffer[1] = jsonBuffer[2];
    jsonBuffer[2] = jsonBuffer[3];
    jsonBuffer[3] = jsonBuffer[4];
    jsonBuffer[4] = jsonBuffer[5];
    jsonBuffer[5] = c;
  } else {
    jsonBuffer[jsonBufferSize] = c;
    jsonBufferSize++;
  }
}

/**************************************************************************

    Writes a string to the display

 **************************************************************************/
void displayString(int stringTablePos, int charsLength) {
  strcpy_P(buffer, (char*) pgm_read_word(&(STRING_TABLE[stringTablePos])));
  for (int i = 0; i < charsLength; i++) {
    display.write(buffer[i]);
  }
}

/**************************************************************************

    Fill pixels above the top of the screen to display how much the
    application has loaded

 **************************************************************************/
void displayProgress(int start, int stop) {
  for (int i = start; i <= stop; i++) {
    display.drawPixel(i, 0, WHITE);
    display.display();
    
    // delay so we have a nice, smooth animation
    delay(4);
  }
}

/**************************************************************************

    Clear the top row of pixels

 **************************************************************************/
void clearProgress() {
  for (int i = 0; i <= 128; i++) {
    display.drawPixel(i, 0, BLACK);
  }
  display.display();
}

/**************************************************************************

    Writes a string to the display

 **************************************************************************/
void displayString(char* chars, int charsLength) {
  for (int i = 0; i < charsLength; i++) {
    display.write(chars[i]);
  }
}

/**************************************************************************

    Ensures everything shuts down correctly, always needs to be called

 **************************************************************************/
void disconnectWiFi() {
  Serial.println(F("\n\nDisconnecting"));
  cc3000.disconnect();
}
