#include "Arduino.h"
#include "TBA_FileSystem.h"
TBA_FileSystem fileSystem = TBA_FileSystem();

void setup()
{
    // Use serial port
    Serial.begin(115200);
    while (!Serial && (millis() <= 1000))
    {
    };

    Serial.println();
    Serial.println("Root directory listing");
    File dir = SD.open("/");
    fileSystem.debugDirectoryList(dir, 0);
    dir.close();

    Serial.println();
    Serial.println("/ROOT directory listing");
    dir = SD.open("/ROOT");
    fileSystem.debugDirectoryList(dir, 0);
    dir.close();

    Serial.print("Done");
}

void loop()
{
    delay(1000);
    Serial.print(".");
}
