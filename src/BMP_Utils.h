#ifndef   BMP_UTILS_H
#define   BMP_UTILS_H



//*
#define TFT_CS    18
#define SD_CS      3

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

  File bitmap;
  bitmap =   SD.open(filename, FILE_READ);    // Open requested file on SD card
  if (!bitmap)
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

  // bitmap header
  signature =       BMPread16(bitmap);  // Signature
  BMPread32(bitmap);                    // skip File Size
  BMPread32(bitmap);                    // skip Reserved
  seekOffset =      BMPread32(bitmap);  // File Offset to Pixel Array
  BMPread32(bitmap);                    // skip DIB header size
  w =               BMPread32(bitmap);  // Image Width
  h =               BMPread32(bitmap);  // Image Height
  BMPread16(bitmap);                    // skip Plane
  depth =           BMPread16(bitmap);  // Bit per Pixel
  field =           depth / 8;          // Byte per Pixel
  //read32(bitmap);                       // skip Compression

  if (signature == 0x4D42 && ( (depth == 16) || (depth == 24) ) )
  //if ( (depth == 16) || (depth == 24) )
  { // casual Windows bitmap of 16 or 24 bit/pixel
    y +=                  h - 1;
    //tft.setSwapBytes(true);       // bitmap starts with last row and ends with 1rst row
    spr.setSwapBytes(true);       // bitmap starts with last row and ends with 1rst row
    bitmap.seek(seekOffset);      // go to Pixel Array

    uint16_t  padding =   (4 - ((w * field) & 3)) & 3;  // each bitmap row is n x 4 bytes
    uint8_t   lineBuffer[w * field];
    //uint8_t   lineBuffer[((w * field + 31) / 32) * 4]; // full row
    //row size    =         ((w * field + 31) / 32) * 4;
    //pixel array =         h * (((w * field + 31) / 32) * 4);

    for (row = 0; row < h; row++)
    //row = ???
    // while (row--)
    {
      bitmap.read(lineBuffer, sizeof(lineBuffer));
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
          rgb =     *tptr;
          //*tptr++ = ((rgb & 0x7C00) << 1) | ((rgb & 0x03E0) << 1) | ((rgb & 0x0200) >> 4) | ((rgb & 0x001F));
          //*tptr++ = ((rgb & 0x7C00) << 1) | ((rgb & 0x03E0) << 1) | ((rgb & 0x0100) >> 3) | ((rgb & 0x001F));
          *tptr++ = ((rgb & 0x7C00) << 1) | ((rgb & 0x03E0) << 1) | ((rgb & 0x0080) >> 2) | ((rgb & 0x001F));
          //*tptr++ = ((rgb & 0x7C00) << 1) | ((rgb & 0x03E0) << 1) | ((rgb & 0x0040) >> 1) | ((rgb & 0x001F));
          //*tptr++ = ((rgb & 0x7C00) << 1) | ((rgb & 0x03E0) << 1) | ((rgb & 0x001F));
        }
      }
        
      if (padding)  bitmap.read((uint8_t*)tptr, padding); // Read any byte padding

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
  
  bitmap.close();
}

#endif      // BMP_UTILS_H
