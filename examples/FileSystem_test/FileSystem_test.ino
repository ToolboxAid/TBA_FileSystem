#include "Arduino.h"
#include "TBA_FileSystem.h"
TBA_FileSystem fileSystem = TBA_FileSystem();

void setup()
{
    File dir = SD.open("/");
    fileSystem.debugDirectoryList(dir, 0);
    
}

void loop()
{
}
