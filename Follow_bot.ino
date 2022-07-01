// Libraries to include
#include <SPI.h>
#include <Pixy2.h>
#include <PIDLoop.h>

// this limits how fast the robot can travel forwards (can be anything below 200)
#define MAX_TRANSLATE_VELOCITY 150

Pixy2 pixy;
PIDLoop panLoop(350, 0, 600, true);
PIDLoop tiltLoop(500, 0, 700, true);
PIDLoop rotateLoop(300, 600, 300, false);
PIDLoop translateLoop(400, 800, 300, false);

 // initialize motor objects
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Starting...\n");
  // initialize motor objects

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

}
