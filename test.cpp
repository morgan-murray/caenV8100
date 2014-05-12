#include <stdlib.h>

#include "ftd2xx.h"

#include "V8100.h"


int main(){

  V8100 * cr = new V8100(0);
  double speed[3];

  cr->makeConnection();
  cr->getCrateName();
  cr->getCrateStatus();
  cr->getNumberChannels();
  cr->getPSTemp();
  cr->getFanSpeeds(speed);
  cr->getFansTemp();
  return 0;

}
