import gab.opencv.*;
import processing.video.*;
import processing.serial.*;
import java.awt.*;

Capture video;
OpenCV opencv;
Serial port;                                           // declare Serial object

final String serialName = "COM5";                     // serial port name

void setup() {
  size(640, 480);                                      // display dimensions
  video = new Capture(this, 640/2, 480/2, Capture.list()[84]);
  opencv = new OpenCV(this, width/2, height/2);        
  opencv.loadCascade(OpenCV.CASCADE_FRONTALFACE);      //Import facial tracking with Harr Cascade Algorithm from OpenCV
  port = new Serial(this, serialName, 115200);         // create Serial port to communicate with Arduino
  video.start();
}

void draw() {
  scale(2);
  opencv.loadImage(video);

   port.setDTR(true);                                 //Forces Serial connection between Arduino and Processing to remain open

  image(video, 0, 0 );

  noFill();
  stroke(0, 255, 0);
  strokeWeight(3);
  Rectangle[] faces = opencv.detect();                  //Array of Faces
  println(faces.length);                               // print the number of faces within the capture

  for (int i = 0; i < faces.length; i++) {
    
    //Used to capture current frame and take a video
    if(port.available() > 0){
      int picture = port.read();
      if(picture == 1){
        saveFrame("photo-######.png");
        background(255,255,255);
      }
    }
    
    println(faces[i].x + "," + faces[i].y);            // print the (x,y) coordinate of face
    rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height);
    int x_val = (faces[i].x + faces[i].width/2);
    if(x_val >= 250){x_val = 250;}                    //buffer overflow issue
      
    byte[] coordinates = {(byte) x_val, (byte)(faces[i].y + faces[i].height/2)};
    port.write(coordinates);
  }
}

void captureEvent(Capture c) {
  c.read();
}
