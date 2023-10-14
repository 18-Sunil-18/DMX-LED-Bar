//#define FunktionRelais    // Wenn Aktiviert Sound Aus
#define FunktionLED
#define FunktionSound
#define FunktionDMX
//#define FunktionSerial
//#define EEPROMschreiben
//#define FunktionPoti    // Wenn Aktiviert FunktionLED Aktivieren

// ------------------library-----------------------
#include <Arduino.h>

#ifdef FunktionDMX
  #include <DMXSerial.h>
#endif

#ifdef FunktionLED
  #include <Adafruit_NeoPixel.h>
#endif

#ifdef __AVR__
  #include <avr/power.h>
#endif

// ------------------DMX-----------------------

byte maxBrightness     = 200;    // brightness range [off..on] = [0..255], keep dim for less current draw
byte brightness,
     strobe,
     effect,
     effectspeed,
     redLevel_1,  // store the received channel level control data
     grnLevel_1,
     bluLevel_1,
     redLevel_2,
     grnLevel_2,
     bluLevel_2,
     redLevel_3,
     grnLevel_3,
     bluLevel_3,
     redLevel_4,
     grnLevel_4,
     bluLevel_4,
     redLevel_5,
     grnLevel_5,
     bluLevel_5,
     redLevel_6,
     grnLevel_6,
     bluLevel_6,
     redLevel_7,
     grnLevel_7,
     bluLevel_7,
     redLevel_8,
     grnLevel_8,
     bluLevel_8,
     relaisLevel_1,
     relaisLevel_2,
     relaisLevel_3,
     relaisLevel_4,
     relaisLevel_5,
     relaisLevel_6;

byte brightnessCh      =   0,   // DMX channel offsets from base channel
     strobeCh          =   1,
     effectCh          =   2,
     effectspeedCh     =   3,
     redCh_1           =   4,   // CH7
     grnCh_1           =   5,
     bluCh_1           =   6,
     redCh_2           =   7,   // CH10
     grnCh_2           =   8,
     bluCh_2           =   9,
     redCh_3           =   10,   // CH16
     grnCh_3           =   11,
     bluCh_3           =   12,
     redCh_4           =   13,
     grnCh_4           =   14,
     bluCh_4           =   15,
     redCh_5           =   16,   // CH22
     grnCh_5           =   17,
     bluCh_5           =   18,
     redCh_6           =   19,
     grnCh_6           =   20,
     bluCh_6           =   21,
     redCh_7           =   22,   // CH28
     grnCh_7           =   23,
     bluCh_7           =   24,
     redCh_8           =   25,
     grnCh_8           =   26,
     bluCh_8           =   27,
     relaisCh_1        =   7,
     relaisCh_2        =   8,
     relaisCh_3        =   9,
     relaisCh_4        =   10,
     relaisCh_5        =   11,
     relaisCh_6        =   12;

#define PIN 13 // Hier wird angegeben, an welchem digitalen Pin die WS2812 LEDs bzw. NeoPixel angeschlossen sind
#define NUMPIXELS 60 // Hier wird die AndmxBaseCh der angeschlossenen WS2812 LEDs bzw. NeoPixel angegeben
int Pixel_0 = 0,
    Abstand_Pixel = 1;

int Segment_1a = 29,    // CH 10

    Segment_2a = 14,    // CH16
    Segment_2b = 29,    // CH16
    Segment_2c = 44,    // CH16
    
    Segment_3a = 9,   // CH 22 (9,19,29,39,49)
    Segment_3b = 19,   // CH 22
    Segment_3c = 29,   // CH 22
    Segment_3d = 39,   // CH 22
    Segment_3e = 49,   // CH 22
    
    Segment_4a = 7,    // CH 28 (7,14,21,28,35,42,50)
    Segment_4b = 14,    // CH 28
    Segment_4c = 21,    // CH 28
    Segment_4d = 28,    // CH 28
    Segment_4e = 35,    // CH 28
    Segment_4f = 42,    // CH 28
    Segment_4g = 50;    // CH 28

#ifdef FunktionLED
  Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#endif


// ------------------TM1637Display-----------------------
#include <TM1637Display.h>
// Module connection pins (Digital Pins)
TM1637Display display(11, 10); // CLK, DIO

const uint8_t d[] = {SEG_B | SEG_C | SEG_D | SEG_E | SEG_G};
const uint8_t A[] = {SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G};
const uint8_t C[] = {SEG_A | SEG_D | SEG_E | SEG_F };
const uint8_t Sound[] = {SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
                         SEG_C | SEG_D | SEG_E | SEG_G,
                         SEG_C | SEG_D | SEG_E,
                         SEG_B | SEG_C | SEG_D | SEG_E | SEG_G};
const uint8_t Bindestrich[] = {SEG_G,
                               SEG_G,
                               SEG_G,
                               SEG_G};

// ------------------EEPROM-----------------------
#include <EEPROM.h>
int Adresse_0 = 0,    // Speicher DMX-Adresse
    Adresse_1 = 1,
    Adresse_2 = 2,
    Adresse_3 = 3,
    Adresse_4 = 4,
    Adresse_5 = 5,
    Adresse_6 = 6,
    Adresse_7 = 7,
    Adresse_8 = 8,
    Adresse_9 = 9,    // Speicher DMX-Adresse
    Adresse_10 = 10,    // Speicher Hauptmenü DMX-Modus oder Autoprogramm
    Adresse_11 = 11,    // Speicher Autoprogramm
    Adresse_12 = 12;    // Speicher Channel Auswahl

// ------------------Taster-----------------------
int TasteUP = 4,    // Pin vom Taster
    StatusTasteUP,    // Zustand vom Taster
    TasteDOWN = 5,
    StatusTasteDOWN,
    TasteMODE = 6,
    StatusTasteMODE,
    TasteENTER = 7,
    StatusTasteENTER;

// ------------------Poti-----------------------
#ifdef FunktionPoti
int Poti_1 = A0,
    Poti_2 = A1,
    Poti_3 = A2,
    Poti_4 = A3;

int PotiWert_1,
    PotiWert_2,
    PotiWert_3,
    PotiWert_4,
    PotiWertNeu_1,
    PotiWertNeu_2,
    PotiWertNeu_3,
    PotiWertNeu_4;
#endif

// ------------------Sound-----------------------
#ifdef FunktionSound
int sound_digital = 8,
    sound_analog = A0;
#endif

#ifdef FunktionRelais
// ------------------Sound-----------------------
int sound_digital = 12,
    sound_analog = A0;
#endif

// ------------------Relais-----------------------
int relais_1 = 8,
    relais_2 = 9,
    relais_3 = 2,
    relais_4 = 3,
    relais_5 = A2,
    relais_6 = A3;
    

// Wert Relais schalten
int WertRelaisHigh = 200,
    WertRelaisLow = 10;

int RelaisAnAus1 = 0,
    RelaisAnAus2 = 1;

int RelaisDMXversatz;

// ------------------Variabeln-----------------------
int dmxBaseCh,    // DMX-Adresse
    stelle,
    MODE = 0,
    ModeMenueZaehler,    // EEprom letzter Zustand
    Hauptmenuezaehler,    // EEprom letzter Zustand
    Hauptmenue,
    chanel_nummer,    // Wert kommt aus/wird berechnet aus dem EEPROM
    Speicher_Channel;

// Zwischenspeicher Hauptmenü Funktion (in welcher Funktion er ist)
int DMX_Modus_Nix,
    DMX_Adresse_einstellen,
    Channel_einstellen,
    Autoprogramme;


// ------------Autoprogramme-------------------------------------------------------------------------

void ModeFunktion(){
  TasterAuslesen();
  if(StatusTasteUP == HIGH){
    ModeMenueZaehler++;
  }
  if(StatusTasteDOWN == HIGH){
    ModeMenueZaehler--;
  }
  pruefenModeMenu();
  ModeMenue();
  
  if(StatusTasteENTER == HIGH){   // Wenn Enter gedrückt, dann
    EEPROM.write(Adresse_10, Hauptmenuezaehler);   // Speichere Wert Hauptmenü im EEPROM
    EEPROM.write(Adresse_11, ModeMenueZaehler);   // Speichere Wert Autoprogramm im EEPROM
    #ifdef FunktionSerial
      Serial.println(Hauptmenuezaehler);
      Serial.println(ModeMenueZaehler);
    #endif
    delay(100);
  }

  switch(ModeMenueZaehler){
    case 1:
      AlleRot();
      break;
    case 2:
      AlleGruen();
      break;
    case 3:
      AlleBlau();
      break;
    case 4:
      AlleGelb();
      break;
    case 5:
      AlleTuerkis();
      break;
    case 6:
      AlleLila();
      break;
    case 7:
      AlleWeiss();
      break;
    case 8:
      Lauflicht_4_S();
      break;
    case 9:
      Lauflicht_1(100, 1000);
      break;
    case 10:
      Lauflicht_2(1000);
      break;
    case 11:
      Lauflicht_1(5000, 10000);
      break;
    case 13:
      rainbow(75);
      break;
    case 12:
      Lauflicht_5(1000);
      break;
    // -
    case 19:
      PingPong();
      break;
    case 20:
      AnalogLichtSteuern();
      break;
  }
}

#ifdef FunktionLED
// -------------------CH07------------------------------------------------------------------

void ansteuern(int r, int g, int b){
  if(Speicher_Channel == 7){
    for(int x=Pixel_0; x<=NUMPIXELS; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

// -------------------CH10------------------------------------------------------------------

void ansteuern_1a(int r, int g, int b){
  if(Speicher_Channel == 10){
    for(int x=Pixel_0; x<=Segment_1a; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_1b(int r, int g, int b){
  if(Speicher_Channel == 10){
      for(int x=Segment_1a+Abstand_Pixel; x<=NUMPIXELS; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

// -------------------CH16------------------------------------------------------------------

void ansteuern_2a(int r, int g, int b){
  if(Speicher_Channel == 16){
    for(int x=Pixel_0; x<=Segment_2a; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_2b(int r, int g, int b){
  if(Speicher_Channel == 16){
    for(int x=Segment_2a+Abstand_Pixel; x<=Segment_2b; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_2c(int r, int g, int b){
  if(Speicher_Channel == 16){
    for(int x=Segment_2b+Abstand_Pixel; x<=Segment_2c; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_2d(int r, int g, int b){
  if(Speicher_Channel == 16){
    for(int x=Segment_2c+Abstand_Pixel; x<=NUMPIXELS; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

// -------------------CH22------------------------------------------------------------------

void ansteuern_3a(int r, int g, int b){
  if(Speicher_Channel == 22){
    for(int x=Pixel_0; x<=Segment_3a; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_3b(int r, int g, int b){
  if(Speicher_Channel == 22){
    for(int x=Segment_3a+Abstand_Pixel; x<=Segment_3b; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_3c(int r, int g, int b){
  if(Speicher_Channel == 22){
    for(int x=Segment_3b+Abstand_Pixel; x<=Segment_3c; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_3d(int r, int g, int b){
  if(Speicher_Channel == 22){
    for(int x=Segment_3c+Abstand_Pixel; x<=Segment_3d; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}


void ansteuern_3e(int r, int g, int b){
  if(Speicher_Channel == 22){
    for(int x=Segment_3d+Abstand_Pixel; x<=Segment_3e; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}


void ansteuern_3f(int r, int g, int b){
  if(Speicher_Channel == 22){
    for(int x=Segment_3e+Abstand_Pixel; x<=NUMPIXELS; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}


// -------------------CH28------------------------------------------------------------------

void ansteuern_4a(int r, int g, int b){
  if(Speicher_Channel == 28){
    for(int x=Pixel_0; x<=Segment_4a; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_4b(int r, int g, int b){
  if(Speicher_Channel == 28){
    for(int x=Segment_4a+Abstand_Pixel; x<=Segment_4b; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_4c(int r, int g, int b){
  if(Speicher_Channel == 28){
    for(int x=Segment_4b+Abstand_Pixel; x<=Segment_4c; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_4d(int r, int g, int b){
  if(Speicher_Channel == 28){
    for(int x=Segment_4c+Abstand_Pixel; x<=Segment_4d; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}


void ansteuern_4e(int r, int g, int b){
  if(Speicher_Channel == 28){
    for(int x=Segment_4d+Abstand_Pixel; x<=Segment_4e; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}


void ansteuern_4f(int r, int g, int b){
  if(Speicher_Channel == 28){
    for(int x=Segment_4e+Abstand_Pixel; x<=Segment_4f; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}

void ansteuern_4g(int r, int g, int b){
  if(Speicher_Channel == 28){
    for(int x=Segment_4f+Abstand_Pixel; x<=Segment_4g; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}


void ansteuern_4h(int r, int g, int b){
  if(Speicher_Channel == 28){
    for(int x=Segment_4g+Abstand_Pixel; x<=NUMPIXELS; x++){
      pixels.setPixelColor(x, pixels.Color(r,g,b));
      pixels.show(); // Durchführen der Pixel-Ansteuerung
    }
  }
}
#endif

#ifdef FunktionRelais
void RelaisStartWerte(){
  // ------------------Relais-----------------------
  digitalWrite(relais_1, RelaisAnAus2);
  digitalWrite(relais_2, RelaisAnAus2);
  digitalWrite(relais_3, RelaisAnAus2);
  digitalWrite(relais_4, RelaisAnAus2);
  digitalWrite(relais_5, RelaisAnAus2);
  digitalWrite(relais_6, RelaisAnAus2);
}

void DMXwerteRelais(){
  if(Speicher_Channel == 7){
    RelaisDMXversatz = 0;
  }
  if(Speicher_Channel == 10){
    RelaisDMXversatz = 3;
  }
  if(Speicher_Channel == 16){
    RelaisDMXversatz = 9;
  }
  if(Speicher_Channel == 22){
    RelaisDMXversatz = 15;
  }
  if(Speicher_Channel == 28){
    RelaisDMXversatz = 21;
  }
  relaisLevel_1 = DMXSerial.read(dmxBaseCh + RelaisDMXversatz + relaisCh_1);     // Dmx Relais Wert auslesen
  relaisLevel_2 = DMXSerial.read(dmxBaseCh + RelaisDMXversatz + relaisCh_2);     // Dmx Relais Wert auslesen
  relaisLevel_3 = DMXSerial.read(dmxBaseCh + RelaisDMXversatz + relaisCh_3);     // Dmx Relais Wert auslesen
  relaisLevel_4 = DMXSerial.read(dmxBaseCh + RelaisDMXversatz + relaisCh_4);     // Dmx Relais Wert auslesen
  relaisLevel_5 = DMXSerial.read(dmxBaseCh + RelaisDMXversatz + relaisCh_5);     // Dmx Relais Wert auslesen
  relaisLevel_6 = DMXSerial.read(dmxBaseCh + RelaisDMXversatz + relaisCh_6);     // Dmx Relais Wert auslesen
}

void AnsteuernRelais(){
  if(relaisLevel_1 > WertRelaisHigh){
    digitalWrite(relais_1, RelaisAnAus1);
  }
  if(relaisLevel_1 < WertRelaisLow){
    digitalWrite(relais_1, RelaisAnAus2);
  }


  if(relaisLevel_2 > WertRelaisHigh){
    digitalWrite(relais_2, RelaisAnAus1);
  }
  if(relaisLevel_2 < WertRelaisLow){
    digitalWrite(relais_2, RelaisAnAus2);
  }


  if(relaisLevel_3 > WertRelaisHigh){
    digitalWrite(relais_3, RelaisAnAus1);
  }
  if(relaisLevel_3 < WertRelaisLow){
    digitalWrite(relais_3, RelaisAnAus2);
  }


  if(relaisLevel_4 > WertRelaisHigh){
    digitalWrite(relais_4, RelaisAnAus1);
  }
  if(relaisLevel_4 < WertRelaisLow){
    digitalWrite(relais_4, RelaisAnAus2);
  }


  if(relaisLevel_5 > WertRelaisHigh){
    digitalWrite(relais_5, RelaisAnAus1);
  }
  if(relaisLevel_5 < WertRelaisLow){
    digitalWrite(relais_5, RelaisAnAus2);
  }


  if(relaisLevel_6 > WertRelaisHigh){
    digitalWrite(relais_6, RelaisAnAus1);
  }
  if(relaisLevel_6 < WertRelaisLow){
    digitalWrite(relais_6, RelaisAnAus2);
  }
}
#endif

#ifdef FunktionDMX
void DMXauslesen(){
  #ifdef FunktionRelais
    DMXwerteRelais();
  #endif
  
  effect = DMXSerial.read(dmxBaseCh + effectCh);     // Dmx Effect Wert auslesen
  effectspeed = DMXSerial.read(dmxBaseCh + effectspeedCh);     // Dmx Effect Speed Wert auslesen
  
  redLevel_1 = DMXSerial.read(dmxBaseCh + redCh_1);     // Dmx Rot Wert auslesen
  grnLevel_1 = DMXSerial.read(dmxBaseCh + grnCh_1);     // Dmx Grün Wert auslesen
  bluLevel_1 = DMXSerial.read(dmxBaseCh + bluCh_1);     // Dmx Blau Wert auslesen
  
  // Segmente
  redLevel_2 = DMXSerial.read(dmxBaseCh + redCh_2);     // Dmx Rot Wert auslesen
  grnLevel_2 = DMXSerial.read(dmxBaseCh + grnCh_2);     // Dmx Grün Wert auslesen
  bluLevel_2 = DMXSerial.read(dmxBaseCh + bluCh_2);     // Dmx Blau Wert auslesen
  
  redLevel_3 = DMXSerial.read(dmxBaseCh + redCh_3);     // Dmx Rot Wert auslesen
  grnLevel_3 = DMXSerial.read(dmxBaseCh + grnCh_3);     // Dmx Grün Wert auslesen
  bluLevel_3 = DMXSerial.read(dmxBaseCh + bluCh_3);     // Dmx Blau Wert auslesen
  
  redLevel_4 = DMXSerial.read(dmxBaseCh + redCh_4);     // Dmx Rot Wert auslesen
  grnLevel_4 = DMXSerial.read(dmxBaseCh + grnCh_4);     // Dmx Grün Wert auslesen
  bluLevel_4 = DMXSerial.read(dmxBaseCh + bluCh_4);     // Dmx Blau Wert auslesen

  redLevel_5 = DMXSerial.read(dmxBaseCh + redCh_5);     // Dmx Rot Wert auslesen
  grnLevel_5 = DMXSerial.read(dmxBaseCh + grnCh_5);     // Dmx Grün Wert auslesen
  bluLevel_5 = DMXSerial.read(dmxBaseCh + bluCh_5);     // Dmx Blau Wert auslesen

  redLevel_6 = DMXSerial.read(dmxBaseCh + redCh_6);     // Dmx Rot Wert auslesen
  grnLevel_6 = DMXSerial.read(dmxBaseCh + grnCh_6);     // Dmx Grün Wert auslesen
  bluLevel_6 = DMXSerial.read(dmxBaseCh + bluCh_6);     // Dmx Blau Wert auslesen

  redLevel_7 = DMXSerial.read(dmxBaseCh + redCh_7);     // Dmx Rot Wert auslesen
  grnLevel_7 = DMXSerial.read(dmxBaseCh + grnCh_7);     // Dmx Grün Wert auslesen
  bluLevel_7 = DMXSerial.read(dmxBaseCh + bluCh_7);     // Dmx Blau Wert auslesen

  redLevel_8 = DMXSerial.read(dmxBaseCh + redCh_8);     // Dmx Rot Wert auslesen
  grnLevel_8 = DMXSerial.read(dmxBaseCh + grnCh_8);     // Dmx Grün Wert auslesen
  bluLevel_8 = DMXSerial.read(dmxBaseCh + bluCh_8);     // Dmx Blau Wert auslesen

  
  strobe = DMXSerial.read(dmxBaseCh + strobeCh);     // Dmx Strobe Wert auslesen
  brightness = DMXSerial.read(dmxBaseCh + brightnessCh);     // Dmx Dimmer Wert auslesen
  brightness = map(brightness, 1, 255, 0, maxBrightness);   // Dimmer Wert runter rechnen

  redLevel_1 = float(redLevel_1) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  grnLevel_1 = float(grnLevel_1) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  bluLevel_1 = float(bluLevel_1) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden

  // Segmente
  redLevel_2 = float(redLevel_2) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  grnLevel_2 = float(grnLevel_2) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  bluLevel_2 = float(bluLevel_2) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden

  redLevel_3 = float(redLevel_3) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  grnLevel_3 = float(grnLevel_3) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  bluLevel_3 = float(bluLevel_3) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden

  redLevel_4 = float(redLevel_4) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  grnLevel_4 = float(grnLevel_4) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  bluLevel_4 = float(bluLevel_4) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden

  redLevel_5 = float(redLevel_5) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  grnLevel_5 = float(grnLevel_5) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  bluLevel_5 = float(bluLevel_5) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden

  redLevel_6 = float(redLevel_6) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  grnLevel_6 = float(grnLevel_6) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  bluLevel_6 = float(bluLevel_6) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden

  redLevel_7 = float(redLevel_7) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  grnLevel_7 = float(grnLevel_7) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  bluLevel_7 = float(bluLevel_7) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden

  redLevel_8 = float(redLevel_8) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  grnLevel_8 = float(grnLevel_8) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  bluLevel_8 = float(bluLevel_8) * (float(brightness) / float(maxBrightness));    // Dimmer auf Farbe anwenden
}

// -------------------------------------------------------------------------------------

void DMXsteuern(){
  int wait = map(effectspeed, 0, 255, 0, 75); // Wert, untere Grenze, obere Grenze, untere Grenze Ziel, obere Grenze Ziel

  if (effect < 9) {
    #ifdef FunktionRelais
      AnsteuernRelais();
    #endif

    #ifdef FunktionLED
      // CH07
      ansteuern(redLevel_1, grnLevel_1, bluLevel_1);    // RGB Farben ansteuern
      
      // CH10
      ansteuern_1a(redLevel_1, grnLevel_1, bluLevel_1);    // RGB Farben ansteuern_1
      ansteuern_1b(redLevel_2, grnLevel_2, bluLevel_2);    // RGB Farben ansteuern_2
  
      // CH16
      ansteuern_2a(redLevel_1, grnLevel_1, bluLevel_1);    // RGB Farben ansteuern_1
      ansteuern_2b(redLevel_2, grnLevel_2, bluLevel_2);    // RGB Farben ansteuern_2
      ansteuern_2c(redLevel_3, grnLevel_3, bluLevel_3);    // RGB Farben ansteuern_3
      ansteuern_2d(redLevel_4, grnLevel_4, bluLevel_4);    // RGB Farben ansteuern_4
  
      // CH22
      ansteuern_3a(redLevel_1, grnLevel_1, bluLevel_1);    // RGB Farben ansteuern_1
      ansteuern_3b(redLevel_2, grnLevel_2, bluLevel_2);    // RGB Farben ansteuern_2
      ansteuern_3c(redLevel_3, grnLevel_3, bluLevel_3);    // RGB Farben ansteuern_3
      ansteuern_3d(redLevel_4, grnLevel_4, bluLevel_4);    // RGB Farben ansteuern_4
      ansteuern_3e(redLevel_5, grnLevel_5, bluLevel_5);    // RGB Farben ansteuern_5
      ansteuern_3f(redLevel_6, grnLevel_6, bluLevel_6);    // RGB Farben ansteuern_6
  
      // CH28
      ansteuern_4a(redLevel_1, grnLevel_1, bluLevel_1);    // RGB Farben ansteuern_1
      ansteuern_4b(redLevel_2, grnLevel_2, bluLevel_2);    // RGB Farben ansteuern_2
      ansteuern_4c(redLevel_3, grnLevel_3, bluLevel_3);    // RGB Farben ansteuern_3
      ansteuern_4d(redLevel_4, grnLevel_4, bluLevel_4);    // RGB Farben ansteuern_4
      ansteuern_4e(redLevel_5, grnLevel_5, bluLevel_5);    // RGB Farben ansteuern_5
      ansteuern_4f(redLevel_6, grnLevel_6, bluLevel_6);    // RGB Farben ansteuern_6
      ansteuern_4g(redLevel_7, grnLevel_7, bluLevel_7);    // RGB Farben ansteuern_7
      ansteuern_4h(redLevel_8, grnLevel_8, bluLevel_8);    // RGB Farben ansteuern_8
    #endif
    
  }
  else if (effect < 30 && effect > 10){
    rainbow(wait);
  }
  else if (effect < 50 && effect > 31){
    Lauflicht_2(wait);
  }
  else if (effect < 255 && effect > 51){
    Lauflicht_4_S();
  }
}
#endif


void dmxwertzeigen(){     // DMX Wert anzeigen d001
  display.showNumberDec(dmxBaseCh, false, 4, 0);
  display.setSegments(d, 1, 0);
}

// -------------------------------------------------------------------------------------

void ModeMenue(){     // Autoprogramm Wert zeigen A001
  display.showNumberDec(ModeMenueZaehler, false, 4, 0);
  display.setSegments(A, 1, 0);
  
  #ifdef FunktionSerial
    Serial.print("Mode Menu: ");
    Serial.println(ModeMenueZaehler);
  #endif
}

// -------------------------------------------------------------------------------------

void Channel(){     // Channel einstellen
  display.setSegments(C, 1, 0);
  TasterAuslesen();
  if(StatusTasteUP == HIGH){   // Wenn UP gedrückt, dann
    chanel_nummer++;
  }
  if(StatusTasteDOWN == HIGH){   // Wenn DOWN gedrückt, dann
    chanel_nummer--;
  }
  pruefenChannel();
  if(StatusTasteENTER == HIGH){   // Wenn Enter gedrückt, dann
    EEPROM.write(Adresse_12, Speicher_Channel);   // Sperichere Wert im EEPROM
    delay(100);
  }
  
  switch(chanel_nummer){
    case 1:   // CH07
      display.showNumberDec(7, false, 4, 0);
      Speicher_Channel = 7;
      break;
    case 2:   // CH10
      display.showNumberDec(10, false, 4, 0);
      Speicher_Channel = 10;
      break;
    case 3:   // CH16
      display.showNumberDec(16, false, 4, 0);
      Speicher_Channel = 16;
      break;
    case 4:   // CH22
      display.showNumberDec(22, false, 4, 0);
      Speicher_Channel = 22;
      break;
    case 5:   // CH28
      display.showNumberDec(28, false, 4, 0);
      Speicher_Channel = 28;
      break;
  }
}

void eepromauslesen(){
  #ifdef FunktionSerial
    Serial.print(EEPROM.read(Adresse_0));Serial.print(EEPROM.read(Adresse_1));Serial.print(EEPROM.read(Adresse_2));Serial.print(EEPROM.read(Adresse_3));Serial.print(EEPROM.read(Adresse_4));Serial.print(EEPROM.read(Adresse_5));Serial.print(EEPROM.read(Adresse_6));Serial.print(EEPROM.read(Adresse_7));Serial.print(EEPROM.read(Adresse_8));Serial.println(EEPROM.read(Adresse_9));
  #endif
  dmxBaseCh = (EEPROM.read(Adresse_0) * 1) + (EEPROM.read(Adresse_1) * 2) + (EEPROM.read(Adresse_2) * 4) + (EEPROM.read(Adresse_3) * 8) + (EEPROM.read(Adresse_4) * 16) + (EEPROM.read(Adresse_5) * 32) + (EEPROM.read(Adresse_6) * 64) + (EEPROM.read(Adresse_7) * 128) + (EEPROM.read(Adresse_8) * 256) + (EEPROM.read(Adresse_9) * 512);
  #ifdef FunktionSerial
    Serial.println(dmxBaseCh);
  #endif
}

// -------------------------------------------------------------------------------------

void eepromschreiben(){
  if(StatusTasteENTER == HIGH){
    int speicher = dmxBaseCh / 2;
    int zs = speicher *2;
    int neu = dmxBaseCh - zs;    // Speichern in bit
    #ifdef FunktionSerial
      Serial.print("dmxBaseCh ist: "); Serial.print(dmxBaseCh);
      Serial.print("\t"); Serial.println(speicher);
      Serial.print("\t"); Serial.println(zs);
      Serial.print("\t"); Serial.println(neu);
      Serial.println("--------------");
    #endif
    if(speicher == 0 && zs == 0){
      stelle = 0;
      goto bailout;
    }
    
    int speicher1;
    speicher1 = speicher / 2;
    int zs1 = speicher1 *2;
    int neu1 = speicher - zs1;    // Speichern in bit
    #ifdef FunktionSerial
      Serial.print("\t"); Serial.println(speicher1);
      Serial.print("\t"); Serial.println(zs1);
      Serial.print("\t"); Serial.println(neu1);
      Serial.println("--------------");
    #endif
    if(speicher1 == 0 && zs1 == 0){
      stelle = 1;
      goto bailout;
    }

    int speicher2;
    speicher2 = speicher1 / 2;
    int zs2 = speicher2 *2;
    int neu2 = speicher1 - zs2;    // Speichern in bit
    #ifdef FunktionSerial
      Serial.print("\t"); Serial.println(speicher2);
      Serial.print("\t"); Serial.println(zs2);
      Serial.print("\t"); Serial.println(neu2);
      Serial.println("--------------");
    #endif
    if(speicher2 == 0 && zs2 == 0){
      stelle = 2;
      goto bailout;
    }

    int speicher3;
    speicher3 = speicher2 / 2;
    int zs3 = speicher3 *2;
    int neu3 = speicher2 - zs3;    // Speichern in bit
    #ifdef FunktionSerial
      Serial.print("\t"); Serial.println(speicher3);
      Serial.print("\t"); Serial.println(zs3);
      Serial.print("\t"); Serial.println(neu3);
      Serial.println("--------------");
    #endif
    if(speicher3 == 0 && zs3 == 0){
      stelle = 3;
      goto bailout;
    }

    int speicher4;
    speicher4 = speicher3 / 2;
    int zs4 = speicher4 *2;
    int neu4 = speicher3 - zs4;    // Speichern in bit
    #ifdef FunktionSerial
      Serial.print("\t"); Serial.println(speicher4);
      Serial.print("\t"); Serial.println(zs4);
      Serial.print("\t"); Serial.println(neu4);
      Serial.println("--------------");
    #endif
    if(speicher4 == 0 && zs4 == 0){
    stelle = 4;
      goto bailout;
    }

    int speicher5;
    speicher5 = speicher4 / 2;
    int zs5 = speicher5 *2;       // Wenn beide hier null dann abbruch
    int neu5 = speicher4 - zs5;    // Speichern in bit
    #ifdef FunktionSerial
      Serial.print("\t"); Serial.println(speicher5);
      Serial.print("\t"); Serial.println(zs5);
      Serial.print("\t"); Serial.println(neu5);
      Serial.println("--------------");
    #endif
    if(speicher5 == 0 && zs5 == 0){
      stelle = 5;
      goto bailout;
    }

    int speicher6;
    speicher6 = speicher5 / 2;
    int zs6 = speicher6 *2;       // Wenn beide hier null dann abbruch
    int neu6 = speicher5 - zs6;    // Speichern in bit
    #ifdef FunktionSerial
      Serial.print("\t"); Serial.println(speicher6);
      Serial.print("\t"); Serial.println(zs6);
      Serial.print("\t"); Serial.println(neu6);
      Serial.println("--------------");
    #endif
    if(speicher6 == 0 && zs6 == 0){
      stelle = 6;
      goto bailout;
    }

    int speicher7;
    speicher7 = speicher6 / 2;
    int zs7 = speicher7 *2;       // Wenn beide hier null dann abbruch
    int neu7 = speicher6 - zs7;    // Speichern in bit
    #ifdef FunktionSerial
      Serial.print("\t"); Serial.println(speicher7);
      Serial.print("\t"); Serial.println(zs7);
      Serial.print("\t"); Serial.println(neu7);
      Serial.println("--------------");
    #endif
    if(speicher7 == 0 && zs7 == 0){
      stelle = 7;
      goto bailout;
    }

    int speicher8;
    speicher8 = speicher7 / 2;
    int zs8 = speicher8 *2;       // Wenn beide hier null dann abbruch
    int neu8 = speicher7 - zs8;    // Speichern in bit
    #ifdef FunktionSerial
      Serial.print("\t"); Serial.println(speicher8);
      Serial.print("\t"); Serial.println(zs8);
      Serial.print("\t"); Serial.println(neu8);
      Serial.println("--------------");
    #endif
    if(speicher8 == 0 && zs8 == 0){
      stelle = 8;
      goto bailout;
    }

    int speicher9;
    speicher9 = speicher8 / 2;
    int zs9 = speicher9 *2;       // Wenn beide hier null dann abbruch
    int neu9 = speicher8 - zs9;    // Speichern in bit
    #ifdef FunktionSerial
      Serial.print("\t"); Serial.println(speicher9);
      Serial.print("\t"); Serial.println(zs9);
      Serial.print("\t"); Serial.println(neu9);
      Serial.println("--------------");
    #endif
    if(speicher9 == 0 && zs9 == 0){
      stelle = 9;
      goto bailout;
    }

    
    bailout:
    #ifdef FunktionSerial
      Serial.print("Bit: ");
    #endif
    
    if(stelle == 9){
      #ifdef FunktionSerial
        Serial.print(neu9);Serial.print(neu8);Serial.print(neu7);Serial.print(neu6);Serial.print(neu5);Serial.print(neu4);Serial.print(neu3);Serial.print(neu2);Serial.print(neu1);Serial.println(neu);
      #endif
      EEPROM.write(Adresse_9, neu9);EEPROM.write(Adresse_8, neu8);EEPROM.write(Adresse_7, neu7);EEPROM.write(Adresse_6, neu6);EEPROM.write(Adresse_5, neu5);EEPROM.write(Adresse_4, neu4);EEPROM.write(Adresse_3, neu3);EEPROM.write(Adresse_2, neu2);EEPROM.write(Adresse_1, neu1);EEPROM.write(Adresse_0, neu);
    }
    if(stelle == 8){
      #ifdef FunktionSerial
        Serial.print(neu8);Serial.print(neu7);Serial.print(neu6);Serial.print(neu5);Serial.print(neu4);Serial.print(neu3);Serial.print(neu2);Serial.print(neu1);Serial.println(neu);
      #endif
      EEPROM.write(Adresse_9, 0);EEPROM.write(Adresse_8, neu8);EEPROM.write(Adresse_7, neu7);EEPROM.write(Adresse_6, neu6);EEPROM.write(Adresse_5, neu5);EEPROM.write(Adresse_4, neu4);EEPROM.write(Adresse_3, neu3);EEPROM.write(Adresse_2, neu2);EEPROM.write(Adresse_1, neu1);EEPROM.write(Adresse_0, neu);
    }
    if(stelle == 7){
      #ifdef FunktionSerial
        Serial.print(neu7);Serial.print(neu6);Serial.print(neu5);Serial.print(neu4);Serial.print(neu3);Serial.print(neu2);Serial.print(neu1);Serial.println(neu);
      #endif
      EEPROM.write(Adresse_9, 0);EEPROM.write(Adresse_8, 0);EEPROM.write(Adresse_7, neu7);EEPROM.write(Adresse_6, neu6);EEPROM.write(Adresse_5, neu5);EEPROM.write(Adresse_4, neu4);EEPROM.write(Adresse_3, neu3);EEPROM.write(Adresse_2, neu2);EEPROM.write(Adresse_1, neu1);EEPROM.write(Adresse_0, neu);
    }
    if(stelle == 6){
      #ifdef FunktionSerial
        Serial.print(neu6);Serial.print(neu5);Serial.print(neu4);Serial.print(neu3);Serial.print(neu2);Serial.print(neu1);Serial.println(neu);
      #endif
      EEPROM.write(Adresse_9, 0);EEPROM.write(Adresse_8, 0);EEPROM.write(Adresse_7, 0);EEPROM.write(Adresse_6, neu6);EEPROM.write(Adresse_5, neu5);EEPROM.write(Adresse_4, neu4);EEPROM.write(Adresse_3, neu3);EEPROM.write(Adresse_2, neu2);EEPROM.write(Adresse_1, neu1);EEPROM.write(Adresse_0, neu);
    }
    if(stelle == 5){
      #ifdef FunktionSerial
        Serial.print(neu5);Serial.print(neu4);Serial.print(neu3);Serial.print(neu2);Serial.print(neu1);Serial.println(neu);
      #endif
      EEPROM.write(Adresse_9, 0);EEPROM.write(Adresse_8, 0);EEPROM.write(Adresse_7, 0);EEPROM.write(Adresse_6, 0);EEPROM.write(Adresse_5, neu5);EEPROM.write(Adresse_4, neu4);EEPROM.write(Adresse_3, neu3);EEPROM.write(Adresse_2, neu2);EEPROM.write(Adresse_1, neu1);EEPROM.write(Adresse_0, neu);
    }
    if(stelle == 4){
      #ifdef FunktionSerial
        Serial.print(neu4);Serial.print(neu3);Serial.print(neu2);Serial.print(neu1);Serial.println(neu);
      #endif
      EEPROM.write(Adresse_9, 0);EEPROM.write(Adresse_8, 0);EEPROM.write(Adresse_7, 0);EEPROM.write(Adresse_6, 0);EEPROM.write(Adresse_5, 0);EEPROM.write(Adresse_4, neu4);EEPROM.write(Adresse_3, neu3);EEPROM.write(Adresse_2, neu2);EEPROM.write(Adresse_1, neu1);EEPROM.write(Adresse_0, neu);
    }
    if(stelle == 3){
      #ifdef FunktionSerial
        Serial.print(neu3);Serial.print(neu2);Serial.print(neu1);Serial.println(neu);
      #endif
      EEPROM.write(Adresse_9, 0);EEPROM.write(Adresse_8, 0);EEPROM.write(Adresse_7, 0);EEPROM.write(Adresse_6, 0);EEPROM.write(Adresse_5, 0);EEPROM.write(Adresse_4, 0);EEPROM.write(Adresse_3, neu3);EEPROM.write(Adresse_2, neu2);EEPROM.write(Adresse_1, neu1);EEPROM.write(Adresse_0, neu);
    }
    if(stelle == 2){
      #ifdef FunktionSerial
        Serial.print(neu2);Serial.print(neu1);Serial.println(neu);
      #endif
      EEPROM.write(Adresse_9, 0);EEPROM.write(Adresse_8, 0);EEPROM.write(Adresse_7, 0);EEPROM.write(Adresse_6, 0);EEPROM.write(Adresse_5, 0);EEPROM.write(Adresse_4, 0);EEPROM.write(Adresse_3, 0);EEPROM.write(Adresse_2, neu2);EEPROM.write(Adresse_1, neu1);EEPROM.write(Adresse_0, neu);
    }
    if(stelle == 1){
      #ifdef FunktionSerial
        Serial.print(neu1);Serial.println(neu);
      #endif
      EEPROM.write(Adresse_9, 0);EEPROM.write(Adresse_8, 0);EEPROM.write(Adresse_7, 0);EEPROM.write(Adresse_6, 0);EEPROM.write(Adresse_5, 0);EEPROM.write(Adresse_4, 0);EEPROM.write(Adresse_3, 0);EEPROM.write(Adresse_2, 0);EEPROM.write(Adresse_1, neu1);EEPROM.write(Adresse_0, neu);
    }
    if(stelle == 0){
      #ifdef FunktionSerial
        Serial.println(neu);
      #endif
      EEPROM.write(Adresse_9, 0);EEPROM.write(Adresse_8, 0);EEPROM.write(Adresse_7, 0);EEPROM.write(Adresse_6, 0);EEPROM.write(Adresse_5, 0);EEPROM.write(Adresse_4, 0);EEPROM.write(Adresse_3, 0);EEPROM.write(Adresse_2, 0);EEPROM.write(Adresse_1, 0);EEPROM.write(Adresse_0, neu);   // Wert in den EEPROM schreiben
    }
  }
}

// -------------------------------------------------------------------------------------

void EEPROMschreibenErstesMal(){
  // DMX-Adresse
  EEPROM.write(Adresse_9, 0);EEPROM.write(Adresse_8, 0);EEPROM.write(Adresse_7, 0);EEPROM.write(Adresse_6, 0);EEPROM.write(Adresse_5, 0);EEPROM.write(Adresse_4, 0);EEPROM.write(Adresse_3, 0);EEPROM.write(Adresse_2, 0);EEPROM.write(Adresse_1, 0);EEPROM.write(Adresse_0, 1);   // Wert in den EEPROM schreiben
  EEPROM.write(Adresse_10, 1);    // Speicher Hauptmenü DMX-Modus oder Autoprogramm
  EEPROM.write(Adresse_11, 1);    // Speicher Autoprogramm
  EEPROM.write(Adresse_12, 7);    // Speicher Channel Auswahl
}

// ------------------Poti-----------------------
void AnalogLichtSteuern(){
  #ifdef FunktionPoti
  PotiAuslesen();
  PotiWertBerechnen();
  
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(PotiWertNeu_2, PotiWertNeu_3 ,PotiWertNeu_4)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}



// -------------------------------------------------------------------------------------


void rainbow(int wait) {
  #ifdef FunktionLED
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<pixels.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
      pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
    }
    pixels.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
  #endif
}

// -------------------------------------------------------------------------------------

void AlleRot(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(255,0,0)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}

// -------------------------------------------------------------------------------------

void AlleGruen(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(0,255,0)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}

// -------------------------------------------------------------------------------------

void AlleBlau(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(0,0,255)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}

// -------------------------------------------------------------------------------------

void AlleGelb(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(255,255,0)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}

// -------------------------------------------------------------------------------------

void AlleTuerkis(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(0,255,255)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}

// -------------------------------------------------------------------------------------

void AlleLila(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(255,0,255)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}

// -------------------------------------------------------------------------------------

void AlleWeiss(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(255,255,255)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}

// -------------------------------------------------------------------------------------

void AlleRot_Reverse(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(NUMPIXELS -x, pixels.Color(255,0,0)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}

// -------------------------------------------------------------------------------------

void AlleWeiss_Reverse(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(NUMPIXELS -x, pixels.Color(255,255,255)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}

// -------------------------------------------------------------------------------------

void AlleAus(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(0,0,0)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  #endif
}

// -------------------------------------------------------------------------------------

void PingPong(){
  #ifdef FunktionLED
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(255,0,0)); // Pixel leuchtet in der Farbe Rot
    pixels.setPixelColor(x-1, pixels.Color(0,0,0)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
    delay(25);
  }
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(0,255,0)); // Pixel leuchtet in der Farbe Rot
    pixels.setPixelColor(x-1, pixels.Color(0,0,0)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
    delay(25);
  }
  for(int x=Pixel_0; x<=NUMPIXELS; x++){
    pixels.setPixelColor(x, pixels.Color(0,0,255)); // Pixel leuchtet in der Farbe Rot
    pixels.setPixelColor(x-1, pixels.Color(0,0,0)); // Pixel leuchtet in der Farbe Rot
    pixels.show(); // Durchführen der Pixel-Ansteuerung
    delay(25);
  }
  #endif
}

// -------------------------------------------------------------------------------------

void Lauflicht_1(int wait_1_1, int wait_1_2){
  int nummer = random(1, 8);
  int wait_1_3 = random(wait_1_1, wait_1_2);
  switch(nummer){
    case 1:
      AlleRot();
      delay(wait_1_3);
      break;
    case 2:
      AlleGruen();
      delay(wait_1_3);
      break;
    case 3:
      AlleBlau();
      delay(wait_1_3);
      break;
    case 4:
      AlleGelb();
      delay(wait_1_3);
      break;
    case 5:
      AlleTuerkis();
      delay(wait_1_3);
      break;
    case 6:
      AlleLila();
      delay(wait_1_3);
      break;
    case 7:
      AlleWeiss();
      delay(wait_1_3);
      break;
  }
}

// -------------------------------------------------------------------------------------

void Lauflicht_2(int wait_2_1){
  int nummer = random(1, 8);
  switch(nummer){
    case 1:
      AlleRot();
      delay(wait_2_1);
      break;
    case 2:
      AlleGruen();
      delay(wait_2_1);
      break;
    case 3:
      AlleBlau();
      delay(wait_2_1);
      break;
    case 4:
      AlleGelb();
      delay(wait_2_1);
      break;
    case 5:
      AlleTuerkis();
      delay(wait_2_1);
      break;
    case 6:
      AlleLila();
      delay(wait_2_1);
      break;
    case 7:
      AlleWeiss();
      delay(wait_2_1);
      break;
  }
}

// -------------------------------------------------------------------------------------

void Lauflicht_3(int wait_3_1, int wait_3_2){
  int nummer = random(1, 8);
  int wait_3_3 = random(wait_3_1, wait_3_2);
  switch(nummer){
    case 1:
      AlleRot();
      delay(wait_3_3);
      break;
    case 2:
      AlleGruen();
      delay(wait_3_3);
      break;
    case 3:
      AlleBlau();
      delay(wait_3_3);
      break;
    case 4:
      AlleGelb();
      delay(wait_3_3);
      break;
    case 5:
      AlleTuerkis();
      delay(wait_3_3);
      break;
    case 6:
      AlleLila();
      delay(wait_3_3);
      break;
    case 7:
      AlleWeiss();
      delay(wait_3_3);
      break;
  }
}

// -------------------------------------------------------------------------------------

void Lauflicht_4_S(){
  int nummer;
  #ifdef FunktionSound
  int val_digital = digitalRead(sound_digital);
  if (val_digital == HIGH){
    nummer = random(1, 8);
  }
  #endif
  switch(nummer){
    case 1:
      AlleRot();
      break;
    case 2:
      AlleGruen();
      break;
    case 3:
      AlleBlau();
      break;
    case 4:
      AlleGelb();
      break;
    case 5:
      AlleTuerkis();
      break;
    case 6:
      AlleLila();
      break;
    case 7:
      AlleWeiss();
      break;
  }
}

// -------------------------------------------------------------------------------------

void Lauflicht_5(int wait_5){
  int nummer = random(1, 8);
  switch(nummer){
    case 1:
      AlleRot_Reverse();
      delay(wait_5);
      break;
    case 2:
      AlleGruen();
      delay(wait_5);
      break;
    case 3:
      AlleBlau();
      delay(wait_5);
      break;
    case 4:
      AlleGelb();
      delay(wait_5);
      break;
    case 5:
      AlleTuerkis();
      delay(wait_5);
      break;
    case 6:
      AlleLila();
      delay(wait_5);
      break;
    case 7:
      AlleWeiss_Reverse();
      delay(wait_5);
      break;
  }
}

// ------------Hauptmenü-------------------------------------------------------------------------

void HauptMenu(){
  TasterAuslesen();
  
  if(StatusTasteMODE == HIGH){
    delay(100);
    Hauptmenue = 1;
    Hauptmenuezaehler++;
    if(Hauptmenuezaehler >= 5){
      Hauptmenuezaehler = 1;
    }
  }

  switch(Hauptmenuezaehler){
    case 1:
      #ifdef FunktionSerial
        Serial.println("DMX-Modus-Nix");
      #endif
      DMX_Modus_Nix = 1;
      while(DMX_Modus_Nix == 1){
        display.setSegments(Bindestrich, 4, 0);

        #ifdef FunktionDMX
          DMXauslesen();
          DMXsteuern();
        #endif
        
        TasterAuslesenDMXModus();

        if(StatusTasteENTER == HIGH){   // Wenn Enter gedrückt, dann
          EEPROM.write(Adresse_10, Hauptmenuezaehler);   // Sperichere Wert im EEPROM
          delay(100);
        }
        if(StatusTasteMODE == HIGH){
          delay(100);
          DMX_Modus_Nix = 0;
          Hauptmenuezaehler++;
          if(Hauptmenuezaehler >= 5){
            Hauptmenuezaehler = 1;
          }
          AlleAus();
          return;
        }
      }
      break;
    case 2:
      #ifdef FunktionSerial
        Serial.println("DMX-Adresse");
      #endif
      DMX_Adresse_einstellen = 1;
      dmxwertzeigen();
      while(DMX_Adresse_einstellen == 1){
        TasterAuslesen();
        
        dmxBaseChberechnen();
        eepromschreiben();

        if(StatusTasteMODE == HIGH){
          delay(100);
          DMX_Adresse_einstellen = 0;
          Hauptmenuezaehler++;
          if(Hauptmenuezaehler >= 5){
            Hauptmenuezaehler = 1;
          }
          return;
        }
      }
      
      break;
    case 3:
      #ifdef FunktionSerial
        Serial.println("Channel");
      #endif
      Channel_einstellen = 1;
      display.setSegments(C, 1, 0);
      while(Channel_einstellen == 1){
        Channel();
        
        TasterAuslesen();
        delay(100);
        
        if(StatusTasteMODE == HIGH){
          delay(100);
          Channel_einstellen = 0;
          Hauptmenuezaehler++;
          if(Hauptmenuezaehler >= 5){
            Hauptmenuezaehler = 1;
          }
          return;
        }
      }
      
      break;
    case 4:
      #ifdef FunktionSerial
        Serial.println("Autoprogramme");
      #endif
      Autoprogramme = 1;
      ModeMenue();
      while(Autoprogramme == 1){
        
        ModeFunktion();
        
        if(StatusTasteMODE == HIGH){
          delay(100);
          Autoprogramme = 0;
          Hauptmenuezaehler++;
          if(Hauptmenuezaehler >= 5){
            Hauptmenuezaehler = 1;
          }
          return;
        }
      }
      
      break;
  }
}

#ifdef FunktionPoti
void PotiAuslesen(){
  PotiWert_1 = 0.6 * PotiWert_1 + 0.4 * analogRead(Poti_1);      // Werte glätten 0.6 * PotiWert_1 + 0.4 * 
  PotiWert_2 = 0.6 * PotiWert_2 + 0.4 * analogRead(Poti_2);
  PotiWert_3 = 0.6 * PotiWert_3 + 0.4 * analogRead(Poti_3);
  PotiWert_4 = 0.6 * PotiWert_4 + 0.4 * analogRead(Poti_4);
  
  /*#ifdef FunktionSerial
    Serial.print("Poti_1"); Serial.print("\t");
    Serial.print("Poti_2"); Serial.print("\t");
    Serial.print("Poti_3"); Serial.print("\t");
    Serial.print("Poti_4"); Serial.println("\t");
    Serial.print(PotiWert_1); Serial.print("\t");
    Serial.print(PotiWert_2); Serial.print("\t");
    Serial.print(PotiWert_3); Serial.print("\t");
    Serial.print(PotiWert_4); Serial.println("\t");
  #endif*/
}

// -------------------------------------------------------------------------------------

void PotiWertBerechnen(){
  PotiWertNeu_1 = map(PotiWert_1, 0, 1023, 0, 255);   // Wert runter rechnen
  PotiWertNeu_2 = map(PotiWert_2, 0, 1023, 0, 255);   // Wert runter rechnen
  PotiWertNeu_3 = map(PotiWert_3, 0, 1023, 0, 255);   // Wert runter rechnen
  PotiWertNeu_4 = map(PotiWert_4, 0, 1023, 0, 255);   // Wert runter rechnen

  // PotiWertNeu_4 = float(PotiWertNeu_4) * (float(PotiWertNeu_1) / float(maxBrightness));    // Dimmer auf Farbe anwenden
  PotiWertNeu_2 = float(PotiWertNeu_2) * (float(PotiWertNeu_1) / float(255));    // Dimmer auf Farbe anwenden
  PotiWertNeu_3 = float(PotiWertNeu_3) * (float(PotiWertNeu_1) / float(255));    // Dimmer auf Farbe anwenden
  PotiWertNeu_4 = float(PotiWertNeu_4) * (float(PotiWertNeu_1) / float(255));    // Dimmer auf Farbe anwenden
  
  /*#ifdef FunktionSerial
    Serial.print("Poti_1"); Serial.print("\t");
    Serial.print("Poti_2"); Serial.print("\t");
    Serial.print("Poti_3"); Serial.print("\t");
    Serial.print("Poti_4"); Serial.println("\t");
    Serial.print(PotiWertNeu_1); Serial.print("\t");
    Serial.print(PotiWertNeu_2); Serial.print("\t");
    Serial.print(PotiWertNeu_3); Serial.print("\t");
    Serial.print(PotiWertNeu_4); Serial.println("\t");
  #endif*/
}
#endif

void TasterAuslesen(){
  StatusTasteUP = digitalRead(TasteUP);
  StatusTasteDOWN = digitalRead(TasteDOWN);
  StatusTasteMODE = digitalRead(TasteMODE);
  StatusTasteENTER = digitalRead(TasteENTER);

  #ifdef FunktionSerial
    Serial.print("UP"); Serial.print("\t");
    Serial.print("DOWN"); Serial.print("\t");
    Serial.print("MODE"); Serial.print("\t");
    Serial.print("Enter"); Serial.println("\t");
    Serial.print(StatusTasteUP); Serial.print("\t");
    Serial.print(StatusTasteDOWN); Serial.print("\t");
    Serial.print(StatusTasteMODE); Serial.print("\t");
    Serial.print(StatusTasteENTER); Serial.println("\t");
  #endif
  
  delay(100);
}

// -------------------------------------------------------------------------------------

void TasterAuslesenDMXModus(){
  StatusTasteMODE = digitalRead(TasteMODE);
  StatusTasteENTER = digitalRead(TasteENTER);
  #ifdef FunktionSerial
    Serial.println(StatusTasteMODE);
    Serial.println(StatusTasteENTER);
  #endif
  delay(50);
}

void zaehlen(){     // DMX Adresse bei Wert Überschreitung (Hoch / Runter zählen) verändern 
  dmxwertzeigen();
  if(dmxBaseCh < 1){
    dmxBaseCh = 512;
  }
  if(dmxBaseCh >= 513){
    dmxBaseCh = 1;
  }
  #ifdef FunktionSerial
    Serial.print("DMX-Adresse: ");
    Serial.println(dmxBaseCh);
  #endif
}

// -------------------------------------------------------------------------------------


void dmxBaseChberechnen(){
  if(StatusTasteUP == HIGH){
    dmxBaseCh++;
  }
  if(StatusTasteDOWN == HIGH){
    dmxBaseCh--;  
  }
  zaehlen();
}

void pruefenModeMenu(){
  if(ModeMenueZaehler < 1){
    ModeMenueZaehler = 20;
  }
  if(ModeMenueZaehler >= 21){
    ModeMenueZaehler = 1;
  }
}

// -------------------------------------------------------------------------------------

void pruefenChannel(){
  if(chanel_nummer >= 6){
    chanel_nummer = 1;
  }
  if(chanel_nummer < 1){
    chanel_nummer = 5;
  }
}

// -------------------------------------------------------------------------------------

void pruefenChannelNumber(){
  if(Speicher_Channel == 7){
    chanel_nummer = 1;
  }
  if(Speicher_Channel == 10){
    chanel_nummer = 2;
  }
  if(Speicher_Channel == 16){
    chanel_nummer = 3;
  }
  if(Speicher_Channel == 22){
    chanel_nummer = 4;
  }
  if(Speicher_Channel == 28){
    chanel_nummer = 5;
  }
}



// ------------------------------------Setup-----------------------------------------
void setup() {
  #ifdef EEPROMschreiben
    EEPROMschreibenErstesMal();
  #endif
  
  #ifdef FunktionSerial
    Serial.begin(9600);
  #endif
  
  // ------------------LED-----------------------
  #ifdef FunktionLED
    pixels.begin(); // Initialisierung der NeoPixel
  #endif

  // ------------------DMX-----------------------
  #ifdef FunktionDMX
    DMXSerial.init(DMXReceiver);  // Pin 2
  #endif

  // ------------------Taster-----------------------
  pinMode(TasteUP, INPUT_PULLUP);
  pinMode(TasteDOWN, INPUT_PULLUP);
  pinMode(TasteMODE, INPUT_PULLUP);
  pinMode(TasteENTER, INPUT_PULLUP);

  #ifdef FunktionRelais
    // ------------------Relais-----------------------
    pinMode(relais_1, OUTPUT);
    pinMode(relais_2, OUTPUT);
    pinMode(relais_3, OUTPUT);
    pinMode(relais_4, OUTPUT);
    pinMode(relais_5, OUTPUT);
    pinMode(relais_6, OUTPUT);
  
    RelaisStartWerte();
  #endif
  
// ------------------Sound-----------------------
  #ifdef FunktionSound
  pinMode(sound_digital, INPUT);
  pinMode(sound_analog, INPUT);
  #endif

  // ------------------Display-----------------------
  display.setBrightness(0x0f);

  // ------------------EEPROM-----------------------
  eepromauslesen();

  // Werte aus dem EEPROM lesen
  Hauptmenuezaehler = EEPROM.read(Adresse_10);
  ModeMenueZaehler = EEPROM.read(Adresse_11);
  Speicher_Channel = EEPROM.read(Adresse_12);

  pruefenChannelNumber();
}

// ------------------------------------Loop-----------------------------------------
void loop() {
  HauptMenu();
}