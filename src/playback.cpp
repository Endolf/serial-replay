#include <Arduino.h>
#include <SdFat.h>

static const char buildInfoLogString[] = "Build data: %s, %s %s";

char logBuffer[256];
SdFat sd;
File file;
unsigned long playbackTime;
unsigned long offset;
String nextDataItem;
bool doneOut = false;
bool doneReturn = false;
String port;

void processLine() {
    if(file.available()<=0) {
        if(!doneOut) {
            doneOut = true;
            file.close();
            file = sd.open("data/return.log", FILE_READ);
            Serial.println("Finished out leg, running return leg");
            offset+=millis();
        } else {
            doneReturn = true;
            file.close();
            Serial.println("Finished return leg");
        }
    }
    int nextFieldIndex = 0;
    String logLine = file.readStringUntil('\n');
    String timeAsString = logLine.substring(nextFieldIndex,logLine.indexOf(' '));
    nextFieldIndex += timeAsString.length()+1;
    playbackTime = timeAsString.toDouble();

    port = logLine.substring(nextFieldIndex, logLine.indexOf(' ',nextFieldIndex));
    nextFieldIndex += port.length()+1;

    nextDataItem = logLine.substring(nextFieldIndex);
}

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
        file = sd.open("data/out.log", FILE_READ);
        processLine();
    }
}

void loop() {
    if(!doneReturn && (millis() > (playbackTime+offset))) {
        Serial.print("Sending to port (");
        Serial.print(port);
        Serial.print(") ");
        if(port.equals("S1")) {
            Serial2.println(nextDataItem);
            Serial.print("Serial 2: ");
        } else if(port.equals("S2")) {
            Serial1.println(nextDataItem);
            Serial.print("Serial 1: ");
        } else {
            Serial.print("None: ");
        }
        Serial.println(nextDataItem);
        processLine();
    }
}