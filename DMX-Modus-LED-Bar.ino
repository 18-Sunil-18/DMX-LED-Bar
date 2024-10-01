#define FunktionDMX
//#define FunktionSerial    // Wenn Aktiviert, FunktionDMX Deaktivieren
//#define EEPROMschreiben

// ------------------library-----------------------
#include <Arduino.h>

// ------------------DMX-Werte-----------------------

byte brightness;

// ------------------DMX-Channel-----------------------

byte dmxBaseCh	=	0,   // DMX channel offsets from base channel
		 brightnessCh	=	1;

// Zeit für DMX Auslesen
unsigned long prevMillDMX = 0;

// Zeit für Taster
unsigned long previousMillis = 0;

// ------------------DMXSerial-----------------------

#ifdef FunktionDMX
  #include <DMXSerial.h>
#endif

#define DMXMODEPIN 2
extern DMXSerialClass DMXSerial;

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
      // Programmablauf
      break;
    case 2:
      // Programmablauf
      break;
}


#ifdef FunktionDMX
void DMXauslesen(){
    unsigned long curMillDMX = millis();

    delay(50);
    
    brightness = DMXSerial.read(dmxBaseCh + brightnessCh);     // Dmx Dimmer Wert auslesen
}

// -------------------------------------------------------------------------------------

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

void Lauflicht_1(int wait_1_1, int wait_1_2){
  int nummer = random(1, 8);
  int wait_1_3 = random(wait_1_1, wait_1_2);
  switch(nummer){
    case 1:
      AlleRot();  delay(wait_1_3);
      break;
    case 2:
      AlleGruen();  delay(wait_1_3);
      break;
    case 3:
      AlleBlau();  delay(wait_1_3);
      break;
    case 4:
      AlleGelb();  delay(wait_1_3);
      break;
    case 5:
      AlleTuerkis();  delay(wait_1_3);
      break;
    case 6:
      AlleLila();  delay(wait_1_3);
      break;
    case 7:
      AlleWeiss();  delay(wait_1_3);
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
    case 1:   // DMX Modus
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
    case 2:   // DMX-Adresse einstellen
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
    case 3:   // Channel einstellen
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
    case 4:   // Autoprogramme
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
          AlleAus();
          return;
        }
      }
      
      break;
  }
}


void DMXsteuern(){
  int wait = map(effectspeed, 0, 255, 0, 75); // Wert, untere Grenze, obere Grenze, untere Grenze Ziel, obere Grenze Ziel
    
  if (effect < 9) {
    #ifdef FunktionLED
      //LedAnsteuern_1_8();
      LedAnsteuern_11_88();
    #endif
    }
}


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
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= 50) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
  
    StatusTasteMODE = digitalRead(TasteMODE);
    StatusTasteENTER = digitalRead(TasteENTER);
    #ifdef FunktionSerial
      Serial.println(StatusTasteMODE);
      Serial.println(StatusTasteENTER);
    #endif
    //delay(50);
  }
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



// -----------------------------------------------------------------------------Setup----------------------------------------------------------------------------------
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
    //_DMX_setMode(RDATA);
  #endif

  // ------------------Taster-----------------------
  pinMode(TasteUP, INPUT_PULLUP);
  pinMode(TasteDOWN, INPUT_PULLUP);
  pinMode(TasteMODE, INPUT_PULLUP);
  pinMode(TasteENTER, INPUT_PULLUP);

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

// -----------------------------------------------------------------------------Loop----------------------------------------------------------------------------------
void loop() {
  HauptMenu();
}
