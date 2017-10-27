#include <OpenAudio_ArduinoLibrary.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=213,76
AudioSynthWaveformDc     dc17;           //xy=222,178
AudioSynthWaveformDc     dc16;           //xy=307,958
AudioEffectMultiply      multiply1;      //xy=528,22
AudioSynthWaveformDc     dc20;           //xy=534,179
AudioMixer4              mixer7;         //xy=778,49
AudioFilterMoog          moog1;          //xy=978,141
AudioEffectFlange        flange1;        //xy=1113,375
AudioEffectDelayExternal delayExt1;      //xy=1308,252
AudioMixer4              mixer6;         //xy=1590,176
AudioMixer4              mixer5;         //xy=1591,274
AudioOutputI2S           i2s1;           //xy=1717,482
AudioConnection          patchCord1(waveform1, 0, multiply1, 0);
AudioConnection          patchCord2(dc17, 0, multiply1, 1);
AudioConnection          patchCord3(multiply1, 0, mixer7, 0);
AudioConnection          patchCord4(dc20, 0, moog1, 1);
AudioConnection          patchCord5(mixer7, 0, moog1, 0);
AudioConnection          patchCord6(moog1, 0, mixer6, 1);
AudioConnection          patchCord7(flange1, delayExt1);
AudioConnection          patchCord8(flange1, 0, i2s1, 1);
AudioConnection          patchCord9(delayExt1, 0, mixer6, 0);
AudioConnection          patchCord10(mixer6, 0, i2s1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=1554,569
// GUItool: end automatically generated code


int dcVal;

//The setup function is called once when the system starts up
void setup(void) {
  //Start the USB serial link (to enable debugging)
  Serial.begin(115200); delay(500);
  Serial.println("Setup starting...");
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);

  //Allocate dynamically shuffled memory for the audio subsystem
  AudioMemory(10);  AudioMemory_F32(10);
//  sine2.begin(1,220,WAVEFORM_SAWTOOTH);
  waveform1.begin(1,440,WAVEFORM_SAWTOOTH);
  //Put your own setup code here
  moog1.frequency(652.0,true);
  moog1.resonance(4);
  moog1.octave(2);
  moog1.drive(1);
  //End of setup
  Serial.println("Setup complete.");
};


//After setup(), the loop function loops forever.
//Note that the audio modules are called in the background.
//They do not need to be serviced by the loop() function.
void loop(void) {
  volumeCheck();
  dcValCheck();
  hang(&waveform1,0,&dc17,0);
};
bool wastouched=false;
void hang( AudioSynthWaveform *sine, AudioSynthWaveform *sinex, AudioSynthWaveformDc *dc, AudioSynthWaveformDc *dcx)
{
  int touch = touchRead(30);
  bool touched = false;
  if (touch > 5000)
  {
    touched = true;
  }
  if (touched)
  {
    {
      Serial.print("touch ");
      Serial.print(" = ");
      Serial.println(touch);
    }
    wastouched=true;
    //once a pad is touched, a value from the note frquency froma table is looked up via a 2D table
    //with x corresponding to a scale and y corresponding to one of the eight notes on the drum.
    sine->frequency(220);
    dc->amplitude(1, 5);
//    sinex->frequency(440);
//    dcx->amplitude(1, 50);
    dc20.amplitude(1, 50);
   }
  if (!touched&&wastouched)
  {
    //one the pad is released, the note fades out with a decay val set by the dcVal knob
    dc->amplitude(0, dcVal);
 //   dcx->amplitude(0,dcVal);
    dc20.amplitude(-1,dcVal);
    wastouched=false;
  }

}
void volumeCheck()
{
  //check knob value for volume
  int knob = analogRead(A7);
  int fknob = analogRead(38);
  int shift = map(analogRead(37), 0, 1023, 0, 440);
  moog1.frequency(shift*4);
  float vol = (float)knob / 1280.0;
  float fBack = (float)fknob / 128;
  moog1.resonance(fBack);
  sgtl5000_1.volume(vol);
}
void dcValCheck()
{
  //check knob and set value as delay on dc constant for sine wave decay
  dcVal = map(analogRead(A6), 0, 1023, 1, 1000);
}
