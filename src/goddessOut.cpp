#include "goddessOut.h"

/************************************************************/
/* GODDESS Tree Output                                      */
/************************************************************/

void goddessOut::Clear() {
  siID.clear();
  siType.clear();
  si.clear();

/// noticed that the ion counter wasn't getting cleared 3/14 @ 03:05 AR
  icDE = 0.0; icE = 0.0; icX = 0.0; icY = 0.0;

  tdc0 = 0.0; tdc1 = 0.0; tdc2 = 0.0; tdc3 = 0.0; tdc4 = 0.0; tdc5 = 0.0; 
}

