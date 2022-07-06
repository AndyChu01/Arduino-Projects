// Libraries to include
#include <SPI.h>
#include <Pixy2.h>
#include <PIDLoop.h>

// this limits how fast the robot can t
Pixy2 pixy;
PIDLoop panLoop(350, 0, 600, true);
PIDLoop tiltLoop(500, 0, 700, true);
PIDLoop rotateLoop(300, 600, 300, false);
PIDLoop translateLoop(400, 800, 300, false);
// Travel forwards (can be anything below 200)
#define MAX_TRANSLATE_VELOCITY 150

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Starting...\n");
  
  // Motor Setup
    //Setup Channel A (Right)
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin
  pinMode(9, OUTPUT); //Initiates Brake Channel A pin
    //Setup Channel B (Left)
  pinMode(13, OUTPUT); //Initiates Motor Channel A pin
  pinMode(8, OUTPUT);  //Initiates Brake Channel A pin

  // initialize motor objects
    //Motor A
  digitalWrite(12, HIGH); //Establishes forward direction of Channel A
  digitalWrite(9, LOW);   //Disengage the Brake for Channel A
  analogWrite(3, 0);   //Motor at 0
     //Motor B
  digitalWrite(13, HIGH);  //Establishes forward direction of Channel B
  digitalWrite(8, LOW);   //Disengage the Brake for Channel B
  analogWrite(11, 0);    //Motor at 0

  // need to initialize pixy object
  pixy.init();
  
  // user color connected components program
  pixy.changeProg("color_connected_components");
}

// Take the biggest block (blocks[0]) that's been around for at least 30 frames (1/2 second)
// and return its index, otherwise return -1
int16_t acquireBlock()
{
  if (pixy.ccc.numBlocks && pixy.ccc.blocks[0].m_age>30)
    return pixy.ccc.blocks[0].m_index;

  return -1;
}

// Find the block with the given index.  In other words, find the same object in the current
// frame -- not the biggest object, but he object we've locked onto in acquireBlock()
// If it's not in the current frame, return NULL
Block *trackBlock(uint8_t index)
{
  uint8_t i;

  for (i=0; i<pixy.ccc.numBlocks; i++)
  {
    if (index==pixy.ccc.blocks[i].m_index)
      return &pixy.ccc.blocks[i];
  }

  return NULL;
}


void loop() {
  // put your main code here, to run repeatedly:
static int16_t index = -1;
  int32_t panOffset, tiltOffset, headingOffset, left, right;
  Block *block=NULL;
  
  pixy.ccc.getBlocks();

  if (index==-1) // search....
  {
    Serial.println("Searching for block...");
    index = acquireBlock();
    if (index>=0)
      Serial.println("Found block!");
 }
  // If we've found a block, find it, track it
  if (index>=0)
     block = trackBlock(index);

  // If we're able to track it, move motors
  if (block)
  {
    // calculate pan and tilt errors
    panOffset = (int32_t)pixy.frameWidth/2 - (int32_t)block->m_x;
    tiltOffset = (int32_t)block->m_y - (int32_t)pixy.frameHeight/2;  

    // calculate how to move pan and tilt servos
    panLoop.update(panOffset);
    tiltLoop.update(tiltOffset);

    // move servos
    pixy.setServos(panLoop.m_command, tiltLoop.m_command);

    // calculate translate and rotate errors
    panOffset += panLoop.m_command - PIXY_RCS_CENTER_POS;
    tiltOffset += tiltLoop.m_command - PIXY_RCS_CENTER_POS - PIXY_RCS_CENTER_POS/2 + PIXY_RCS_CENTER_POS/8;

    rotateLoop.update(panOffset);
    translateLoop.update(-tiltOffset);

    // keep translation velocity below maximum
    if (translateLoop.m_command>MAX_TRANSLATE_VELOCITY)
      translateLoop.m_command = MAX_TRANSLATE_VELOCITY;

    // calculate left and right wheel velocities based on rotation and translation velocities
    left = -rotateLoop.m_command + translateLoop.m_command;
    right = rotateLoop.m_command + translateLoop.m_command;

    // set wheel velocities
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);
    analogWrite(3, right); //Motor A
    analogWrite(11, left); //Motor B
    
    // print the block we're tracking -- wait until end of loop to reduce latency
    block->print();
  }  
  else // no object detected, stop motors, go into search state
  {
    rotateLoop.reset();
    translateLoop.reset();
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);
    analogWrite(3, 0); //Motor A
    analogWrite(11, 0); //Motor B
    index = -1; // set search state
  }
}
