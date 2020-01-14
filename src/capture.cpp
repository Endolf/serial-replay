#include <Arduino.h>
#include <SdFat.h>

static const char buildInfoLogString[] = "Build data: %s, %s %s";

char logBuffer[256];
SdFat sd;
File file;
unsigned long lastFileSyncTime=0;

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, 14, 12);
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    sprintf(logBuffer, buildInfoLogString, PIO_GIT_REV, __DATE__, __TIME__);
    Serial.println(logBuffer);

    // Initialize the SD.
    if (!sd.begin(SS,SD_SCK_MHZ(10)))
    {
        sd.initErrorPrint();
    } else {
        if(!sd.exists("logs")) sd.mkdir("logs",true);

        if(sd.exists("logs/log.txt")) {
            int i=1;
            char nextLogfileCheckName[50] = "logs/log1.txt";
            while(sd.exists(nextLogfileCheckName)) {
                sprintf(nextLogfileCheckName,"logs/log%d.txt",++i);
            }
            Serial.print("Renaming logs/log.txt to ");
            Serial.println(nextLogfileCheckName);
            sd.rename("logs/log.txt",nextLogfileCheckName);
        }
        file = sd.open("logs/log.txt", FILE_WRITE);

        lastFileSyncTime = millis();
    }

}

void loop() {
    unsigned long currentTime = millis();
    if(currentTime - lastFileSyncTime>10000 && file.isOpen()) {
        file.flush();
        lastFileSyncTime = currentTime;
    }

    while (Serial1.available()) {
        String data = Serial1.readStringUntil('\n');
        Serial2.println(data);
        data.trim();
        Serial.print(millis());
        Serial.print(" S1 ");
        Serial.println(data);
        if(file.isOpen()) {
            file.print(millis());
            file.print(" S1 ");
            file.print(data);
            file.print("\n");
        }
    }

    while (Serial2.available()) {
        String data = Serial2.readStringUntil('\n');
        Serial1.println(data);
        data.trim();
        Serial.print(millis());
        Serial.print(" S2 ");
        Serial.println(data);
        if(file.isOpen()) {
            file.print(millis());
            file.print(" S2 ");
            file.print(data);
            file.print("\n");
        }
    }
}