#include <Adafruit_GFX.h>
#include <gfxfont.h>


#include <Adafruit_LEDBackpack.h>

#include <RTClib.h>
#include <Adafruit_BMP085.h>
#include <Wire.h>


RTC_DS3231 rtc;
float temp=0;
float baro=0;
float vbatt;
Adafruit_BMP085 bmp;
Adafruit_7segment sev = Adafruit_7segment();

DateTime now;

#define C_0 A0
#define C_1 A1
#define C_2 A2
#define C_3 A3
#define CHAR_0 0
#define CHAR_1 1
#define CHAR_2 3
#define CHAR_3 4
#define COL_COUNT 4
#define ROW_COUNT 8
#define MODE_COUNT 10
#define MODE_TIME 0
#define MODE_BARO 1
#define MODE_TEMP 3


//const int cathmap[] = {3,2,1,0,4,5,6,7};
const int cathmap[] = {7,6,5,4,0,1,2,3,8,9,10,11,12,13,14,15};
const int rowpins[] = {2,3,4,5,6,7,8,9};   
const int colpins[] = {C_0,C_1,C_2,C_3};
const unsigned char sevenSegments[] = {0x3F,0x06,0x5B,0x27,0x66,0x6D,0x7D,0x07,0x7F,0x67};
unsigned char matrix[8] = {0,1,3,7,15,31,63,127};
float tdot[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float pdot[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int gstep=1;
float tmax=-99999;
float tmin=99999;
float bmax=-99999;
float bmin=99999;
const float vmin=3.3;
const float vmax=4.0;
long count=0;
int mode=0;
int timemode=4;
bool invert_matrix=false;
int speed_div=100;
unsigned long ltime=0;
const unsigned char matdig[10][8]={{0b0110,0b1001,0b1001,0b1001,0b1001,0b1001,0b1001,0b0110},
                                   {0b0010,0b0110,0b1010,0b0010,0b0010,0b0010,0b0010,0b1111},
                                   {0b0110,0b1001,0b0001,0b0010,0b0100,0b1000,0b1000,0b1111},
                                   {0b0110,0b1001,0b0001,0b0110,0b0001,0b0001,0b1001,0b0110},
                                   {0b1010,0b1010,0b1010,0b1111,0b0010,0b0010,0b0010,0b0010},
                                   {0b1111,0b1000,0b1000,0b1110,0b0001,0b0001,0b0001,0b1110},
                                   {0b0110,0b1001,0b1000,0b1110,0b1001,0b1001,0b1001,0b0110},
                                   {0b1111,0b1001,0b0001,0b0010,0b0010,0b0100,0b0100,0b0100},
                                   {0b0110,0b1001,0b1001,0b0110,0b1001,0b1001,0b1001,0b0110},
                                   {0b0110,0b1001,0b1001,0b0111,0b0001,0b0001,0b1001,0b0110}};
const unsigned char matchar[38][8]={
                                    {0,4,10,10,10,10,4,0}
                                    ,{0,12,4,4,4,4,14,0}
                                    ,{0,12,2,4,4,8,14,0}
                                    ,{0,12,2,4,2,2,12,0}
                                    ,{0,10,10,10,14,2,2,0}
                                    ,{0,14,8,12,2,2,12,0}
                                    ,{0,6,8,12,10,10,4,0}
                                    ,{0,14,2,2,4,4,8,0}
                                    ,{0,4,10,4,10,10,4,0}
                                    ,{0,4,10,10,6,2,12,0}
                                    ,{0,4,10,10,14,10,10,0}
                                    ,{0,12,10,12,10,10,12,0}
                                    ,{0,6,8,8,8,8,6,0}
                                    ,{0,12,10,10,10,10,12,0}
                                    ,{0,14,8,12,8,8,14,0}
                                    ,{0,14,8,12,8,8,8,0}
                                    ,{0,6,8,8,10,10,6,0}
                                    ,{0,10,10,14,10,10,10,0}
                                    ,{0,14,4,4,4,4,14,0}
                                    ,{0,6,2,2,2,2,12,0}
                                    ,{0,10,10,12,10,10,10,0}
                                    ,{0,8,8,8,8,8,14,0}
                                    ,{0,10,14,14,14,10,10,0}
                                    ,{0,12,10,10,10,10,10,0}
                                    ,{0,4,10,10,10,10,4,0}
                                    ,{0,12,10,10,12,8,8,0}
                                    ,{0,4,10,10,10,4,4,0}
                                    ,{0,12,10,10,12,10,10,0}
                                    ,{0,6,8,4,2,2,12,0}
                                    ,{0,14,4,4,4,4,4,0}
                                    ,{0,10,10,10,10,10,4,0}
                                    ,{0,10,10,10,10,4,4,0}
                                    ,{0,10,10,14,14,14,4,0}
                                    ,{0,10,10,4,4,10,10,0}
                                    ,{0,10,10,10,4,4,4,0}
                                    ,{0,14,2,4,4,8,14,0}
                                    ,{0,0,0,4,0,4,0,0}
                                    ,{0,0,0,0,0,0,0,0}
};


void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  bmp.begin();
  rtc.begin();
  now=rtc.now();
  sev.begin(0x70);
  sev.setBrightness(32);
  for(int i=0;i<ROW_COUNT;i++){
    pinMode(rowpins[i],OUTPUT);
    digitalWrite(rowpins[i],LOW);
  }
  for(int i=0;i<COL_COUNT;i++){
    pinMode(colpins[i],OUTPUT);
    digitalWrite(colpins[i],LOW);
  }
  pinMode(A7,INPUT);
  digitalWrite(A7,LOW);
  pinMode(A6,INPUT);
  digitalWrite(A6,HIGH);
}

void setCathodeAddress(int addr){
  addr=cathmap[addr];
  PORTC&=0b11110000;
  PORTC+=(addr&0b1111); 
}

void set7(int char_addr, int digit,boolean point){
  sev.writeDigitNum(char_addr,digit,point);
}



void fifoIn(float *array,float val,int len){
  for(int i=0;i<len-1;i++){
    array[i]=array[i+1];
  }
  array[len-1]=val;
}

void showBaro7(){
  sev.print(baro);
  sev.drawColon(false);
  sev.writeDisplay();
}

void showTemp7(){
  sev.print(temp);
  sev.writeDigitRaw(CHAR_3,0b01110001);
  sev.drawColon(false);
  sev.writeDisplay();
}

void logBaro(){
  baro = bmp.readPressure()*0.0002953;
  fifoIn(pdot,baro,32);
  bmax=-9999;
  bmin=9999;
  for(int i=0;i<32;i++){
    Serial.print(pdot[i]);
    Serial.print(",");
    if(pdot[i]>bmax && pdot[i]!=0){bmax=pdot[i];}
    if(pdot[i]<bmin && pdot[i]!=0){bmin=pdot[i];}
  }
  Serial.print("    ");
  Serial.print(bmax);
  Serial.print(",");
  Serial.print(bmin);
  Serial.println(" ");
  if(bmin==bmax){bmin-=0.01;bmax+=0.01;}
  if(mode==MODE_BARO){fifoToMatrix(pdot,bmin,bmax);}
}

void logTemp(){
  temp = bmp.readTemperature()*9/5+32;
  fifoIn(tdot,temp,32);
  tmax=-9999;
  tmin=9999;
  for(int i=0;i<32;i++){
    if(tdot[i]>tmax && tdot[i]!=0){tmax=tdot[i];}
    if(tdot[i]<tmin && tdot[i]!=0){tmin=tdot[i];}
  }
  if(tmin==tmax){tmin-=0.01;tmax+=0.01;}
  if(mode==MODE_TEMP){fifoToMatrix(tdot,tmin,tmax);}
}

void checkButtons(){
  
  int b=0;
  int omode = mode;
  noInterrupts();
  for(int i=9;i<13;i++){
    setCathodeAddress(i);
    b = b*2+(analogRead(A6)<400);
  }
  setCathodeAddress(8);
  bool minus=(analogRead(A6)<400);
  setCathodeAddress(13);
  bool plus=(analogRead(A6)<400);
  interrupts();
  switch (b){
    case 0:
    default:
      mode=MODE_TIME;
      timemode=0;
      if(plus || minus){invert_matrix=!invert_matrix;}
      break; 
    case 1:
      mode=MODE_TIME;
      timemode=1;
      if(plus || minus){invert_matrix=!invert_matrix;}
      break; 
    case 2:
      mode=MODE_TIME;
      timemode=2;
      if(plus || minus){invert_matrix=!invert_matrix;}
      break; 
    case 3:
      mode=MODE_TIME;
      timemode=3;
      if(plus || minus){invert_matrix=!invert_matrix;}
      break;
    case 4:
      mode=MODE_TIME;
      timemode=4;
      if(plus || minus){invert_matrix=!invert_matrix;}
      break; 
    case 5:
      mode=MODE_TIME;
      timemode=5;
      if(minus){speed_div+=10;}
      if(plus && speed_div>10){speed_div-=10;}
      if(minus && plus){speed_div=100;}
      break;  
    case 6:
      mode=MODE_BARO;
      if(mode!=omode){
        fifoToMatrix(pdot,bmin,bmax);
        showBaro7();
      }
      
      if(plus && gstep>1){gstep--;fifoToMatrix(pdot,bmin,bmax);}
      if(minus && gstep<4){gstep++;fifoToMatrix(pdot,bmin,bmax);}
      if(plus && minus){gstep=1;fifoToMatrix(pdot,bmin,bmax);}
      break; 
    case 7:
      mode=MODE_TEMP;
      if(mode!=omode){
        showTemp7();
        fifoToMatrix(tdot,tmin,tmax);
      }
      if(plus && gstep>1){gstep--;fifoToMatrix(tdot,tmin,tmax);}
      if(minus && gstep<4){gstep++;fifoToMatrix(tdot,tmin,tmax);}
      if(plus && minus){gstep=1;fifoToMatrix(tdot,tmin,tmax);}
      break;
    case 8:
      mode=MODE_TIME;
      timemode=6;
      if(minus){
        rtc.adjust(rtc.now() + TimeSpan(0,-1,0,0));
      }
      if(plus){
        rtc.adjust(rtc.now() + TimeSpan(0,1,0,0));
      }
      break;
    case 9:
      mode=MODE_TIME;
      timemode=7;
      if(minus){
        rtc.adjust(rtc.now() + TimeSpan(0,0,-1,0));
      }
      if(plus){
        rtc.adjust(rtc.now() + TimeSpan(0,0,1,0));
      }
      break; 
  }  
}
int os=0;
void loop() {
  if(!(count%521)){
    checkButtons();
    os=now.second();
    now = rtc.now();
    if(now.unixtime()>ltime+300){logBaro();logTemp();ltime=now.unixtime();}
  }
  switch (mode){
    case MODE_TIME:
      switch(timemode){
        case 5:
          if(!(count%97)){
            showTimeMatrix();
            showTime7();
          }
          break;
        default:
          if(!(count%491)){
            if(now.second()!=os){
              showTimeMatrix();
              showTime7();
            }
          }
      }
      break;
    case MODE_TEMP:
      if(!(count%10000)){
        temp = bmp.readTemperature()*9/5+32;
        showTemp7();
      }
    break;
    case MODE_BARO:
      if(!(count%10000)){
        baro = bmp.readPressure()*0.0002953;
        showBaro7();
      }
  }
 
  count++;

  drawMatrixRow(count%8);
  delayMicroseconds(500);
}
