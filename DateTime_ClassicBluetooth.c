#include "BluetoothSerial.h" //****Import necessary libraries**** 
#include "esp_bt_main.h"	 //****							 ****
#include "esp_bt_device.h"	 //**********************************

#include <Wire.h> 			 //Library to use I2C
#include <Adafruit_GFX.h>    //Write to OLED to display. Cre: https://github.com/adafruit/Adafruit_SSD1306 
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
//Declaration for SSD1306 display connected to I2C (SDA,SCL pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//int received;// received value will be stored in this variable
String receivedChar;// received value will be stored as CHAR in this variable

unsigned long current_time;
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


void setup() {
  current_time = millis();
  Serial.begin(115200);	//baud rate
  receivedChar = "";

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Starting address
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay(); //Reset OLED (erase all datas)

  //display.setCursor(0, 10); //Starting position: collumn 0 row 10

  
  SerialBT.begin("ESP32_DucNguyen"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!"); 
 
}



void loop() {
  //Starting to display on OLED whenever received datas from serial
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  
  if (SerialBT.available()) {
    receivedChar =(SerialBT.readString()); 

    Serial.print ("Received:");//print on serial monitor
    Serial.println(receivedChar);//print on serial monitor  
    
    countD = ((int(receivedChar[0])-48)*10) + (int(receivedChar[1])-48);
    countMo = ((int(receivedChar[2])-48)*10) + (int(receivedChar[3])-48);
    countY = ((int(receivedChar[4])-48)*1000) + ((int(receivedChar[5])-48)*100) + ((int(receivedChar[6])-48)*10) + (int(receivedChar[7])-48); 
    countH = ((int(receivedChar[8])-48)*10) + (int(receivedChar[9])-48);  
    countM = ((int(receivedChar[10])-48)*10) + (int(receivedChar[11])-48);          
    countS = ((int(receivedChar[12])-48)*10) + (int(receivedChar[13])-48);
  }

//--------------Caculate Time---------------------//
    if ( (unsigned long) (millis() - current_time) > 1000)
    {
        countS += 1;
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
     display.clearDisplay(); //erase OLED
     display.setTextColor(WHITE); //set text color to white and background to black
     
     display.setTextSize(1); 
     display.setCursor(21, 12);  //Starting position: collumn 21 row 12
     display.println(DateTotal); //Print Date
     
     display.println("");
     display.print(" ");
     
     display.setTextSize(2);
     display.println(TimeTotal); //Print Time 
     display.display();     //Display to OLED
  //delay(20);
}