void blankMatrix(){
  PORTD&=0b00000011;
  PORTB&=0b11111100;
  /*for(int i=0;i<ROW_COUNT;i++){
    digitalWrite(rowpins[i],LOW);
  }*/
}

void fifoToMatrix(float *array,float fmin,float fmax){
  int val;
  for(int i=0;i<8;i++){
    if(array[i]==0){val=0;}else{
      val = 8*(array[i]-fmin)/(fmax-fmin);
    }
    for(int j=0;j<8;j++){
      setMatrixPixel(j,7-i,j<=val);
    }
  }
}


void drawMatrixRow(int row){
  PORTD&=0b00000011;
  PORTB&=0b11111100;
  
  
  /*for(int i=0;i<8;i++){
    digitalWrite(rowpins[i],LOW);
  }*/
  setCathodeAddress(row);
  /*for(int i=0;i<8;i++){
    boolean val = (matrix[row]>>i)&1;
    digitalWrite(rowpins[i],val);
  }*/
  
  PORTD+=(matrix[row]&0b00111111)<<2;
  PORTB+=((matrix[row]&0b11000000)/64);
  /*Serial.print(matrix[row])
  Serial.print(",");
  Serial.print*/
}

void setMatrixPixel(int row,int col,boolean value){
  if(value){
    matrix[row]=matrix[row]|(1<<col);
  }else{
    matrix[row]=matrix[row]&(127-(1<<col));
  }
}
