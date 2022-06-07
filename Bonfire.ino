/* ****** LIBRARIES ****** */

#include "FastLED.h"


/* ****** VARIABLES ****** */

#define NUM_LEDS 228
#define LED_TYPE   WS2812
CRGB leds[NUM_LEDS];
bool flag = true;

byte order[20][15] = { // Square matrix for pyramide LEDs ('255' means that cell doesn't exist):
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 255, 255, 255, 255, 255, 255}, 
  {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 255, 255, 255}, 
  {21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35}, 
  {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 255, 255, 255}, 
  {48, 49, 50, 51, 52, 53, 54, 55, 56, 255, 255, 255, 255, 255, 255}, 
  {57, 58, 59, 60, 61, 62, 63, 64, 65, 255, 255, 255, 255, 255, 255}, 
  {66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 255, 255, 255}, 
  {78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92}, 
  {93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 255, 255, 255}, 
  {105, 106, 107, 108, 109, 110, 111, 112, 113, 255, 255, 255, 255, 255, 255}, 
  {114, 115, 116, 117, 118, 119, 120, 121, 122, 255, 255, 255, 255, 255, 255}, 
  {123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 255, 255, 255}, 
  {135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149}, 
  {150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 255, 255, 255}, 
  {162, 163, 164, 165, 166, 167, 168, 169, 170, 255, 255, 255, 255, 255, 255}, 
  {171, 172, 173, 174, 175, 176, 177, 178, 179, 255, 255, 255, 255, 255, 255}, 
  {180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 255, 255, 255}, 
  {192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206}, 
  {207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 255, 255, 255}, 
  {219, 220, 221, 222, 223, 224, 225, 226, 227, 255, 255, 255, 255, 255, 255}
};

byte heats[20][15] = {};


/* ****** SETUP ****** */

void setup()
{
  FastLED.addLeds<WS2811, 2, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  clearLeds();
  FastLED.show();
}


/* ****** LOOP ****** */

void loop() 
{                                                               
  for (byte i = 0; i < 20; i++) 
    if (i == 2 or i == 7 or i == 12 or i == 17)
      Fire(i,10,30);
    else if (i == 1 or i == 3 or i == 6 or i == 8 or i == 11 or i == 13 or i == 16 or i == 18)
      Fire(i,5,15);
    else
      Fire(i,3,10);
      
  FastLED.show();
}


/* ****** FUNCTIONS ****** */

void Fire(byte raw, byte Cooling, byte Sparking) {

  byte leds_qty = 0;
  
  for (byte i = 0; i < sizeof(order[raw]); i++)
    if(order[raw][i] != 255)
      leds_qty++;
      
  byte led_line[leds_qty] = {};
  
  for (byte i = 0; i < leds_qty; i++)
  {
    led_line[i] = order[raw][i];
  }
  
  byte heat[leds_qty];
  for( byte i = 0; i < leds_qty; i++) {
    heat[i] = heats[raw][i];
  }
  
  int cooldown;

  // Step 1.  Cool down every cell a little:
  for( byte i = 0; i < leds_qty; i++) 
  {
    cooldown = random(0, ((Cooling * 10) / leds_qty) + 2);
 
    if(cooldown>heat[i])
      heat[i]=0;
    else
      heat[i]=heat[i]-cooldown;
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little:
  for( byte k = leds_qty - 1; k >= 2; k--)
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom:
  if( random(255) < Sparking ) 
  {
    byte y = random(5);
    heat[y] = heat[y] + random(160,200);
    if (heat[y] > 180)
      heat[y] = 180;
  }

  // Step 4.  Convert heat to LED colors and save heats:
  for( byte j = 0; j < leds_qty; j++)
    setPixelHeatColor(led_line[j], heat[j]);
    
  for( byte i = 0; i < leds_qty; i++)
    heats[raw][i] = heat[i];
  
}


void setPixelHeatColor (byte Pixel, byte temperature) 
{
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // Calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // Figure out which third of the spectrum we're in:
  if( t192 > 0x80) // hottest
    setPixel(Pixel, 255, 255, heatramp);
  else if( t192 > 0x40 ) // middle
    setPixel(Pixel, 255, heatramp, 0);
  else // coolest
    setPixel(Pixel, heatramp, 0, 0);
}


void setPixel(byte Pixel, byte red, byte green, byte blue) 
{
   leds[Pixel].red = red;
   leds[Pixel].green = green;
   leds[Pixel].blue = blue;
}


void clearLeds()
{
  for(byte i = 0; i < NUM_LEDS; i++)
    setPixel(i, 0, 0, 0);
}
