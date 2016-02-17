/* 
  Arduino Skecth reads analog input from two Lilypad accelerometers and two EMG sensors, using TinyLily Mini microcontroller (Arduino Compatible). - Sara
 */

// Set serial to 4800 BaudRate to being 
void setup() {
  
  delay(100); // Wait for sensors to get ready

  Serial.begin(4800);  
  
}

// Main loop body
void loop() {
    
    // Acceleration data from Lilypad accelerometer 1 and 2
    float accX1= analogRead(0);
    float accY1= analogRead(1);
    float accZ1= analogRead(2);
    
    float accX2= analogRead(3);
    float accY2= analogRead(4);
    float accZ2= analogRead(5);
    
    // Analog voltage from EMG 1 and 2
    float EMG1= analogRead(6);
    float EMG2 = analogRead(7);
    
    
// Calibrate coordinates of accelerometer > when sensors are getting 3 volts
// Sensor 1 MAX and MIN
float maxX1 = 422; float minX1 = 290;
float maxY1 = 428; float minY1 = 290;
float maxZ1 = 434; float minZ1 = 296;

// Sensor 2 MAX and MIN
float maxX2 = 422; float minX2 = 290;
float maxY2 = 424; float minY2 = 292;
float maxZ2 = 436; float minZ2 = 300;

// Calibration
// X1
float magX1 = 2/(maxX1 - minX1);
float cX1 = (magX1 * maxX1) - 1;
float gX1 = accX1* magX1 - cX1; 

// Y1
float magY1 = 2/(maxY1 - minY1);
float cY1 = (magY1 * maxY1) - 1;
float gY1 = accY1* magY1 - cY1; 

// Z1
float magZ1 = 2/(maxZ1 - minZ1);
float cZ1 = (magZ1 * maxZ1) - 1;
float gZ1 = accZ1* magZ1 - cZ1; 

// X2
float magX2 = 2/(maxX2 - minX2);
float cX2 = (magX2 * maxX2) - 1;
float gX2 = accX2* magX2 - cX2; 

// Y2
float magY2 = 2/(maxY2 - minY2);
float cY2 = (magY2 * maxY2) - 1;
float gY2 = accY2* magY2 - cY2;

// Z2
float magZ2 = 2/(maxZ2 - minZ2);
float cZ2 = (magZ2 * maxZ2) - 1;
float gZ2 = accZ2* magZ2 - cZ2;

// Print to serial
Serial.print(gX1); Serial.print(", ");
Serial.print(gY1); Serial.print(", ");
Serial.print(gZ1); Serial.print(", ");

Serial.print(gX2); Serial.print(", ");
Serial.print(gY2); Serial.print(", ");
Serial.print(gZ2); Serial.print(", ");
    
Serial.print(EMG1); Serial.print(", ");
Serial.print(EMG2); Serial.println(", ");    
}
