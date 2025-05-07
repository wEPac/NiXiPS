
#ifndef   USE_SPI
#define   USE_SPI       // SDcard reader is using SPI
#endif

#ifndef   BMP_UTILS_H
#define   BMP_UTILS_H



#define TFT_CS    18
#define SD_CS      3
//*
void selectCSl(int num)
{
  digitalWrite(TFT_CS, !(num == TFT_CS));
  digitalWrite(SD_CS,  !(num == SD_CS));
}
//*/

uint16_t BMPread16(fs::File &f)
{
  uint16_t  result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t BMPread32(fs::File &f)
{
  uint32_t  result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void BMPdraw(const char *filename, int16_t x, int16_t y)
{ // draw a 16 or 24 bits Bitmap with NO alpha to display
  //if ((x >= tft.width()) || (y >= tft.height())) return;

  File bmpFS;

  // Open requested file on SD card
#ifdef USE_SPI
  bmpFS = SD.open(filename, FILE_READ);
#else
  bmpFS = SD_MMC.open(filename);
#endif

  if (!bmpFS)
  {
    //Serial.println("File not found\n");
    //spr.println("File not found\n");
    spr.print("File '"); spr.print(filename); spr.println("' not found\n");
    return;
  }
  //Serial.println("File found\n");
  //spr.println("File found\n"); while(1) return;
  //spr.print("File "); spr.print(filename); spr.println(" found\n"); while(1) return;

  uint32_t    seekOffset;
  uint16_t    w, h, row, col, rgb, signature, depth, field;
  uint8_t     r, g, b;
  //uint32_t    startTime =     millis();

  //if (BMPread16(bmpFS) == 0x4D42)         // signature
  //{
  signature =       BMPread16(bmpFS);   // signature
  BMPread32(bmpFS);                     // skip File Size
  BMPread32(bmpFS);                     // skip Reserved
  seekOffset =      BMPread32(bmpFS);   // file Offset to Pixel Array
  BMPread32(bmpFS);                     // skip DIB header size
  w =               BMPread32(bmpFS);   // Image Width
  h =               BMPread32(bmpFS);   // Image Height
  BMPread16(bmpFS);                     // skip Plane
  depth =           BMPread16(bmpFS);   // Bit per Pixel
  field =           depth / 8;          // Byte per Pixel

    //if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    //{ // if Plane == 1, Bit per Pixels == 24, Compression == 0

  if (signature == 0x4D42 && ( (depth == 16) || (depth == 24) ) )
  //if ( (depth == 16) || (depth == 24) )
  {
    y +=                  h - 1;
    //tft.setSwapBytes(true);       // BMP starts with last row and ends with 1rst row
    spr.setSwapBytes(true);       // BMP starts with last row and ends with 1rst row
    bmpFS.seek(seekOffset);       // go to Pixel Array

    uint16_t  padding =   (4 - ((w * field) & 3)) & 3;  // each BMP row is n x 4 bytes
    uint8_t   lineBuffer[w * field];
    //uint8_t   lineBuffer[((w * field + 31) / 32) * 4]; // full row
    // row size    = ((w * field + 31) / 32) * 4;
    // pixel array = h * (((w * field + 31) / 32) * 4);

    for (row = 0; row < h; row++)
    {
      bmpFS.read(lineBuffer, sizeof(lineBuffer));
      uint8_t*  bptr =    lineBuffer;
      uint16_t* tptr =    (uint16_t*)lineBuffer;
      
      if (depth == 24)
      { // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b =       *bptr++;  g = *bptr++;  r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }
      }
      else
      { // Convert (555) to (565) 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          //*
          rgb =     *tptr;
          //*tptr++ = ((rgb & 0x7C00) << 1) | ((rgb & 0x03E0) << 1) | ((rgb & 0x0200) >> 4) | ((rgb & 0x001F));
          //*tptr++ = ((rgb & 0x7C00) << 1) | ((rgb & 0x03E0) << 1) | ((rgb & 0x0100) >> 3) | ((rgb & 0x001F));
          *tptr++ = ((rgb & 0x7C00) << 1) | ((rgb & 0x03E0) << 1) | ((rgb & 0x0080) >> 2) | ((rgb & 0x001F));
          //*tptr++ = ((rgb & 0x7C00) << 1) | ((rgb & 0x03E0) << 1) | ((rgb & 0x0040) >> 1) | ((rgb & 0x001F));
          //*tptr++ = ((rgb & 0x7C00) << 1) | ((rgb & 0x03E0) << 1) | ((rgb & 0x001F));
        }
      }
        
      if (padding)  bmpFS.read((uint8_t*)tptr, padding);  // Read any byte padding

      // Push the pixel line to screen, pushImage will crop the line if needed
      //tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      spr.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);

      //selectCSl(TFT_CS);
      //spr.pushSprite(0, 0);
      //selectCSl(SD_CS);
    }
    //tft.setSwapBytes(false);       // 
    spr.setSwapBytes(false);       //
    
    //Serial.print("Loaded in "); Serial.print(millis() - startTime); Serial.println(" ms");
    //spr.print("Loaded in "); spr.print(millis() - startTime); spr.println(" ms");
  }
  else
  {
    //Serial.println("BMP format not recognized");
    spr.println("BMP format not recognized\n");
  }
  
  bmpFS.close();
}

#endif      // BMP_UTILS_H
