#include "Arduino.h"
#include "autotune.h"

// Set frequencies of all notes
const uint16_t MID_C = 262; // 261.63
const uint16_t MID_DB = 277; //277.18;
const uint16_t MID_D = 294; //293.66;
const uint16_t MID_EB = 311; // 311.13;
const uint16_t MID_E = 330; // 329.63;
const uint16_t MID_F = 349; // 349.23;
const uint16_t MID_GB = 370; // 369.99;
const uint16_t MID_G = 392; //392.00;
const uint16_t MID_AB = 415; // 415.30;
const uint16_t MID_A = 440; // 440.00;
const uint16_t MID_BB = 466; // 466.16;
const uint16_t MID_B = 494; // 493.88;

// Set notes of current scale or chord
// Lowest note up one octave must be higher than highest note
const uint16_t SCALE [7] = {MID_C, MID_D, MID_E, MID_F, MID_G, MID_A, MID_B}; // One octave
const int SCALE_LEN = 7 ;
uint16_t SCALE_FULL[21] ; // Three octaves
int SCALE_FULL_LEN = 21 ;
uint16_t TONE_EDGE[22] ; // Set edges above and below three octaves




int set_tone_edges()
{
  #ifdef SERIAL_ENABLED
    int value_temp = 0; // for debugging
    Serial.print("\nAutotune setup begins\n");
 #endif
  //Initialize three octaves of notes
  for (int i = 0 ; i < SCALE_LEN ; i++){ // lowest octave
    SCALE_FULL[i] = (SCALE[i] >> 1); // use bit shift instead of division
  }

  for (int i = 0 ; i < SCALE_LEN ; i++){ // middle octave
    SCALE_FULL[i + SCALE_LEN] = (SCALE[i]);
  }

  for (int i = 0 ; i < SCALE_LEN ; i++){ // Highest octave
    SCALE_FULL[i + (2 * SCALE_LEN)] = (SCALE[i] * 2);

  }




  //Set frequency edges for autotuning
  // Lowest octave
  TONE_EDGE[0] = (((SCALE_FULL[SCALE_LEN - 1] >> 1 ) + SCALE_FULL[0])) >> 1 ;

  for (int i = 1 ; i < SCALE_LEN ; i++){

    TONE_EDGE[i] = (SCALE_FULL[i-1] + SCALE_FULL[i]) >> 1 ;
    value_temp = (SCALE_FULL[i-1] + SCALE_FULL[i]) >> 1;
    }

  //Middle octave
  for (int i = SCALE_LEN ; i < ( 2 * SCALE_LEN ); i++){
    TONE_EDGE[i] = ((SCALE_FULL[i-1]) + (SCALE_FULL[i])) >> 1 ;
  }

  //Highest octave
  for (int i = SCALE_LEN << 1  ; i < ( 3 * SCALE_LEN ); i++){
    TONE_EDGE[i] = ((SCALE_FULL[i-1]) + (SCALE_FULL[i])) >> 1 ;
  }

  // Upper limit
  TONE_EDGE[SCALE_FULL_LEN] = ((SCALE_FULL[SCALE_FULL_LEN - 1] + (SCALE_FULL[0] * 8)) >> 1) ; // upper edge of the top note

  #if SERIAL_ENABLED
    Serial.print("\nAutotune setup complete\nScale: ");
    for (int i = 0 ; i < SCALE_LEN ;i++) {
      Serial.print(SCALE[i]);
      Serial.print(" ");
    }
    Serial.print("\nFull scale: ");
    for (int i = 0 ; i < SCALE_FULL_LEN ; i++){ //
      Serial.print(SCALE_FULL[i]);
      Serial.print("  ");
    }
    Serial.print("\nTone edges: ");
    for (int i = 0 ; i < SCALE_FULL_LEN + 1 ; i++){ //
      Serial.print(TONE_EDGE[i]);
      Serial.print("  ");
    }

 #endif
 return 1; // successful completion


}

int autotune(uint16_t PITCH_IN, int KEY, int TAU) {
  static int SCALE_INDEX = 9999; // remember last tuned tone array position
  uint16_t TONE_TUNED ;
  if (SCALE_INDEX = 9999) {
    SCALE_INDEX = SCALE_FULL_LEN ; // begin at the upper limit
  }
  #ifdef SERIAL_ENABLED
    Serial.print("\nScale Index: ");
    Serial.print(SCALE_INDEX);
    Serial.print(" ");
  #endif

  if (PITCH_IN < TONE_EDGE[ SCALE_INDEX ]) { // shift down one note if room
    #ifdef SERIAL_ENABLED
      Serial.print("\nDecrease scale index from ");
      Serial.print(SCALE_INDEX);
      Serial.print(" TONE EDGE: ");
      Serial.print(TONE_EDGE[SCALE_INDEX]);
      Serial.print("PITCH_IN: ");
      Serial.print(PITCH_IN);
    #endif

    if ( SCALE_INDEX <= 0 ) {
      TONE_TUNED = PITCH_IN;
    }

    if ( SCALE_INDEX > 0 ) {
        --SCALE_INDEX;
        TONE_TUNED = PITCH_IN ; // first time input in range only
    return TONE_TUNED;
    }
  }
    else if (PITCH_IN > TONE_EDGE[ SCALE_INDEX + 1 ]) {
       // shift up one note if room
    if ( SCALE_INDEX >= ( SCALE_FULL_LEN - 1 ) ) { TONE_TUNED = PITCH_IN ; }
    if ( SCALE_INDEX < ( SCALE_FULL_LEN - 1 ) ) {
      ++SCALE_INDEX;
      TONE_TUNED = PITCH_IN ; // first time input in range only
      return TONE_TUNED;
    }
  }
    else { // eventually vary how hard we autotune here
      TONE_TUNED = SCALE_FULL[ SCALE_INDEX ];
      return TONE_TUNED; // input pitch in tuned range at least twice so start tuning
    }

  }
