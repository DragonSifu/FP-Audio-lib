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

#ifndef synth_dc_f32_h_
#define synth_dc_f32_h_
#include "Arduino.h"
#include "AudioStream_f32.h"
#include "utility/dspinst.h"
#include "arm_math.h"

// compute (a - b) / c
// handling 32 bit interger overflow at every step
// without resorting to slow 64 bit math


class AudioSynthWaveformDc_F32 : public AudioStream_F32
{
public:
	AudioSynthWaveformDc_F32() :  AudioStream_F32(0, NULL), state(0), magnitude(0) {}
	// immediately jump to the new DC level
	void amplitude(float n) {
		if (n > 1.0) n = 1.0;
		else if (n < -1.0) n = -1.0;
//		int i;
//		int32_t m = (int32_t)(n * 0x7fff);
		__disable_irq();
		magnitude = n;
		state = 0;
//		incblock = allocate_f32();
//		for(i=0;i<AUDIO_BLOCK_SAMPLES;i++)
//			incblock->data[i]=i+1;
		__enable_irq();
	}
	// slowly transition to the new DC level
	void amplitude(float n, float milliseconds) {
		if (milliseconds <= 0.0) {
			amplitude(n);
			return;
		}
		if (n > 1.0) n = 1.0;
		else if (n < -1.0) n = -1.0;
		float c = (milliseconds*(AUDIO_SAMPLE_RATE_EXACT/1000.0));
		if (c == 0) {
			amplitude(n);
			return;
		}
//		int32_t t = (int32_t)(n * 0x7fff);
		__disable_irq();
		target = n;
		if (target == magnitude) {
			state = 0;
			__enable_irq();
			return;
		}
		increment = (target-magnitude)/ c;
		if (increment == 0) {
			increment = (target > magnitude) ? 1 : -1;
		}
		state = 1;
		__enable_irq();
	}
	float read(void) {
		return magnitude;
	}
	virtual void update(void);
private:
	uint8_t  state;     // 0=steady output, 1=transitioning
	float  magnitude; // current output
	float  target;    // designed output (while transitiong)
	float  increment; // adjustment per sample (while transitiong)
//	audio_block_f32_t *incblock;


};

#endif
