// MFRC522 card reader
#include <SPI.h>
#include <MFRC522.h>

// Liquid Cristal Screen
//#include <Wire.h>
//#include <LiquidCrystal_I2C.h>

// Ethernet
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>

// Liquid Crystal initialize PIN 2 (SDA), PIN 3 (SCL) if arduino leonardo
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Software version
String version =  "V1.0";

// MFRC522 card reader
#define SS_PIN 10
#define RST_PIN 9

// Lcd rows and columns
#define lcdRows 2
#define lcdCols 16


// Create MFRC522 instance.
MFRC522 cardReader(SS_PIN, RST_PIN);


// Log messages to serial port, for debug and stuff
void logMessage(const String &message)
{
  Serial.println(message);
}

// Print a message into the lcd screen centered
void printLcdMessage(byte line, const String &message)
{
  //lcd.setBacklight(HIGH);
  //lcd.setBacklight(LOW);
  lcd.clear();
  int len = message.length();
  int pos = max((lcdCols - len) / 2, 0);
  lcd.setCursor(pos, line);
  lcd.print(message);
}

// Print the welcome message on the screen
void printWelcomeMessage()
{
  printLcdMessage(0, "ParkingGate " + version);
  logMessage("ParkingGate " + version);
  printLcdMessage(1, "Pass card please");
  logMessage( "Pass card please");
}

// Check if we can start, like if barrier is up or down
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

  // Print a message centered into the LCD screen.
  printLcdMessage(0, "Cesar Araujo");
  printLcdMessage(1, "ParkingGate " + version);
  logMessage("Cesar Araujo  ParkingGate " + version);

  // Wait 3 seconds for message to be displayed
  delay(3000);

  // checkSafety, if it fails exit with an error code.
  if ( checkSafety() == false )
  {
    logMessage("Fail on check safety, going to exit, bye!");
    exit;
  }
  printWelcomeMessage();
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

  logMessage("Reading serial data.");

  // Clear the card id
  String cardId = "";
  for (byte i = 0; i < cardReader.uid.size; i++)
  {
    Serial.print(cardReader.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(cardReader.uid.uidByte[i], HEX);
    cardId.concat(String(cardReader.uid.uidByte[i] < 0x10 ? " 0" : " "));
    cardId.concat(String(cardReader.uid.uidByte[i], HEX));
  }

  logMessage("Card id: " + cardId);

  printLcdMessage(0, "card:" + cardId);

  // For debug only!
  delay(3000);

  //if (cardId.substring(1) == "ED 78 03 CA")

  // All done, lets print the welcome message on the lcd screen
  printWelcomeMessage();


}
