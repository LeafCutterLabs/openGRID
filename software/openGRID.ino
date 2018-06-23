#include <FastLED.h>
#include <Keypad.h>

#define LED_PIN     A0
#define NUM_LEDS    16
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

int colorSTEP = 15; //change on color based on press

const byte ROWS = 4; //four rows ABCD D is row 1
const byte COLS = 4; //four columns 1234
char keys[ROWS][COLS] = {
  {'4','5','C','D'},
  {'3','6','B','E'},
  {'2','7','A','F'},
  {'1','8','9','G'}
};
byte rowPins[ROWS] = {A1, A2, A3, A10}; //connect to the row pinouts of the keypad D, C, B, A 
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad 1, 2, 3, 4

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

byte colorSTATE[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void setup(){
    Serial.begin(9600);
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
}

void loop(){
    buttonREAD();
    colorDISPLAY();
}

void colorDISPLAY(){
   for (int i=0;i<16;i++){
    if (colorSTATE[i]==0){
      leds[i] = CRGB::Black;
    }
    else {
      leds[i] = CHSV(colorSTATE[i],255,255);
    }
   }
   FastLED.show();
}


void buttonREAD(){
  if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
            {
                switch (kpd.key[i].kchar) {
                    case '1':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[0]<255){
                          colorSTATE[0] = colorSTATE[0]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[0] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '2':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[1]<255){
                          colorSTATE[1] = colorSTATE[1]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[1] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '3':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[2]<255){
                          colorSTATE[2] = colorSTATE[2]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[2] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '4':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[3]<255){
                          colorSTATE[3] = colorSTATE[3]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[3] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '5':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[4]<255){
                          colorSTATE[4] = colorSTATE[4]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[4] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                       
                     break;
                    }
                break;
                    case '6':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[5]<255){
                          colorSTATE[5] = colorSTATE[5]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[5] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '7':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[6]<255){
                          colorSTATE[6] = colorSTATE[6]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[6] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '8':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[7]<255){
                          colorSTATE[7] = colorSTATE[7]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[7] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '9':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[8]<255){
                          colorSTATE[8] = colorSTATE[8]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[8] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'A':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[9]<255){
                          colorSTATE[9] = colorSTATE[9]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[9] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'B':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[10]<255){
                          colorSTATE[10] = colorSTATE[10]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[10] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'C':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[11]<255){
                          colorSTATE[11] = colorSTATE[11]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[11] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'D':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[12]<255){
                          colorSTATE[12] = colorSTATE[12]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[12] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'E':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[13]<255){
                          colorSTATE[13] = colorSTATE[13]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[13] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'F':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[14]<255){
                          colorSTATE[14] = colorSTATE[14]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[14] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'G':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (colorSTATE[15]<255){
                          colorSTATE[15] = colorSTATE[15]+colorSTEP;
                        }
                      break;
                      case HOLD:
                        colorSTATE[15] = 0;
                      break;
                      case RELEASED:
                        
                      break;
                      case IDLE:
                       
                      break;
                    }
                }
             }
        }
    }
}
