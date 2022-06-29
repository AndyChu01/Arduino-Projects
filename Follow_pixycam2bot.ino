#include <SPI.h>
#include <Pixy2.h>
Pixy2 pixy;
/////////////////

//ENA IN1 IN2 IN3 IN4 ENB
int myPins[6] = {5, 6, 7, 8, 9, 10};
float deadZone = 0.15;
int baseSpeed = 70;
/////////////////

int cont = 0;
int signature, x, y, width, height;
float cx, cy, area;

void setup() 
{
  // put your setup code here, to run once:
    //Serial Port begin
    Serial.begin (115200);
    Serial.print("Starting...\n");
    pixy.init();

for (int i = 0; i < 6; i++)
  {
   pinMode (myPins[i], OUTPUT);
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  float turn = pixyCheck();
  if (turn > -deadZone && turn < deadZone)
  {
    turn = 0;
  }
  if (turn < 0)
  {
  moveRobot(-25,50);
  }
  else if (turn > 0)
  {
  moveRobot(50,-25);
  }
  else
  {
  moveRobot(25,25);
  }
  delay(1);
}

float pixyCheck()
{
  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32];
  // grab blocks
  if (blocks)
  {
    signature = pixy.ccc.blocks[0].m_signature;
    height = pixy.ccc.blocks[0].m_height;
    width = pixy.ccc.blocks[0].m_width;
    x = pixy.ccc.blocks[0].m_x;
    y = pixy.ccc.blocks[0].m_y;
    cx = (x + (width / 2));
    cy = (y + (height / 2));
    cx = mapfloat(cx, 0, 320, -1, 1);
    cy = mapfloat(cy, 0, 200, 1, -1);
    area = width * height;
    // Serial.print("sig: ");
    // Serial.print(signature);
    // Serial.print("x: ");
    // Serial.print(x );
    // Serial.print("y: ");
    // Serial.print(y );
    // Serial.print("width: ");
    // Serial.print(width);
    // Serial.print("height: ");
    // Serial.print(height);
    // Serial.print("cx: ");
    // Serial.print(cx);
    // Serial.print("cy: ");
    // Serial.print(cy);

  }
  else 
  {
   cont += 1;
   if (cont == 100)
   {
    cont = 0;
    cx = 0; 
   }
  }
  return cx;
}

float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

 void moveRobot(int leftSpeed, int rightSpeed)
 {
 if (leftSpeed >= 0)
{
  digitalWrite(myPins[1], 1);
  digitalWrite(myPins[2], 0);
}
  else
{
  digitalWrite(myPins[1], 0);
  digitalWrite(myPins[2], 1);
 }

if (rightSpeed >= 0)
 {
  digitalWrite(myPins[3], 0);
  digitalWrite(myPins[4], 1);
 }
  else
 {
  digitalWrite(myPins[3], 1);
  digitalWrite(myPins[4], 0);
 }

  analogWrite(myPins[0], abs(leftSpeed));
  analogWrite(myPins[5], abs(rightSpeed));
 }
