// MFRC522 card reader
#include <SPI.h>
#include <MFRC522.h>

// Liquid Cristal Screen
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Ethernet
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>

// Sd card reader
#include <SD.h> // We are going to read and write PICC's UIDs from/to SD

// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Lcd rows and columns
#define lcdRows 4
#define lcdCols 20

// MFRC522 card reader
/*
  We need to define MFRC522's pins and create instance
  Pin layout should be as follows (on Arduino Uno):
  MOSI: Pin 11 / ICSP-4
  MISO: Pin 12 / ICSP-1
  SCK : Pin 13 / ICSP-3
  SDA : (Configurable)
  RST : Not Needed
 */

#define SS_PIN 53
#define RST_PIN 5

// Create MFRC522 instance.
MFRC522 cardReader(SS_PIN, RST_PIN);

/************ ETHERNET STUFF ************/
// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 245);
EthernetServer server(80);

// Set Sd card pin
#define sdPin 4


// Rfids cards
byte readCard[4];    // Stores scanned ID read from RFID Module
byte masterCard[4]; // Stores master card's ID

// File name on sdcard and folder
char filename[] = "rfidcards.dat";  // Stores variable filename
char extension[] = "dat";          // sometimes the extension gets modified
char dir[] = "/PICCS/";

// Software version
String version =  "Ver. 1.0";

String MyIpAddress = "";


// Log messages to serial port, for debug and stuff
void logMessage(const String &message)
{
  Serial.println(message);
}

void clearLcd()
{
  lcd.clear();
}

// Print a message into the lcd screen centered
void printLcdMessage(byte line, const String &message)
{
  //lcd.setBacklight(HIGH);
  //lcd.setBacklight(LOW);
  int len = message.length();
  int pos = max((lcdCols - len) / 2, 0);
  lcd.setCursor(pos, line);
  lcd.print(message);
  logMessage(message);
}

// Print the welcome message on the screen
void printWelcomeMessage()
{
  clearLcd();
  printLcdMessage(0, "Camping Ave");
  printLcdMessage(1, "ParkingGate " + version);
  printLcdMessage(3, "Waiting for card");
}

// Convert the ip to a string
String ipAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") + \
         String(ipAddress[1]) + String(".") + \
         String(ipAddress[2]) + String(".") + \
         String(ipAddress[3])  ;
}

// Check if we can start, like if barrier is up or down
void initializeEthernet()
{
  logMessage("Network initialize");
  Ethernet.begin(mac, ip);
  server.begin();
  MyIpAddress = ipAddress2String(Ethernet.localIP());
  logMessage("Server ip: " + MyIpAddress);
  logMessage("Sdcard initialize:");
  // Initialize SD Hardware
  if (!SD.begin(sdPin)) {
    // Could not initialize
    // stopExecution("SD initialization failed! Where will i read the cards?");
  }
}



void stopExecution(const String &message)
{
  logMessage("STOPING EXECUTION WITH REASON:");
  logMessage(message);
  while (true)
  {
    delay(2000);
  }
}
bool checkSafety()
{

  return true;
}

void setup()
{
  // Start the serial port
  Serial.begin(9600);
  // Wait for serial to initialize
  delay(3000);
  logMessage("Serial begin, welcome!");

  // Start the spi bus
  SPI.begin();
  logMessage("Spi bus initialization.");

  // MFRC522 Start the mfrc522 card reader.
  cardReader.PCD_Init();
  logMessage("mfrc522 initialization.");

  // set up the LCD's number of columns and rows:
  lcd.begin(lcdCols, lcdRows);
  //logMessage("Lcd rows:" + lcdRows + ", columns:" + lcdCols +".");

  // Initialize ethernet shield
  initializeEthernet();

  // Print a message centered into the LCD screen.
  clearLcd();
  printLcdMessage(0, "Cesar Araujo");
  printLcdMessage(2, "ParkingGate " + version);
  printLcdMessage(3, "IP: " + MyIpAddress);

  // Wait 5 seconds for message to be displayed
  delay(5000);

  // RFID card details
  ShowReaderDetails();

  // checkSafety, if it fails exit with an error code.
  if ( checkSafety() == false )
  {
    logMessage("Fail on check safety, going to exit, bye!");
    exit;
  }
  printWelcomeMessage();
}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = cardReader.PCD_ReadRegister(cardReader.VersionReg);
  logMessage("MFRC522 Software Version: 0x");
  Serial.print(v, HEX);
  if (v == 0x91)
  {
    logMessage(" = v1.0");
  }
  else if (v == 0x92)
  {
    logMessage(" = v2.0");
  }
  else
  {
    logMessage(" (unknown)");
  }
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) 
  {
    stopExecution("Communication failure, is the MFRC522 properly connected?");
  }
}

void loop()
{
  // Chill out a little
  delay(500);
  // Look for new cards
  if ( ! cardReader.PICC_IsNewCardPresent())
  {
    // We dont have a card, lets go away.
    return;
  }

  logMessage("New card present!");

  // Select one of the cards
  if ( ! cardReader.PICC_ReadCardSerial())
  {
    return;
  }

  // Clear the card id
  String cardId = "";
  for (byte i = 0; i < cardReader.uid.size; i++)
  {
   // Serial.print(cardReader.uid.uidByte[i] < 0x10 ? " 0" : " ");
   // Serial.print(cardReader.uid.uidByte[i], HEX);
    cardId.concat(String(cardReader.uid.uidByte[i] < 0x10 ? " 0" : " "));
    cardId.concat(String(cardReader.uid.uidByte[i], HEX));
  }

  Serial.write("size" + sizeof(cardId));

 // logMessage("Card id: " + cardId);
  printLcdMessage(3, "Card id:" + cardId);

  // For debug only!
  delay(5000);

  //if (cardId.substring(1) == "ED 78 03 CA")

  // All done, lets print the welcome message on the lcd screen
  printWelcomeMessage();


}

