// =======================================================================================
// ============================== Teeces PSI Lighting Sketch =============================
// =======================================================================================
//
// A sketch for running front and rear Teeces PSI's only (no logics!)
//
//   Arduino Pins 9,8,7 -> Front PSI IN D,C,L -> Rear PSI

// set brightness levels here (a value of 0-15)...
int RPSIbright=12; //rear PSI
int FPSIbright=12; //front PSI

//set the type of our front and rear PSI's
#define PSItype 4
// 1 = Teeces original (6 LEDs of each color, arranged side by side)
// 2 = Teeces original checkerboard (6 LEDs of each color arranged in a checkerboard pattern)
// 3 = Teeces V3.2 PSI by John V (13 LEDs of each color, arranged side by side)
// 4 = Teeces V3.2 PSI checkerboard by John V  (13 LEDs of each color, in a checkerboard pattern)

//set timing of the PSI's here (in milliseconds)...
int psiRed=2500;    //how long front PSI stays red
int psiBlue=1700;   //how long front PSI stays blue
int psiYellow=1700; //how long rear PSI stays yellow
int psiGreen=2500;  //how long rear PSI stays green
int rbSlide=125; // mts - time to transition between red and blue in slide mode
int ygSlide=300; // mts - time to transition between yellow and green in slide mode

//pulse the PSI Brightness (0=off, 1=Front, 2=Rear, 3=Both)
#define PSIPULSER 1

//Use CuriousMarc's Holo Light boards in PWM mode
//(this means they'll need to be connected to pins 3 and 6)
//#define CURIOUS

// =======================================================================================
// =======================================================================================

#define FPSIDEV 0 //front PSI is device #0 in the chain
#define RPSIDEV 1 // rear PSI is device #1 in the chain

#include <LedControl.h>
#undef round 

//START UP LEDCONTROL...
LedControl lcChain=LedControl(9,8,7,2); //

// =======================================================================================
#if (PSItype==4)  // slide animation code for Teeces V2 PSI boards (code by John V)
#define HPROW 5
class PSI {
  int stage; //0 thru 6
  int inc;
  int stageDelay[7];
  int cols[7][5];
  int randNumber; //a random number to decide the fate of the last stage

  unsigned long timeLast;
  int device;

  public:
  
  PSI(int _delay1, int _delay2, int _delay3, int _device)
  {
    device=_device;
    
    stage=0;
    timeLast=0;
    inc=1;
    
    cols[0][0] = B10101000;
    cols[0][1] = B01010100;
    cols[0][2] = B10101000;
    cols[0][3] = B01010100;
    cols[0][4] = B10101000;
    
    cols[1][0] = B00101000; //R B R B R B
    cols[1][1] = B11010100; //B R B R B R
    cols[1][2] = B00101000; //R B R B R B
    cols[1][3] = B11010100; //B R B R B R
    cols[1][4] = B00101000; //R B R B R B

    cols[2][0] = B01101000;
    cols[2][1] = B10010100;
    cols[2][2] = B01101000;
    cols[2][3] = B10010100;
    cols[2][4] = B01101000;
    
    cols[3][0] = B01001000;
    cols[3][1] = B10110100;
    cols[3][2] = B01001000;
    cols[3][3] = B10110100;
    cols[3][4] = B01001000;
    
    cols[4][0] = B01011000;
    cols[4][1] = B10100100;
    cols[4][2] = B01011000;
    cols[4][3] = B10100100;
    cols[4][4] = B01011000;
    
    cols[5][0] = B01010000;
    cols[5][1] = B10101100;
    cols[5][2] = B01010000;
    cols[5][3] = B10101100;
    cols[5][4] = B01010000;
    
    cols[6][0] = B01010100;
    cols[6][1] = B10101000;
    cols[6][2] = B01010100;
    cols[6][3] = B10101000;
    cols[6][4] = B01010100;
    
    stageDelay[0] = _delay1 - _delay3;
    stageDelay[1] = _delay3/5;
    stageDelay[2] = _delay3/5;
    stageDelay[3] = _delay3/5;
    stageDelay[4] = _delay3/5;
    stageDelay[5] = _delay3/5;
    stageDelay[6] = _delay2 - _delay3;
  }
  
  void Animate(unsigned long elapsed, LedControl control)
  {
    if ((elapsed - timeLast) < stageDelay[stage]) return;
    
    timeLast = elapsed;
    stage+=inc;

    if (stage>6 || stage<0 )
    {
      inc *= -1;
      stage+=inc*2;
    }
    
    if (stage==6) //randomly choose whether or not to go 'stuck'
      {
        randNumber = random(9);
        if (randNumber<5) { //set the last stage to 'stuck' 
          cols[6][0] = B01010000;
          cols[6][1] = B10101100;
          cols[6][2] = B01010000;
          cols[6][3] = B10101100;
          cols[6][4] = B01010000; 
        }
        else //reset the last stage to a solid color
        {
          cols[6][0] = B01010100;
          cols[6][1] = B10101000;
          cols[6][2] = B01010100;
          cols[6][3] = B10101000;
          cols[6][4] = B01010100;
        }
      }
     if (stage==0) //randomly choose whether or not to go 'stuck'
      {
        randNumber = random(9);
        if (randNumber<5) { //set the first stage to 'stuck' 
          cols[0][0] = B00101000; //R B R B R B
          cols[0][1] = B11010100; //B R B R B R
          cols[0][2] = B00101000; //R B R B R B
          cols[0][3] = B11010100; //B R B R B R
          cols[0][4] = B00101000; //R B R B R B
        }
        else //reset the first stage to a solid color
        {
          cols[0][0] = B10101000;
          cols[0][1] = B01010100;
          cols[0][2] = B10101000;
          cols[0][3] = B01010100;
          cols[0][4] = B10101000;
        }
      }

    for (int row=0; row<5; row++)
      control.setRow(device,row,cols[stage][row]);
  }
};

#endif  

// =======================================================================================
#if (PSItype==3)  // slide animation code for Teeces V2 PSI boards (code by John V)
#define HPROW 5
  class PSI {    
    int stage; //0 thru 6
  int inc;
  int stageDelay[7];
  int cols[7];

  unsigned long timeLast;
  int device;

  public:
  
  PSI(int _delay1, int _delay2, int _delay3, int _device)
  {
    device=_device;
    
    stage=0;
    timeLast=0;
    inc=1;
    
    cols[0] = B11100000;
    cols[1] = B11110000;
    cols[2] = B01110000;
    cols[3] = B01111000;
    cols[4] = B00111000;
    cols[5] = B00111100;
    cols[6] = B00011100;
    
    stageDelay[0] = _delay1 - _delay3;
    stageDelay[1] = _delay3/5;
    stageDelay[2] = _delay3/5;
    stageDelay[3] = _delay3/5;
    stageDelay[4] = _delay3/5;
    stageDelay[5] = _delay3/5;
    stageDelay[6] = _delay2 - _delay3;
    }   
    void Animate(unsigned long elapsed, LedControl control)
    {
    if ((elapsed - timeLast) < stageDelay[stage]) return;
    
    timeLast = elapsed;
    stage+=inc;

    if (stage>6 || stage<0 )
    {
      inc *= -1;
      stage+=inc*2;
    }
    
    for (int row=0; row<5; row++)
      control.setRow(device,row,cols[stage]);
    }
  };
#endif 
// =======================================================================================
//michael smith's checkerboard PSI method for Teeces original PSI boards
// Michael's original sketch is here : http://pastebin.com/hXeZb7Gd
#if (PSItype==2) 
#define HPROW 4
static const int patternAtStage[] = { B01010000, B11010000, B10010000, B10110000, B10100000, B00100000, B01100000, B01000000, B01010000 };
class PSI
{    
    bool state;
    int stage;
    unsigned long timeLast;
    int delay1, delay2, delay3;
    int device;    
    int delayAtStage[9];
    int slideDirection;  // is either 1 or -1
    int maxStage;  // for PSIslide it's either 5 or 9 stages, for traditional PSI it's just back and forth between 2    
public:        
    PSI(int _delay1, int _delay2, int _delay3, int _device)
    {
        delayAtStage[0] = _delay1;
        delayAtStage[1] = _delay3/3;        // delay3 is total transition time - divide it by the 3 stages of transition
        delayAtStage[2] = delayAtStage[1];
        delayAtStage[3] = delayAtStage[1];
        delayAtStage[4] = _delay2;
        delayAtStage[5] = delayAtStage[1];
        delayAtStage[6] = delayAtStage[1];
        delayAtStage[7] = delayAtStage[1];
        delayAtStage[8] = _delay1;          // repeated because it's not a loop it cycles back and forth across the pattern.        
        stage=0;
        slideDirection=1;
        maxStage=8;         // change to 5 would skip the LtoR from blue to red.
        timeLast=0;
        device=_device;        
        // legacy for traditional PSI animation
        delay1=_delay1;
        delay2=_delay2;
        delay3=_delay3;
        state=false;
    }     
    void Animate(unsigned long timeNow, LedControl control)
    {
            if ((timeNow - timeLast) < delayAtStage[stage]) return;
            //Serial.begin(9600);
            //Serial.println(stage);
            //Serial.println(patternAtStage[stage]);            
            timeLast = timeNow;
            stage+=slideDirection; //move to the next stage, which could be up or down in the array
            if (stage >= maxStage)
            {
                // limit the stage to the maxStage and reverse the direction of the slide
                stage=maxStage;
                slideDirection = -1;
            }
            else if (stage <= 0)
            {
                stage=0;
                slideDirection = 1;
            }
            // set the patterns for this stage
            control.setRow(device,0,patternAtStage[stage]);
            control.setRow(device,1,~patternAtStage[stage]);
            control.setRow(device,2,patternAtStage[stage]);
            control.setRow(device,3,~patternAtStage[stage]);
    }
};
#endif
// =======================================================================================
// slide animation code for Teeces original PSI boards
#if (PSItype==1)
#define HPROW 4
  class PSI {
    int stage; //0 thru 4
    int inc;
    int stageDelay[5];
    int cols[5];
    unsigned long timeLast;
    int device;
    public:  
    PSI(int _delay1, int _delay2, int _device)  {
      device=_device;    
      stage=0;
      timeLast=0;
      inc=1;    
      cols[0] = B11000000;
      cols[1] = B11100000;
      cols[2] = B01100000;
      cols[3] = B01110000;
      cols[4] = B00110000;    
      stageDelay[0] = _delay1 - 300;
      stageDelay[1] = 100;
      stageDelay[2] = 100;
      stageDelay[3] = 100;
      stageDelay[4] = _delay2 - 300;
    }  
    void Animate(unsigned long elapsed, LedControl control)  {
      if ((elapsed - timeLast) < stageDelay[stage]) return;
      timeLast = elapsed;
      stage+=inc;
      if (stage>4 || stage<0 ) {
        inc *= -1;
        stage+=inc*2;
      }    
      for (int row=0; row<4; row++) control.setRow(device,row,cols[stage]);
    }
  };
#endif  
// =======================================================================================
#if (PSItype==1)
  PSI psiFront=PSI(psiRed, psiBlue, FPSIDEV); // device is FPSIDEV (#2 or #4 for an R7 dome) 
  PSI psiRear =PSI(psiYellow, psiGreen, RPSIDEV); // device is #3
//#endif
//#if (PSItype==2) || (PSItype==3)
#else
  PSI psiFront=PSI(psiRed, psiBlue, rbSlide, FPSIDEV); //2000 ms on red, 1000 ms on blue.
  PSI psiRear =PSI(psiYellow, psiGreen, ygSlide, RPSIDEV); //1000 ms on yellow, 2000 ms on green.
#endif
// =======================================================================================
void setup() {
  Serial.begin(9600); //used for debugging

  lcChain.shutdown(0, false); //take the device out of shutdown (power save) mode
  lcChain.clearDisplay(0);
  lcChain.shutdown(1, false); //take the device out of shutdown (power save) mode
  lcChain.clearDisplay(1);

  lcChain.setIntensity(FPSIDEV, FPSIbright); //Front PSI
  lcChain.setIntensity(RPSIDEV, RPSIbright); //Rear PSI
  
  //HP lights on constantly...
  lcChain.setRow(RPSIDEV,HPROW,255); //rear psi
  lcChain.setRow(FPSIDEV,HPROW,255); //front psi
  
  #if (BOARDtype==2)
  pinMode(17, OUTPUT);  // Set RX LED of Pro Micro as an output
  #endif
  
  #if defined(CURIOUS)
  pinMode(3, OUTPUT);
  pinMode(6, OUTPUT);
  #endif
  
}
// =======================================================================================
void loop() {
  unsigned long timeNew= millis();
  psiFront.Animate(timeNew, lcChain);
  psiRear.Animate(timeNew, lcChain);
#if PSIPULSER>0
  PSIpulse(timeNew);
#endif
#if defined(CURIOUS)
  CuriousFade(timeNew);
#endif
}

// =======================================================================================
// PULSING PSI LED BRIGHTNESS/INTENSITY
#if PSIPULSER>0
int pulseState = LOW; //initial state of our PSI (high intensity, going down)
int pulseVal = 15; //initial value of our PSI (full intensity)
void PSIpulse(unsigned long elapsed) {
  static unsigned long timeLast=0;
  if ((elapsed - timeLast) < 100) return; //proceed if 100 milliseconds have passed
  timeLast = elapsed; 
  if (pulseState == HIGH) { //increase intensity
    pulseVal++; //increase value by 1
    if (pulseVal == 16) { //if we've gone beyond full intensity, start going down again
      pulseVal = 15;
      pulseState = LOW;
    }
  }
  else { //decrease intensity
    pulseVal--; //increase value by 1
    if (pulseVal == 0) { //if we've gone beyond full intensity, start going down again
      pulseVal = 1;
      pulseState = HIGH;
    }
  }
  #if PSIPULSER==1 || PSIPULSER==3
  lcChain.setIntensity(0,pulseVal); //set the front intensity
  #endif   
  #if PSIPULSER==2 || PSIPULSER==3
  lcChain.setIntensity(1,pulseVal); //set the rear intensity
  #endif  
}
#endif
////////////////////////////////////

// =======================================================================================
// PULSE THE BRIGHTNESS OF THE CURIOUS MARC HOLO LIGHTS
#if defined(CURIOUS)
int Cbrightness = 0;    // how bright the LED starts at
int CfadeAmount = 1;    // how many points to fade the LED by
int holdtime=1000; //default 'pause' is set to 5 seconds
int fadespeed=1; //milliseconds to wait between actions (lower number speeds things up)
void CuriousFade(unsigned long elapsed) {
  analogWrite(3, Cbrightness);  
  analogWrite(6, Cbrightness);  
  static unsigned long timeLast=0;
  if ((elapsed - timeLast) < (fadespeed + holdtime)) return; //proceed if some fadespeed+holdtime milliseconds have passed
  timeLast = elapsed; 
  holdtime = 0;
  Cbrightness = Cbrightness + CfadeAmount;
  if (Cbrightness == 0 || Cbrightness == 255) {
    CfadeAmount = -CfadeAmount ; //reverse fade direction
    holdtime = random(9)*1000; //generate a new 'pause' time of 0 to 9 seconds
    fadespeed = random(1,20); //generate a new fadespeed time of 1 to 20 milliseconds
  } 
}
#endif
////////////////////////////////////



// =======================================================================================
