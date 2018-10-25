/*
 ToDo
 onboard change BPM - DONE hold mod button (1) and press 3-down or 4-up 
 step color changes with BPM HSV color 170-210 - DONE slow BPM is blue and fast BPM is red
 */

#include <FastLED.h>
#include <Keypad.h>
#include <MIDIUSB.h>
#include <pitchToNote.h>
#include <frequencyToNote.h>

#define LED_PIN     A0 //should add 300-500ohm resistor per https://learn.adafruit.com/adafruit-neopixel-uberguide/best-practices
#define NUM_LEDS    32
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

#define MIDI_CHANNEL 0   //Default MIDI channel

const byte ROWS = 4; //A B C D
const byte COLS = 8; //A1 A2 B1 B2 C1 C2 D1 D2

byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad A, B, C, D
byte colPins[COLS] = {10, 16, 14, 15, A1, A2, A3, 9}; //connect to the column pinouts of the keypad 1, 2

/*gridMODEs
0 is piano - instantly plays the note when pressed rather than waiting for the step
1 is step mode - only plays the note is on an active step
*/
byte gridMODE = 1; // piano is the stockmode

int stepBPM = 100; //range 20-200 steps per minute ~BPM
long timeLASTSTEP = 0; //miliseconds that have passed since the last step
int noteLENGTH = 65; //in miliseconds range 10-200, how long the button is pressed for in miliseconds
int activeCOL = 0; //which col is being used
int lastCOL = 0;
bool noteOFFflag = 0; //flag so when the note length passes the notes are turned off 
bool buttonPRESS = 0; //see if any buttons have been pressed
bool sheet = 0; // sees if the first sheet or 2nd sheet for more steps
byte noteVELOCITY = 127;
bool modBUTTON = 0; //mod button, if pressed then adds additional functionality

char keys[ROWS][COLS] = { //uniqu character assigned to specific spots
{'4','5','C','D','K','L','S','T'},
{'3','6','B','E','J','M','R','U'},
{'2','7','A','F','I','N','Q','V'},
{'1','8','9','G','H','O','P','W'},
};

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

byte note[ROWS][COLS] = { //note values assigned to positions                                                
{60,60,60,60,60,60,60,60}, //first row                                                          
{64,64,64,64,64,64,64,64}, //2nd                                                         
{68,68,68,68,68,68,68,68}, //3rd                                                         
{72,72,72,72,72,72,72,72}, //4th
//{60,61,62,63,64,65,66,67}, //first row                                                          
//{68,69,70,71,72,73,74,75}, //2nd                                                         
//{76,77,78,79,80,81,82,83}, //3rd                                                         
//{84,85,86,87,88,89,90,91}, //4th                                                         
};

byte buttonSTATE[ROWS][COLS] = { //holds the values for how many times the button has been pressed, hold the button and resets to zero
{0,0,0,0,0,0,0,0}, //bottom row
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0}, 
};

void setup(){
    Serial.begin(9600);
    delay(3000); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);
    startANIMATION(); //just shows stuff is working
    colorDISPLAY();
}

void loop(){
	buttonREAD();
	if (buttonPRESS){ //sees if anybuttons have changed
		colorDISPLAY(); //updates the grid to reflect new button press
		stepLIGHT(activeCOL); //draws the current step position over the updated grid
		buttonPRESS=0; //resets button flag for future changes  
	}
	if ((millis()-timeLASTSTEP)>(60000/stepBPM)){ //pulls stepplay at the BPM interval
		Serial.print("step:");
    Serial.println(millis()-timeLASTSTEP);
		colorDISPLAY();
		stepLIGHT(activeCOL);
		stepPLAY(activeCOL); //plays the values from that column
		noteOFFflag=0; //flag so when the note length passes the notes are turned off 
		lastCOL = activeCOL;
		if (activeCOL<7) activeCOL++; //loops the active column around
		else {
			sheet = !sheet; //flip flops the sheet
			activeCOL=0;
		}
		timeLASTSTEP=millis(); //resets for next step
	}
	if (((millis()-timeLASTSTEP)>(noteLENGTH))&&!noteOFFflag){
		Serial.print("notelength:");
    Serial.println(millis()-timeLASTSTEP);
		noteOFFflag=1; //resets flag
    notesOFF(lastCOL); //turns off notes
	}  
}

void startANIMATION(){
  for (int i=0;i<32;i++){
    leds[i]=CRGB::Purple;
    FastLED.show();
    delay(75);
  }
}

void stepPLAY(int col){ //plays the notes in the column
	Serial.println("stepPLAY");
	if (!sheet){ //first sheet, if button state 1,2 then note will play
		if (buttonSTATE[0][col]>0&&buttonSTATE[0][col]<3) noteON(MIDI_CHANNEL,note[0][col],noteVELOCITY);
		if (buttonSTATE[1][col]>0&&buttonSTATE[1][col]<3) noteON(MIDI_CHANNEL,note[1][col],noteVELOCITY);
		if (buttonSTATE[2][col]>0&&buttonSTATE[2][col]<3) noteON(MIDI_CHANNEL,note[2][col],noteVELOCITY);
		if (buttonSTATE[3][col]>0&&buttonSTATE[3][col]<3) noteON(MIDI_CHANNEL,note[3][col],noteVELOCITY);		
	}
	else{ //2nd sheet, if button state 2,3 then note will play
		if (buttonSTATE[0][col]>1&&buttonSTATE[0][col]<4) noteON(MIDI_CHANNEL,note[0][col],noteVELOCITY);
		if (buttonSTATE[1][col]>1&&buttonSTATE[1][col]<4) noteON(MIDI_CHANNEL,note[1][col],noteVELOCITY);
		if (buttonSTATE[2][col]>1&&buttonSTATE[2][col]<4) noteON(MIDI_CHANNEL,note[2][col],noteVELOCITY);
		if (buttonSTATE[3][col]>1&&buttonSTATE[3][col]<4) noteON(MIDI_CHANNEL,note[3][col],noteVELOCITY);
	}
	MidiUSB.flush();
}

void stepLIGHT(int col){ //lights the active column
	Serial.println("stepLIGHT");
	byte bpmLIGHT = map(stepBPM,20,200,171,255); //maps the BPM from slow ~20BPM and blue to fast ~200BPM and red
	for (int row=0;row<4;row++)leds[col*4+row] = CHSV(bpmLIGHT, 255, 255); //sets the step color absed on the BPM , CRGB::PaleGreen;
	FastLED.show();
}

void colorSET(byte bSTATE, int ledNUM){ //sets color based on the buttonSTATE value 
	switch(bSTATE){
		case 1: //one press only plays on the first sheet
			leds[ledNUM] = CRGB::Yellow;
		break;
		case 2: //two press plays notes on 1st and 2nd sheet
			leds[ledNUM] = CRGB::Orange;
		break; 
		case 3: //three press only plays on the 2nd sheet
			leds[ledNUM] = CRGB::Red;
		break;
		default: //all other presses past 3, does not play anything, just changes the color or the button
			leds[ledNUM] = CRGB::PaleGoldenrod;
		break;
	}
}

void colorDISPLAY(){
	int ledcount=0;
	for (int i=0;i<4;i++){ //loop for the even columns
		if(!buttonSTATE[i][0])leds[ledcount] = CRGB::Black;
		else colorSET(buttonSTATE[i][0], ledcount);
		if(!buttonSTATE[i][2])leds[ledcount+8] = CRGB::Black;
		else colorSET(buttonSTATE[i][2], ledcount+8); 
		if(!buttonSTATE[i][4])leds[ledcount+16] = CRGB::Black;
		else colorSET(buttonSTATE[i][4], ledcount+16);
		if(!buttonSTATE[i][6])leds[ledcount+24] = CRGB::Black;
		else colorSET(buttonSTATE[i][6], ledcount+24);
		ledcount++;
	}
	for (int i=3;i>=0;i--){ //loop for the odd columns
		if(!buttonSTATE[i][1])leds[ledcount] = CRGB::CRGB::Black;
		else colorSET(buttonSTATE[i][1], ledcount);
		if(!buttonSTATE[i][3])leds[ledcount+8] = CRGB::CRGB::Black;
		else colorSET(buttonSTATE[i][3], ledcount+8);
		if(!buttonSTATE[i][5])leds[ledcount+16] = CRGB::CRGB::Black;
		else colorSET(buttonSTATE[i][5], ledcount+16);
		if(!buttonSTATE[i][7])leds[ledcount+24] = CRGB::CRGB::Black;
		else colorSET(buttonSTATE[i][7], ledcount+24);
		ledcount++;
	}
	FastLED.show(); //push the new colors
}

void noteON(byte channel, byte pitch, byte velocity) { //turns on a specific note
	midiEventPacket_t noteON = {0x09, uint8_t(0x90 | channel), pitch, velocity};
	MidiUSB.sendMIDI(noteON);
}

void noteOFF(byte channel, byte pitch, byte velocity) { //turns off a specific note
	midiEventPacket_t noteOFF = {0x08, uint8_t(0x80 | channel), pitch, velocity};
	MidiUSB.sendMIDI(noteOFF);
}

void notesOFF(int c){ //turns off all the notes
	//Serial.println("notesOFF");
	for (int r=0;r<4;r++){
	  noteOFF(MIDI_CHANNEL,note[r][c],0);
	}
	MidiUSB.flush();
}

void buttonREAD(){ //sees what button is pushed
  if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
            buttonPRESS=1;
            {
                switch (kpd.key[i].kchar) {
                    case '1': //mod button - get mod functionality when holding down the button
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
						if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[0][0],127);
							MidiUSB.flush();
						}
                        buttonSTATE[0][0]=buttonSTATE[0][0]+1;
                      break;
                      case HOLD:
                        buttonSTATE[0][0]=0;
						modBUTTON = 1;
                      Serial.print("modButton");
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[0][0],0);
							MidiUSB.flush();
						}
						modBUTTON=0;
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '2':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[1][0],127);
							MidiUSB.flush();
						}
                        buttonSTATE[1][0]=buttonSTATE[1][0]+1;
                      break;
                      case HOLD:
                        buttonSTATE[1][0]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[1][0],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '3': 
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[2][0],127);
							MidiUSB.flush();
						}
                        buttonSTATE[2][0]=buttonSTATE[2][0]+1;
						if (modBUTTON&&stepBPM>20){
						  stepBPM--; //keeps stepBPM above 20
						  Serial.print(stepBPM);
						}
                      break;
                      case HOLD:
                        buttonSTATE[2][0]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[2][0],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '4':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[3][0],127);
							MidiUSB.flush();
						}
                        buttonSTATE[3][0]=buttonSTATE[3][0]+1;
						if (modBUTTON&&stepBPM<200){
						  stepBPM++; //keeps stepBPM under 200
						  Serial.print(stepBPM);
						}
                      break;
                      case HOLD:
                        buttonSTATE[3][0]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[3][0],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '5':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[3][1],127);
							MidiUSB.flush();
						}
                        buttonSTATE[3][1]=buttonSTATE[3][1]+1;
                      break;
                      case HOLD:
                        buttonSTATE[3][1]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[3][1],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '6':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[2][1],127);
							MidiUSB.flush();
						}
                        buttonSTATE[2][1]=buttonSTATE[2][1]+1;
                      break;
                      case HOLD:
                        buttonSTATE[2][1]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[2][1],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '7':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[1][1],127);
							MidiUSB.flush();
						}
                        buttonSTATE[1][1]=buttonSTATE[1][1]+1;
                      break;
                      case HOLD:
                        buttonSTATE[1][1]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[1][1],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '8':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
					  case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[0][1],127);
							MidiUSB.flush();
						}
                        buttonSTATE[0][1]=buttonSTATE[0][1]+1;
                      break;
                      case HOLD:
                        buttonSTATE[0][1]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[0][1],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case '9':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[0][2],127);
							MidiUSB.flush();
						}
                        buttonSTATE[0][2]=buttonSTATE[0][2]+1;
                      break;
                      case HOLD:
                        buttonSTATE[0][2]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[0][2],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'A':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[1][2],127);
							MidiUSB.flush();
						}
                        buttonSTATE[1][2]=buttonSTATE[1][2]+1;
                      break;
                      case HOLD:
                        buttonSTATE[1][2]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[1][2],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'B':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[2][2],127);
							MidiUSB.flush();
						}
                        buttonSTATE[2][2]=buttonSTATE[2][2]+1;
                      break;
                      case HOLD:
                        buttonSTATE[2][2]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[2][2],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'C':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[3][2],127);
							MidiUSB.flush();
						}
                        buttonSTATE[3][2]=buttonSTATE[3][2]+1;
                      break;
                      case HOLD:
                        buttonSTATE[3][2]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[3][2],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'D':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[3][3],127);
							MidiUSB.flush();
						}
                        buttonSTATE[3][3]=buttonSTATE[3][3]+1;
                      break;
                      case HOLD:
                        buttonSTATE[3][3]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[3][3],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;
                    case 'E':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[2][3],127);
							MidiUSB.flush();
						}
                        buttonSTATE[2][3]=buttonSTATE[2][3]+1;
                      break;
                      case HOLD:
                        buttonSTATE[2][3]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[2][3],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'F':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[1][3],127);
							MidiUSB.flush();
						}
                        buttonSTATE[1][3]=buttonSTATE[1][3]+1;
                      break;
                      case HOLD:
                        buttonSTATE[1][3]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[1][3],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'G':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[0][3],127);
							MidiUSB.flush();
						}
                        buttonSTATE[0][3]=buttonSTATE[0][3]+1;
                      break;
                      case HOLD:
                        buttonSTATE[0][3]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[0][3],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'H':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[0][4],127);
							MidiUSB.flush();
						}
                        buttonSTATE[0][4]=buttonSTATE[0][4]+1;
                      break;
                      case HOLD:
                        buttonSTATE[0][4]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[0][4],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'I':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[1][4],127);
							MidiUSB.flush();
						}
                        buttonSTATE[1][4]=buttonSTATE[1][4]+1;
                      break;
                      case HOLD:
                        buttonSTATE[1][4]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[1][4],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'J':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[2][4],127);
							MidiUSB.flush();
						}
                        buttonSTATE[2][4]=buttonSTATE[2][4]+1;
                      break;
                      case HOLD:
                        buttonSTATE[2][4]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[2][4],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'K':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[3][4],127);
							MidiUSB.flush();
						}
                        buttonSTATE[3][4]=buttonSTATE[3][4]+1;
                      break;
                      case HOLD:
                        buttonSTATE[3][4]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[3][4],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'L':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[3][5],127);
							MidiUSB.flush();
						}
                        buttonSTATE[3][5]=buttonSTATE[3][5]+1;
                      break;
                      case HOLD:
                        buttonSTATE[3][5]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[3][5],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                     case 'M':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[2][5],127);
							MidiUSB.flush();
						}
                        buttonSTATE[2][5]=buttonSTATE[2][5]+1;
                      break;
                      case HOLD:
                        buttonSTATE[2][5]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[2][5],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                     case 'N':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[1][5],127);
							MidiUSB.flush();
						}
                        buttonSTATE[1][5]=buttonSTATE[1][5]+1;
                      break;
                      case HOLD:
                        buttonSTATE[1][5]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[1][5],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'O':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[0][5],127);
							MidiUSB.flush();
						}
                        buttonSTATE[0][5]=buttonSTATE[0][5]+1;
                      break;
                      case HOLD:
                        buttonSTATE[0][5]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[0][5],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'P':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[0][6],127);
							MidiUSB.flush();
						}
                        buttonSTATE[0][6]=buttonSTATE[0][6]+1;
                      break;
                      case HOLD:
                        buttonSTATE[0][6]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[0][6],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'Q':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[1][6],127);
							MidiUSB.flush();
						}
                        buttonSTATE[1][6]=buttonSTATE[1][6]+1;
                      break;
                      case HOLD:
                        buttonSTATE[1][6]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[1][6],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'R':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[2][6],127);
							MidiUSB.flush();
						}
                        buttonSTATE[2][6]=buttonSTATE[2][6]+1;
                      break;
                      case HOLD:
                        buttonSTATE[2][6]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[2][6],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'S':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[3][6],127);
							MidiUSB.flush();
						}
                        buttonSTATE[3][6]=buttonSTATE[3][6]+1;
                      break;
                      case HOLD:
                        buttonSTATE[3][6]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[3][6],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'T':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[3][7],127);
							MidiUSB.flush();
						}
                        buttonSTATE[3][7]=buttonSTATE[3][7]+1;
                      break;
                      case HOLD:
                        buttonSTATE[3][7]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[3][7],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'U':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[2][7],127);
							MidiUSB.flush();
						}
                        buttonSTATE[2][7]=buttonSTATE[2][7]+1;
                      break;
                      case HOLD:
                        buttonSTATE[2][7]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[2][7],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'V':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[1][7],127);
							MidiUSB.flush();
						}
                        buttonSTATE[1][7]=buttonSTATE[1][7]+1;
                      break;
                      case HOLD:
                        buttonSTATE[1][7]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[1][7],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
                    case 'W':
                    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                      case PRESSED:
                        if (gridMODE==0){
							noteON(MIDI_CHANNEL,note[0][7],127);
							MidiUSB.flush();
						}
                        buttonSTATE[0][7]=buttonSTATE[0][7]+1;
                      break;
                      case HOLD:
                        buttonSTATE[0][7]=0;
                      break;
                      case RELEASED:
                        if (gridMODE==0){
							noteOFF(MIDI_CHANNEL,note[0][7],0);
							MidiUSB.flush();
						}
                      break;
                      case IDLE:
                        
                      break;
                    }
                break;                
               }
             }
          //Serial.print("button:");
          //Serial.println(millis());
        }
    }

}
