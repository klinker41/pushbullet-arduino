#ifndef _menu_h_
#define _menu_h_

static const char SELECTOR[] PROGMEM = "> ";
static const char MY_ACCOUNT[] PROGMEM = "My Account";
static const char MY_PUSHES[] PROGMEM = "My Pushes";
static const char MY_CONTACTS[] PROGMEM = "My Contacts";
static const char MY_DEVICES[] PROGMEM = "My Devices";
static const char SEND_PUSH[] PROGMEM = "Send Push";
static const char DISCONNECT[] PROGMEM = "Disconnect";
static const char SENDING_PUSH[] PROGMEM = "Sending Push...";
static const char DISCONNECTED[] PROGMEM = "Disconnected";
static const char RESET_INST[] PROGMEM = "To reconnect, reset  Arduino.";
static const char HI[] PROGMEM = "Hi, ";
static const char NAME[] PROGMEM = "Name: ";
static const char EMAIL[] PROGMEM = "Email: ";
static const char DONE[] PROGMEM = "Done Sending!";

static const char* const STRING_TABLE[] PROGMEM = {
  SELECTOR,
  MY_ACCOUNT,
  MY_PUSHES,
  MY_CONTACTS,
  MY_DEVICES,
  SEND_PUSH,
  DISCONNECT,
  SENDING_PUSH,
  DISCONNECTED,
  RESET_INST,
  HI,
  NAME,
  EMAIL,
  DONE
};

#endif
