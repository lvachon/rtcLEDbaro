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

#define B_SENSE 20
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
#define BTN_1 12
#define BTN_2 13
#define BTN_3 14
#define MODE_COUNT 10
#define MODE_TIME 0
#define MODE_BARO 1
#define MODE_TEMP 3
#define MODE_BATT 2


const int rowpins[] = {2,3,4,5,6,7,8,9};   
const int colpins[] = {C_0,C_1,C_2,C_3};
const unsigned char sevenSegments[] = {0x3F,0x06,0x5B,0x27,0x66,0x6D,0x7D,0x07,0x7F,0x67};
unsigned char matrix[8] = {0,1,3,7,15,31,63,127};
float tdot[8] = {0,0,0,0,0,0,0,0};
float pdot[8] = {0,0,0,0,0,0,0,0};
float vdot[8] = {0,0,0,0,0,0,0,0};
float tmax=-99999;
float tmin=99999;
float bmax=-99999;
float bmin=99999;
const float vmin=3.3;
const float vmax=4.0;
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



void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  bmp.begin();
  rtc.begin();
 
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
  digitalWrite(C_0,addr&1);
  digitalWrite(C_1,addr&2);
  digitalWrite(C_2,addr&4);
  digitalWrite(C_3,addr&8);

}

void set7(int char_addr, int digit,boolean point){
  sev.writeDigitNum(char_addr,digit,point);
}

void drawMatrixRow(int row){
  for(int i=0;i<8;i++){
    digitalWrite(rowpins[i],LOW);
  }
  setCathodeAddress(row);
  for(int i=0;i<8;i++){
    boolean val = (matrix[row]>>i)&1;
    digitalWrite(rowpins[i],val);
  }
}

void setMatrixPixel(int row,int col,boolean value){
  if(value){
    matrix[row]=matrix[row]|(1<<col);
  }else{
    matrix[row]=matrix[row]&(127-(1<<col));
  }
}
void fifoIn(float *array,float val,int len){
  for(int i=0;i<len-1;i++){
    array[i]=array[i+1];
  }
  array[len-1]=val;
}

void fifoToMatrix(float *array,float fmin,float fmax){
  for(int i=0;i<8;i++){
    int val = 8*(array[i]-fmin)/(fmax-fmin);
    for(int j=0;j<8;j++){
      setMatrixPixel(j,7-i,j<=val);
    }
  }
}

void showTime7(){
  int h = now.hour()/10;
  set7(CHAR_0,h,false);
  h=now.hour()%10;
  set7(CHAR_1,h,false);
  h=now.minute()/10;
  set7(CHAR_2,h,false);
  h=now.minute()%10;
  set7(CHAR_3,h,false);
  sev.drawColon(true);
  sev.writeDisplay();
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

void showBatt7(){
  sev.print(vbatt);
  sev.drawColon(false);
  sev.writeDisplay();
}

void logBaro(){
  baro = bmp.readPressure()*0.0002953;
  fifoIn(pdot,baro,8);
  bmax=-9999;
  bmin-9999;
  for(int i=0;i<8;i++){
    if(pdot[i]>bmax){bmax=pdot[i];}
    if(pdot[i]<bmin){bmin=pdot[i];}
  }
  if(bmin==bmax){bmin-=0.01;bmax+=0.01;}
}

void logTemp(){
  temp = bmp.readTemperature()*9/5+32;
  if(temp>tmax){tmax=temp;}
  if(temp<tmin){tmin=temp;}
  if(tmin==tmax){tmin-=0.1;tmax+=0.1;}
  fifoIn(tdot,temp,8);
  tmax=-9999;
  tmin-9999;
  for(int i=0;i<8;i++){
    if(tdot[i]>tmax){tmax=tdot[i];}
    if(tdot[i]<tmin){tmin=tdot[i];}
  }
  if(tmin==tmax){tmin-=0.01;tmax+=0.01;}
}

void timeMatrix0(){
  //Show unixtime in binary form, 2-tall pixels
  unsigned long u = now.unixtime();
  for(int j=0;j<4;j+=1){
    for(int i=0;i<8;i++){
      setMatrixPixel(7-j*2,i,(u>>(i+24-j*8))&1);
      setMatrixPixel(7-j*2-1,i,(u>>(i+24-j*8))&1);
    }
  }
}

void timeMatrix1(){
  //Shows progress bars for seconds,minutes, hours,days,and months
  matrix[0]=0;
  matrix[1]=0;
  matrix[2]=0;
  matrix[3]=0;
  matrix[4]=0;
  matrix[5]=0;
  matrix[6]=0;
  matrix[7]=0;
  
  int x = 8*(now.second()+3.5)/60;
  for(int i=0;i<x&&i<8;i++){
    setMatrixPixel(i,0,true);
  }
  x = 8*(now.second())/60;
  for(int i=0;i<x;i++){
    setMatrixPixel(i,1,true);
  }
  
  x = 8*(now.minute()+3.5)/60;
  for(int i=0;i<x&&i<8;i++){
    setMatrixPixel(i,2,true);
  }
  x = 8*(now.minute())/60;
  for(int i=0;i<x;i++){
    setMatrixPixel(i,3,true);
  }
  
  x = 8*(now.hour()+1.5)/24;
  for(int i=0;i<x&&i<8;i++){
    setMatrixPixel(i,4,true);
  }
  x = 8*(now.hour())/24;
  for(int i=0;i<x;i++){
    setMatrixPixel(i,5,true);
  }
  x = 8*now.day()/31;
  
  for(int i=0;i<x;i++){
    setMatrixPixel(i,6,true);
  }
  
  x = 8*now.month()/12;
  for(int i=0;i<x;i++){
    setMatrixPixel(i,7,true);
  }
  
}
void timeMatrix2(){
  //Show big 'ol digits for the seconds
  int a = now.second()/10;
  int b = now.second()%10;
  for(int i=0;i<8;i++){
    matrix[i]=matdig[a][7-i]<<4|matdig[b][7-i];
  }
}

void timeMatrix3(){
  //Show local time H:M:S in binary
  int a = now.second();
  int b = now.minute();
  int c = now.hour();
  matrix[0]=0;
  matrix[1]=0;
  matrix[2]=0;
  matrix[3]=0;
  matrix[4]=0;
  matrix[5]=0;
  matrix[6]=0;
  matrix[7]=0;

  for(int i=0;i<6;i++){
    setMatrixPixel(1+i,0,(a>>i)&1);
    setMatrixPixel(1+i,1,(a>>i)&1);
    setMatrixPixel(1+i,3,(b>>i)&1);
    setMatrixPixel(1+i,4,(b>>i)&1);
    setMatrixPixel(1+i,6,(c>>i)&1);
    setMatrixPixel(1+i,7,(c>>i)&1);
  }
  
}

void timeMatrix4(){
  //A spiral that grows one led per second
  matrix[0]=0;
  matrix[1]=0;
  matrix[2]=0;
  matrix[3]=0b00011000;
  matrix[4]=0b00011000;
  matrix[5]=0;
  matrix[6]=0;
  matrix[7]=0;
  int a = now.second();
  /*if(a%2){
    matrix[3]=0b00011000;
    matrix[4]=0b00011000;
     
  }else{
     matrix[3]=0;
     matrix[4]=0;
  }*/
  if(a==0){a=60;} 
  int x=5;
  int y=4;
  int dx=0;
  int dy=1;
  if(a>0){
   setMatrixPixel(y-dy,x-dx,true); 
  }
  a--;
  int sidemax=2;
  int curside=0;
  while(a>0){
    setMatrixPixel(y,x,true);
    a--;
    curside++;
    if(curside==sidemax){
      curside=0;
      if(dy!=0){
        sidemax++;        
        dx=dy*-1;
        dy=0;
      }else{
        dy=dx;
        dx=0;
      }
    }
    y+=dy;
    x+=dx;
  }
}

int timemode=4;
void showTimeMatrix(){
  switch(timemode){
    case 0:
      timeMatrix0();
      break;
    case 1:
      timeMatrix2();
      break;
    case 2:
      timeMatrix3();
      break;
    case 3:
      timeMatrix4();
      break;
  }
}

void showBattMatrix(){
  matrix[7]=0b00011000;
  matrix[6]=0b00111100;
  matrix[5]=0b00100100;
  matrix[4]=0b00100100;
  matrix[3]=0b00100100;
  matrix[2]=0b00100100;
  matrix[1]=0b00100100;
  matrix[0]=0b00111100;
  int h = max(0,min(5,5*(vbatt-vmin)/(vmax-vmin)));
  for(int i=7-h;i<7;i++){
    matrix[7-i]=matrix[7-i]|0b00011000;
  }
}

void blankMatrix(){
  for(int i=0;i<ROW_COUNT;i++){
    digitalWrite(rowpins[i],LOW);
  }
}

long count=0;
int mode=0;

void checkButtons(){
  int b=0;
  noInterrupts();
  for(int i=9;i<13;i++){
    setCathodeAddress(i);
    b = b*2+(analogRead(A6)<400);
  }
  interrupts();
  switch (b){
    case 0:
    default:
      mode=MODE_TIME;
      timemode=0;
      break; 
    case 1:
      mode=MODE_TIME;
      timemode=1;
      break; 
    case 2:
      mode=MODE_TIME;
      timemode=2;
      break; 
    case 3:
      mode=MODE_TIME;
      timemode=3;
      break; 
    case 4:
      mode=MODE_BARO;
      break; 
    case 5:
      mode=MODE_TEMP;
      break; 
    case 6:
      mode=MODE_BATT;
      break; 
  }
  
  
}


unsigned long ltime=0;

void loop() {
  count++; 
  if(!(count%1024)){
    blankMatrix();
    checkButtons();
    now = rtc.now();
    switch (mode%MODE_COUNT){
      case MODE_BATT:
        vbatt = 0.0049*analogRead(A7);
        showBattMatrix();
        showBatt7();
        break;
      case MODE_TEMP:
      case MODE_BARO:
        break;
      case MODE_TIME:
      default:
        showTimeMatrix();
        showTime7();
        break;
    }
  }
  if(count>32768){
    switch (mode){
      case MODE_BARO:
        baro = bmp.readPressure()*0.0002953;
        fifoToMatrix(pdot,bmin,bmax);
        showBaro7();
        break;
      case MODE_BATT:
        vbatt = 0.0049*analogRead(A7);
        showBattMatrix();
        showBatt7();
        break;
      case MODE_TEMP:
        temp = bmp.readTemperature()*9/5+32;
        fifoToMatrix(tdot,tmin,tmax);
        showTemp7();
        break;
      case MODE_TIME:
      default:
        //now = rtc.now();
        if(now.unixtime()>ltime+300){logBaro();logTemp();ltime=now.unixtime();}
        showTimeMatrix();
        showTime7();
        break;
    }
    count=0;
  }
  
  drawMatrixRow(count%8);
  
  
}
