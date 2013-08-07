/**
 * Copyright (c) 2013, Gordon S. Good (velo27 [at] yahoo [dot] com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * The author's name may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GORDON S. GOOD BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
// This sketch, for a PJRC Teensy microcontroller, turns the
// controller into a MIDI breath controller. I use a Freescale
// MPXV4006GP pressure sensor to read breath pressure.
// 
// This sketch outputs both MIDI breath control continuous
// controller data and monophonic aftertouch on MIDI channel 1.
// When used with a correctly designed synthesizer patch, this
// sketch can be used to articulate notes in a very expressive
// fashion, typically while playing the notes on a MIDI keyboard.

 
// The MIDI controller number for breath control
#define BREATH_CONTROLLER 2
// Send MIDI data on this channel
#define MIDI_CHANNEL 1
// Send continuous controller message no more than
// every CC_INTERVAL milliseconds
#define CC_INTERVAL 20
// Only send CC data if the pressure sensor reading
// a value larger than this. This value will need to
// be adjusted for the particular sensor in use.
#define BREATH_THRESHOLD 130

// The last time we sent a CC value
unsigned long ccSendTime = 0L;
// The value read from the sensor
int sensorValue;
// The CC value we will send
int ccVal;
// The last CC value we sent
int lastCcVal = 0;

void setup() {
  // Enable the LED for output - we'll turn it on
  // when a CC value is being sent.
  pinMode(13, OUTPUT);
}

void loop() {
  // Only read the sensor if enough time has passed
  if (millis() - ccSendTime > CC_INTERVAL) {
    // read the input on analog pin 0
    sensorValue = analogRead(A0);
    if (sensorValue > BREATH_THRESHOLD) {
      // Map the value, which may range from BREATH_THRESHOLD
      // to 1023, to a value in the range 0 to 127, which is
      // the valid range for a MIDI continuous controller
      ccVal = lastCcVal = map(sensorValue, BREATH_THRESHOLD, 1023, 0, 127);
      // And send the value as a MIDI CC message
      usbMIDI.sendControlChange(BREATH_CONTROLLER, ccVal, MIDI_CHANNEL);
      usbMIDI.sendAfterTouch(ccVal, MIDI_CHANNEL);
      digitalWrite(13, HIGH);
      ccSendTime = millis();
    } 
    else if (lastCcVal > 0) {
      // The pressure has just dropped below the threshold, so
      // send a CC value of zero
      usbMIDI.sendControlChange(BREATH_CONTROLLER, 0, MIDI_CHANNEL);
      usbMIDI.sendAfterTouch(0, MIDI_CHANNEL);
      ccSendTime = millis();
      digitalWrite(13, LOW);
      lastCcVal = 0;
    }
  }
}

