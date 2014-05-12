#include <stdlib.h>

#include "ftd2xx.h"

#include "V8100.h"


int main(){

  V8100 * cr = new V8100(2);

  cr->getCrateName();
  cr->getCrateStatus();

  return 0;

}
