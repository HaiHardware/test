#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//---------OLED-------------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//-------------------------------------------

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

//Clone HM10

#define SERVICE_UUID        "0000ffe0-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "0000ffe1-0000-1000-8000-00805f9b34fb"
//
//const int ledPin =  GPIO_NUM_2;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
//bool ledOn = false;  // whether the string is complete

//----------Define variables to get Date and Time-------
unsigned long current_time;
int stopMode = 0;
int countD = 01;
int countMo = 01;
int countY = 2000;
int countS;
int countM;
int countH;
String TimeD;
String TimeMo;
String TimeY;
String TimeH;
String TimeM;
String TimeS;
String TimeTotal;
String DateTotal;
//--------------------------------------------------------

class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class WriteCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
        {
         // Serial.print(rxValue[i]);
          char inChar = (char)rxValue[i];
          // add it to the inputString:
          inputString += inChar;
          // if the incoming character is a newline, set a flag so the main loop can
          // do something about it:
          if (inChar == '\n') {
            stringComplete = true;
          }
        }
        Serial.println();
        Serial.println("*********");
      }
    }
};

void blePrint(std::string value) {
  pCharacteristic->setValue(value);
  pCharacteristic->notify();
  delay(10); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
}

void setup() {
  //pinMode(ledPin, OUTPUT);
  //Serial.begin(115200);

//-------------------------------------
  current_time = millis();
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
//-----------------------------------------

  // Create the BLE Device
  BLEDevice::init("ESP32BLE_LMAO");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new WriteCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {

  if (stringComplete) {
    inputString.trim();
    Serial.print ("Received:");//print on serial monitor
    Serial.println(inputString);//print on serial monitor
    if(inputString == "StopPause")
    {
        stopMode++;
        blePrint("Stop Mode\r\n");
    }
    else
    {
        countD = ((int(inputString[0])-48)*10) + (int(inputString[1])-48);
        countMo = ((int(inputString[2])-48)*10) + (int(inputString[3])-48);
        countY = ((int(inputString[4])-48)*1000) + ((int(inputString[5])-48)*100) + ((int(inputString[6])-48)*10) + (int(inputString[7])-48); 
        countH = ((int(inputString[8])-48)*10) + (int(inputString[9])-48);  
        countM = ((int(inputString[10])-48)*10) + (int(inputString[11])-48);          
        countS = ((int(inputString[12])-48)*10) + (int(inputString[13])-48);    
    }


    // clear the string:
    inputString = "";
    stringComplete = false;
    //delay(20);//Time for Bluetooth stack handle    
  }    
//------------------------------------------------
//--------------Caculate Time--------------------
    if ( (unsigned long) (millis() - current_time) > 1000)
    {        
        if(stopMode == 0) {countS += 1;}
        if(stopMode == 2) 
        {
            stopMode = 0; 
        }

        if(countS > 59)
        {
            countS = 0;
            countM += 1;
        }
        if(countM >59)
        {
            countM = 0;
            countH += 1;
        }        
        if(countH > 23)
        {
            countH = 0;
            countD += 1;
        } 
        if(countD > 31)
        {
            countD = 1;
            countMo += 1;
        } 
        if(countMo > 12)
        {
            countMo = 1;
            countY += 1;
        }                              
        current_time = millis();
    }

    if(countH<10)
    {
      TimeH = "0"+String(countH);
    } else
      {
        TimeH = String(countH);
      }
    if(countM<10)
    {
      TimeM = "0"+String(countM);
    } else
      {
        TimeM = String(countM);
      }
    if(countS<10)
    {
      TimeS = "0"+String(countS);
    } else
      {
        TimeS = String(countS);
      }  
    if(countD<10)
    {
      TimeD = "0"+String(countD);
    } else
      {
        TimeD = String(countD);
      }
      
    if(countMo == 1)
    {
      TimeMo = "Jan";
    } 
    if(countMo == 2)
    {
      TimeMo = "Feb";
    }
    if(countMo == 3)
    {
      TimeMo = "Mar";
    }
    if(countMo == 4)
    {
      TimeMo = "Apr";
    }
    if(countMo == 5)
    {
      TimeMo = "May";
    }
    if(countMo == 6)
    {
      TimeMo = "Jun";
    }
    if(countMo == 7)
    {
      TimeMo = "Jul";
    }
    if(countMo == 8)
    {
      TimeMo = "Aug";
    }
    if(countMo == 9)
    {
      TimeMo = "Sep";
    }
    if(countMo == 10)
    {
      TimeMo = "Oct";
    }
    if(countMo == 11)
    {
      TimeMo = "Nov";
    }
    if(countMo == 12)
    {
      TimeMo = "Dec";
    }
      
    TimeY = String(countY);           
    DateTotal = TimeD + " - "  + TimeMo + " - " + TimeY;  //Ngay 00 Thang 00 Nam 0000  //25  
       
    TimeTotal = " " + TimeH + ":" + TimeM + ":" + TimeS;                     //00:00:00                   //8
//--------------------------------------------------------//    
     display.clearDisplay();
     display.setTextColor(WHITE);
     
     display.setTextSize(1);
     display.setCursor(21, 12);  
     display.println(DateTotal);
     
     display.println("");
     display.print(" ");
     
     display.setTextSize(2);
     display.println(TimeTotal);
     display.display();            
  //delay(20);
//--------------------------------------------------------


  

 
// ---------------disconnecting--------------------
//  if (!deviceConnected && oldDeviceConnected) {
//    delay(500); // give the bluetooth stack the chance to get things ready
//    pServer->startAdvertising(); // restart advertising
//    Serial.println("start advertising");
//    oldDeviceConnected = deviceConnected;
//  }
  // connecting
//  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
//    oldDeviceConnected = deviceConnected;
//  }
//------------------------------------
}