/*
    VoiceRecognition.ino
        Demonstrate the voice recognition technology.

    This sketch supports the following board types:
        Arduino Nano RP2040 Connect
            https://store.arduino.cc/products/arduino-nano-rp2040-connect
        Arduino Nano 33 BLE Sense
            https://store.arduino.cc/products/arduino-nano-33-ble-sense
        Arduino Portenta H7
            https://store.arduino.cc/products/portenta-h7

    To run the free demo with a pre-defined keyword set:
        1. Verify & upload the GetSerialNumber.ino sketch to get the board's serial number and print it on the console output.
        2. Go to the DSpotter Free Demo License Page:
            https://tool.cyberon.com.tw/ArduinoDSpotterAuth/FDMain.php
        3. Enter the board's serial number and click Submit. The license data of the board will show on the webpage.
        4. Copy & paste the license data into the CybLicense.h file under your sketch folder.
        5. Verify & upload the sketch to the board.
        6. The voice recognition is ready to go.

    To try out a custom keyword set:
        Users can create their own custom keyword sets through the DSpotter Model Configuration Page:
            https://tool.cyberon.com.tw/ArduinoDSpotterAuth/CTMain.php
        and obtain the Custom Trial Model & the corresponding Custom Trial License to test on the board.
            1. Go to the DSpotter Model Configuration Page.
            2. Select the board type of your board, enter your email address and enter the board's serial number.
            3. Read & agree to the end-user license agreement, then click Next.
            4. You can choose to create a new model set or import model from an existing .dsproj file.
            5. To create a new model, select the desired language for the model and click Create.
            6. You can set the trigger & command keyword simply by text input.
                    The keyword ID will be used in the sketch to identify the speech recognition results.
            7. After completing trigger keyword configuration, click Next and proceed to command keyword configuration.
            8. If all the configurations in the review project page are correct, click Confirm.
                    The Custom Trial Model, the corresponding Custom Trial License, and the .dsproj file will be sent to your email.
            9. For example, you would receive files like:
                    Custom Trial Model     Model_xxxxxxxxxxx.h
                    Custom Trial License   CybLicense_xxxxxxxxxxx.h
                    DSpotter Project File  Model_xxxxxxxxx_Arduino_[board type].dsproj
               xxxxxxxxxxx is a random tamp number use to differ each download.
               Copy & paste Model_xxxxxxxxxxx.h and CybLicense_xxxxxxxxxxx.h to your sketch folder.
            10. Modify the following lines in the sketch:
                    #include "CybLicense.h" ----> #include "CybLicense_xxxxxxxxxxx.h"
                    #include "Model_L1.h" ----> #include "Model_xxxxxxxxxxx.h"
            11. Verify & upload the sketch to the board.
            12. The voice recognition with your custom keyword set is ready to go.
        Please note that the Custom Trial Model and the Custom Trial License are free to use with some limitations:
            1. Recognize 50 times each reboot. Once the number of recognitions is reached, the model will stop until the next reboot.
            2. There is a 20-second delay between entering the trigger mode and starting to recognize the trigger mode keywords.

    To unlock the limitations of the Custom Trial version:
        Users can remove the limitations of the Custom Trial version by upgrading to the Custom Formal version
        through the DSpotter License Activation Page:
            https://tool.cyberon.com.tw/ArduinoDSpotterAuth/CFMain.php
        to obtain the Custom Formal Model & the corresponding Custom Formal License for the final product.
            1. Purchase a valid voucher code from Arduino Online Store:
                    https://store.arduino.cc/speech-recognition-engine
            2. Go to the DSpotter License Activation Page.
            3. Select the board type of your board, then enter your email address, the board's serial number, and the voucher code you just purchased.
            4. Import the .dsproj file you received with your tested Custom Trial Model.
            5. Read & agree to the end-user license agreement, then click Next.
            6. If all the configurations in the review project page are correct, click Confirm.
                    The Custom Formal Model, the corresponding Custom Formal License will be sent to your email.
            7. For example, you would receive files like:
                    Custom Formal Model     Model_xxxxxxxxxxx.h
                    Custom Formal License   CybLicense_xxxxxxxxxxx.h
               xxxxxxxxxxx is a random tamp number use to differ each download.
               Please keep these data properly.
               Copy & paste Model_xxxxxxxxxxx.h and CybLicense_xxxxxxxxxxx.h to your sketch folder.
            8. Modify the following lines in the sketch:
                    #include "CybLicense.h" ----> #include "CybLicense_xxxxxxxxxxx.h"
                    #include "Model_L1.h" ----> #include "Model_xxxxxxxxxxx.h"
            9. Verify & upload the sketch to the board.
            10.The voice recognition for the custom keyword set is ready and without any limitation.

    16 Feb 2023 by Cyberon Corporation.
    https://www.cyberon.com.tw/index.php?lang=en
*/
#include <Arduino.h>
#include <DSpotterSDK_MakerHL.h>
#include <LED_Control.h>

// The DSpotter License Data.
#include "CybLicense.h"
#define DSPOTTER_LICENSE g_lpdwLicense

// The DSpotter Keyword Model Data.
#if defined(TARGET_ARDUINO_NANO33BLE) || defined(TARGET_PORTENTA_H7) || defined(TARGET_NICLA_VISION)
// For ARDUINO_NANO33BLE and PORTENTA_H7
#include "Model_L1.h"  // The packed level one model file.
// For NANO_RP2040_CONNECT
#elif defined(TARGET_NANO_RP2040_CONNECT)
#include "Model_L0.h"  // The packed level zero model file.
#endif
#define DSPOTTER_MODEL g_lpdwModel

// The VR engine object. Only can exist one, otherwise not worked.
static DSpotterSDKHL g_oDSpotterSDKHL;

#define left_right 10
#define left_center 11
#define left_left 12

#define right_right 2
#define right_center 3
#define right_left 4

#define NUM_LED 3

int slot;
bool right_on = false;
bool left_on = false;

// Callback function for VR engine
void VRCallback(int nFlag, int nID, int nScore, int nSG, int nEnergy) {
  if (nFlag == DSpotterSDKHL::InitSuccess) {
    //ToDo
  } else if (nFlag == DSpotterSDKHL::GetResult) {
    Serial.print("GetResult: ");
    Serial.println(nID);

    switch (nID) {
      case 10000:  // 10000 a destra
      case 10002:  // 10000 destra
      case 10007:  // 10007 frecce a destra
      case 10010:  // 10010 frecce destra
        right_on = true;
        // digitalWrite(left_right, HIGH);
        // digitalWrite(left_center, HIGH);
        // digitalWrite(left_left, HIGH);
        break;
      case 10001:  // 10001	a sinistra
      case 10003:  // 10003	sinistra
      case 10008:  // 10008	frecce a sinistra
      case 10011:  // 10011	frecce sinistra
        left_on = true;

        // digitalWrite(right_right, HIGH);
        // digitalWrite(right_center, HIGH);
        // digitalWrite(right_left, HIGH);
        break;
      case 10005:  // 10005	stop
      case 10006:  // 10006	frecce stop
        right_on = false;
        left_on = false;
        // digitalWrite(right_right, LOW);
        // digitalWrite(right_center, LOW);
        // digitalWrite(right_left, LOW);
        // digitalWrite(left_right, LOW);
        // digitalWrite(left_center, LOW);
        // digitalWrite(left_left, LOW);
        break;

      case 10004:  //	quattro frecce
      case 10009:  //frecce quattro frecce
        right_on = true;
        left_on = true;
        break;
      default:
        Serial.println("Not implemented yet!");
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
        LED_BUILTIN_Off();
        break;
      case DSpotterSDKHL::CommandStage:
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

void setup() {
  // Init LED control
  LED_Init_All();

  // Init Serial output for show debug info
  Serial.begin(9600);
  while (!Serial)
    ;
  DSpotterSDKHL::ShowDebugInfo(true);

  // Init VR engine & Audio
  if (g_oDSpotterSDKHL.Init(DSPOTTER_LICENSE, sizeof(DSPOTTER_LICENSE), DSPOTTER_MODEL, VRCallback) != DSpotterSDKHL::Success)
    return;

  pinMode(left_right, OUTPUT);
  pinMode(left_center, OUTPUT);
  pinMode(left_left, OUTPUT);
  pinMode(right_right, OUTPUT);
  pinMode(right_center, OUTPUT);
  pinMode(right_left, OUTPUT);

  digitalWrite(left_right, HIGH);
  digitalWrite(left_center, HIGH);
  digitalWrite(left_left, HIGH);
  digitalWrite(right_right, HIGH);
  digitalWrite(right_center, HIGH);
  digitalWrite(right_left, HIGH);
  delay(500);
  digitalWrite(left_right, LOW);
  digitalWrite(left_center, LOW);
  digitalWrite(left_left, LOW);
  digitalWrite(right_right, LOW);
  digitalWrite(right_center, LOW);
  digitalWrite(right_left, LOW);
}

void loop() {
  // Do VR
  g_oDSpotterSDKHL.DoVR();
  unsigned long time = millis();
  unsigned long millis_in_second = time % 1000;
  unsigned long current_slot = millis_in_second / 100;
  if (slot != current_slot) {
    slot = current_slot;
    Serial.print("****************** seconds: => ");
    Serial.print(millis_in_second);
    Serial.print("    slot => ");
    Serial.println(current_slot);

  }
  switch (slot) {
    case 0:
      if (right_on) {
        digitalWrite(right_left, HIGH);
        // digitalWrite(right_center, HIGH);
        // digitalWrite(right_right, HIGH);
      } else {
        digitalWrite(right_right, LOW);
        digitalWrite(right_center, LOW);
        digitalWrite(right_left, LOW);
      }
      if (left_on) {
        digitalWrite(left_right, HIGH);
        // digitalWrite(left_center, HIGH);
        // digitalWrite(left_left, HIGH);
      } else {
        digitalWrite(left_right, LOW);
        digitalWrite(left_center, LOW);
        digitalWrite(left_left, LOW);
      }
      break;
    case 1:
      if (right_on) {
        digitalWrite(right_left, HIGH);
        digitalWrite(right_center, HIGH);
        // digitalWrite(right_right, HIGH);
      } else {
        digitalWrite(right_right, LOW);
        digitalWrite(right_center, LOW);
        digitalWrite(right_left, LOW);
      }
      if (left_on) {
        digitalWrite(left_right, HIGH);
        digitalWrite(left_center, HIGH);
        // digitalWrite(left_left, HIGH);
      } else {
        digitalWrite(left_right, LOW);
        digitalWrite(left_center, LOW);
        digitalWrite(left_left, LOW);
      }
      break;
    case 2:
      if (right_on) {
        digitalWrite(right_left, HIGH);
        digitalWrite(right_center, HIGH);
        digitalWrite(right_right, HIGH);
      } else {
        digitalWrite(right_right, LOW);
        digitalWrite(right_center, LOW);
        digitalWrite(right_left, LOW);
      }
      if (left_on) {
        digitalWrite(left_right, HIGH);
        digitalWrite(left_center, HIGH);
        digitalWrite(left_left, HIGH);
      } else {
        digitalWrite(left_right, LOW);
        digitalWrite(left_center, LOW);
        digitalWrite(left_left, LOW);
      }
      break;
    case 3:
        digitalWrite(right_left, LOW);
        digitalWrite(right_center, LOW);
        digitalWrite(right_right, LOW);
        digitalWrite(left_right, LOW);
        digitalWrite(left_left, LOW);
        digitalWrite(left_center, LOW);
      break;
    default:
        digitalWrite(right_left, LOW);
        digitalWrite(right_center, LOW);
        digitalWrite(right_right, LOW);
        digitalWrite(left_right, LOW);
        digitalWrite(left_center, LOW);
        digitalWrite(left_left, LOW);
      break;


  }

}
