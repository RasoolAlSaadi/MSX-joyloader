
;******************************************************************************
; Joystick I/O port ROM loader v0.3
; MSX ROM loader through general purpose I/O (Joystick) port
; Developed by Rasool Al-Saadi (2020)
; works with 16KB and 32KB roms
; Some ideas/code had been taken from https://github.com/rolandvans/msxvdisk 
;*******************************************************************************

#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
//Sd2Card card;
//SdVolume volume;
//SdFile root;
unsigned long duration;
#define WL 140

const byte cas[] PROGMEM=
{ 0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,'R','O','M','L','D','R',
  0x00, 0xD0, 0x00, 0xD2,
  0x00, 0xD0, 0x21, 0x91, 0xD1, 0xCD, 0x3C, 0xD1, 0x21, 0x60, 0xEA, 0x00,
  0x2B, 0x7C, 0xB5, 0xC2, 0x09, 0xD0, 0xF3, 0xCD, 0x0C, 0xD1, 0xCD, 0xF6,
  0xD0, 0x0E, 0x01, 0x21, 0xFD, 0xD1, 0xCD, 0x89, 0xD0, 0xCD, 0xF6, 0xD0,
  0xFB, 0x21, 0xFD, 0xD1, 0x7E, 0xA7, 0xCA, 0x41, 0xD0, 0xE5, 0xCD, 0x3C,
  0xD1, 0x3E, 0x0D, 0xCD, 0xA2, 0x00, 0x3E, 0x0A, 0xCD, 0xA2, 0x00, 0xE1,
  0x11, 0x10, 0x00, 0x19, 0xC3, 0x26, 0xD0, 0x21, 0xF9, 0xD1, 0xCD, 0x46,
  0xD1, 0xF3, 0xCD, 0x0C, 0xD1, 0x21, 0xF9, 0xD1, 0x11, 0x04, 0x00, 0xCD,
  0xCA, 0xD0, 0xCD, 0xF6, 0xD0, 0x0E, 0x01, 0x21, 0xFD, 0xD1, 0xCD, 0x89,
  0xD0, 0xCD, 0xF6, 0xD0, 0x21, 0xDA, 0xD1, 0xCD, 0x3C, 0xD1, 0xCD, 0x17,
  0xD1, 0xF3, 0xCD, 0x0C, 0xD1, 0x3A, 0xFD, 0xD1, 0x4F, 0x21, 0x00, 0x40,
  0xCD, 0x89, 0xD0, 0x0D, 0xC2, 0x76, 0xD0, 0xFB, 0x21, 0xE7, 0xD1, 0xCD,
  0x3C, 0xD1, 0x2A, 0x02, 0x40, 0xE9, 0xC9, 0x3E, 0x0E, 0xD3, 0xA0, 0xDB,
  0xA2, 0xE6, 0x10, 0xC2, 0x8D, 0xD0, 0x3E, 0x0F, 0xD3, 0xA0, 0xDB, 0xA2,
  0xE6, 0xDF, 0x57, 0xF6, 0x20, 0x5F, 0xCD, 0xA3, 0xD0, 0x06, 0x00, 0x7B,
  0xD3, 0xA1, 0x3E, 0x0E, 0xD3, 0xA0, 0xDB, 0xA2, 0xED, 0x67, 0x3E, 0x0F,
  0xD3, 0xA0, 0x7A, 0xD3, 0xA1, 0x3E, 0x0E, 0xD3, 0xA0, 0xDB, 0xA2, 0xED,
  0x67, 0x3E, 0x0F, 0xD3, 0xA0, 0x23, 0x10, 0xDF, 0x7B, 0xD3, 0xA1, 0xC9,
  0x3E, 0x0F, 0xD3, 0xA0, 0xDB, 0xA2, 0x06, 0x08, 0x4E, 0xE6, 0xDF, 0xD3,
  0xA1, 0xF6, 0x08, 0xCB, 0x09, 0xDA, 0xE0, 0xD0, 0xE6, 0xF7, 0xF6, 0x20,
  0xD3, 0xA1, 0x10, 0xED, 0x23, 0x1B, 0x47, 0x7A, 0xB3, 0x78, 0xC2, 0xD0,
  0xD0, 0xE6, 0xDF, 0xF6, 0x0C, 0xD3, 0xA1, 0xC9, 0x3E, 0x0F, 0xD3, 0xA0,
  0xDB, 0xA2, 0xE6, 0xDF, 0xD3, 0xA1, 0xC9, 0x3E, 0x0F, 0xD3, 0xA0, 0xDB,
  0xA2, 0xE6, 0xBF, 0xD3, 0xA1, 0xC9, 0x3E, 0x0F, 0xD3, 0xA0, 0xDB, 0xA2,
  0xF6, 0x40, 0xD3, 0xA1, 0xC9, 0xCD, 0x38, 0x01, 0x0F, 0x0F, 0x0F, 0x0F,
  0x0F, 0x0F, 0xE6, 0x03, 0x4F, 0x06, 0x00, 0x21, 0xC1, 0xFC, 0x09, 0xB6,
  0x4F, 0x23, 0x23, 0x23, 0x23, 0x7E, 0xE6, 0xC0, 0x0F, 0x0F, 0x0F, 0x0F,
  0xB1, 0x26, 0x40, 0xC3, 0x24, 0x00, 0x7E, 0xA7, 0xC8, 0xCD, 0xA2, 0x00,
  0x23, 0xC3, 0x3C, 0xD1, 0x06, 0x00, 0xCD, 0x9F, 0x00, 0x4F, 0xFE, 0x0D,
  0xCA, 0x89, 0xD1, 0xFE, 0x08, 0xC2, 0x6F, 0xD1, 0x78, 0xA7, 0xCA, 0x48,
  0xD1, 0x3E, 0x08, 0xCD, 0xA2, 0x00, 0x3E, 0x20, 0xCD, 0xA2, 0x00, 0x3E,
  0x08, 0xCD, 0xA2, 0x00, 0x05, 0x2B, 0xC3, 0x48, 0xD1, 0x78, 0xFE, 0x03,
  0xD2, 0x48, 0xD1, 0x79, 0xFE, 0x30, 0xDA, 0x48, 0xD1, 0xFE, 0x3A, 0xD2,
  0x48, 0xD1, 0x77, 0x23, 0x04, 0xCD, 0xA2, 0x00, 0xC3, 0x48, 0xD1, 0x78,
  0xA7, 0xCA, 0x48, 0xD1, 0x36, 0x00, 0xC9, 0x4A, 0x6F, 0x79, 0x73, 0x74,
  0x69, 0x63, 0x6B, 0x20, 0x49, 0x2F, 0x4F, 0x20, 0x70, 0x6F, 0x72, 0x74,
  0x20, 0x52, 0x4F, 0x4D, 0x20, 0x6C, 0x6F, 0x61, 0x64, 0x65, 0x72, 0x20,
  0x76, 0x30, 0x2E, 0x33, 0x0D, 0x0A, 0x44, 0x65, 0x76, 0x65, 0x6C, 0x6F,
  0x70, 0x65, 0x64, 0x20, 0x62, 0x79, 0x20, 0x52, 0x61, 0x73, 0x6F, 0x6F,
  0x6C, 0x20, 0x41, 0x4C, 0x2D, 0x53, 0x61, 0x61, 0x64, 0x69, 0x20, 0x28,
  0x32, 0x30, 0x32, 0x30, 0x29, 0x0D, 0x0A, 0x00, 0x4C, 0x6F, 0x61, 0x64,
  0x69, 0x6E, 0x67, 0x2E, 0x2E, 0x2E, 0x0D, 0x0A, 0x00, 0x73, 0x74, 0x61,
  0x72, 0x74, 0x69, 0x6E, 0x67, 0x20, 0x74, 0x68, 0x65, 0x20, 0x67, 0x61,
  0x6D, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
 };

inline void cas_send1(){
        PORTB |= B00000010;
        delayMicroseconds(WL/2);
        PORTB &= B11111101;
        delayMicroseconds(WL/2);
        PORTB |= B00000010;
        delayMicroseconds(WL/2);
        PORTB &= B11111101;
        delayMicroseconds(WL/2);
}

inline void cas_send0(){
        PORTB |= B00000010;
        delayMicroseconds(WL);
        PORTB &= B11111101;
        delayMicroseconds(WL);
}

void sendcas()
{
  word i,j, bits;
  
  PORTB &= B11111101;
 
  for(i=0;i<sizeof(cas)-1;i++) {
    
    if (i ==0 || i == 16){
      delay(1000);
      for(j=0;j<15000;j++)
        cas_send1();
    }
        
    cas_send0();
    
    for(bits=0;bits<8;bits++)
      if ((pgm_read_byte(&cas[i]) >> bits) & 1)      
          cas_send1();
        else
          cas_send0();
 
    cas_send1();
    cas_send1();
  }

  PORTD |= B01000000;  // we are not ready to send     
}


void send_512_bytes(byte Buffer[])
{
    byte x;
    int i;

    PORTD &= B10111111; //digitalWrite(6, LOW); // we are ready
    for (i=0; i<512; i++) { 
      x = Buffer[i];

      while(!(PIND & B10000000));
      if(!duration)
        duration = micros();
      PORTD = (PORTD & B11000011) | ((x << 2) & B00111100 );
      while((PIND & B10000000));
   
      PORTD = (PORTD & B11000011) | ((x >> 2) & B00111100 );    
      while(!(PIND & B10000000));
    }

    PORTD |= B01000000; //digitalWrite(6, HIGH); // we are not ready  

}

void sendROM(String filename)
{
  unsigned int i, bytes=0, c=0;
  byte Buffer[512];
 
  duration=0;
  
  File dataFile = SD.open(filename, FILE_READ);
  if (dataFile) {
    while (dataFile.available()) {
      dataFile.read(Buffer,512);
      send_512_bytes(Buffer);
      bytes += 512;

      c++;
      Serial.print(c);
      Serial.print("-");
      Serial.println("512 sent");  
    }
    dataFile.close();
  }
  else {
    Serial.println("err opn");
  }
  duration = micros() - duration;
  ;Serial.println("Sending finished!");
  Serial.print("Duration:");
  Serial.print((float)duration/1000000);
  Serial.println("s");
  Serial.print(bytes /((float)duration/1000000));  
  Serial.println("byte/s");
 }


void sendmenu()
{

  byte Buffer[512],b,i=0;
  char str[16];
  File dir;
  
  dir = SD.open("/");
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    if (!entry.isDirectory()){
      itoa(i, str,10);
      strcat(str,"- ");
      strncat(str, entry.name(), 10);
      b = entry.size()/512;
      str[15] = b;
      
      memcpy(&Buffer[i*16], str, 16);
      i++;
      if (i>30) {
        break;
      }
      
      Serial.print(str);
      Serial.println(str[15], DEC);
      
    }
    entry.close();
  }

  dir.close();
  
  Buffer[i*16]=0;
  send_512_bytes(Buffer);
}

void sendelectedROMinfo(int choice)
{

  byte Buffer[512],b,i=0;
  File dir;
  
  dir = SD.open("/");
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    if (!entry.isDirectory()){
        if (i==choice){
          b = entry.size()/512;
          entry.close();
          break;
        }
        i++;
    }
    entry.close();
  }
  dir.close();
  
  Buffer[0]=b;
  send_512_bytes(Buffer);
}

void getromfilename(String *filename, int choice)
{
  int i=0;
  File dir;
  
  dir = SD.open("/");
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    if (!entry.isDirectory()){
        if (i==choice){
          *filename = entry.name();
          entry.close();
          break;
        }
        i++;
    }
    entry.close();
  }
  dir.close();
 
}

int receiveBytes(int n){
  int i,x,k;
  byte a[4];
  
  for(k=0;k<n;k++){
    x=0;
    for (i=0;i<8;i++) {
      while(!(PIND & B10000000));
      delayMicroseconds(2); // allow signal to stablize
      x |= (PINB & 1) << i; 
      while((PIND & B10000000));
    }
    a[k]=x;
  }
  return atoi(a);   
}



// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT); //I/O 1st pin 
  pinMode(7, INPUT); //O pin
    
  pinMode(8, INPUT); //I/O 2nd pin
  pinMode(9, OUTPUT); //Cassette

  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(9, HIGH);


// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  //Serial.println("card initialized.");
  
}


void loop() {
  String filename;
  int choice;
 
  Serial.println("cas");
  sendcas();

 
  Serial.println("menu");
  sendmenu();
  
  Serial.println("read");
  choice = receiveBytes(4);
  PORTD |= B01000000;  // we are not ready to send
  Serial.println(choice);
  
  
  Serial.println("Rinf");
  sendelectedROMinfo(choice);
  getromfilename(&filename, choice);
  
  sendROM(filename);
  Serial.println("done");

}
