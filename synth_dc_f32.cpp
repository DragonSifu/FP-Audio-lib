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

#include "synth_dc_f32.h"

void AudioSynthWaveformDc_F32::update(void)
{
	audio_block_f32_t *block;
	float *p, *end;
	float count;

	block = allocate_f32();
	if (!block) return;
//	inc = allocate_f32();
//	if (!inc) return;
	uint32_t block_length = (uint32_t)block->length;
	p = (float *)(block->data);
	end = p + AUDIO_BLOCK_SAMPLES;

	if (state == 0) {
		// steady DC output, simply fill the buffer with fixed value
		arm_fill_f32(magnitude,p,block_length);
 	} else {
		// transitioning to a new DC level
		//count = (target - magnitude) / increment;
		count = (target-magnitude)/ increment;
//				Serial.println(count);
//				Serial.println(target);
		if (count >= AUDIO_BLOCK_SAMPLES) {
			// this update will not reach the target
//	    arm_scale_f32(incblock->data,increment,inc->data,AUDIO_BLOCK_SAMPLES);
//		arm_offset_f32(inc->data,magnitude,p,AUDIO_BLOCK_SAMPLES);

		do {
				magnitude += increment;
				*p++ = magnitude;
			} while (p < end);
		} else {
			// this update reaches the target
			while (count >= 0) {
				count --;
				magnitude += increment;
				*p++ = magnitude;
			}
			magnitude = target;
			state = 0;
			if(p < end) {
				arm_fill_f32(magnitude,p,end-p);
//				Serial.println(end-p);
			}
		}
	}
	AudioStream_F32::transmit(block);
	AudioStream_F32::release(block);
}



