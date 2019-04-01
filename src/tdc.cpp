#include "tdc.h"

tdc_t::tdc_t(Int_t numChannels_) :
  numChannels(numChannels_)
{ ; }

void tdc_t::Clear() {
   tdcRaw.clear(); tdcCal.clear();
   tdc0 = 0; tdc1 = 0; tdc2 = 0; tdc3 = 0; tdc4 = 0; tdc5 = 0;
}
/************************************************************/

Bool_t tdc_t::ValidChannel(UInt_t channel, Int_t chType) { 
  size_t size = numChannels;
  if (channel >= size || channel < 0) { 
    cout << "ERROR (ionChamber::ValidChannel): Channel specified "
	 << channel << " is not valid\n";
    return kFALSE;
  }
  return kTRUE;
}


/************************************************************/

void tdc_t::SetRawValue(UInt_t detChannel, Int_t chType, UInt_t rawValue, Int_t ignoreThresh) {

  if (!ValidChannel(detChannel,chType)) { return; }
  UInt_t threshold = 0;

  if ( detChannel < threshTDC.size() ) {
     threshold = threshTDC.at(detChannel);
  }
  if (ignoreThresh!=0 || (ignoreThresh == 0 && rawValue > threshold)) {
     tdcRaw[detChannel] = rawValue;
  }
}

/************************************************************/
void tdc_t::SetChannelMap(std::map<Short_t, Short_t> channelMap, Int_t chType) {
  if (chType == 1) { mapTDC = channelMap; }
}

std::map<Short_t, Short_t> tdc_t::GetChannelMap(Int_t chType) {
  if (chType == 1) { return mapTDC; }
}
/************************************************************/

Bool_t tdc_t::SetThresholds(std::vector<Int_t> thresholds, Int_t chType) {
    if (thresholds.size() != numChannels) { 
      printf("ERROR: Size of vector for thresholds (%d) not as expected (%d).\n", thresholds.size(), numChannels);
      return kFALSE;
    }
    threshTDC = thresholds;
  return kTRUE;
}

std::vector<Int_t> tdc_t::GetThresholds() {
  return threshTDC; 
}

/************************************************************/
Int_t tdc_t::GetTDCValue(UInt_t channel) {
    if ( channel == 0 ) {
    	return tdc0;
    } else if ( channel == 1 ) {
    	return tdc1;
    } else if ( channel == 2) {
   	return tdc2;
    } else if ( channel == 3 ) {
	return tdc3;
    } else if ( channel == 4 ) {
	return tdc4;
    } else if ( channel == 5 ) {
	return tdc5;
    }
}
void tdc_t::SetTDCValue(UInt_t channel, Int_t value ) {
    if ( channel == 0 ) {
    	tdc0 = value;
    } else if ( channel == 1 ) {
	tdc1 = value;
    } else if ( channel == 2) {
	tdc2 = value;
    } else if ( channel == 3 ) {
	tdc3 = value;
    } else if ( channel == 4 ) {
	tdc4 = value;
    } else if ( channel == 5 ) {
	tdc5 = value;
    } 

}


/************************************************************/

void tdc_t::LoadEvent(goddessEvent *ev, Bool_t ignoreThresholds) {
  std::map<Short_t, Short_t>::iterator itr;
  std::map<Short_t, Short_t> map;
  map = GetChannelMap(1);
  
  Short_t begin;
  Short_t end;
  itr = map.begin(); begin = itr->second;
  itr = map.end(); itr--; end = itr->second;

  for (Int_t i=0; i<ev->channels.size(); i++) {
    if (ev->channels[i] >= begin && ev->channels[i] <= end) {      
      for (itr = map.begin(); itr!=map.end(); ++itr) {
	if (ev->channels[i] == itr->second) {
	  SetRawValue(itr->first-1, 1, ev->values[i], ignoreThresholds); 
	  break;
	}
      }
    }
  }

  std::map<Short_t, Float_t>::iterator itr2;
  for (itr2 = tdcRaw.begin(); itr2 != tdcRaw.end(); itr2++) {

    SetTDCValue(itr2->first,itr2->second);

  }
}
