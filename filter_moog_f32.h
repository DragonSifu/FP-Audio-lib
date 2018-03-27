/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef filter_moog_f32_h_
#define filter_moog_f32_h_

#include "Arduino.h"
#include "AudioStream_F32.h"
#include "utility/dspinst.h"
#include "arm_math.h"

class AudioFilterMoog_F32: public AudioStream_F32
{
	float g;
	float q;
	float driv;
	float oct;
	float basef;
	
	float y_a;
	float y_b;
	float y_c;
	float y_d;
	float y_d_1;
	
	
public:
	AudioFilterMoog_F32() : AudioStream_F32(2, inputQueueArray) {
	frequency(1000);
	resonance(1);
	drive(1);
	octave(1);
	y_a = 0;
	y_b = 0;
	y_c = 0;
	y_d = 0;
	y_d_1 = 0;
	}
	void frequency(float freq,bool setf=true) {
		if (freq < 20.0) freq = 20.0;
		else if (freq > AUDIO_SAMPLE_RATE_EXACT/2.5) freq = AUDIO_SAMPLE_RATE_EXACT/2.5;
	    g = 1 - expf(-2 * tanf(2 * M_PI * freq/(2 * AUDIO_SAMPLE_RATE_EXACT)));
//		Serial.println(freq);
		if(setf)
			basef=freq;
	}
	void resonance(float qi) {
		if (qi < 0.7) qi = 0.7;
		else if (qi > 5.0) qi = 5.0;
		q=qi;
	}
	void octave(float n) {
		if (n < 0.0) n = 0.0;
		else if (n > 6.9999) n = 6.9999;
		oct=n;
	}
void drive(float d) {
	if (d > 10.0f) d = 10.0f;
	if (d < 0.1f) d = 0.1f;
	driv = d;
}	
virtual void update(void);
private:
	void update_fixed(const float *in,float *lp);
	void update_variable(const float *in, const float *ctl,	float *lp);
	audio_block_f32_t *inputQueueArray[2];
};

#endif
