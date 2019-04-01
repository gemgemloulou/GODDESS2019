#ifndef goddessEvent_h
#define goddessEvent_h

using namespace std;

#include <map>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdint.h>

#include "TObject.h"
#include "TMath.h"
#include "TString.h"
#include "TVector3.h"

/* Raw data holder */
struct goddessEvent {
  std::vector<UShort_t> channels;
  std::vector<UShort_t> values;
  ULong64_t timestamp;
};

#endif

