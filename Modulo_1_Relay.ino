
#include <SPI.h>
#include <mcp_can.h>
#include <EEPROM.h>

const int SPI_CS_PIN = 9;
const int interrupcion = 5;
const int LED=8;
const int Relay_1=7;
const int Relay_2=6;
boolean ledON=1;

unsigned char canId;
unsigned char ID_Local=0x02;
 char ID_Master=0x01;

unsigned char MsgUpOk[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char MsgUpEEprom[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char MsgLeido[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char  reg_control=0x00;
unsigned char  reg_config=0x00;
unsigned char  reg_5=0x00;
unsigned char  reg_4=0x00;
unsigned char  reg_3=0x00;
unsigned char  reg_2=0x00;
unsigned char  reg_1=0x00;
unsigned char  reg_0=0x00;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    Serial.begin(9600);
    
    EEPROM.write(0x00,ID_Local);// escribe en la dir 0x00 el id del dispositivo LOCAL
    EEPROM.write(0x01,ID_Master);// escribe en la dir 0x01 el id del MASTER controlador
    ID_Local= EEPROM.read(0x00);    // almaceno el Id del receptor
    ID_Master= EEPROM.read(0x01);
    MsgUpEEprom[0]=ID_Local;
    MsgUpEEprom[1]=ID_Master;
    
    pinMode(LED,OUTPUT);
    digitalWrite(LED,false);
    pinMode(Relay_1,OUTPUT);
    pinMode(Relay_2,OUTPUT);
    digitalWrite(Relay_1,true);
    digitalWrite(Relay_2,true);
    pinMode(interrupcion ,INPUT);
  //  ID_Local=EEPROM.read(0x00);
  //  ID_Master=EEPROM.read(0x01);
   
START_INIT:

    if(CAN_OK == CAN.begin(CAN_250KBPS,MCP_8MHz))                 
        {CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
         Led_CanUpOK();
         }
    else
        {
          Led_CanFail();
          goto START_INIT;
          }
    
}

unsigned char len = 0;
unsigned char buf[8];

void loop()
{   

    if(!digitalRead(interrupcion))            // check if data coming
    
    {     
          CAN.readMsgBuf(&len, buf);    // leo el mensaje recibido
          canId = CAN.getCanId();       // almaceno el Id del emisor
          ID_Local= EEPROM.read(0x00);    // leo el Id del receptor de la EEPROM
          ID_Master= EEPROM.read(0x01);
          int ID=int(ID_Local);
        
          MsgLeido[7]=buf[7];MsgLeido[6]=buf[6];MsgLeido[5]=buf[5];MsgLeido[4]=buf[4];
          MsgLeido[3]=buf[3];MsgLeido[2]=buf[2];MsgLeido[1]=buf[1];MsgLeido[0]=buf[0];


        

          if( canId==0x00){// MAster  broadcast 0xFF para que todos los ID LOCALEs publiquen su info
            Led_mensaje_recibido_blink();
            CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
          }
          if( canId == ID_Master){  // su el Master conside con el emisor
              Led_mensaje_recibido_blink();
              if(0x00==MsgLeido[0]){    //si el control es 00 solo envia su info el local
                 CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
                 }
              if(MsgLeido[0]==0x01){ 
                if(MsgLeido[1]==0x00){
                  digitalWrite(Relay_1,true);
                  digitalWrite(Relay_2,true);}
                if(MsgLeido[1]==0x01){
                  digitalWrite(Relay_1,false);
                  digitalWrite(Relay_2,true);}
                if(MsgLeido[1]==0x10){
                  digitalWrite(Relay_1,true);
                  digitalWrite(Relay_2,false);}
                if(MsgLeido[1]==0x11){
                  digitalWrite(Relay_1,false);
                  digitalWrite(Relay_2,false);}
                ledColor(MsgLeido[1]);
                CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
                }
            }

           if(MsgLeido[0]==0xFF){
            if(MsgLeido[7]==ID_Local){
              EEPROM.write(0x00, MsgLeido[6]);// escribe en la dir 0x00 el id del dispositivo LOCAL
              EEPROM.write(0x01, MsgLeido[5]);// escribe en la dir 0x01 el id del MASTER controlador
              ID_Local= EEPROM.read(0x00);    // almaceno el Id del receptor
              ID_Master= EEPROM.read(0x01);
              MsgUpEEprom[0]=ID_Local;
              MsgUpEEprom[1]=ID_Master;
              CAN.sendMsgBuf(ID_Local,0,8,MsgUpEEprom);
              Led_grabacion_3();}
            }
      
          
       
      
      }
        
    }


 void Led_grabacion_3(){
          digitalWrite(LED,true);
          delay(200);
          digitalWrite(LED,false);
          delay(200);
          digitalWrite(LED,true);
          delay(200);
          digitalWrite(LED,false);
          delay(200);
          digitalWrite(LED,true);
          delay(500);
          digitalWrite(LED,false);
  
  
  }

 void Led_mensaje_recibido_blink(){
          digitalWrite(LED,true);
          delay(50);
          digitalWrite(LED,false);  
  }

 void Led_0xFF_CanID(){
                digitalWrite(LED,true);
                delay(50);
                digitalWrite(LED,false);
                delay(50);
                digitalWrite(LED,true);
                delay(50);
                digitalWrite(LED,false);
                delay(50);
                digitalWrite(LED,true);
                delay(50);
                digitalWrite(LED,false);
                delay(50);
                digitalWrite(LED,true);
                delay(50);
                digitalWrite(LED,false);
 }

 void Led_CanUpOK(){
           digitalWrite(LED,true);
           delay(100);
           digitalWrite(LED,false);
           delay(100);
           digitalWrite(LED,true);
           delay(100);
           digitalWrite(LED,false);
  
 }

 void Led_CanFail(){
     digitalWrite(LED,true);
           delay(200);
           digitalWrite(LED,false);
           delay(200);
           digitalWrite(LED,true);
           delay(200);
           digitalWrite(LED,false);
           delay(200);
           digitalWrite(LED,true);
           delay(1000);
           digitalWrite(LED,false);
           delay(200);
  }


void ledColor(char var) {
 switch (var) {
   case 0x01:
       digitalWrite(LED,true);
     break;
    
    case 0x00:
       digitalWrite(LED,false);
     break;
  default:
  break;
 }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
