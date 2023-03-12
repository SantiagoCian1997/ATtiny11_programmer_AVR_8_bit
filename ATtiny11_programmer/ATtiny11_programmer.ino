// PROGRAMADOR PARA ATTINY 11
// Repositorio publico (.exe interfaz de usuario):
// Proyecto realizado por Santiago Cian 
// Contacto/ayuda: santiagocian97@gmail.com
// Basado en el proyecto: http://www.rickety.us/2010/03/arduino-avr-high-voltage-serial-programmer/



#define  RST_PB5     13   //PB5 transistor 11.5 - 12.5V
#define  SCI_PB3     12   //PB3 SERIAL CLOCK INPUT
#define  SDO_PB2     11   //PB2 SERIAL DATA OUTPUT
#define  SII_PB1     10   //PB1 SERIAL INSTR. INPUT
#define  SDI_PB0     9    //PB0 SERIAL DATA INPUT
#define  ToVcc       8    //Vcc del ATtiny11

void setup()
{
  pinMode(ToVcc, OUTPUT);
  pinMode(RST_PB5, OUTPUT);
  pinMode(SDI_PB0, OUTPUT);
  pinMode(SII_PB1, OUTPUT);
  pinMode(SCI_PB3, OUTPUT);
  pinMode(SDO_PB2, OUTPUT);  
  digitalWrite(RST_PB5, HIGH);  
  Serial.begin(38400);
  Serial.println("Programador_ATtiny11_v1.0");
}

char L;
boolean interfazEnable=false;
void loop()
{
  if (Serial.available() > 0) {
    delay(60);
    L = Serial.read();
    switch (L) {
      case 'i' : {// "iON" o "iOFF"
          if(Serial.read()=='O') {
            L=Serial.read();
            if(L=='N') {
              interfazEnable=true;
              Serial.println("Programador_ATtiny11_v1.0");
              programarON();
            }
            if(L=='F'){
              interfazEnable=false;
              programarOFF();
            }
          }
        } break;
      case ':' : {
          lineaHEX();
        } break;
      case 'f': {
          readFuses();
        } break;
      case 'W': {
          if(Serial.read()=='f'){
            escribir_fusible(((charToHEX(Serial.read())<<4)&0b11110000)+(charToHEX(Serial.read())&0b00001111));
          }
        } break;
      case 's': {
          readSignature();
        } break;
      case 'e' : {
          chipErase();
          Serial.println("Fin borrado");
        } break;
      case 'r' : {
        if(!interfazEnable)programarON();
            Serial.println("leyendo flash");
            for (int i = 0; i < 512; i++) {
              Serial.print("flash lec:");
              Serial.print(i,HEX);
              Serial.print(":");
              Serial.print(readFlash(i), HEX);
              Serial.println(":");
            }
            Serial.println("fin lectura flash");
         if(!interfazEnable) programarOFF();
        } break;
      case '?' : {
          imprimirRef();
        } break;
      default : {
          imprimirRef();
        } break;
    }
    while (Serial.available() > 0) Serial.read();
  }
}
void lineaHEX() {
  byte tamano, type, datoAlto, datoBajo;
  unsigned int address = 0;
  tamano = charToHEX(Serial.read());
  tamano <<= 4;
  tamano += charToHEX(Serial.read());

  address = charToHEX(Serial.read());
  address <<= 4;
  address += charToHEX(Serial.read());
  address <<= 4;
  address += charToHEX(Serial.read());
  address <<= 4;
  address += charToHEX(Serial.read());


  type = charToHEX(Serial.read());
  type <<= 4;
  type += charToHEX(Serial.read());

  Serial.print("tamano: ");
  Serial.print(tamano);
  Serial.print(", direccion: ");
  Serial.print(address);
  Serial.print(", tipo: ");
  Serial.println(type);
  if (type == 0) {
    if(!interfazEnable)programarON();
    for (byte i = 0; i < tamano / 2; i++) {
      datoBajo = charToHEX(Serial.read());
      datoBajo <<= 4;
      datoBajo += charToHEX(Serial.read());
      datoAlto = charToHEX(Serial.read());
      datoAlto <<= 4;
      datoAlto += charToHEX(Serial.read());
      escribirFlash((address/2)+i, datoBajo, datoAlto);
      Serial.print("Dir: ");
      Serial.print((address/2)+i, HEX);
      Serial.print(", Bajo: ");
      Serial.print(datoBajo, HEX);
      Serial.print(", Alto: ");
      Serial.println(datoAlto, HEX);

    }
    if(!interfazEnable)programarOFF();
  }
}
byte charToHEX(char H) {
  if (H >= '0' && H <= '9') return ((byte)(H - '0'));
  if (H >= 'A' && H <= 'F') return ((byte)(H - 'A'+10));
  if (H >= 'a' && H <= 'f') return ((byte)(H - 'a'+10));
  return(0);
}
void escribirFlash(unsigned int address, byte bajo, byte alto) {
  byte altoAddress = 0;
  byte bajoAddress = 0;
  altoAddress = (byte)((address & 0b100000000) >> 8);
  bajoAddress = (byte)(address & 0b11111111);
  shiftOut( 0x10, 0x4C);//0_0001_0000_00  0_0100_1100_00
  shiftOut( altoAddress, 0x1C);//0_0000_000a_00  0_0001_1100_00
  shiftOut( bajoAddress, 0x0C);//0_bbbb_bbbb_00  0_0000_1100_00
  //escribir bits bajos de la flash
  shiftOut( bajo, 0x2C);//0_iiii_iiii_00  0_0010_1100_00
  shiftOut( 0x00, 0x64);//0_0000_0000_00  0_0110_0100_00
  shiftOut( 0x00, 0x6C);//0_0000_0000_00  0_0110_1100_00
  //escribir bits altos de la flash
  shiftOut( alto, 0x3C);//0_iiii_iiii_00  0_0011_1100_00
  shiftOut( 0x00, 0x74);//0_0000_0000_00  0_0111_0100_00
  shiftOut( 0x00, 0x7C);//0_0000_0000_00  0_0111_1100_00
}
void chipErase() {
  if(!interfazEnable)programarON();
  Serial.println("Blanqueando chip");
  //chip Erase
  shiftOut( 0x80, 0x4C);//0_1000_0000_00  0_0100_1100_00
  shiftOut( 0x00, 0x64);//0_0000_0000_00  0_0110_0100_00
  shiftOut( 0x00, 0x6C);//0_0000_0000_00  0_0110_1100_00
  shiftOut( 0x00, 0x4C);//0_0000_0000_00  0_0100_1100_00
  delay(3000);
  if(!interfazEnable)programarOFF();
}
byte shiftOut (byte val1, byte val2) {
  digitalWrite(SDI_PB0, LOW);
  digitalWrite(SII_PB1, LOW);
  int inBits = 0;
  while (!digitalRead(SDO_PB2));
  unsigned int dout = (unsigned int) (val1 << 2);
  unsigned int iout = (unsigned int) (val2 << 2);
  dout &= 0b00001111111100;
  iout &= 0b00001111111100;
  for (int i = 10; i >= 0; i--)  {
    if ((dout & (1 << i)) > 0) digitalWrite(SDI_PB0, 1);
    else digitalWrite(SDI_PB0, 0);
    if ((iout & (1 << i)) > 0) digitalWrite(SII_PB1, 1);
    else digitalWrite(SII_PB1, 0);
    inBits <<= 1;
    inBits |= digitalRead(SDO_PB2);
    digitalWrite(SCI_PB3, HIGH);
    delayMicroseconds(2);
    digitalWrite(SCI_PB3, LOW);
    delayMicroseconds(2);
  }
  digitalWrite(SDI_PB0, LOW);
  digitalWrite(SII_PB1, LOW);
  delayMicroseconds(10);
  return inBits >> 2;
}
unsigned int readFlash(unsigned int address) {
  unsigned int baja = 0;
  unsigned int alta = 0;
  byte altoAddress = 0;
  byte bajoAddress = 0;
  altoAddress = (byte)((address & 0b100000000) >> 8);
  bajoAddress = (byte)(address & 0b11111111);
  //Set address
  shiftOut(0x02, 0x4C); //0_0000_0010_00  0_0100_1100_00
  shiftOut(altoAddress, 0x1C); //0_0000_000a_00  0_0001_1100_00
  shiftOut(bajoAddress, 0x0C); //0_bbbb_bbbb_00  0_0000_1100_00
  //read parte baja
  shiftOut(0x00, 0x68); //0_0000_0000_00  0_0110_1000_00
  baja = shiftOut(0x00, 0x6C); //0_0000_0000_00  0_0110_1100_00  o_oooo_ooox_xx
  //read parte alta
  shiftOut(0x00, 0x78); //0_0000_0000_00  0_0111_1000_00
  alta = shiftOut(0x00, 0x7C); //0_0000_0000_00  0_0111_1100_00  o_oooo_ooox_xx
  baja &= 0b11111111;
  baja += (alta & 0b11111111) << 8;
  return baja;
}
void readSignature () {
  unsigned int firma;
  programarON();
  shiftOut(0x08, 0x4C);//0_0000_1000_00  0_0100_1100_00
  shiftOut(0x01, 0x0C);//0_0000_00bb_00  0_0000_1100_00
  shiftOut(0x00, 0x68);//0_0000_0000_00  0_0110_1000_00
  firma = shiftOut(0x00, 0x6C);//0_0000_0000_00  0_0110_1100_00  o_oooo_ooox_xx
  shiftOut(0x08, 0x4C);//0_0000_1000_00  0_0100_1100_00
  shiftOut(0x02, 0x0C);//0_0000_00bb_00  0_0000_1100_00
  shiftOut(0x00, 0x68);//0_0000_0000_00  0_0110_1000_00
  firma<<=8;
  firma += shiftOut(0x00, 0x6C);//0_0000_0000_00  0_0110_1100_00  o_oooo_ooox_xx
  programarOFF();
  Serial.print("Firma: ");
  Serial.println(firma,HEX);
}
void readFuses() {
  programarON();
  byte Leido;
  shiftOut( 0x04, 0x4C);
  shiftOut( 0x00, 0x68);
  Leido = shiftOut( 0x00, 0x6C);
  Leido>>1;
  Leido|=0b11100000;
  Serial.print("Fuse: ");
  if(Leido<=15) Serial.print("0");
  Serial.println(Leido, HEX);
  programarOFF();
}
void escribir_fusible(byte fuse_valor){
  programarON();
  Serial.print("Escribiendo fusible: ");
  Serial.println(fuse_valor,HEX);

  shiftOut( 0x80      , 0x4C);//0_0100_0000_00    0_0100_1100_00
  shiftOut( fuse_valor, 0x2C);//0_0007_6543_00    0_0010_1100_00
  shiftOut( 0x0       , 0x64);//0_0000_0000_00    0_0110_0100_00
  delay(5);//requerido para escribir el fusible
  shiftOut( 0x0       , 0x6C);//0_0000_0000_00    0_0110_1100_00
  

  
  programarOFF();
  
}
void imprimirRef() {
  Serial.println("'?'  comandos");
  Serial.println("':'  comienzo linea .hex");
  Serial.println("'r'  leer flash completa");
  Serial.println("'f'  leer fusibles");
  Serial.println("'Wf' escribir fusibles");
  Serial.println("'s'  leer signature");
  Serial.println("'e'  erase flash");
  Serial.println("");
  while (Serial.available() > 0) Serial.read();
}

void programarON() {
  Serial.println("Programacion_ON");
  pinMode(SDO_PB2, OUTPUT);  //Temporary
  digitalWrite(SDI_PB0, LOW);
  digitalWrite(SII_PB1, LOW);
  digitalWrite(SCI_PB3, LOW);
  digitalWrite(SDO_PB2, LOW);
  digitalWrite(RST_PB5, HIGH);  // Level shifter is inverting, this shuts off 12V
  // Enter High-voltage Serial programming mode
  digitalWrite(ToVcc, HIGH);  // Apply VCC to start programming process
  delayMicroseconds(30);
  digitalWrite(SCI_PB3, LOW);
  delayMicroseconds(10);
  digitalWrite(SCI_PB3, HIGH);
  delayMicroseconds(10);
  digitalWrite(SCI_PB3, LOW);
  delayMicroseconds(10);
  digitalWrite(SCI_PB3, HIGH);
  delayMicroseconds(10);
  digitalWrite(SCI_PB3, LOW);
  delayMicroseconds(10);
  digitalWrite(SCI_PB3, HIGH);
  delayMicroseconds(10);
  digitalWrite(SCI_PB3, LOW);
  delayMicroseconds(10);
  digitalWrite(SCI_PB3, HIGH);
  delayMicroseconds(10);
  digitalWrite(SCI_PB3, LOW);
  delayMicroseconds(30);
  digitalWrite(RST_PB5, LOW);   //Turn on 12v
  delayMicroseconds(20);
  pinMode(SDO_PB2, INPUT);   //Release DATAIN
  delayMicroseconds(300);
}
void programarOFF() {
  digitalWrite(SCI_PB3, LOW);
  delayMicroseconds(30);
  digitalWrite(RST_PB5, LOW);
  delayMicroseconds(30);
  digitalWrite(ToVcc, LOW);
  delay(50);
  digitalWrite(RST_PB5, HIGH);
  Serial.println("Programacion_OFF");
}
