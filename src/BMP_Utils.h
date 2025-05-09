#ifndef   BMP_UTILS_H
#define   BMP_UTILS_H

#define BMP_RECORD_TO     1   // 0:TFT, 1:Sprite, 2:SPIFF
#define BMP_SWAP_COLOR_BYTES  // uncomment to swap MSB<->LSB bytes for end result
//#define BMP_SHOW_TIME         // uncomment to print how long it took to decode


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
  //if ((x >= tft.width()) || (y >= tft.height())) return;  // 1st filter, may be muted comment

  File        bitmap;
  bitmap =    SD.open(filename, FILE_READ);   // Open requested file from SD card
  if (!bitmap)
  {
    //Serial.printf("File % not found\n", filename);
    #if   (BMP_RECORD_TO == 0)
    tft.printf("File %s not found\n", filename);
    #elif (BMP_RECORD_TO == 1)
    spr.printf("File %s not found\n", filename);
    #elif (BMP_RECORD_TO == 2)
    #endif // BMP_RECORD_TO
    return;
  }
  
  uint32_t    seekOffset;
  uint16_t    w, h, row, col, rgb, signature, depth, field;
  uint8_t     r, g, b;
  #ifdef BMP_SHOW_TIME
  uint32_t    startTime =   millis();
  #endif // BMP_SHOW_TIME

  // bitmap header
  signature =               BMPread16(bitmap);  // Signature
  BMPread32(bitmap);                    // skip File Size
  BMPread32(bitmap);                    // skip Reserved
  seekOffset =              BMPread32(bitmap);  // File Offset to Pixel Array
  BMPread32(bitmap);                    // skip DIB header size
  w =                       BMPread32(bitmap);  // Image Width
  h =                       BMPread32(bitmap);  // Image Height
  BMPread16(bitmap);                    // skip Plane
  depth =                   BMPread16(bitmap);  // Bit per Pixel
  field =                   depth / 8;          // Byte per Pixel
  //read32(bitmap);                       // skip Compression

  if (signature == 0x4D42 && ( (depth == 16) || (depth == 24) ) )
  { // legacy Windows bitmap and (16 or 24 bit/pixel), that's for us!!!
    y +=                    h - 1;  // bitmap starts with last line
    bitmap.seek(seekOffset);      // go to Pixel Array
    uint8_t   lineBuffer[w * field];
    uint16_t  padding =     (4 - ((w * field) & 3)) & 3;  // each bitmap row is n x 4 bytes
    // ((w * field + 31) / 32) * 4] // full row, // h * (((w * field + 31) / 32) * 4) // pixel array
    
    // Now separate 16 and 24 bit process to increase speed
    if (depth == 24)
    { // Convert 24 to 16 bit rgb(565) colours
      for (row = 0; row < h; row++)
      {
        bitmap.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr =    lineBuffer;
        uint16_t* tptr =    (uint16_t*)lineBuffer;

        for (uint16_t col = 0; col < w; col++)
        {
          b =       *bptr++;  g = *bptr++;  r = *bptr++;
          #ifndef BMP_SWAP_COLOR_BYTES  // MSB 1st
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
          #else                         // swap, LSB 1st
          *tptr++ = (r & 0xF8) | ((g & 0xE0) >> 5) | ((g & 0x1C) << 11) | ((b & 0xF8) << 5);
          #endif  // BMP_SWAP_COLOR_BYTES
        }
        // Push the pixel line to screen, pushImage will crop the line if needed
        #if   (BMP_RECORD_TO == 0)
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
        #elif (BMP_RECORD_TO == 1)
        spr.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
        #elif (BMP_RECORD_TO == 2)
        #endif // BMP_RECORD_TO
      
        if (padding)  bitmap.read((uint8_t*)lineBuffer, padding); // Read any byte padding to match next row
      }
    }
    else
    { // Convert 16 bit colours rgb(555) to (565)
      for (row = 0; row < h; row++)
      {
        bitmap.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr =    lineBuffer;
        uint16_t* tptr =    (uint16_t*)lineBuffer;

        for (uint16_t col = 0; col < w; col++)
        {
          rgb =     *tptr;
          #ifndef BMP_SWAP_COLOR_BYTES  // MSB 1st
          *tptr++ = ((rgb & 0x7FE0) << 1) | ((rgb & 0x0200) >> 4) | ((rgb & 0x001F)); // G: b1 copied to b6
          //*tptr++ = ((rgb & 0x7FE0) << 1) | ((rgb & 0x0100) >> 3) | ((rgb & 0x001F));
          //*tptr++ = ((rgb & 0x7FE0) << 1) | ((rgb & 0x0080) >> 2) | ((rgb & 0x001F));
          //*tptr++ = ((rgb & 0x7FE0) << 1) | ((rgb & 0x0040) >> 1) | ((rgb & 0x001F));
          //*tptr++ = ((rgb & 0x7FE0) << 1) | (rgb & 0x0020)        | ((rgb & 0x001F)); // G: b5 copied to b6
          #else                         // swap, LSB 1st
          *tptr++ = ((rgb & 0x7F80) >> 7) | ((rgb & 0x0060) << 9) | ((rgb & 0x0200) << 4) | ((rgb & 0x001F) << 8); // G: b1 copied to b6
          //*tptr++ = ((rgb & 0x7F80) >> 7) | ((rgb & 0x0060) << 9) | ((rgb & 0x0100) << 5) | ((rgb & 0x001F) << 8);
          //*tptr++ = ((rgb & 0x7F80) >> 7) | ((rgb & 0x0060) << 9) | ((rgb & 0x0080) << 6) | ((rgb & 0x001F) << 8);
          //*tptr++ = ((rgb & 0x7F80) >> 7) | ((rgb & 0x0060) << 9) | ((rgb & 0x0040) << 7) | ((rgb & 0x001F) << 8);
          //*tptr++ = ((rgb & 0x7F80) >> 7) | ((rgb & 0x0060) << 9) | ((rgb & 0x0020) << 8) | ((rgb & 0x001F) << 8); // G: b5 copied to b6
          #endif  // BMP_SWAP_COLOR_BYTES
        }
        // Push the pixel line to screen, pushImage will crop the line if needed
        #if   (BMP_RECORD_TO == 0)
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
        #elif (BMP_RECORD_TO == 1)
        spr.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
        #elif (BMP_RECORD_TO == 2)
        #endif // BMP_RECORD_TO
      
        if (padding)  bitmap.read((uint8_t*)lineBuffer, padding); // Read any byte padding to match next row
      }
    }
    
    #ifdef BMP_SHOW_TIME
    //Serial.printf("%s loaded in %d ms\n", filename, millis() - startTime);
    #if   (BMP_RECORD_TO == 0)
    tft.printf("%s loaded in %u ms\n", filename, millis() - startTime);
    #elif (BMP_RECORD_TO == 1)
    spr.printf("%s loaded in %u ms\n", filename, millis() - startTime);
    #elif (BMP_RECORD_TO == 2)
    #endif // BMP_RECORD_TO
    #endif // BMP_SHOW_TIME
  }
  else
  { // not a windows bitmap, not a 16 or 24 bit color 
    //Serial.println("BMP format not recognized");
    #if   (BMP_RECORD_TO == 0)
    tft.println("BMP format not recognized\n");
    #elif (BMP_RECORD_TO == 1)
    spr.println("BMP format not recognized\n");
    #elif (BMP_RECORD_TO == 2)
    #endif // BMP_RECORD_TO
  }
  
  bitmap.close();
}

#endif      // BMP_UTILS_H
