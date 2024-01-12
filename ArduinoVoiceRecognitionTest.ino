/*
    VoiceRecognition.ino
        Demonstrate the voice recognition technology.

    See also:
        https://store.arduino.cc/products/speech-recognition-engine
        https://www.cyberon.com.tw/index.php?lang=en
    
    This sketch has been customized to support ONLY the following board types:
        Arduino Nano RP2040 Connect
            https://store.arduino.cc/products/arduino-nano-rp2040-connect

    For info about generating your licence (in demo mode) go to the DSpotter Free Demo License Page:
            https://tool.cyberon.com.tw/ArduinoDSpotterAuth/FDMain.php

    To unlock the limitations of the Custom Trial version:
            https://tool.cyberon.com.tw/ArduinoDSpotterAuth/CFMain.php

    https://www.cyberon.com.tw/index.php?lang=en
*/
#include <Arduino.h>
#include <DSpotterSDK_MakerHL.h>
#include <NeoPixelConnect.h>
#include <LED_Control.h>

// The DSpotter License Data.
#include "CybLicense.h"
#define DSPOTTER_LICENSE g_lpdwLicense

#include "Model_L0.h"  // The packed level zero model file.
#define DSPOTTER_MODEL g_lpdwModel

// The VR engine object. Only can exist one, otherwise not worked.
static DSpotterSDKHL g_oDSpotterSDKHL;

#define RIGHT_PIN 16 // GPIO16 -> PIN D4 on PR2040, because of the NeoPixels' library mapping
#define LEFT_PIN 19  // GPIO19 -> PIN D7 on PR2040, likewise
#define BUZZER_PIN 8

#define MAXIMUM_NUM_NEOPIXELS 9
#define AUTO_OFF_TIMEOUT 5 * 1000

NeoPixelConnect rightStrip(RIGHT_PIN, MAXIMUM_NUM_NEOPIXELS, pio1, 0);
NeoPixelConnect leftStrip(LEFT_PIN, MAXIMUM_NUM_NEOPIXELS, pio1, 2);

unsigned long slot;
bool right_on = false;
bool left_on = false;
bool buzzer_playing = false;
unsigned long auto_off_time = 0;

// Callback function for VR engine
void VRCallback(int nFlag, int nID, int nScore, int nSG, int nEnergy) {
  if (nFlag == DSpotterSDKHL::InitSuccess) {
    Serial.println("DSpotter initialized successfully");
  } else if (nFlag == DSpotterSDKHL::GetResult) {
    Serial.print("GetResult: ");
    Serial.println(nID);

    switch (nID) {
      case 10000:  // 10000 a destra
      case 10002:  // 10000 destra
      case 10007:  // 10007 frecce a destra
      case 10010:  // 10010 frecce destra
        right_on = true;
        if (left_on) {
          left_on = false;
        }
        break;
      case 10001:  // 10001	a sinistra
      case 10003:  // 10003	sinistra
      case 10008:  // 10008	frecce a sinistra
      case 10011:  // 10011	frecce sinistra
        left_on = true;
        if (right_on) {
          right_on = false;
        }
        break;
      case 10005:  // 10005	stop
      case 10006:  // 10006	frecce stop
        right_on = false;
        left_on = false;
        break;

      case 10004:  //	quattro frecce
      case 10009:  //frecce quattro frecce
        right_on = true;
        left_on = true;
        break;
      default:
        // Serial.println("Not implemented yet!")
        ;
    }
    /*
      Trigger:
      100	frecce

      Command:
      10000	a destra
      10001	a sinistra
      10002	destra
      10003	sinistra
      10004	quattro frecce
      10005	stop
      10006	frecce stop
      10007	frecce a destra
      10008	frecce a sinistra
      10009	frecce quattro frecce
      10010	frecce destra
      10011	frecce sinistra
      */
    /*
      When getting an recognition result,
      the following index and scores are also return to the VRCallback function:
          nID        The result command id
          nScore     nScore is used to evaluate how good or bad the result is.
                     The higher the score, the more similar the voice and the result command are.
          nSG        nSG is the gap between the voice and non-command (Silence/Garbage) models.
                     The higher the score, the less similar the voice and non-command (Silence/Garbage) models are.
          nEnergy    nEnergy is the voice energy level.
                     The higher the score, the louder the voice.
      */
    //ToDo
  } else if (nFlag == DSpotterSDKHL::ChangeStage) {
    switch (nID) {
      case DSpotterSDKHL::TriggerStage:
        LED_RGB_Off();
        beep();
        delay(100);
        beep();
        LED_BUILTIN_Off();
        auto_off_time = millis();
        // // turn off right light if it's only right
        // if (right_on && !left_on) {
        //   right_on = false;
        // }

        // // turn off left light if it's only left
        // if (!right_on && left_on) {
        //   left_on = false;
        // }

        break;
      case DSpotterSDKHL::CommandStage:
        beep();
        auto_off_time = 0;
        LED_BUILTIN_On();
        break;
      default:
        break;
    }
  } else if (nFlag == DSpotterSDKHL::GetError) {
    if (nID == DSpotterSDKHL::LicenseFailed) {
      //Serial.print("DSpotter license failed! The serial number of your device is ");
      //Serial.println(DSpotterSDKHL::GetSerialNumber());
    }
    g_oDSpotterSDKHL.Release();
    while (1)
      ;  //hang loop
  } else if (nFlag == DSpotterSDKHL::LostRecordFrame) {
    //ToDo
  }
}


void beep() {
  for (int i=0; i<30; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(1000);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(1000);
  }
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);

  // Init LED control
  LED_Init_All();

  // Init Serial output for show debug info
  Serial.begin(9600);
  while (!Serial && millis() < 2000);

  DSpotterSDKHL::ShowDebugInfo(false);

  for (int i = 0; i < 3; i++) {
    beep();
    delay(500);
  }

  // Init VR engine & Audio
  if (g_oDSpotterSDKHL.Init(DSPOTTER_LICENSE, sizeof(DSPOTTER_LICENSE), DSPOTTER_MODEL, VRCallback) != DSpotterSDKHL::Success)
    return;

  // pixels_right.begin();
  for (int i = 0; i < 3; i++) {
    LED_BUILTIN_On();
    rightStrip.neoPixelFill(255, 44, 0, true);
    leftStrip.neoPixelFill(255, 44, 0, true);
    delay(250);

    LED_BUILTIN_Off();
    rightStrip.neoPixelClear(true);
    leftStrip.neoPixelClear(true);

    delay(250);
  }
}

void loop() {
  // Do VR
  g_oDSpotterSDKHL.DoVR();
  unsigned long now = millis();
  unsigned long millis_in_second = now % 1000;
  unsigned long current_slot = millis_in_second / 100;

  // turn off automatically unless both lights are on
  if (auto_off_time != 0 && now > auto_off_time + AUTO_OFF_TIMEOUT ) {
    Serial.println("Should turn it off automatically");
    if (right_on && left_on) {
      // do nothing
    } else {
      right_on = false;
      left_on = false;
    }
    auto_off_time = 0;
  }

  if ((right_on || left_on) && slot == 0) {
    if (!buzzer_playing) {
      beep();
      buzzer_playing = true;
    }
  } else {
    buzzer_playing = false;
  }
  if (slot != current_slot) {
    slot = current_slot;
    if (right_on) {
      if (slot == MAXIMUM_NUM_NEOPIXELS) {
        rightStrip.neoPixelClear(true);
      } else {
        rightStrip.neoPixelSetValue(slot, 255, 44, 0, true);
      }
    } else {
      rightStrip.neoPixelClear(true);
    }

    if (left_on) {
      if (slot == MAXIMUM_NUM_NEOPIXELS) { // <- clear everything at last iteration
        leftStrip.neoPixelClear(true);

      } else {
        leftStrip.neoPixelSetValue(slot, 255, 44, 0, true);
      }
    } else {
      leftStrip.neoPixelClear(true);
    }
  }
}
