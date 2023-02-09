/*
  FileSystem.cpp - Library for FileSystem code.
  Created by Mr Q, December 2, 2022.
  Released into the public domain.

  https://github.com/espressif/arduino-esp32/issues/6372
  spi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
*/

#include "Arduino.h"
// #include "C:\Users\davidq\Documents\Arduino\libraries\TBA_FileSystem\src\TBA_FileSystem.h"
#include "TBA_FileSystem.h"

TBA_FileSystem::TBA_FileSystem()
{
  this->setupSD();
  this->setupSPIFFS();
}

void TBA_FileSystem::SD2SPIFFS(String fileName)
{
  File sourceFile = SD.open(fileName);
  File destFile = SPIFFS.open(fileName, FILE_WRITE);
  static uint8_t buf[bufSize];
  Serial.print("SD2SPIFFS -> copy progress: ");
  Serial.println(fileName);

  boolean showDebug = false;
  while (sourceFile.available() > 0)
  {
    int bytesAvailable = sourceFile.available();
    if (showDebug)
    {
      Serial.print("BytesAvailable A: ");
      Serial.print(bytesAvailable);
    }
    else
    {
      Serial.print(".");
    }

    if (bytesAvailable > bufSize)
    {
      bytesAvailable = bufSize;
    }

    sourceFile.read(buf, bytesAvailable);

    if (showDebug)
    {
      Serial.print(" -> BytesAvailable B: ");
      Serial.println(bytesAvailable);
    }
    destFile.write(buf, bytesAvailable);
  }

  sourceFile.close();
  destFile.close();
  Serial.println("");
  Serial.println("copy done");
}

void TBA_FileSystem::SPIFFS2SD(String fileName)
{
  File sourceFile = SPIFFS.open(fileName);
  File destFile = SD.open(fileName, FILE_WRITE);
  static uint8_t buf[bufSize];
  Serial.print("SPIFFS2SD -> copy progress: ");
  Serial.println(fileName);

  boolean showDebug = false;
  while (sourceFile.available() > 0)
  {
    int bytesAvailable = sourceFile.available();
    if (showDebug)
    {
      Serial.print("BytesAvailable A: ");
      Serial.print(bytesAvailable);
    }
    else
    {
      Serial.print(".");
    }

    if (bytesAvailable > bufSize)
    {
      bytesAvailable = bufSize;
    }

    sourceFile.read(buf, bytesAvailable);

    if (showDebug)
    {
      Serial.print(" -> BytesAvailable B: ");
      Serial.println(bytesAvailable);
    }
    destFile.write(buf, bytesAvailable);
  }

  sourceFile.close();
  destFile.close();
  Serial.println("");
  Serial.println("copy done");
}

uint64_t TBA_FileSystem::get_cardSize()
{
  return cardSize;
}
uint64_t TBA_FileSystem::get_cardSizeUsed()
{
  return cardSizeUsed;
}
uint64_t TBA_FileSystem::get_cardSizeFree()
{
  return cardSizeFree;
}

void TBA_FileSystem::get_cardSizeUsed(File dir)
{
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    { // no more files
      break;
    }
    if (strcmp(entry.name(), "SYSTEM~1") == 0)
    {
      continue;
    }
    if (entry.isDirectory())
    {
      get_cardSizeUsed(entry);
    }
    else
    {
    }

    cardSizeUsed += entry.size();

    entry.close();
  }
}

void TBA_FileSystem::udpateCardInfo()
{
  cardSize = (uint64_t)SD.cardSize();

  cardSizeUsed = 0;
  root = SD.open("/");
  get_cardSizeUsed(root);

  cardSizeFree = cardSize - cardSizeUsed;
}

void TBA_FileSystem::setupSD()
{
  if (!SD.begin())
  {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();

  String stgCardType = "";

  switch (cardType)
  {
  case CARD_NONE:
    stgCardType = "No SD card attached";
    return;
  case CARD_MMC:
    stgCardType = "MMC";
    break;
  case CARD_SD:
    stgCardType = "SD";
    break;
  case CARD_SDHC:
    stgCardType = "SDHC";
    break;
  default:
    Serial.println("Unknown");
  }

  Serial.printf("SD Card Type: %s\n", stgCardType);

  udpateCardInfo();

  //  String rtnVal = supportFunctions.humanReadableSize( cardSize );
  //  Serial.printf("SD Card Size: %s\n", rtnVal);
  //
  //  rtnVal = supportFunctions.humanReadableSize( cardSizeUsed );
  //  Serial.printf("SD Card Used: %s\n", rtnVal);
  //
  //  rtnVal = supportFunctions.humanReadableSize( cardSizeFree );
  //  Serial.printf("SD Card Free: %s\n", rtnVal);
}

void TBA_FileSystem::setupSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // check file system exists
  if (!SPIFFS.begin())
  {
    Serial.println("Formating SPIFFS file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  Serial.println("Type: SPDIFF");

  //  uint64_t tSize = (uint64_t)SPIFFS.totalBytes();
  //  String rtnVal = supportFunctions.humanReadableSize( tSize );
  //  Serial.printf("SPIFFS Size: %s\n", rtnVal);
  //
  //  uint64_t cardSizeUsed = (uint64_t)SPIFFS.usedBytes();
  //  rtnVal = supportFunctions.humanReadableSize( cardSizeUsed );
  //  Serial.printf("SPIFFS Used: %s\n", rtnVal);
  //
  //  rtnVal = supportFunctions.humanReadableSize( tSize - cardSizeUsed );
  //  Serial.printf("SPIFFS Free: %s\n", rtnVal);
}

void TBA_FileSystem::renameFile(fs::FS &fs, const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2))
  {
    Serial.println("File renamed");
  }
  else
  {
    Serial.println("Rename failed");
  }
}

void TBA_FileSystem::removeFile(fs::FS &fs, const char *path)
{
  if (fs.exists(path))
  {
    Serial.printf("Deleting file: %s\n", path);
    fs.remove(path);
  }
  else
  {
    Serial.printf("Deleting file (Not found): %s\n", path);
  }
}

void TBA_FileSystem::readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  int num = 0;
  //  char buffer[110];
  Serial.print("Read from file: ");
  while (file.available())
  {
    String line = file.readStringUntil('\n');
    num++;
    if (!line.startsWith(";"))
    {
      if (true)
      {
        int mod = num % 1000;

        if (mod == 1)
        {
          delay(100);
          Serial.print(mod);
          Serial.print(" _ ");
          Serial.print(num);
          Serial.print(" : ");
          Serial.println(line);
        }
      }
      else
      {
        char buffer[200];
        sprintf(buffer, "L: %i : %s", num, line);
        Serial.println(buffer);
      }
    }
    else
    {
      //        Serial.println(line);
    }
  }
  file.close();
  Serial.print("\n\n\n");
}

// ----------------------- Test only below this line

void TBA_FileSystem::debugDirectoryList(File dir, int numTabs)
{

  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    { // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++)
    {
      Serial.print('.  ');
    }
    if (entry.isDirectory())
    {
      Serial.print("/");
      Serial.println(entry.name());
      debugDirectoryList(entry, numTabs + 1);
    }
    else
    {
      // files have sizes, directories do not
      Serial.print(entry.size(), DEC);
      Serial.print("\t\t\t");
      Serial.println(entry.name());
    }
    entry.close();
  }
}
void TBA_FileSystem::listDirs()
{
  boolean debugMethod = true;
  if (debugMethod)
  {
    Serial.println("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    Serial.println("SPIFFS  SPIFFS  SPIFFS  SPIFFS  SPIFFS  SPIFFS  SPIFFS  SPIFFS");
    Serial.println("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");

    root = SPIFFS.open("/");
    debugDirectoryList(root, 0);

    Serial.println("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    Serial.println("SD      SD      SD      SD      SD      SD      SD      SD");
    Serial.println("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");

    root = SD.open("/");
    debugDirectoryList(root, 0);

    Serial.println("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    Serial.println("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    Serial.println("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");

    //    Serial.println("/gcode");
    //    cardSizeUsed = 0;
    //    root = SD.open("/gcode");
    //    get_cardSizeUsed(root);
    //    Serial.printf("SD used Size: %llu.%llu MB\n", cardSizeUsed/(1024*1024),cardSizeUsed%(1024*1024));
  }
}

// ----------------------- This needs to move to ????

void TBA_FileSystem::extractGcodeImage(fs::FS &fs, const char *path)
{
  boolean debugMethod = false;

  String pathIn = path;
  String pngExt = PNG_ext;
  int path_len = pathIn.length() + 1;
  int png_len = pngExt.length() + 1;
  char char_array[path_len + png_len];
  String png_file = (String)pathIn + (String)pngExt;
  png_file.toCharArray(char_array, path_len + png_len);
  png_file = (String)char_array;

  removeFile(fs, ThumbnailEncoded); // remove ENC file
  removeFile(fs, char_array);       // remove PNG file
                                    //  removeFile(fs, png_file);        // remove ERROR file

  if (debugMethod)
  {
    Serial.printf("extractGcodeImage file : %s\n", path);
    Serial.printf("ThumbnailEncoded file  : %s\n", ThumbnailEncoded);
    Serial.printf("ThumbnailPNG file      : %s\n", ThumbnailPNG);
    Serial.printf("char_array             : %s\n", char_array);
    Serial.printf("png_file file          : %s\n", png_file);
  }

  //  supportFunctions.memoryInfo();

  File gcodeFile = fs.open(path);
  if (!gcodeFile)
  {
    Serial.println("Failed to open gcodeFile for reading");
    return;
  }

  File encodedFile = SD.open(ThumbnailEncoded, FILE_WRITE);

  boolean thumbnailFound = false;
  int imageSize = -1;
  int num = 0;
  if (debugMethod)
  {
    Serial.print("looking for thumbnail: ");
  }

  while (gcodeFile.available())
  {
    num++;
    if (num > 1000)
    {
      break; // No Image in GCODE file before line 1000
    }

    String line = gcodeFile.readStringUntil('\n');
    if (line.startsWith(";"))
    {
      if (debugMethod)
      {
        Serial.print(num);
        Serial.print(" > ");
      }
      if (thumbnailFound)
      {
        int location = line.indexOf("thumbnail end");
        if (location > 0)
        {
          if (debugMethod)
          {
            Serial.print("end @ ");
            Serial.println(location);
          }
          break;
        }
        // write to Encoded file here
        encodedFile.print(line.substring(2, line.length()));
        if (debugMethod)
        {
          Serial.print(line.substring(2, line.length()));
          Serial.println("");
        }
      }
      else
      {
        if (line.indexOf(thumbnailStartIndicator) > 0)
        {
          if (debugMethod)
          {
            Serial.println(" found");
          }
          thumbnailFound = true;
          int ind = line.indexOf(thumbnailStartIndicator, 16);
          int len = line.length();
        }
      }
    }
  }

  if (debugMethod)
  {
    Serial.println("encodedFile: flush & close");
  }

  encodedFile.flush();
  encodedFile.close();

  if (debugMethod)
  {
    Serial.println("gcodeFile: close");
  }
  gcodeFile.close();

  //  supportFunctions.memoryInfo();

  //////////////////////////////////////////////////////////////////////////////////////////////////////
}
void TBA_FileSystem::extractPngImage(fs::FS &fs, const char *path)
{
  boolean debugMethod = true;

  String pathIn = path;
  String pngExt = PNG_ext;
  int path_len = pathIn.length() + 1;
  int png_len = pngExt.length() + 1;
  char char_array[path_len + png_len];
  String png_file = (String)pathIn + (String)pngExt;
  png_file.toCharArray(char_array, path_len + png_len);
  png_file = (String)char_array;

  if (debugMethod)
  {
    Serial.printf("extractGcodeImage file : %s\n", path);
    Serial.printf("ThumbnailEncoded file  : %s\n", ThumbnailEncoded);
    Serial.printf("ThumbnailPNG file      : %s\n", ThumbnailPNG);
    Serial.printf("char_array             : %s\n", char_array);
    Serial.printf("png_file file          : %s\n", png_file);
  }

  //  Base64File::decodeFile(SD, ThumbnailEncoded, ThumbnailPNG);

  listDirs();
  renameFile(SD, ThumbnailPNG, char_array);
  SD.remove(ThumbnailEncoded);
  listDirs();
}
/* gcode sample thumbnail

    ; thumbnail begin 16x16 964
    ; thumbnail begin 200x200 26568
    ; thumbnail end
    0000000000111111111122222222223
    0123456789012345678901234567890
*/

/*
// -------------------------------------------------------------------------------------
// https://randomnerdtutorials.com/esp32-microsd-card-arduino/
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
 Serial.printf("Listing directory: %s\n", dirname);

 File root = fs.open(dirname);
 if(!root){
   Serial.println("Failed to open directory");
   return;
 }
 if(!root.isDirectory()){
   Serial.println("Not a directory");
   return;
 }

 File file = root.openNextFile();
 while(file){
   if(file.isDirectory()){
     Serial.print("  DIR : ");
     Serial.println(file.name());
     if(levels){
       listDir(fs, file.name(), levels -1);
     }
   } else {
     Serial.print("  FILE: ");
     Serial.print(file.name());
     Serial.print("  SIZE: ");
     Serial.println(file.size());
   }
   file = root.openNextFile();
 }
}

void createDir(fs::FS &fs, const char * path){
 Serial.printf("Creating Dir: %s\n", path);
 if(fs.mkdir(path)){
   Serial.println("Dir created");
 } else {
   Serial.println("mkdir failed");
 }
}

void removeDir(fs::FS &fs, const char * path){
 Serial.printf("Removing Dir: %s\n", path);
 if(fs.rmdir(path)){
   Serial.println("Dir removed");
 } else {
   Serial.println("rmdir failed");
 }
}

void readFile(fs::FS &fs, const char * path){
 Serial.printf("Reading file: %s\n", path);

 File file = fs.open(path);
 if(!file){
   Serial.println("Failed to open file for reading");
   return;
 }

 Serial.print("Read from file: ");
 while(file.available()){
   Serial.write(file.read());
 }
 file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
 Serial.printf("Writing file: %s\n", path);

 File file = fs.open(path, FILE_WRITE);
 if(!file){
   Serial.println("Failed to open file for writing");
   return;
 }
 if(file.print(message)){
   Serial.println("File written");
 } else {
   Serial.println("Write failed");
 }
 file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
 Serial.printf("Appending to file: %s\n", path);

 File file = fs.open(path, FILE_APPEND);
 if(!file){
   Serial.println("Failed to open file for appending");
   return;
 }
 if(file.print(message)){
     Serial.println("Message appended");
 } else {
   Serial.println("Append failed");
 }
 file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
 Serial.printf("Renaming file %s to %s\n", path1, path2);
 if (fs.rename(path1, path2)) {
   Serial.println("File renamed");
 } else {
   Serial.println("Rename failed");
 }
}

void deleteFile(fs::FS &fs, const char * path){
 Serial.printf("Deleting file: %s\n", path);
 if(fs.remove(path)){
   Serial.println("File deleted");
 } else {
   Serial.println("Delete failed");
 }
}

void testFileIO(fs::FS &fs, const char * path){
 File file = fs.open(path);
 static uint8_t buf[512];
 size_t len = 0;
 uint32_t start = millis();
 uint32_t end = start;
 if(file){
   len = file.size();
   size_t flen = len;
   start = millis();
   while(len){
     size_t toRead = len;
     if(toRead > 512){
       toRead = 512;
     }
     file.read(buf, toRead);
     len -= toRead;
   }
   end = millis() - start;
   Serial.printf("%u bytes read for %u ms\n", flen, end);
   file.close();
 } else {
   Serial.println("Failed to open file for reading");
 }

 file = fs.open(path, FILE_WRITE);
 if(!file){
   Serial.println("Failed to open file for writing");
   return;
 }

 size_t i;
 start = millis();
 for(i=0; i<2048; i++){
   file.write(buf, 512);
 }
 end = millis() - start;
 Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
 file.close();
}


-------------------------------------------------------------------------------------


esp_err_t formatSD(){
   FRESULT res = FR_OK;
   esp_err_t err = ESP_OK;
   const size_t workbuf_size = 4096;
   void *workbuf = NULL;
   ESP_LOGW(TAG, "partitioning card");

   workbuf = ff_memalloc(workbuf_size);
   if (workbuf == NULL)
   {
       return ESP_ERR_NO_MEM;
   }

   DWORD plist[] = {100, 0, 0, 0};
   res = f_fdisk(pdrv, plist, workbuf);
   if (res != FR_OK)
   {
       err = ESP_FAIL;
       ESP_LOGE(TAG, "f_fdisk failed (%d)", res);
   }
   else
   {
       size_t alloc_unit_size = 512;
       ESP_LOGW(TAG, "formatting card, allocation unit size=%d", alloc_unit_size);
       res = f_mkfs("", FM_FAT, alloc_unit_size, workbuf, workbuf_size);
       if (res != FR_OK)
       {
           err = ESP_FAIL;
           ESP_LOGE(TAG, "f_mkfs failed (%d)", res);
       }
   }

   ESP_LOGW(TAG, "partitioning card finished");
   free(workbuf);
   return err;
}

 */
