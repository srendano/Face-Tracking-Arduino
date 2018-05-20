#include <Servo.h>
#include <ESP8266WiFi.h> //NEEDED FOR BLYNK
#include <BlynkSimpleEsp8266.h> //NEEDED FOR BLYNK
#include <Adafruit_SSD1306.h>
//Adafruit_SSD1306 lcd(-1); // create display object

Servo panServo;
Servo tiltServo;
int panPosition;
int tiltPosition;

int remoteTilt;
int remotePan;

bool toggle;

//char ssid[] = "OnePlus3";
char ssid[] = "MAKE";
//char pass[] = "testNetwork1"; //NEEDED FOR BLYNK
char pass[] = "cics290m";

//unique user token
const char auth[] = "6abac5a9a63a48a7baf1a269889212b2"; //NEEDED FOR BLYNK
int resetCounter = 0;
void setup() {
  panServo.attach(14);
  tiltServo.attach(13);
  
 /* lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C); // init LCD
  lcd.clearDisplay();
  lcd.setTextColor(WHITE);
  lcd.setTextSize(2);
  lcd.setCursor(0, 28);*/

  if (resetCounter == 0) {
     Serial.begin(115200);
  }
  resetCounter++;
  panServo.write(90);                    // initialize servo to 90 degrees
  panPosition = 90;
  delay(100);
  tiltServo.write(90);                  // initialize servo to 90 degrees
  tiltPosition = 90;
  
  remotePan = 90;
  remoteTilt = 90;
  toggle = false;

  Blynk.begin(auth, ssid, pass); //NEEDED FOR BLYNK
}

byte data[2];                       // coordinate data sent via Processing
int midCaptureX = 640/4;            // X coord of center of capture
int midCaptureY = 480/4;            // Y coord of center of capture
int errorMargin = 30;               // threshhold of error for center of capture
int stepper = 2;                    //stepper to increment camera position

void loop() {
  Blynk.run();
  
  //lcd.setCursor(0, 28);
  //lcd.clearDisplay();
  
  if(toggle == true){
    panServo.write(remotePan);
    delay(5);
    tiltServo.write(remoteTilt);
    delay(5);
  }
  else if (Serial.available()) {
      serialFlush();
      Serial.readBytes(data, 2);
      int midFaceX = data[0];  // x value of face coordinate
      int midFaceY = data[1];  // y value of face coordinate
  
      // lcd formatting for debugging purposes
      /*lcd.print("(");
      lcd.print(midFaceX);
      lcd.print(",");
      lcd.print(midFaceY);
      lcd.print(")");
      lcd.print(panPosition);
      lcd.display();*/
      
      // GOAL: keep the (x,y) coordinate within the center of capture
      if (midFaceX < (midCaptureX - errorMargin)){
        if (panPosition < 175) {
          panPosition += stepper;
          panServo.write(panPosition);
          delay(1);
        } else {
          panPosition -= stepper;
          panServo.write(panPosition);
          delay(1);
        }
      }
      else if (midFaceX > (midCaptureX + errorMargin)){
        if (panPosition > 5) {
          panPosition -= stepper;
          panServo.write(panPosition);
          delay(1);
        }
      }
      
      if (midFaceY < (midCaptureY - errorMargin)) {
        if (tiltPosition <175) {
          tiltPosition += stepper;
          tiltServo.write(tiltPosition);
          delay(1);
        }
      }
      else if (midFaceY > (midCaptureY + errorMargin)) {
        if (tiltPosition >5) {
          tiltPosition -= stepper;
          tiltServo.write(tiltPosition);
          delay(1);
        }
      }
    }
}

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
    delay(1);
  }
  data[0] = NULL;
  data[1] = NULL;
}

  //Take Picture
BLYNK_WRITE(V0){ // V0 is the number of Virtual Pin  
  int takePicture = param.asInt();
  if(takePicture == 1){
    Serial.write(takePicture);
  }
}

BLYNK_WRITE(V1) {
  remotePan = param.asInt(); // assign V1 value to pan
}
BLYNK_WRITE(V2) {
  remoteTilt = param.asInt(); // assign V2 value to tilt
}

BLYNK_WRITE(V4) {
  int positionReset = param.asInt();
  if(positionReset == 1){
    panServo.write(90);                    // initialize servo to 90 degrees
    panPosition = 90;
    delay(100);
    tiltServo.write(90);                  // initialize servo to 90 degrees
    tiltPosition = 90;
    
    remotePan = 90;
    remoteTilt = 90;
    //toggle = false;
  }
}

BLYNK_WRITE(V3) {
  int toggleButton = param.asInt(); 
  if(toggleButton == 1){ //maybe volatile
    toggle = true;
  }
  else{
    panServo.write(90);                    // initialize servo to 90 degrees
    panPosition = 90;
    delay(100);
    tiltServo.write(90);                  // initialize servo to 90 degrees
    tiltPosition = 90;
    
    remotePan = 90;
    remoteTilt = 90;
    toggle = false;
  }
}
