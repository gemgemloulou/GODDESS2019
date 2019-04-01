#include "orrubaDet.h"

orrubaDet::orrubaDet() :
  numNtype (0), numPtype(0) 
{ 
  Clear();
}

orrubaDet::orrubaDet(std::string serial_Num, UShort_t sector_, UShort_t depth_, Bool_t upstream_, solidVector position) :
  numNtype (0), numPtype(0), serialNum(serial_Num), sector(sector_), depth(depth_), upStream(upstream_), detPos(position)
{
  Clear();
  SetPosID();
}

void orrubaDet::Clear() {
  eRawPmap.clear();  eRawNmap.clear();
  eCalPmap.clear();  eCalNmap.clear();
  timePmap.clear();  timeNmap.clear();
}

/************************************************************/

void orrubaDet::SetDetector(std::string serial_Num, UShort_t sector_, UShort_t depth_, Bool_t upstream_, solidVector position) {
  serialNum = serial_Num;
  sector = sector_;
  depth = depth_;
  upStream = upstream_;
  detPos = position;

  SetPosID();
}

void orrubaDet::SetPosID() {
  posID.clear();
  if (upStream) { posID.append("U"); }
  else { posID.append("D"); }
  std::stringstream sect;  sect << sector;
  posID.append(sect.str());
  posID.append("_");
if (depth == 0) { posID.append("dE"); }
  else if (depth == 1) { posID.append("E1"); }
  else if (depth == 2) { posID.append("E2"); }
}

/************************************************************/

void orrubaDet::SetChannelMap(std::map<Short_t, Short_t> channelMap, Bool_t nType) {
  if (nType) { chMapN = channelMap; } 
  else { chMapP = channelMap; }
}

std::map<Short_t, Short_t> orrubaDet::GetChannelMap(Bool_t nType) {
  if (nType) { return chMapN; }
  return chMapP;
}

void orrubaDet::SetNumChannels(Int_t pType, Int_t nType) {
  numPtype = pType;  numNtype = nType;
  parECalP.resize(pType);
  parECalN.resize(nType);
}

Int_t orrubaDet::GetNumChannels(Bool_t nType) {
  if (nType) { return numNtype; } 
  return numPtype;
}

Bool_t orrubaDet::ValidChannel(UInt_t channel, Bool_t nType) {
  size_t size = numPtype;
  if (nType) { size = numNtype; }
  if (channel >= size) {
    printf("ERROR: Contact specified, %u, is invalid!\n", channel);
    return kFALSE; 
  }
  return kTRUE;
}

Bool_t orrubaDet::ChannelHit(UInt_t channel, Bool_t nType) {
  valueMap *map = &eRawPmap;
  if (nType) { map = &eRawNmap; }
  if (map->find(channel) == map->end()) { return kFALSE; }
  return kTRUE;
}

/************************************************************/

void orrubaDet::SetRawEValue(UInt_t detChannel, UInt_t rawValue, Int_t ignoreThresh) {
  if (detChannel < numPtype) { SetRawEValue(detChannel, orrubaDet::pTYPE, rawValue, ignoreThresh); }
  else if (detChannel < numPtype+numNtype) {
    SetRawEValue(detChannel, orrubaDet::nTYPE, rawValue, ignoreThresh); 
  } else { printf("ERROR: Cannot SetRawValue for invalid channel %d\n", detChannel); }
}

void orrubaDet::SetRawEValue(UInt_t detChannel, Bool_t nType, UInt_t rawValue, Int_t ignoreThresh) {
  if (!ValidChannel(detChannel, nType)) { return; }
  UInt_t threshold = 0;
  if (nType) { 
    if (detChannel < threshN.size()) { threshold = threshN.at(detChannel); }
    if (ignoreThresh!=0 || (ignoreThresh==0 && rawValue > threshold)) {
      eRawNmap[detChannel] = rawValue;
    }
  } else {
    if (detChannel < threshP.size()) { threshold = threshP.at(detChannel); }
    if (ignoreThresh!=0 || (ignoreThresh==0 && rawValue > threshold)) {
      eRawPmap[detChannel] = rawValue;
    }
  }
}

Float_t orrubaDet::GetRawEValue(UInt_t detChannel, Bool_t nType) {
  if (nType) { return eRawNmap[detChannel]; }
  return eRawPmap[detChannel];
}

Float_t orrubaDet::GetCalEValue(UInt_t detChannel, Bool_t nType) {
  if (nType) { return eCalNmap[detChannel]; }
  return eCalPmap[detChannel];
}

orrubaDet::valueMap orrubaDet::GetRawE(Bool_t nType) {
  if (nType) { return eRawNmap; }
  return eRawPmap;
}

orrubaDet::valueMap orrubaDet::GetCalE(Bool_t nType) {
  if (nType) { return eCalNmap; }
  return eCalPmap;
}

/************************************************************/

void orrubaDet::SetTimestamp(UInt_t detChannel, Bool_t nType, uint64_t timestamp) {
  if (!ValidChannel(detChannel,nType)) { return; }
  if (nType == orrubaDet::nTYPE) { timeNmap[detChannel] = timestamp; }
  else { timePmap[detChannel] = timestamp; }
}

uint64_t orrubaDet::GetTimestamp(UInt_t detChannel, Bool_t nType) {
  uint64_t timestamp = 0xffffffffffffffff;
  for (map<Short_t, uint64_t>::iterator itr = timePmap.begin(); itr != timePmap.end(); ++itr) {
    if (timestamp > itr->second) { timestamp = itr->second; }
  }
  for (map<Short_t, uint64_t>::iterator itr = timeNmap.begin(); itr != timeNmap.end(); ++itr) {
    if (timestamp > itr->second) { timestamp = itr->second; }
  }
  return timestamp;
}

orrubaDet::timeMap orrubaDet::GetTSmap(Bool_t nType) {
  if (nType) { return timeNmap; }
  return timePmap;
}

/************************************************************/

Bool_t orrubaDet::SetECalParameters(std::vector<Float_t> par, Int_t contact, Bool_t nType) {
  if (!ValidChannel(contact,nType)) { return kFALSE; }
  if (nType) { parECalN.at(contact) = par; }
  else { parECalP.at(contact) = par; }
  return kTRUE;
}

std::vector< std::vector<Float_t> > orrubaDet::GetECalParameters(Bool_t nType) {
  if (nType) { return parECalN; }
  return parECalP;
}

Bool_t orrubaDet::SetThresholds(std::vector<Int_t> thresholds, Bool_t nType, Int_t thrSize) {
  if (thrSize == 0) { thrSize = (UInt_t)GetNumChannels(nType); }
  if (thresholds.size() != (UInt_t)thrSize) {
    printf("ERROR: Size of vector for thresholds (%d) was not as expected (%d)\n",
	   thresholds.size(), thrSize);
    return kFALSE;
  }
  if (nType == orrubaDet::nTYPE) { threshN = thresholds; }
  else { threshP = thresholds; }
  return kTRUE;
}

std::vector<Int_t> orrubaDet::GetThresholds(Bool_t nType) {
  if (nType) { return threshN; }
  return threshP;
}



