/* Board: Arduino Uno
   IDE: Arduino 1.8.13

   programmer(s) name: Miguel Cano
   date when code is created/modified: 08/05/2021

   The purpose of this code is to simulate a security camera by using the following electrical components: camera module(OV2640), passive buzzer, ultrasonic sensor, servo motor, potentiometer, and bluethooth module(SH-H3).

   Important Notes before using the code:

   Bluethooth Application used "ArduinoBlue". It's only avaliable for IOS and Android devices.
   Include "memorysaver.h" file to avoid getting compile errors.

*/

#include <Wire.h> // This library allows you to communicate with I2C / TWI devices.
#include <ArduCAM.h> // Camera Module(OV2640)
#include <SPI.h> // Defines a synchronous serial communication
#include "memorysaver.h" // Library needed to define which camera module the user needs.
#include <Servo.h> // Servo header
#include <SoftwareSerial.h> // Bluetooth header
#include <string.h> // defines one variable type, one macro, and various functions for manipulating arrays of characters.

#define BMPIMAGEOFFSET 66 // Camera Module settings.

#define BLUETOOTH_RX 8 // Bluetooth pin assignment

#define BLUETOOTH_TX 9 // Bluetooth pin assignment

#define buzzer 6 // Define the PIN assignment for the Buzzer
#define POT A0 // Define potentiometer as a analog input.

// Global Variables
Servo myservo;
SoftwareSerial btSerial(BLUETOOTH_TX, BLUETOOTH_RX);

// Camera Module global variables settings.
const char bmp_header[BMPIMAGEOFFSET] PROGMEM =
{
  0x42, 0x4D, 0x36, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x28, 0x00,
  0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x1F, 0x00,
  0x00, 0x00
};

// Camera Module Variables
const int CS = 7;
bool is_header = false;
int mode  = 0;
uint8_t start_capture = 0;
ArduCAM myCAM(OV2640, CS); // Defines the camera that is being used.
uint8_t read_fifo_burst(ArduCAM myCAM); // unsigned integer of length 8 bits to remove unused file sizes glitch.

int pos = 0; // Initialization
const int trigPin = 4; // Sensor
const int echoPin = 3; // Sensor
const int motor = 5; // Servo motor
double distanceCm, DistanceSec, duration; // Variables to calculate distance for the Ultra Sensor
unsigned long BAUD_RATE = 115200; // Setting the amount of bits for the bluetooth module

void setup() {

  uint8_t vid, pid;
  uint8_t temp;
#if defined(__SAM3X8E__)
  Wirel.begin();
  Serial.begin(115200);
#else
  Wire.begin();
  Serial.begin(115200);
#endif
  Serial.println(F("ArduCAM Start! END"));
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  SPI.begin();
  //Reset the CPLD
  myCAM.write_reg(0x07, 0x80);
  delay(100);
  myCAM.write_reg(0x07, 0x00);
  delay(100);
  while (1) {
    //Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55) {
      Serial.println(F("SPI interface Error! END"));
      delay(1000); continue;
    } else {
      Serial.println(F("SPI interface OK. END")); break;
    }
  }
  // Initialize the bluetooth
  Serial.begin(BAUD_RATE);
  btSerial.begin(BAUD_RATE);
  btSerial.print("AT\r\n"); delay(100);
  btSerial.print("AT+BDNAME\r\n"); delay(100);
  btSerial.print("AT+BDADDR\r\n"); delay(100);
  btSerial.print("AT+BDBAUD\r\n"); delay(100);
  btSerial.print("AT+BDPIN\r\n");

  myservo.attach(motor); // attach the Servo motor to pin number 5
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);
  DistanceSec = 20; // Distance recognitizion time

  // check if the camera is being setup correctly.
#if defined (OV2640_MINI_2MP)
  while (1) {
    //Check if the camera module type is OV2640
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))) {
      Serial.println(F("Can't find OV2640 module! END"));
      delay(1000); continue;
    }
    else {
      Serial.println(F("OV2640 detected. END")); break;
    }
  }
#endif
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
#if defined (OV2640_MINI_2MP)
  myCAM.OV2640_set_JPEG_size(OV2640_320x240);
#else
  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
  myCAM.OV5642_set_JPEG_size(OV5642_320x240);
#endif
  delay(1000);
  myCAM.clear_fifo_flag();
#if !(defined (OV2640_MINI_2MP))
  myCAM.write_reg(ARDUCHIP_FRAMES, 0x00);
#endif
}

String cmd = " ";

void loop() {

  uint8_t temp = 0xff, temp_last = 0;
  bool is_header = false;

  // Manage the sound of the passive buzzer.
  int knobValue = analogRead(POT);
  byte pwn = map(knobValue, 0, 1024, 0, 0);
  Serial.println(knobValue);
  analogWrite(6, pwn);

  while (btSerial.available()) { // While statement to make the bluetooth keeps looking for a device to be connected.
    cmd = btSerial.readString();
    Serial.println(cmd);
  }
  while (Serial.available()) {
    btSerial.print(Serial.read());
  }

  if (cmd.length() == 3 && cmd.substring(1, 2) == "\1") { // Turn ON by pressing 1
    Serial.println("Clockwise");
    for (pos = 0; pos <= 180; pos += 1) { // go from 0 to 180 degrees
      // in steps of 1 degree
      myservo.write(pos); // Program the Servo to go to the position (pos)
      digitalWrite(trigPin, LOW); // send a pulse of 2 microseconds
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH); // send a pulse of 10 microseconds
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      duration = pulseIn(echoPin, HIGH);
      distanceCm = duration * 0.034 / 2;
      if (distanceCm <= DistanceSec) // Detection statements
      {

        if (distanceCm <= DistanceSec / 2) // Detection statements near objects
        {
          if (Serial.available())
          {
            temp = Serial.read();

            switch (temp)
            {
              case 0:
                myCAM.OV2640_set_JPEG_size(OV2640_160x120); delay(1000); // Camera Resolution 160x120 setup
                Serial.println(F("Switch to OV2640_160x120 END"));
                temp = 0xff;
                break;
              case 1:
                myCAM.OV2640_set_JPEG_size(OV2640_176x144); delay(1000); // Camera Resolution 176x144 setup
                Serial.println(F("Switch to OV2640_176x144 END"));
                temp = 0xff;
                break;
              case 2:
                myCAM.OV2640_set_JPEG_size(OV2640_320x240); delay(1000); // Camera Resolution 320x240 setup
                Serial.println(F("Switch to OV2640_320x240 END"));
                temp = 0xff;
                break;
              case 3:
                myCAM.OV2640_set_JPEG_size(OV2640_352x288); delay(1000); // Camera Resolution 352x288 setup
                Serial.println(F("Switch to OV2640_352x288 END"));
                temp = 0xff;
                break;
              case 4:
                myCAM.OV2640_set_JPEG_size(OV2640_640x480); delay(1000); // Camera Resolution 640x480 setup
                Serial.println(F("Switch to OV2640_640x480 END"));
                temp = 0xff;
                break;
              case 5:
                myCAM.OV2640_set_JPEG_size(OV2640_800x600); delay(1000); // Camera Resolution 800x600 setup
                Serial.println(F("Switch to OV2640_800x600 END"));
                temp = 0xff;
                break;
              case 6:
                myCAM.OV2640_set_JPEG_size(OV2640_1024x768); delay(1000); // Camera Resolution 1024x768 setup
                Serial.println(F("Switch to OV2640_1024x768 END"));
                temp = 0xff;
                break;
              case 7:
                myCAM.OV2640_set_JPEG_size(OV2640_1280x1024); delay(1000); // Camera Resolution 1280x1024 setup
                Serial.println(F("Switch to OV2640_1280x1024 END"));
                temp = 0xff;
                break;
              case 8:
                myCAM.OV2640_set_JPEG_size(OV2640_1600x1200); delay(1000); // Camera Resolution 1600x1200 setup
                Serial.println(F("Switch to OV2640_1600x1200 END"));
                temp = 0xff;
                break;

              case 0x10:
                mode = 1;
                temp = 0xff;
                start_capture = 1;
                btSerial.println("CAM start single shoot.");
                break;
              case 0x11:
                temp = 0xff;
                myCAM.set_format(JPEG);
                myCAM.InitCAM();
                break;
              case 0x30:
                mode = 2;
                temp = 0xff;
                start_capture = 2;
                btSerial.println("CAM start single shoot.");
                break;
              case 0x31:
                myCAM.set_format(BMP);
                myCAM.InitCAM();

#if !(defined (OV2640_MINI_2MP))
                myCAM.clear_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
#endif
                myCAM.wrSensorReg16_8(0x3818, 0x81);
                myCAM.wrSensorReg16_8(0x3621, 0xA7);
                break;

              // Special effects are addons. However make sure to include at least one. Recommendeded "Auto"
              case 0x40:
                myCAM.OV2640_set_Light_Mode(Auto); temp = 0xff; // Light Effects "Auto"
                Serial.println(F("ACK CMD Set to Auto END"));
                break;
              case 0x41:
                myCAM.OV2640_set_Light_Mode(Sunny); temp = 0xff; // Light Effects "Sunny"
                Serial.println(F("ACK CMD Set to Sunny END")); break;
              case 0x42:
                myCAM.OV2640_set_Light_Mode(Cloudy); temp = 0xff; // Light Effects "Cloudy"
                Serial.println(F("ACK CMD Set to Cloudy END")); break;
              case 0x43:
                myCAM.OV2640_set_Light_Mode(Office); temp = 0xff; // Light Effects "Office"
                Serial.println(F("ACK CMD Set to Office END")); break;
              case 0x44:
                myCAM.OV2640_set_Light_Mode(Home);   temp = 0xff; // Light Effects "Home"
                Serial.println(F("ACK CMD Set to Home END")); break;
              case 0x50:
                myCAM.OV2640_set_Color_Saturation(Saturation2); temp = 0xff; // Light Effects "Saturation"
                Serial.println(F("ACK CMD Set to Saturation+2 END")); break;
              case 0x51:
                myCAM.OV2640_set_Color_Saturation(Saturation1); temp = 0xff; // Light Effects "Saturation"
                Serial.println(F("ACK CMD Set to Saturation+1 END")); break;
              case 0x52:
                myCAM.OV2640_set_Color_Saturation(Saturation0); temp = 0xff; // Light Effects "Saturation"
                Serial.println(F("ACK CMD Set to Saturation+0 END")); break;
              case 0x53:
                myCAM. OV2640_set_Color_Saturation(Saturation_1); temp = 0xff; // Light Effects "Saturation"
                Serial.println(F("ACK CMD Set to Saturation-1 END")); break;
              case 0x54:
                myCAM.OV2640_set_Color_Saturation(Saturation_2); temp = 0xff; // Light Effects "Saturation"
                Serial.println(F("ACK CMD Set to Saturation-2 END")); break;
              case 0x60:
                myCAM.OV2640_set_Brightness(Brightness2); temp = 0xff; // Light Effects "Brightness"
                Serial.println(F("ACK CMD Set to Brightness+2 END")); break;
              case 0x61:
                myCAM.OV2640_set_Brightness(Brightness1); temp = 0xff; // Light Effects "Brightness"
                Serial.println(F("ACK CMD Set to Brightness+1 END")); break;
              case 0x62:
                myCAM.OV2640_set_Brightness(Brightness0); temp = 0xff; // Light Effects "Brightness"
                Serial.println(F("ACK CMD Set to Brightness+0 END")); break;
              case 0x63:
                myCAM. OV2640_set_Brightness(Brightness_1); temp = 0xff; // Light Effects "Brightness"
                Serial.println(F("ACK CMD Set to Brightness-1 END")); break;
              case 0x64:
                myCAM.OV2640_set_Brightness(Brightness_2); temp = 0xff; // Light Effects "Brightness"
                Serial.println(F("ACK CMD Set to Brightness-2 END")); break;
              case 0x70:
                myCAM.OV2640_set_Contrast(Contrast2); temp = 0xff; // Light Effects "Contrast"
                Serial.println(F("ACK CMD Set to Contrast+2 END")); break;
              case 0x71:
                myCAM.OV2640_set_Contrast(Contrast1); temp = 0xff; // Light Effects "Contrast"
                Serial.println(F("ACK CMD Set to Contrast+1 END")); break;
              case 0x72:
                myCAM.OV2640_set_Contrast(Contrast0); temp = 0xff; // Light Effects "Contrast"
                Serial.println(F("ACK CMD Set to Contrast+0 END")); break;
              case 0x73:
                myCAM.OV2640_set_Contrast(Contrast_1); temp = 0xff; // Light Effects "Contrast"
                Serial.println(F("ACK CMD Set to Contrast-1 END")); break;
              case 0x74:
                myCAM.OV2640_set_Contrast(Contrast_2); temp = 0xff; // Light Effects "Contrast"
                Serial.println(F("ACK CMD Set to Contrast-2 END")); break;
              case 0x80:
                myCAM.OV2640_set_Special_effects(Antique); temp = 0xff; // Special Effects "Antique"
                Serial.println(F("ACK CMD Set to Antique END")); break;
              case 0x81:
                myCAM.OV2640_set_Special_effects(Bluish); temp = 0xff; // Special Effects "Bluish"
                Serial.println(F("ACK CMD Set to Bluish END")); break;
              case 0x82:
                myCAM.OV2640_set_Special_effects(Greenish); temp = 0xff; // Special Effects "Greenish"
                Serial.println(F("ACK CMD Set to Greenish END")); break;
              case 0x83:
                myCAM.OV2640_set_Special_effects(Reddish); temp = 0xff; // Special Effects "Reddish"
                Serial.println(F("ACK CMD Set to Reddish END")); break;
              case 0x84:
                myCAM.OV2640_set_Special_effects(BW); temp = 0xff; // Special Effects "BW"
                Serial.println(F("ACK CMD Set to BW END")); break;
              case 0x85:
                myCAM.OV2640_set_Special_effects(Negative); temp = 0xff; // Special Effects "Negative"
                Serial.println(F("ACK CMD Set to Negative END")); break;
              case 0x86:
                myCAM.OV2640_set_Special_effects(BWnegative); temp = 0xff; // Special Effects "BWnegative"
                Serial.println(F("ACK CMD Set to BWnegative END")); break;
              case 0x87:
                myCAM.OV2640_set_Special_effects(Normal); temp = 0xff; // Special Effects "Normal"
                Serial.println(F("ACK CMD Set to Normal END")); break;
            }
          }
          if (mode == 1) // Mode 1 to begin using the capture mode
          {
            if (start_capture == 1)
            {
              myCAM.flush_fifo();
              myCAM.clear_fifo_flag();
              //Start capture
              myCAM.start_capture();
              start_capture = 0;
            }
            if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
            {
              Serial.println(F("CAM Capture Done. END")); delay(50);
              read_fifo_burst(myCAM);
              //Clear the capture done flag
              myCAM.clear_fifo_flag();
            }
          }

          else if (mode == 2) // Resets every time a change has been made.
          {
            if (start_capture == 2)
            {
              //Flush the FIFO
              myCAM.flush_fifo();
              myCAM.clear_fifo_flag();
              //Start capture
              myCAM.start_capture();
              start_capture = 0;
            }
            if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
            {
              Serial.println(F("ACK CMD CAM Capture Done. END")); delay(50);
              uint8_t temp, temp_last;
              uint32_t length = 0;
              length = myCAM.read_fifo_length();
              if (length >= MAX_FIFO_SIZE )
              {
                Serial.println(F("ACK CMD Over size. END"));
                myCAM.clear_fifo_flag();
                return;
              }
              if (length == 0 ) //0 kb
              {
                Serial.println(F("ACK CMD Size is 0. END"));
                myCAM.clear_fifo_flag();
                return;
              }
              myCAM.CS_LOW();
              myCAM.set_fifo_burst();//Set fifo burst mode

              Serial.write(0xFF);
              Serial.write(0xAA);
              for (temp = 0; temp < BMPIMAGEOFFSET; temp++)
              {
                Serial.write(pgm_read_byte(&bmp_header[temp]));
              }

              // SPI.transfer(0x00);
              char VH, VL;
              int i = 0, j = 0;
              for (i = 0; i < 240; i++)
              {
                for (j = 0; j < 320; j++)
                {
                  VH = SPI.transfer(0x00);;
                  VL = SPI.transfer(0x00);;
                  Serial.write(VL);
                  delayMicroseconds(12);
                  Serial.write(VH);
                  delayMicroseconds(12);
                }
              }
              Serial.write(0xBB);
              Serial.write(0xCC);
              myCAM.CS_HIGH();
              //Clear the capture done flag
              myCAM.clear_fifo_flag();
            }
          }

          if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
          {
            btSerial.println("CAM Capture Done!");
            read_fifo_burst(myCAM);
            //Clear the capture done flag
            myCAM.clear_fifo_flag();
          }

          // Manage the sound of the passive buzzer.
          int knobValue = analogRead(POT);
          byte pwn = map(knobValue, 0, 1024, 0, 125);
          Serial.println(knobValue);
          analogWrite(6, pwn);
          Serial.println(distanceCm);
          Serial.println(pos);
          delay(2000);
        }

        else
        {
          // Manage the sound of the passive buzzer.
          int knobValue = analogRead(POT);
          byte pwn = map(knobValue, 0, 1024, 0, 125);
          Serial.println(knobValue);
          analogWrite(6, pwn);
          delay(100);
          Serial.println(distanceCm);
          Serial.println(pos);
          delay(2000);
        }
      }
      else {
        digitalWrite(buzzer, LOW);

        delay(10);
        delay(80);

      }
    }
  }

  if (cmd.length() == 3 && cmd.substring(1, 2) == "\2") { // Turn OFF by pressing 2
    Serial.println("Counterclockwise");
    for (pos = 180; pos >= 0; pos = 0) {
      myservo.write(pos); //
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, LOW);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      duration = pulseIn(echoPin, LOW);
      distanceCm = duration * 0.034 / 2;

      if (distanceCm <= DistanceSec) {

        if (distanceCm <= DistanceSec / 2) {
          digitalWrite(buzzer, LOW);
          Serial.println(distanceCm);
          Serial.println(pos);

        }
        else
        {
          digitalWrite(buzzer, LOW);
          delay(100);
          Serial.println(distanceCm);
          Serial.println(pos);
        }
      }

      else
      {
        digitalWrite(buzzer, LOW);
      }
      delay(80);
    }
  }
}

uint8_t read_fifo_burst(ArduCAM myCAM) // Useful to avoid the glitch of Unused files.
{
  uint8_t temp, temp_last;
  uint32_t length = 0;
  length = myCAM.read_fifo_length();
  if (length >= MAX_FIFO_SIZE ) // 384kb
  {
    btSerial.println("Over size.");
    return;
  }
  if (length == 0 ) //0 kb
  {
    btSerial.println("Size is 0.");
    return;
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();//Set fifo burst mode
  temp = SPI.transfer(0x00);
  length--;
  while ( length-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    if (is_header == true)
    {
      Serial.write(temp);
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      Serial.println(F("IMG END"));
      Serial.write(temp_last);
      Serial.write(temp);
    }
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end , break while,
      break;
    delayMicroseconds(15);
  }
  myCAM.CS_HIGH();
  is_header = false;
  return 1;
}
