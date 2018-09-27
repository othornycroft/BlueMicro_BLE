/*
Copyright 2018 <Pierre Constantineau, Julian Komaromy>

3-Clause BSD License

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "Key.h"
#include <array>
#include <utility>
#include <cstdint>
#include "hid_keycodes.h"
#include "keyboard_config.h"

// ToDo: There seems to be lots of redundency in data.
// ToDo: consider interrupts or GPIOTE
// ToDo: there must be a better way to debounce
// ToDo: consider multiple boards and the merging of multiple buffers/modifiers and layer requests.
// ToDo: Action Keycodes - Bluetooth commands
// ToDo: Action Keycodes - Reset/DFU commands

Key::Key() {    // Constructor
;
}


/**************************************************************************************************************************/
// KEY SCANNING - THIS ROUTINE ONLY TAKES CARE OF THE DEBOUNCING LOGIC FOR EACH KEY
// TODO: debouncing on key release
/**************************************************************************************************************************/
 bool Key::scanMatrix(const int& currentState,unsigned long currentMillis, const int& row, const int& col)
 {
  // 
  if (currentState == 0 ) //if key pressed
  {
    if (timestamps[row][col] > 0) //if key was previously pressed
    {
     if((currentMillis - timestamps[row][col] )>=DEBOUNCETIME)
     {
        matrix[1][row][col] = 1;
     }
     else // not enough debounce time
     {
      ; // do nothing
     }
    }
    else //if key was NOT previously pressed
    {
      timestamps[row][col] = currentMillis;
    }
  }
  else // key not pressed
  {
     matrix[1][row][col] = 0;
     timestamps[row][col] = 0;
  }
    //matrix[0][row][col] = last pressed state
    matrix[0][row][col] = currentState;
    //matrix[1][row][col] = Active
    //timestamps[row][col] = last timestamp
}



/**************************************************************************************************************************/
// Called by callback function when remote data is received
/**************************************************************************************************************************/
void Key::updateRemoteLayer(uint8_t data)
{
  remoteLayer = data;
}

/**************************************************************************************************************************/
// Called by callback function when remote data is received
/**************************************************************************************************************************/
void Key::updateRemoteReport(uint8_t data0, uint8_t data1, uint8_t data2,uint8_t data3, uint8_t data4, uint8_t data5,uint8_t data6)
{
  remoteMod=data0;
  remoteReport[0]= data0;
  remoteReport[1]= data1;
  remoteReport[2]= data2;
  remoteReport[3]= data3;
  remoteReport[4]= data4;
  remoteReport[5]= data5;
  remoteReport[6]= data6;
}


/**************************************************************************************************************************/
void Key::resetRemoteReport()
{
  remoteReport[0]= 0;
  remoteReport[1]= 0;
  remoteReport[2]= 0;
  remoteReport[3]= 0;
  remoteReport[4]= 0;
  remoteReport[5]= 0;
  remoteReport[6]= 0;
}


/**************************************************************************************************************************/
void Key::resetReport() {
     bufferposition = 1;
     currentMod = 0;
     currentReport[0] = 0;
     currentReport[1] = 0;
     currentReport[2] = 0;
     currentReport[3] = 0;
     currentReport[4] = 0;
     currentReport[5] = 0;
     currentReport[6] = 0;
}


/**************************************************************************************************************************/
void Key::copyRemoteReport()
 {
     #if BLE_PERIPHERAL == 1  // PERIPHERAL MUST BE HANDLED DIFFERENTLY THAN CENTRAL - OTHERWISE, THE REPORTS WILL JUST KEEP BOUNCING FROM ONE BOARD TO THE OTHER 
        resetReport();
     #else
       currentMod = remoteMod;
//       currentReport[0] = remoteReport[0];
       bufferposition = 1;
       if (remoteReport[1]>0){currentReport[bufferposition] = remoteReport[1]; bufferposition++; }
       if (remoteReport[2]>0){currentReport[bufferposition] = remoteReport[2]; bufferposition++; }
       if (remoteReport[3]>0){currentReport[bufferposition] = remoteReport[3]; bufferposition++; }
       if (remoteReport[4]>0){currentReport[bufferposition] = remoteReport[4]; bufferposition++; }
       if (remoteReport[5]>0){currentReport[bufferposition] = remoteReport[5]; bufferposition++; }
       if (remoteReport[6]>0){currentReport[bufferposition] = remoteReport[6]; bufferposition++; }
     #endif
}
 
/**************************************************************************************************************************/
// Scan for layer changes - must do this first.
/**************************************************************************************************************************/
bool Key::updateLayer()
 {
  uint8_t prevlayer = localLayer;       // remember last layer
  bool layerChanged = false;

  for(int row = 0; row < MATRIX_ROWS; ++row) {
    for (int col = 0; col < MATRIX_COLS; ++col) {
      uint8_t keycode =  keymaps[prevlayer][row][col]; // get the key...
      if (matrix[1][row][col] != 1){continue;}
      if (keycode >= LAYER_0 && keycode < TOGGLE_0) {
        layerChanged = true;
        layerPushed = true;
        localLayer = keycode - LAYER_0;
        layerKey = &matrix[1][row][col];
      } else if (keycode >= TOGGLE_0) {
        layerChanged = true;
        localLayer = keycode - TOGGLE_0;
      }
    }
  }

  if (layerPushed && !(*layerKey)) {
    localLayer = 0;
    layerPushed = false;
    layerKey = false;
  }

  return layerChanged;
 }

/**************************************************************************************************************************/
// Update Mods - can be done before or after rest of matrix
// All 8 modifiers are handled through a 8-bit byte.  This is the standard HID implementation
/**************************************************************************************************************************/
 bool Key::updateModifiers()
 {
    uint8_t layer = localLayer;                     
   bool val = false;                                // indicates "changed" mods
   if (localLayer < remoteLayer)                    // Compares local Layer to Remote Layer requests and selects larger one.
  {
    layer = remoteLayer;
  }
   for(int row = 0; row < MATRIX_ROWS; ++row) {
    for (int col = 0; col < MATRIX_COLS; ++col) {
      uint8_t keycode =  keymaps[layer][row][col]; // get the key...
      if (matrix[1][row][col] != 1){keycode =0;}     
            switch(keycode){
            case KC_LCTRL:  currentMod  = currentMod  | 1; val = true; break;
            case KC_LSHIFT: currentMod  = currentMod  | 2; val = true; break;
            case KC_LALT:   currentMod  = currentMod  | 4; val = true; break;
            case KC_LGUI:   currentMod  = currentMod  | 8; val = true; break;
            case KC_RCTRL:  currentMod  = currentMod  | 16;  val = true; break;
            case KC_RSHIFT: currentMod  = currentMod  | 32;  val = true; break;
            case KC_RALT:   currentMod  = currentMod  | 64;  val = true; break;
            case KC_RGUI:   currentMod  = currentMod  | 128;  val = true; break;
           }
      }
    }
   return val;
 }


/**************************************************************************************************************************/
 
 bool Key::getReport()
 {
  uint8_t layer = localLayer;
  resetReport();
  copyRemoteReport();
  updateLayer();
  updateModifiers();
 
  if (localLayer < remoteLayer)
  {
    layer = remoteLayer;
  }


  for(int row = 0; row < MATRIX_ROWS; ++row) {
    for (int col = 0; col < MATRIX_COLS; ++col) {
      uint8_t keycode =  keymaps[layer][row][col]; // get the key...
      if (matrix[1][row][col] != 1){keycode =0;}
        switch(keycode){
        case KC_A ... KC_EXSEL: // key pressed
              currentReport[bufferposition] = keycode;
              bufferposition++;
              break;
        }
      if (bufferposition>6){bufferposition=1;} // lots of keys being pressed - looping around buffer
    }
  }
  currentReport[0] = currentMod;
  currentReport[7] = layer;

  if((currentReport[0] != 0) | (currentReport[1] != 0)| (currentReport[2] != 0)| (currentReport[3] != 0)| (currentReport[4] != 0)| (currentReport[5] != 0)| (currentReport[6] != 0))
  {reportEmpty = false;}
  else
  {reportEmpty = true;}
    
  return reportEmpty;
}

/**************************************************************************************************************************/
 

uint8_t Key::currentReport[8] = {0, 0, 0 ,0, 0, 0, 0, 0}; 
uint8_t Key::remoteReport[8]  = {0, 0, 0 ,0, 0, 0, 0, 0};
bool    Key::layerChanged = false;
bool    Key::reportEmpty = true;
uint8_t Key::localLayer = 0;
uint8_t Key::remoteLayer = 0;
uint8_t Key::remoteMod = 0;
uint8_t Key::currentMod = 0;
uint8_t Key::matrix[2][MATRIX_ROWS][MATRIX_COLS]  = {0};
unsigned long Key::timestamps[MATRIX_ROWS][MATRIX_COLS]  = {0};
uint8_t Key::bufferposition = 0;
bool    Key::layerPushed = false;
uint8_t *Key::layerKey = NULL;
