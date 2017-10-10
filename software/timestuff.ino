
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
