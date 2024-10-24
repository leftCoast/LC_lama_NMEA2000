#ifndef LC_llama_NMEA200_h
#define LC_llama_NMEA200_h

#include  <idlers.h>
#include  <mapper.h>
#include  <CAN.h>

#define DEF_2515_RST_PIN   8
#define DEF_2515_INT_PIN   2
#define DEF_NUM_DATA_BYTES 8


enum msgTypes {
  noType,
  waterSpeed,  // 0x1F503
  waterDepth,  // 0x1F50B
  waterTemp,   // 0x1FD08
  fluidLevel   // 0x1F211
};

enum tankType {
   fuel,
   water,
   grayWater,
   liveWell,
   oil,
   blackWater
};

struct msg_t {
  uint32_t  pgn;
  uint8_t   sa;
  uint8_t   ps;
  uint8_t   dp;
  uint8_t   priority;
};



class CANMsgObj;


// ************ llama_NMEA200 ************


class llama_NMEA200 : public linkList,
                     public idler {
   public:
            llama_NMEA200(int inResetPin=DEF_2515_RST_PIN,int inIntPin=DEF_2515_INT_PIN);
            ~llama_NMEA200(void);

            bool        begin(int inCSPin);
            bool        addMsgObj(msgTypes inType);
            CANMsgObj* getMsgObj(uint32_t inType);
            CANMsgObj* getMsgObj(msgTypes inType);
   virtual  void        idle(void);
    
   protected:
            void        readAddress(uint32_t can_id, msg_t * msg);
    
            int   resetPin;
            int   intPin;
            msg_t msg;
};



// ************* CANMsgObj *************


class CANMsgObj  : public linkListObj {

   public:
            CANMsgObj(int inNumBytes=DEF_NUM_DATA_BYTES);
            ~CANMsgObj(void);

            msgTypes getType(void);
            uint32_t getPGN(void);
            int      getNumBytes(void);
   virtual  void     decodeMessage(void)=0;
            void     showDataBytes(void);
   virtual  void     sendMessage(void)=0;
            
            byte*    dataBytes;
            int      numBytes;
            msgTypes msgType;
            uint32_t msgPGN;
};



// ************* waterSpeedObj *************


class waterSpeedObj  : public CANMsgObj {

   public:
          waterSpeedObj(void);
          ~waterSpeedObj(void);
          
            float getSpeed(void);
   virtual  void  sendMessage(void);
   
   protected:
   virtual  void  decodeMessage(void);
             
          mapper  speedMap;
          float   knots;
};



// ************* waterDepthObj *************


class waterDepthObj  : public CANMsgObj {

   public:
          waterDepthObj(void);
          ~waterDepthObj(void);
          
            float getDepth(void);
   virtual  void  sendMessage(void);
   
   protected:
  virtual   void  decodeMessage(void);
  
  
            float feet;
          
};


// ************* waterTempObj *************


class waterTempObj  : public CANMsgObj {

   public:
            waterTempObj(void);
            ~waterTempObj(void);
          
            float getTemp(void);
   virtual  void  sendMessage(void);
   
   protected:
   virtual  void  decodeMessage(void);
   
            float   degF;
};



// ************* fluidLevelObj *************


class fluidLevelObj  : public CANMsgObj {

   public:
            fluidLevelObj(void);
            ~fluidLevelObj(void);
          
            byte     getInstance(void);
            void     setInstance(byte inInstance);
            tankType getTankType(void);
            void     setTankType(tankType inType);
            float    getLevel(void);
            void     setLevel(float inLevel);
            float    getCapacity(void);
            void     setCapacity(float inCapacity);
            virtual  void     sendMessage(void);
            
   protected:
   virtual  void     decodeMessage(void);
   
             
            byte     instance;
            tankType fluidType;
            float    level;
            float    capacity;
            int      tankID;
            //float    autoPeriod;
            //mapper   periodTimer;
};


#endif
