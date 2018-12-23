#include <unistd.h>
#include <iostream>

#include "mc_utils/declarations.h"
#include "substrate/timeline.h"
#include "substrate/domain_configurator.h"
#include "substrate/agent.h"
#include "substrate/controller.h"

#include "mars_domain.h"
#include "mars_scoring.h"

#include "mclLogger.h"
#include "mclHostAPI.h"
#include "mclMonitorResponse.h"
#include "mclSettings.h"

#include <time.h>

using namespace std;

#define USLEEP_PER_TICK 100000

bool test_imagebank() {
  imagetype nul = create_empty_img();
  cout << "empty image:" << endl;
  describe_imagetype(nul);
  cout << endl;

  cout << "sequence (255x1x0-7):" << endl;  
  for (int i=0; i< 8; i++) {
    imagetype tseq = create_img(255,1,i);
    describe_imagetype(tseq);
    cout << endl;
  }

  cout << "sequence (255x1x0-7):" << endl;  
  for (int i=0; i< 10; i++) {
    imagetype tseq = create_img(255,1,i);
    img_set_pano(tseq);
    describe_imagetype(tseq);
    cout << endl;
  }
  
  return true;
}

int main(int argc, char* args[]) {
  cout << "testing imagebank....";
  if (test_imagebank()) 
    cout << "passed." << endl;
  else
    cout << "failed." << endl;
}
