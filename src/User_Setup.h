//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc.
//
//   See the User_Setup_Select.h file if you wish to be able to define multiple
//   setups and then easily select which setup file is used by the compiler.
//
//   If this file is edited correctly then all the library example sketches should
//   run without the need to make any more changes for a particular hardware setup!
//   Note that some sketches are designed for a particular TFT pixel width/height

// User defined information reported by "Read_User_Setup" test & diagnostics example
#define USER_SETUP_INFO "User_Setup"


// Define to disable all #warnings in library (can be put in User_Setup_Select.h)
//#define DISABLE_ALL_LIBRARY_WARNINGS


// To make the Library not over-write all this:
#define USER_SETUP_LOADED


// ##################################################################################
//
// Section 1. Call up the right driver file and any options for it
//
// ##################################################################################

// Only define one driver, the other ones must be commented out
#define ST7789_DRIVER      // Full configuration option, define additional parameters below for this display
//#define ST7789_2_DRIVER    // Minimal configuration option, define additional parameters below for this display


// For ST7789, ST7735, ILI9163 and GC9A01 ONLY, define the pixel width and height in portrait orientation
#define TFT_WIDTH  135 // ST7789 135 x 240
#define TFT_HEIGHT 240 // ST7789 135 x 240
#define CGRAM_OFFSET      // Library will add offsets required


// For ST7735, ST7789 and ILI9341 ONLY, define the colour order IF the blue and red are swapped on your display
// Try ONE option at a time to find the correct colour order for your display
//  #define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
//  #define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red


// If colours are inverted (white shows as black) then uncomment one of the next
// 2 lines try both options, one of the options should correct the inversion.
//#define TFT_INVERSION_ON
//#define TFT_INVERSION_OFF


// ##################################################################################
//
// Section 2. Define the pins that are used to interface with the display here
//
// ##################################################################################

// If a backlight control signal is available then define the TFT_BL pin below. 
// The backlight will be turned ON when tft.begin() is called, but the library
// needs to know if the LEDs are ON with the pin HIGH or LOW. If the LEDs are to be
// driven with a PWM signal or turned OFF/ON then this must be handled by the user
// sketch. e.g. with digitalWrite(TFT_BL, LOW);
//#define TFT_BL   32            // LED back-light control pin
//#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)


// Some displays support SPI reads via the MISO pin, other displays have a single
// bi-directional SDA pin and the library will try to read this via the MOSI line.
// To use the SDA line for reading data from the TFT uncomment the following line:
#define TFT_SDA_READ      // This option is for ESP32 ONLY, tested with ST7789 and GC9A01 display only


// ###### EDIT THE PIN NUMBERS IN THE LINES FOLLOWING TO SUIT YOUR ESP32 SETUP   ######
// For ESP32 Dev board (only tested with ILI9341 display)
// The hardware SPI can be mapped to any pins
/*
#define TFT_SCLK  3 //
#define TFT_MOSI  8 // It might be written as "SDA"
#define TFT_MISO -1 // Set to -1 if displays have a single bi-directional SDA pin
#define TFT_DC   17 // Data Command control pin, might be written as "RS"
#define TFT_CS   18 // Chip select control pin
#define TFT_RST  46 // Set to -1 if display RESET is connected to ESP32 board RST
//*/
#define TFT_SCLK 43 //  3
#define TFT_MOSI 44 //  8 // It might be written as "SDA"
#define TFT_MISO -1 //  1 // Set to -1 if displays have a single bi-directional SDA pin
#define TFT_DC    2 // 17 // Data Command control pin, might be written as "RS"
#define TFT_CS   18 // 18 // Chip select control pin
#define TFT_RST  -1 // -1 // Set to -1 if display RESET is connected to ESP32 board RST


// ##################################################################################
//
// Section 3. Define the fonts that are to be used here
//
// ##################################################################################

// Comment out the #defines below with // to stop that font being loaded
// The ESP8366 and ESP32 have plenty of memory so commenting out fonts is not
// normally necessary. If all fonts are loaded the extra FLASH space required is
// about 17Kbytes. To save FLASH space only enable the fonts you need!
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
//#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
//#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
//#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH
#define SMOOTH_FONT


// ##################################################################################
//
// Section 4. Other options
//
// ##################################################################################
// Define the SPI clock frequency, this affects the graphics rendering speed. Too
// fast and the TFT driver will not keep up and display corruption appears.
//#define SPI_FREQUENCY  10000000 // 
//#define SPI_FREQUENCY  40000000 // 
//#define SPI_FREQUENCY  50000000 // 
//#define SPI_FREQUENCY  60000000 // 
//#define SPI_FREQUENCY  70000000 // 22 fps
#define SPI_FREQUENCY  80000000 // 24 fps
//#define SPI_FREQUENCY  90000000 // 28 fps
//#define SPI_FREQUENCY 100000000 // 28 fps
//#define SPI_FREQUENCY 110000000 // 28 fps
//#define SPI_FREQUENCY 120000000 // 28 fps

// Optional reduced SPI frequency for reading TFT
//#define SPI_READ_FREQUENCY  20000000

// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
//#define SPI_TOUCH_FREQUENCY  2500000

// The ESP32 has 2 free SPI ports i.e. VSPI and HSPI, the VSPI is the default.
// If the VSPI port is in use and pins are not accessible (e.g. TTGO T-Beam)
// then uncomment the following line:
#define USE_HSPI_PORT

// Comment out the following #define if "SPI Transactions" do not need to be
// supported. When commented out the code size will be smaller and sketches will
// run slightly faster, so leave it commented out unless you need it!
//
// Transaction support is needed to work with SD library but not needed with TFT_SdFat
// Transaction support is required if other SPI devices are connected.
// Transactions are automatically enabled by the library for an ESP32 (to use HAL mutex)
//    so changing it here has no effect
// #define SUPPORT_TRANSACTIONS
