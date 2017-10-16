void blankMatrix(){
  PORTD&=0b00000011;
  PORTB&=0b11111100;
}

void fifoToMatrix(float *array,float fmin,float fmax){
  int val,index;
 
  for(int i=0;i<8;i++){
    index = (32-8*gstep)+i*gstep;
    if(array[index]==0){
      val=-1;
    }else{
      val = 8*(array[index]-fmin)/(fmax-fmin);
    }
    for(int j=0;j<8;j++){
      setMatrixPixel(j,7-i,j<=val);
    }
  }
}


void drawMatrixRow(int row){
  PORTD&=0b00000011;
  PORTB&=0b11111100;
  setCathodeAddress(row);
 
  PORTD+=(matrix[row]&0b00111111)<<2;
  PORTB+=((matrix[row]&0b11000000)/64);
  
  if(invert_matrix){
    PORTD=PORTD^0b11111100;
    PORTB=PORTB^0b00000011;  
  }
  

}

void setMatrixPixel(int row,int col,boolean value){
  if(value){
    matrix[row]=matrix[row]|(1<<col);
  }else{
    matrix[row]=matrix[row]&(127-(1<<col));
  }
}
