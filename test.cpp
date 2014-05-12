#include <stdlib.h>

#include "ftd2xx.h"

#include "V8100.h"
#include "V8100channel.h"

int main(){

  V8100 * cr = new V8100(0);
  double speed[3];

  ///////////////////////
  cr->makeConnection();
  cr->getCrateName();
  cr->getCrateStatus();
  int numChanns = cr->getNumberChannels();
  cr->getPSTemp();
  cr->getFanSpeeds(speed);
  cr->getFansTemp();

  ///////////////////////////////////////////

  V8100channel *ch[numChanns];
  std::string * name = new std::string();

  for (int ii = 0; ii < numChanns; ii++){

    ch[ii] = new V8100channel(cr,ii);
    ch[ii]->getChannelName(name);
    ch[ii]->getSetVoltage();
    ch[ii]->getMinVoltage();
    ch[ii]->getMaxVoltage();
    ch[ii]->getResVoltage();
    ch[ii]->getOverVoltage();
    ch[ii]->getUnderVoltage();
    ch[ii]->getMonVoltage();
    ch[ii]->getSetCurrent();
    ch[ii]->getMinCurrent();
    ch[ii]->getMaxCurrent();
    ch[ii]->getResCurrent();
    ch[ii]->getMonCurrent();
  }
  


  return 0;

}
