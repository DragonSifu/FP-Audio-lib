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

#include "filter_moog_f32.h"


__inline__ float tanh_fast(float x)
{
  // rational tanh aproximation
  float x2 = x * x;
  float a = x * (135135.0f + x2 * (17325.0f + x2 * (378.0f + x2)));
  float b = 135135.0f + x2 * (62370.0f + x2 * (3150.0f + x2 * 28.0f));
  return a / b;
}

#if defined(KINETISK)

void AudioFilterMoog_F32::update_fixed(const float *in,float *lp)
{
	for (int i = 0; i < 2*AUDIO_BLOCK_SAMPLES; i++) {
		float cs = tanh_fast(in[i/2] * driv);
		y_a = y_a + g * (tanh_fast(cs - q * ((y_d_1 + y_d)/2) - tanh_fast(y_a)));
		y_b = y_b + g * (tanh_fast(y_a) - tanh_fast(y_b));
		y_c = y_c + g * (tanh_fast(y_b) - tanh_fast(y_c));
		y_d_1 = y_d;
		y_d = y_d + g * (tanh_fast(y_c) - tanh_fast(y_d));
		lp[i/2]  = y_d;
	}
}


void AudioFilterMoog_F32::update_variable(const float *in,const float *ctl, float *lp)
{
	int over=2;
		float nf=basef*(exp2f(ctl[0/over]*oct));
		frequency(nf,false);
	for (int i = 0; i < 2*AUDIO_BLOCK_SAMPLES; i++) {
		float cs = tanh_fast(in[i/over] * driv);
		y_a = y_a + g * (tanh_fast(cs - q * ((y_d_1 + y_d)/2) - tanh_fast(y_a)));
		y_b = y_b + g * (tanh_fast(y_a) - tanh_fast(y_b));
		y_c = y_c + g * (tanh_fast(y_b) - tanh_fast(y_c));
		y_d_1 = y_d;
		y_d = y_d + g * (tanh_fast(y_c) - tanh_fast(y_d));
		lp[i/over]  = y_d;
	}
}


void AudioFilterMoog_F32::update(void)
{
	audio_block_f32_t *input_block=NULL, *control_block=NULL;
	audio_block_f32_t *lowpass_block=NULL;

	input_block = receiveReadOnly_f32(0);
	control_block = receiveReadOnly_f32(1);
	if (!input_block) {
		if (control_block) release(control_block);
		return;
	}
	lowpass_block = allocate_f32();
	if (!lowpass_block) {
		release(input_block);
		if (control_block) release(control_block);
		return;
	}

	if (control_block) {
		update_variable(input_block->data, control_block->data, lowpass_block->data);
		release(control_block);
	} else {
		update_fixed(input_block->data, lowpass_block->data);
	}
	AudioStream_F32::release(input_block);
	AudioStream_F32::transmit(lowpass_block, 0);
	AudioStream_F32::release(lowpass_block);
	return;
}

#elif defined(KINETISL)

void AudioFilterMoog_F32::update(void)
{
	audio_block_f32_t *block;

	block = receiveReadOnly_f32(0);
	if (block) AudioStream_F32::release(block);
	block = receiveReadOnly_f32(1);
	if (block) AudioStream_F32::release(block);
}

#endif

