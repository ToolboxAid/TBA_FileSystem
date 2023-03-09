/*
  FileSystem.h - Library for FileSystem operations.
  Created by Mr Q, December 2, 2022.
  Released into the public domain.
*/

#ifndef FileSystem_h
#define FileSystem_h

  // SD includes
  #include <FS.h>
  #include <SD.h>
  #include <SPIFFS.h>

//  #include "SupportFunctions.h"

//   #include "base64.hpp"
// //  #include "./Base64File.h"
  
  // Defines
  #define ThumbnailName "/thumbnail"
  #define ThumbnailNum "-7"
  #define GCD_ext ".gcode"
  #define ENC_ext ".encoded"
  #define PNG_ext ".png" 
  #define ThumbnailEncoded ThumbnailName ThumbnailNum ENC_ext
  #define ThumbnailPNG ThumbnailName ThumbnailNum PNG_ext
  //#define thumbnailStartIndicator "16x16"
  #define thumbnailStartIndicator "200x200"
  
  #define bufSize 255
  
  class TBA_FileSystem
  {
    public:
      TBA_FileSystem();
      void SD2SPIFFS(String fileName);
      void SPIFFS2SD(String fileName);
      void get_cardSizeUsed(File dir);
      void setupSD();
      void setupSD(boolean updateCardInfo);
      void setupSPIFFS();
 //     void memoryInfo();
      void extractGcodeImage(fs::FS &fs, const char * path);
      void extractPngImage(fs::FS &fs, const char * path);

      void debugDirectoryList(File dir, int numTabs);
      void listDirs();
      void removeFile(fs::FS &fs, const char * path);
      void readFile(fs::FS &fs, const char * path);

      File OpenWrite(fs::FS &fs, const char *path);

      void renameFile(fs::FS &fs, const char * path1, const char * path2);

      uint64_t get_cardSize();
      uint64_t get_cardSizeUsed();
      uint64_t get_cardSizeFree();

      void     udpateCardInfo();
      uint64_t cardSize;
      uint64_t cardSizeUsed;
      uint64_t cardSizeFree;

 //     extern SupportFunctions supportFunctions;
  
    private:
      File root;
//      SupportFunctions supportFunctions;
  };

#endif  // end FileSystem_h
