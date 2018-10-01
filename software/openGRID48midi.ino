#include <FastLED.h>
#include <Keypad.h>
#include <MIDIUSB.h>
#include <pitchToNote.h>
#include <frequencyToNote.h>

#define LED_PIN     A0
#define NUM_LEDS    32
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

//midi stuff
#define MIDI_CHANNEL 0   //Default MIDI channel



int colorSTEP = 15; //change on color based on press

long int lastACTION = 0;

const byte ROWS = 4; //A B C D
const byte COLS = 8; //A1 A2 B1 B2 C1 C2 D1 D2
char keys[ROWS][COLS] = {
{'4','5','C','D','K','L','S','T'},
{'3','6','B','E','J','M','R','U'},
{'2','7','A','F','I','N','Q','V'},
{'1','8','9','G','H','O','P','W'},
};

byte note[ROWS][COLS] = {                                                 
{60,62,64,65,67,69,71,1}, //first row                                                          
{2,61,63,3,66,68,70,4}, //2nd                                                         
{40,41,42,43,44,45,46,47}, //3rd                                                         
{32,33,34,35,36,38,38,39}, //4th                                                         
};

byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad A, B, C, D
byte colPins[COLS] = {10, 16, 14, 15, A1, A2, A3, 9}; //connect to the column pinouts of the keypad 1, 2

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

byte colorSTATE[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

byte colorMATRIX[ROWS][COLS] = {
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0}, 
};

void setup(){
    Serial.begin(9600);
    delay(3000); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);
    startANIMATION();
}

void loop(){
    buttonREAD();
    colorDISPLAY();
  if ((millis()-lastACTION)>500){
    lastACTION = millis();
    colorDECAY();  
  }
}

void startANIMATION(){
  for (int i=0;i<32;i++){
    colorSTATE[i]=(i*8)+1;
  }
  colorDISPLAY();
  //delay(250);
}

void colorDECAY(){
  for (int i=0;i<32;i++){
    if(colorSTATE[i]>4){
      colorSTATE[i]=colorSTATE[i]-2;
    }
    else
      colorSTATE[i]=0;
  }
  colorDISPLAY();
}

void colorDISPLAY(){
   for (int i=0;i<32;i++){
    if (colorSTATE[i]==0){
      leds[i] = CRGB::Black;
    }
    else {
      leds[i] = CHSV(colorSTATE[i],255,255);
    }
   }
   FastLED.show();
}

/*
 * Function: noteOn()
 * Description: Sends a noteOn MIDI message.
 * Input:
 *  -Channel: MIDI Channel. Recommended 0.
 *  -Pith: Note value, from 0 to 255
 *  -Velocity: Note intensity, form 0 to 255
 * Output:
 *  -Void.
 */
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, uint8_t(0x90 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

/*
 * Function: noteOff()
 * Description: Sends a noteOff MIDI message.
 * Input:
 *  -Channel: MIDI Channel. Recommended 0.
 *  -Pith: Note value, from 0 to 255
 *  -Velocity: Note intensity, form 0 to 255
 * Output:
 *  -Void.
 */
void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, uint8_t(0x80 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void buttonREAD(){
  if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
            lastACTION = millis();
            {
                switch (kpd.key[i].kchar) {
                    case '1':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[0][0],127);
                        MidiUSB.flush();
                        if (colorSTATE[0]<255){
                          colorSTATE[0] = colorSTATE[0]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        
                        //colorSTATE[0] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[0][0],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '2':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[1][0],127);
                        MidiUSB.flush();
                        if (colorSTATE[1]<255){
                          colorSTATE[1] = colorSTATE[1]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[1] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[1][0],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '3':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[2][0],127);
                        MidiUSB.flush();
                        if (colorSTATE[2]<255){
                          colorSTATE[2] = colorSTATE[2]+colorSTEP;
                        }
                      break;
                       case HOLD:
                        
                        //colorSTATE[1] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[2][0],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '4':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[3][0],127);
                        MidiUSB.flush();
                        if (colorSTATE[3]<255){
                          colorSTATE[3] = colorSTATE[3]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[1] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[3][0],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '5':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[3][1],127);
                        MidiUSB.flush();
                        if (colorSTATE[4]<255){
                          colorSTATE[4] = colorSTATE[4]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[4] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[3][1],0);
                        MidiUSB.flush();  
                      break;
                      case IDLE:
                       
                     break;
                    }
                break;
                    case '6':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[2][1],127);
                        MidiUSB.flush();
                        if (colorSTATE[5]<255){
                          colorSTATE[5] = colorSTATE[5]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[5] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[2][1],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '7':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[1][1],127);
                        MidiUSB.flush();
                        if (colorSTATE[6]<255){
                          colorSTATE[6] = colorSTATE[6]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[6] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[1][1],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '8':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[0][1],127);
                        MidiUSB.flush();
                        if (colorSTATE[7]<255){
                          colorSTATE[7] = colorSTATE[7]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[7] = 0;
                      break;
                      case RELEASED:
                        noteOn(MIDI_CHANNEL,note[0][1],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '9':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[0][2],127);
                        MidiUSB.flush();
                        if (colorSTATE[8]<255){
                          colorSTATE[8] = colorSTATE[8]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[8] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[0][2],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'A':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[1][2],127);
                        MidiUSB.flush();
                        if (colorSTATE[9]<255){
                          colorSTATE[9] = colorSTATE[9]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[9] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[1][2],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'B':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[2][2],127);
                        MidiUSB.flush();
                        if (colorSTATE[10]<255){
                          colorSTATE[10] = colorSTATE[10]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[10] = 0;
                      break;
                      case RELEASED:
                        noteOn(MIDI_CHANNEL,note[2][2],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'C':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[3][2],127);
                        MidiUSB.flush();
                        if (colorSTATE[11]<255){
                          colorSTATE[11] = colorSTATE[11]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[11] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[3][2],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'D':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[3][3],127);
                        MidiUSB.flush();
                        if (colorSTATE[12]<255){
                          colorSTATE[12] = colorSTATE[12]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[12] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[3][3],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'E':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[2][3],127);
                        MidiUSB.flush();
                        if (colorSTATE[13]<255){
                          colorSTATE[13] = colorSTATE[13]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[13] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[2][3],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'F':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[1][3],127);
                        MidiUSB.flush();
                        if (colorSTATE[14]<255){
                          colorSTATE[14] = colorSTATE[14]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[14] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[1][3],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'G':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[0][3],127);
                        MidiUSB.flush();
                        if (colorSTATE[15]<255){
                          colorSTATE[15] = colorSTATE[15]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[15] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[0][3],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'H':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[0][4],127);
                        MidiUSB.flush();
                        if (colorSTATE[16]<255){
                          colorSTATE[16] = colorSTATE[16]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[16] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[0][4],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'I':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[1][4],127);
                        MidiUSB.flush();
                        if (colorSTATE[17]<255){
                          colorSTATE[17] = colorSTATE[17]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[17] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[1][4],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'J':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[2][4],127);
                        MidiUSB.flush();
                        if (colorSTATE[18]<255){
                          colorSTATE[18] = colorSTATE[18]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[18] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[2][4],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'K':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[3][4],127);
                        MidiUSB.flush();
                        if (colorSTATE[19]<255){
                          colorSTATE[19] = colorSTATE[19]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[19] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[3][4],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'L':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[3][5],127);
                        MidiUSB.flush();
                        if (colorSTATE[20]<255){
                          colorSTATE[20] = colorSTATE[20]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[20] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[3][5],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                     case 'M':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[2][5],127);
                        MidiUSB.flush();
                        if (colorSTATE[21]<255){
                          colorSTATE[21] = colorSTATE[21]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[21] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[2][5],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                     case 'N':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[1][5],127);
                        MidiUSB.flush();
                        if (colorSTATE[22]<255){
                          colorSTATE[22] = colorSTATE[22]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[22] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[1][5],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'O':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[0][5],127);
                        MidiUSB.flush();
                        if (colorSTATE[23]<255){
                          colorSTATE[23] = colorSTATE[23]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[23] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[0][5],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'P':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[0][6],127);
                        MidiUSB.flush();
                        if (colorSTATE[24]<255){
                          colorSTATE[24] = colorSTATE[24]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[24] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[0][6],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'Q':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[1][6],127);
                        MidiUSB.flush();
                        if (colorSTATE[25]<255){
                          colorSTATE[25] = colorSTATE[25]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[25] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[1][6],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'R':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[2][6],127);
                        MidiUSB.flush();
                        if (colorSTATE[26]<255){
                          colorSTATE[26] = colorSTATE[26]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[26] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[2][6],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'S':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[3][6],127);
                        MidiUSB.flush();
                        if (colorSTATE[27]<255){
                          colorSTATE[27] = colorSTATE[27]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[27] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[3][6],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'T':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[3][7],127);
                        MidiUSB.flush();
                        if (colorSTATE[28]<255){
                          colorSTATE[28] = colorSTATE[28]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[28] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[3][7],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'U':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[2][7],127);
                        MidiUSB.flush();
                        if (colorSTATE[29]<255){
                          colorSTATE[29] = colorSTATE[29]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[29] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[2][7],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'V':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[1][7],127);
                        MidiUSB.flush();
                        if (colorSTATE[30]<255){
                          colorSTATE[30] = colorSTATE[30]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[30] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[1][7],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'W':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        noteOn(MIDI_CHANNEL,note[0][7],127);
                        MidiUSB.flush();
                        if (colorSTATE[31]<255){
                          colorSTATE[31] = colorSTATE[31]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        //colorSTATE[31] = 0;
                      break;
                      case RELEASED:
                        noteOff(MIDI_CHANNEL,note[0][7],0);
                        MidiUSB.flush();
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
               }
             }
        }
    }
}
