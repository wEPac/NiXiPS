// This sketch draws Jpeg files stored on an SD card on a TFT screen, it is based on the UTFT_Bitmap
// by Henning Karlsen.
// web: http://www.RinkyDinkElectronics.com/
//
// This demo uses an Arduino Due and a 320x240 ILI9341 SPI based TFT screen.
// The demo should also run on an Arduino Mega, but it will be slower
/*
  By default the UTFT library does not configure the Gamma curve settings for the ILI9341 TFT,
  so photo images may not render well.  To correct this ensure the set Gamma curve section
  in initlcd.h (library folder UTFT\tft_drivers\ili9341\s5p\initlcd.h) is NOT commented out.

  You can generate your own Jpeg images from digital photographs by cropping and resizing
  by using commonly available picture/image editors such as Paint or IrfanView.
*/

//  The latest JPEGDecoder library can be found here:
//  https://github.com/Bodmer/JPEGDecoder

//  Information on JPEG compression can be found here:
//  https://en.wikipedia.org/wiki/JPEG 
  
/*
* /0.bmp
* /DIGITS/0.bmp
* /DIGITS/NIXIES_01/0.bmp
//*/




//====================================================================================
//  definitions
//====================================================================================

// SD directories
#define DIR_CORE      "/CORE"     // to record basic needs
#define DIR_DIGITS    "/DIGITS"   // to record digit sets
#define DIG_AMOUNT    6           //    Digit set Amount
#define SET_NIXIE1    "/NIXIE1"   //    Digit set#
#define SET_NIXIE2    "/NIXIE2"   //    Digit set#
#define SET_VFD1      "/VFD1"     //    Digit set#
#define SET_VFD2      "/VFD2"     //    Digit set#
#define SET_VFD3      "/VFD3"     //    Digit set#
#define SET_FLIP1     "/FLIP1"    //    Digit set#
#define DIR_MUSIC     "/MUSIC"    // to record music (WAW/MP3) used for alarms
#define FILE_CORE_TEST_TXT  "/CORE/test.txt"



// Set the pins to the correct ones for your SPI TFT
// ------------------------------------------------------------
/*
#define TFT_SDA    7  // Do not use hardware SPI MOSI pin
#define TFT_SCL    6  // Do not use hardware SPI SCK pin
#define TFT_CS    10  // Chip Select for TFT
#define TFT_RS     9  // Register select (also called DC)
#define TFT_RST    8  // uncomment if you have ILI9340
*/

// SD card connects to hardware SPI pins MOSI, MISO and SCK and the following chip select
#define SD_SCLK   40//43
#define SD_MOSI   39//44
#define SD_MISO   38// 1
#define SD_CS      3//8 // Chip Select for SD card

#define SD_CS_PIN     SD_CS


//====================================================================================
//  libraries
//====================================================================================


//#pragma execution_character_set("utf-8")



// Hardware-specific library
#include <SPI.h>
//SPIClass *vspi =  NULL;               //uninitialized pointers to SPI objects
//SPIClass *hspi =  NULL;


//#define USER_SETUP_LOADED
#include "User_Setup.h"               // eSPI configuration (!DO NOT CHANGE NAME and ORDER!)
#include <TFT_eSPI.h>                 // eSPI library
TFT_eSPI    tft = TFT_eSPI();         // Library instance,  this will set the TFT chip select high
//SPIClass    tftspi = tft.getSPIinstance();
TFT_eSprite spr = TFT_eSprite(&tft);  // Declare Sprite object "spr" with pointer to "tft" object

#include <FS.h>

#include "SPIFFS.h"                   // For ESP32 only

#include <SD.h>                       // Arduino IDE SD library 8.3 DOS files
#include "SD_Utils.h"                 // BMP library


#include "BMP_Utils.h"                // BMP library
//#include <JPEGDecoder.h>              // JPEG decoder library




//====================================================================================
//  functions
//====================================================================================

// this function determines the minimum of two numbers
//#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

void initCS()
{
  pinMode(TFT_CS, OUTPUT);
  pinMode(SD_CS,  OUTPUT);
  selectCS(-1);
}

void selectCS(int num)
{
  digitalWrite(TFT_CS, !(num == TFT_CS));
  digitalWrite(SD_CS,  !(num == SD_CS));
  //Serial.print(!(num == TFT_CS)); Serial.print("    "); Serial.println(!(num == SD_CS));
}



//====================================================================================
//  setup
//====================================================================================
void setup()
{
  Serial.begin(115200); while (!Serial); delay(500);

  
  // ===== set each CS to HIGH =====
  initCS(); //delay(1000);


  //Serial.println(F("Initialising SPI...")); delay(500);
  //vspi = new SPIClass(VSPI);  //initialize two instances of the SPIClass
  //vspi->begin(SD_SCLK, SD_MISO, SD_MOSI, -1);


  // ===== init Software SPI =====
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, -1);


  // ===== init SD card =====
  //*
  selectCS(SD_CS);
  if (!SD.begin())  { Serial.println(F("SD failed!")); while (true) delay(500); }
  else              { Serial.println(F("SD OK!")); delay(500); }
  //*/


  // ===== init TFT IPS =====
  selectCS(TFT_CS);
  tft.init();           // initialize harware SPI
  tft.fillScreen(TFT_WHITE); delay(500);
  tft.fillScreen(TFT_RED);   delay(500);
  tft.fillScreen(TFT_GREEN); delay(500);
  tft.fillScreen(TFT_BLUE);  delay(500);
  tft.fillScreen(TFT_BLACK); delay(500);
  tft.setRotation(0);  // default 0
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  tft.println("SPI OK!");
  tft.println("SD  OK!");
  tft.println("TFT OK!");
  delay(1000);

  //spr.setColorDepth(8);  // default 16
  spr.createSprite(TFT_WIDTH, TFT_HEIGHT);


  spr.setTextColor(TFT_GREEN, TFT_BLACK);
  //spr.setTextFont(1);
  //spr.setTextFont(2);
  spr.setTextFont(4);
  spr.setTextSize(1);

  /*
  spr.fillScreen(TFT_BLACK); spr.setCursor(0, 0);
  selectCS(SD_CS);    cardSpec();
  selectCS(TFT_CS);   spr.pushSprite(0, 0);
  delay(4000);
  //*/

  /*
  spr.fillScreen(TFT_BLACK); spr.setCursor(0, 0);
  selectCS(SD_CS);    listDir(SD, "/DIGITS", 1);
  selectCS(TFT_CS);   spr.pushSprite(0, 0);
  delay(4000);
  //*/

  readFile(SD, "/lisez.txt");
  readFile(SD, "/lisez-bien-plus.txt");
  readFile(SD, "/lisez bien plus.txt");
}


//====================================================================================
//  loop
//====================================================================================

uint8_t digit_idx = 0;

void loop()
{
  // draw jpeg image at 0,0
  //spr.fillSprite(TFT_BLACK);
  ////jpegDraw( "tiger.jpg", 0, 0 );
  //spr.pushSprite(0, 0);
  //delay(5000);

  //*
  for (uint8_t cnt = 0; cnt < 10; cnt++)
  {
    spr.fillSprite(TFT_BLACK);
    spr.setCursor(0, 0);
    selectCS(SD_CS);
    std::string pat;
    pat = pat + DIR_DIGITS;
    switch (digit_idx / 2)
    {
      case 0:   pat = pat + SET_NIXIE1;   break;
      case 1:   pat = pat + SET_NIXIE2;   break;
      case 2:   pat = pat + SET_VFD1;     break;
      case 3:   pat = pat + SET_VFD2;     break;
      case 4:   pat = pat + SET_VFD3;     break;
      case 5:   pat = pat + SET_FLIP1;    break;
      default:  pat = pat + SET_NIXIE1;
    }
    pat = pat + "/" + std::to_string(cnt) + ".bmp";
    const char* path = pat.c_str();
    BMPdraw(path, 0, 0);
    selectCS(TFT_CS);
    spr.pushSprite(0, 0);
    delay(1000);
  }
  if (++digit_idx / 2 >= DIG_AMOUNT) digit_idx = 0;
  //*/


}

//====================================================================================

