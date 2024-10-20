#include "LC_lama_NMEA200.h"


int bytesToInt(byte hiByte,int lowByte) {
  
  struct int16 {
    byte lowByte;
    byte hiByte;
  };
  int16*  bytes;
  int     value;

  bytes = (int16*)&value;
  bytes.lowByte = lowByte;
  bytes.hiByte = hiByte;
  return(value);
}


CANMessage* createMsgObj(msgType inType) {

  switch(inType) {
    case noType     : return NULL;
    case waterSpeed : return (CANMessage*) new waterSpeedObj();
    case waterDepth : return (CANMessage*) new waterDepthObj();
  }
  return NULL;
}



// ************ lama_NMEA200 ************


lama_NMEA200::lama_NMEA200(int inResetPin=DEF_2515_RST_PIN,int inIntPin=DEF_2515_INT_PIN)
  : linkList(),
  idler() {

  resetPin = inResetPin;
  intPin   = inIntPin;
}


lama_NMEA200::~lama_NMEA200(void) {  }


bool lama_NMEA200::begin(int inCSPin) {

  hookup();
  pinMode(resetPin, OUTPUT);
  delay(50);
  digitalWrite(resetPin, LOW);
  delay(50);
  digitalWrite(resetPin, HIGH);
  return CAN.begin(500E3);
}


CANMessage* lama_NMEA200::getMsgObj(uint32_t inPGN) {

  CANMessage* trace;
  
  trace = (CANMessage*)getFirst();
  while(trace) {
    if (trace->getPGN()==inPGN) {
      return trace;
    }
    trace = (CANMessage*)trace->getNext();
  }
  return NULL;
}


CANMessage* lama_NMEA200::getMsgObj(NMEAType inType) {

  CANMessage* trace;
  
  trace = (CANMessage*)getFirst();
  while(trace) {
    if (trace->getType()==inType) {
      return trace;
    }
    trace = (CANMessage*)trace->getNext();
  }
  return NULL;
}


bool lama_NMEA200::addMsgObj(msgType inType) {

  CANMessage* newMsgObj;
  
  if (!getMsgObj(inType)) {
    newMsgObj = createMsgObj(inType);
    if (newMsgObj) {
      addToTop(newMsgObj);
    }
}


void lama_NMEA200::idle(void) {

  int         packetSize;
  uint32_t    theID;
  msg_t       msg;
  CANMessage* decodeObj;
  int         i;
  
  packetSize = CAN.parsePacket();
  if (packetSize) {
    theID = CAN.packetId();
    readAddress (theID, &msg);
    decodeObj = (CANMessage*)getMsgObj(msg.pgn);
    if (decodeObj) {
      i = 0;
      while (CAN.available()&&i<NUM_DATA_BYTES) {
        decodeObj->dataPack[i] = CAN.read();
        i++;
      }
      decodeObj->decodeMessage();
    }
  }
}


void lama_NMEA200::readAddress (uint32_t can_id, msg_t* msg) {
  
  uint32_t buffer = can_id;
  
  msg->sa = buffer & 0xFF;
  buffer = buffer >> 8;
  msg->pgn = buffer & 0x3FFFF;
  msg->ps = buffer & 0xFF;
  msg->dp = (buffer & 0xFF00) >> 8;
  msg->priority = (buffer & 0x1C0000) >> 18;
}



// ************* CANMessage *************


CANMessage::CANMessage(void)
  : linkListObj() {  
  
  dataBytes = NULL;
  numBytes = 0;
  msgType = noType;
  msgPGN  = 0;
}


CANMessage::~CANMessage(void) {  }

NMEAType CANMessage::getType(void) { return msgType; }

uint32_t CANMessage::getPGN(void) { return msgPGN; }


 
// ************* waterSpeedOBj *************


waterSpeedOBj::waterSpeedOBj(void) {

  msgType = waterSpeed;
  msgPGN  = 0x1F503;
  knots   = 0;
  speedMap.setValues(0,1023,0,(1023*1.943844)*0.01);
}


waterSpeedOBj::~waterSpeedOBj(void) {  }


void waterSpeedOBj::decodeMessage(void) {

  unsigned int rawSpeed;
  
  rawSpeed = (unsigned int) bytesToInt(dataPack[2],dataPack[1]);
  knots = speedMap.map(rawSpeed);
}

  
float waterSpeedOBj::getSpeed(void) { return knots; }
          