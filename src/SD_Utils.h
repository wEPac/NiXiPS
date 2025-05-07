 /*
 * Note:  The SPI pins can be manually configured by using `SPI.begin(sck, miso, mosi, cs).`
 *        Alternatively, you can change the CS pin and use the other default settings by using `SD.begin(cs)`.
 */

/*
#include "SPI.h"
#include "FS.h"
#include "SD.h"
//*/


#define TFT_CS    18
#define SD_CS      3
//*
void selectCS2(int num)
{
  digitalWrite(TFT_CS, !(num == TFT_CS));
  digitalWrite(SD_CS,  !(num == SD_CS));
}

void cardSpec()
{
  uint8_t     cardType =    SD.cardType();

  //if (cardType == CARD_NONE) { Serial.println("No SD card attached"); }
  if (cardType == CARD_NONE) { spr.println("No SD card attached"); }
  else {
    /*
    Serial.print("SD Card Type: ");
    if      (cardType == CARD_MMC)  { Serial.println("MMC"); }
    else if (cardType == CARD_SD)   { Serial.println("SDSC"); }
    else if (cardType == CARD_SDHC) { Serial.println("SDHC"); }
    else                            { Serial.println("UNKNOWN"); }
    Serial.printf("SD Card Size: %llu MB\n", cardSize);
    //*/
    spr.print("SD Card:\n   ");
    if      (cardType == CARD_MMC)  { spr.println("MMC"); }
    else if (cardType == CARD_SD)   { spr.println("SDSC"); }
    else if (cardType == CARD_SDHC) { spr.println("SDHC"); }
    else                            { spr.println("UNKNOWN"); }

    uint64_t  cardSize =    SD.cardSize() / (1024 * 1024 * 1024);
    spr.printf("   %llu GB\n", cardSize);
  }
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{ //ie listDir(SD, "/", 0);
  spr.printf("Dir: %s\n   ", dirname); //selectCS2(TFT_CS); spr.pushSprite(0, 0); selectCS2(SD_CS); delay(4000);
    
  File root = fs.open(dirname);

  if      (!root)               { spr.println("Failed to open directory"); } //selectCS2(TFT_CS); spr.pushSprite(0, 0); delay(4000); }
  else if (!root.isDirectory()) { spr.println("Not a directory"); } //selectCS2(TFT_CS); spr.pushSprite(0, 0); delay(4000); }
  else
  {
    //spr.printf("Listing directory: %s\n", dirname); selectCS2(TFT_CS); spr.pushSprite(0, 0); selectCS2(SD_CS); delay(4000);
    File file = root.openNextFile();
    while (file)
    {
      if (file.isDirectory())
      {
        spr.print("  Dir: "); spr.println(file.name());
        if (levels) { listDir(fs, file.path(), levels - 1); }
      }
      else
      {
        spr.print("  File: "); spr.print(file.name());
        spr.print("  Size: "); spr.println(file.size());
      }
      file = root.openNextFile();
    }
  }
}

void createDir(fs::FS &fs, const char *path)
{ // ie createDir(SD, "/mydir");
  if (fs.mkdir(path)) { Serial.printf("Dir %s created\n", path); }
  else                { Serial.printf("mkDir %s failed\n", path); }
}

void removeDir(fs::FS &fs, const char *path)
{ // ie removeDir(SD, "/mydir");
  if (fs.rmdir(path)) { Serial.printf("Dir %s removed\n", path); }
  else                { Serial.printf("rmDir %s failed\n", path); }
}

void readFile(fs::FS &fs, const char *path)
{ // ie readFile(SD, "/hello.txt");
  File file = fs.open(path);

  if (!file)                  { Serial.printf("Failed to read %s", path); }
  else
  {
    Serial.printf("Read from %s: ", path);
    while (file.available())  { Serial.write(file.read()); }
    file.close();
  }
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{ // ie writeFile(SD, "/hello.txt", "Hello ");
  File file = fs.open(path, FILE_WRITE);

  if (!file)                  { Serial.printf("Write %s failed\n", path); }
  else
  {
    if (file.print(message))  { Serial.printf("File %s written\n", path); }
    else                      { Serial.printf("Write %s failed\n", path); }
    file.close();
  }
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{ // ie appendFile(SD, "/hello.txt", "World!\n");
  File file = fs.open(path, FILE_APPEND);

  if (!file)                  { Serial.printf("Append to %s failed\n", path); }
  else
  {
    if (file.print(message))  { Serial.printf("Appended to %s\n", path); }
    else                      { Serial.printf("Append to %s failed\n", path); }
    file.close();
  }
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{ // ie renameFile(SD, "/hello.txt", "/foo.txt");
  Serial.printf("Renaming file %s to %s ", path1, path2);

  if (fs.rename(path1, path2))  { Serial.println("succeed"); }
  else                          { Serial.println("failed"); }
}

void deleteFile(fs::FS &fs, const char *path)
{ // ie deleteFile(SD, "/foo.txt");
  Serial.printf("Deleting file: %s ", path);

  if (fs.remove(path))  { Serial.println("succeed"); }
  else                  { Serial.println("failed"); }
}

void testFileIO(fs::FS &fs, const char *path)
{ // ie testFileIO(SD, "/test.txt");
  static uint8_t  buf[512];
  size_t          len =       0;
  uint32_t        start =     millis();
  uint32_t        end =       start;

  File file = fs.open(path);
  if (file)
  {
    len =             file.size();
    size_t  flen =    len;
    start =           millis();
    while (len)
    {
      size_t  toRead =              len;
      if (toRead > 512) { toRead =  512; }
      file.read(buf, toRead);
      len -=                        toRead;
    }
    end =             millis() - start;
    Serial.printf("%u bytes read for %lu ms\n", flen, end);
    file.close();
  }
  else      { Serial.println("Failed to open file for reading"); }

  file = fs.open(path, FILE_WRITE);
  if (!file) { Serial.println("Failed to open file for writing"); }
  else
  {
    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++) { file.write(buf, 512); }
    end = millis() - start;
    Serial.printf("%u bytes written for %lu ms\n", 2048 * 512, end);
    file.close();
  }
}

