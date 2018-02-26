#include <SoftwareSerial.h>
SoftwareSerial mySerial(0, 1);
#include <Servo.h>
// Declare the Servo pin
int servopin = 10;
int analogInPin = A1, state;
Servo Servo1;
char c;
String Data;
int duration_main;
int distance_main1;
int sensorValue, c_pos_prev, c_pos_next;
int rpin = 5, gpin = 6, bpin = 11; //SET VALUES OF THESE PINS HERE!!
int trigpin_main = 9, echopin_main = 8;
int temppin = A0;
int bright = 1;
float red, green, blue;
int prev_state, next_state = 3;

void motor_pos_move(int n) //ROTATES MOTOR BY 18 DEGREES IN POSITIVE DIRECTION,N TIMES
{

  Servo1.write(0);
  delay(abs(n) * 1500);
  Servo1.write(90);

}

void motor_neg_move(int n) //ROTATES MOTOR BY 18 DEGREES IN NEGATIVE DIRECTION,N TIMES
{

  Servo1.write(180);
  delay(abs(n) * 1500);
  Servo1.write(90);

}

void setup()
{
  pinMode(rpin, OUTPUT);
  pinMode(bpin, OUTPUT);
  pinMode(bpin, OUTPUT);
  Servo1.attach(servopin);
  Serial.begin(9600);
  pinMode(temppin, INPUT);
  pinMode(trigpin_main, OUTPUT);
  pinMode(echopin_main, INPUT);
}


void loop()
{
  // if (mySerial.available()) {
  //Read on Bluetooth
  while (Serial.available())
  {
    delay(3);
    c = Serial.read();
    if (c == 'N' || c == 'F')
      Data = c;
    else {
      Data += c;
    }
  }
  //  Data = "N281,20,24,127,5";    //HARDCODED for test purposes

  /*DATA PROCESSING*/

  String stat = Data.substring(0, 1);     //Stores ON or OFF
  String wo_stat = Data.substring(1);     //Stores Data without ON and OFF
  int c_pos_prev = wo_stat.indexOf(",");  //Finds position of 1st comma
  red = wo_stat.substring(0, c_pos_prev).toInt();
  c_pos_next = wo_stat.indexOf(",", c_pos_prev + 1); //Finds position of next comma
  green = wo_stat.substring(c_pos_prev + 1, c_pos_next).toInt();
  c_pos_prev = c_pos_next;
  c_pos_next = wo_stat.indexOf(",", c_pos_prev + 1);
  blue = wo_stat.substring(c_pos_prev + 1, c_pos_next).toInt();
  c_pos_prev = c_pos_next;
  c_pos_next = wo_stat.indexOf(",", c_pos_prev + 1);
  bright = wo_stat.substring(c_pos_prev + 1, c_pos_next).toInt();
  c_pos_prev = c_pos_next;
  c_pos_next = wo_stat.indexOf(",", c_pos_prev + 1);
  next_state = abs( wo_stat.substring(c_pos_prev + 1, c_pos_next).toInt());  
  /*HAVE LEFT ANGLE CUZ PROBAB NOT POSSIBLE*/
  sensorValue = analogRead(analogInPin);

  // print the results to the serial monitor:
  //  Serial.print("sensor = " );
  Serial.println(sensorValue);
  if (sensorValue < 1010 && state == 0)
  {
    bright++;
    if (bright == 3)
      bright = 1;
    state = 1;
  }
  if (sensorValue > 1010)
    state = 0;

  //    int colourflag = 0;
  if (red == 300 && green == 300 && blue == 300) {
    int rawvoltage = analogRead(temppin); //temppin takes analog input of temperature
    float celsius = (((rawvoltage / 1024.0) * 500) - 273.15) / 10;
    red = (-1 * celsius + 50) * 5.1;
    green = (celsius + 25) * 8 / 5;
    blue = (celsius + 8) * 19 / 5;
    //      colourflag++;
  }

  digitalWrite(trigpin_main, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin_main, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin_main, LOW);
  float    duration_main = pulseIn(echopin_main, HIGH);  // Reads the echoPin, returns the sound wave travel time in microseconds
  int    distance_main = duration_main * 0.034 / 2;
  // Serial.print("main:");
  // Serial.println(distance_main);
  // prev_state = next_state;
  if (distance_main > 25)
    next_state = prev_state;
  else if (distance_main > 20)
    next_state = 5;
  else if (distance_main > 15)
    next_state = 4;
  else if (distance_main > 10)
    next_state = 3;
  else if (distance_main > 5)
    next_state = 2;
  else
    next_state = 1;
  //}
  // Serial.println(next_state);

  int diff = prev_state - next_state;
  if (diff > 0)
    motor_pos_move(diff);
  else
    motor_neg_move(diff);
  prev_state = next_state;
  Serial.print("Red: ");
  Serial.println(red);
  Serial.print("Green: ");
  Serial.println(green);
  Serial.print("Blue: ");
  Serial.println(blue);
  Serial.print("Bright: ");
  Serial.println(bright);
  Serial.print("State: ");
  Serial.println(next_state);
  Serial.print("Distance: ");
  Serial.println(distance_main);
  analogWrite(rpin, red * bright / 3);
  analogWrite(gpin, green * bright / 3);
  analogWrite(bpin, blue * bright / 3);
  delay(100);

}
