/*
 *
 *  Bluetooth low-complexity, subband codec (SBC) library
 *
 *  Copyright (C) 2004-2009  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2004-2005  Henryk Ploetz <henryk@ploetzli.ch>
 *  Copyright (C) 2005-2006  Brad Midgley <bmidgley@xmission.com>
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdint.h>
#include <limits.h>
#include "sbc.h"
#include "sbc_math.h"
#include "sbc_tables.h"

#include "sbc_primitives_mmx.h"

/*
 * MMX optimizations
 */

#ifdef SBC_BUILD_WITH_MMX_SUPPORT

static inline void sbc_analyze_four_mmx(const int16_t *in, int32_t *out,
					const FIXED_T *consts)
{
	static const SBC_ALIGNED int32_t round_c[2] = {
		1 << (SBC_PROTO_FIXED4_SCALE - 1),
		1 << (SBC_PROTO_FIXED4_SCALE - 1),
	};
	asm volatile (
		"movq        (%0), %%mm0\n"
		"movq       8(%0), %%mm1\n"
		"pmaddwd     (%1), %%mm0\n"
		"pmaddwd    8(%1), %%mm1\n"
		"paddd       (%2), %%mm0\n"
		"paddd       (%2), %%mm1\n"
		"\n"
		"movq      16(%0), %%mm2\n"
		"movq      24(%0), %%mm3\n"
		"pmaddwd   16(%1), %%mm2\n"
		"pmaddwd   24(%1), %%mm3\n"
		"paddd      %%mm2, %%mm0\n"
		"paddd      %%mm3, %%mm1\n"
		"\n"
		"movq      32(%0), %%mm2\n"
		"movq      40(%0), %%mm3\n"
		"pmaddwd   32(%1), %%mm2\n"
		"pmaddwd   40(%1), %%mm3\n"
		"paddd      %%mm2, %%mm0\n"
		"paddd      %%mm3, %%mm1\n"
		"\n"
		"movq      48(%0), %%mm2\n"
		"movq      56(%0), %%mm3\n"
		"pmaddwd   48(%1), %%mm2\n"
		"pmaddwd   56(%1), %%mm3\n"
		"paddd      %%mm2, %%mm0\n"
		"paddd      %%mm3, %%mm1\n"
		"\n"
		"movq      64(%0), %%mm2\n"
		"movq      72(%0), %%mm3\n"
		"pmaddwd   64(%1), %%mm2\n"
		"pmaddwd   72(%1), %%mm3\n"
		"paddd      %%mm2, %%mm0\n"
		"paddd      %%mm3, %%mm1\n"
		"\n"
		"psrad         %4, %%mm0\n"
		"psrad         %4, %%mm1\n"
		"packssdw   %%mm0, %%mm0\n"
		"packssdw   %%mm1, %%mm1\n"
		"\n"
		"movq       %%mm0, %%mm2\n"
		"pmaddwd   80(%1), %%mm0\n"
		"pmaddwd   88(%1), %%mm2\n"
		"\n"
		"movq       %%mm1, %%mm3\n"
		"pmaddwd   96(%1), %%mm1\n"
		"pmaddwd  104(%1), %%mm3\n"
		"paddd      %%mm1, %%mm0\n"
		"paddd      %%mm3, %%mm2\n"
		"\n"
		"movq       %%mm0, (%3)\n"
		"movq       %%mm2, 8(%3)\n"
		:
		: "r" (in), "r" (consts), "r" (&round_c), "r" (out),
			"i" (SBC_PROTO_FIXED4_SCALE)
		: "memory");
}

static inline void sbc_analyze_eight_mmx(const int16_t *in, int32_t *out,
							const FIXED_T *consts)
{
	static const SBC_ALIGNED int32_t round_c[2] = {
		1 << (SBC_PROTO_FIXED8_SCALE - 1),
		1 << (SBC_PROTO_FIXED8_SCALE - 1),
	};
	asm volatile (
		"movq        (%0), %%mm0\n"
		"movq       8(%0), %%mm1\n"
		"movq      16(%0), %%mm2\n"
		"movq      24(%0), %%mm3\n"
		"pmaddwd     (%1), %%mm0\n"
		"pmaddwd    8(%1), %%mm1\n"
		"pmaddwd   16(%1), %%mm2\n"
		"pmaddwd   24(%1), %%mm3\n"
		"paddd       (%2), %%mm0\n"
		"paddd       (%2), %%mm1\n"
		"paddd       (%2), %%mm2\n"
		"paddd       (%2), %%mm3\n"
		"\n"
		"movq      32(%0), %%mm4\n"
		"movq      40(%0), %%mm5\n"
		"movq      48(%0), %%mm6\n"
		"movq      56(%0), %%mm7\n"
		"pmaddwd   32(%1), %%mm4\n"
		"pmaddwd   40(%1), %%mm5\n"
		"pmaddwd   48(%1), %%mm6\n"
		"pmaddwd   56(%1), %%mm7\n"
		"paddd      %%mm4, %%mm0\n"
		"paddd      %%mm5, %%mm1\n"
		"paddd      %%mm6, %%mm2\n"
		"paddd      %%mm7, %%mm3\n"
		"\n"
		"movq      64(%0), %%mm4\n"
		"movq      72(%0), %%mm5\n"
		"movq      80(%0), %%mm6\n"
		"movq      88(%0), %%mm7\n"
		"pmaddwd   64(%1), %%mm4\n"
		"pmaddwd   72(%1), %%mm5\n"
		"pmaddwd   80(%1), %%mm6\n"
		"pmaddwd   88(%1), %%mm7\n"
		"paddd      %%mm4, %%mm0\n"
		"paddd      %%mm5, %%mm1\n"
		"paddd      %%mm6, %%mm2\n"
		"paddd      %%mm7, %%mm3\n"
		"\n"
		"movq      96(%0), %%mm4\n"
		"movq     104(%0), %%mm5\n"
		"movq     112(%0), %%mm6\n"
		"movq     120(%0), %%mm7\n"
		"pmaddwd   96(%1), %%mm4\n"
		"pmaddwd  104(%1), %%mm5\n"
		"pmaddwd  112(%1), %%mm6\n"
		"pmaddwd  120(%1), %%mm7\n"
		"paddd      %%mm4, %%mm0\n"
		"paddd      %%mm5, %%mm1\n"
		"paddd      %%mm6, %%mm2\n"
		"paddd      %%mm7, %%mm3\n"
		"\n"
		"movq     128(%0), %%mm4\n"
		"movq     136(%0), %%mm5\n"
		"movq     144(%0), %%mm6\n"
		"movq     152(%0), %%mm7\n"
		"pmaddwd  128(%1), %%mm4\n"
		"pmaddwd  136(%1), %%mm5\n"
		"pmaddwd  144(%1), %%mm6\n"
		"pmaddwd  152(%1), %%mm7\n"
		"paddd      %%mm4, %%mm0\n"
		"paddd      %%mm5, %%mm1\n"
		"paddd      %%mm6, %%mm2\n"
		"paddd      %%mm7, %%mm3\n"
		"\n"
		"psrad         %4, %%mm0\n"
		"psrad         %4, %%mm1\n"
		"psrad         %4, %%mm2\n"
		"psrad         %4, %%mm3\n"
		"\n"
		"packssdw   %%mm0, %%mm0\n"
		"packssdw   %%mm1, %%mm1\n"
		"packssdw   %%mm2, %%mm2\n"
		"packssdw   %%mm3, %%mm3\n"
		"\n"
		"movq       %%mm0, %%mm4\n"
		"movq       %%mm0, %%mm5\n"
		"pmaddwd  160(%1), %%mm4\n"
		"pmaddwd  168(%1), %%mm5\n"
		"\n"
		"movq       %%mm1, %%mm6\n"
		"movq       %%mm1, %%mm7\n"
		"pmaddwd  192(%1), %%mm6\n"
		"pmaddwd  200(%1), %%mm7\n"
		"paddd      %%mm6, %%mm4\n"
		"paddd      %%mm7, %%mm5\n"
		"\n"
		"movq       %%mm2, %%mm6\n"
		"movq       %%mm2, %%mm7\n"
		"pmaddwd  224(%1), %%mm6\n"
		"pmaddwd  232(%1), %%mm7\n"
		"paddd      %%mm6, %%mm4\n"
		"paddd      %%mm7, %%mm5\n"
		"\n"
		"movq       %%mm3, %%mm6\n"
		"movq       %%mm3, %%mm7\n"
		"pmaddwd  256(%1), %%mm6\n"
		"pmaddwd  264(%1), %%mm7\n"
		"paddd      %%mm6, %%mm4\n"
		"paddd      %%mm7, %%mm5\n"
		"\n"
		"movq       %%mm4, (%3)\n"
		"movq       %%mm5, 8(%3)\n"
		"\n"
		"movq       %%mm0, %%mm5\n"
		"pmaddwd  176(%1), %%mm0\n"
		"pmaddwd  184(%1), %%mm5\n"
		"\n"
		"movq       %%mm1, %%mm7\n"
		"pmaddwd  208(%1), %%mm1\n"
		"pmaddwd  216(%1), %%mm7\n"
		"paddd      %%mm1, %%mm0\n"
		"paddd      %%mm7, %%mm5\n"
		"\n"
		"movq       %%mm2, %%mm7\n"
		"pmaddwd  240(%1), %%mm2\n"
		"pmaddwd  248(%1), %%mm7\n"
		"paddd      %%mm2, %%mm0\n"
		"paddd      %%mm7, %%mm5\n"
		"\n"
		"movq       %%mm3, %%mm7\n"
		"pmaddwd  272(%1), %%mm3\n"
		"pmaddwd  280(%1), %%mm7\n"
		"paddd      %%mm3, %%mm0\n"
		"paddd      %%mm7, %%mm5\n"
		"\n"
		"movq       %%mm0, 16(%3)\n"
		"movq       %%mm5, 24(%3)\n"
		:
		: "r" (in), "r" (consts), "r" (&round_c), "r" (out),
			"i" (SBC_PROTO_FIXED8_SCALE)
		: "memory");
}

static inline void sbc_analyze_4b_4s_mmx(int16_t *pcm, int16_t *x,
						int32_t *out, int out_stride)
{
	/* Fetch audio samples and do input data reordering for SIMD */
	x[64] = x[0]  = pcm[8 + 7];
	x[65] = x[1]  = pcm[8 + 3];
	x[66] = x[2]  = pcm[8 + 6];
	x[67] = x[3]  = pcm[8 + 4];
	x[68] = x[4]  = pcm[8 + 0];
	x[69] = x[5]  = pcm[8 + 2];
	x[70] = x[6]  = pcm[8 + 1];
	x[71] = x[7]  = pcm[8 + 5];

	x[72] = x[8]  = pcm[0 + 7];
	x[73] = x[9]  = pcm[0 + 3];
	x[74] = x[10] = pcm[0 + 6];
	x[75] = x[11] = pcm[0 + 4];
	x[76] = x[12] = pcm[0 + 0];
	x[77] = x[13] = pcm[0 + 2];
	x[78] = x[14] = pcm[0 + 1];
	x[79] = x[15] = pcm[0 + 5];

	/* Analyze blocks */
	sbc_analyze_four_mmx(x + 12, out, analysis_consts_fixed4_simd_odd);
	out += out_stride;
	sbc_analyze_four_mmx(x + 8, out, analysis_consts_fixed4_simd_even);
	out += out_stride;
	sbc_analyze_four_mmx(x + 4, out, analysis_consts_fixed4_simd_odd);
	out += out_stride;
	sbc_analyze_four_mmx(x + 0, out, analysis_consts_fixed4_simd_even);

	asm volatile ("emms\n");
}

static inline void sbc_analyze_4b_8s_mmx(int16_t *pcm, int16_t *x,
						int32_t *out, int out_stride)
{
	/* Fetch audio samples and do input data reordering for SIMD */
	x[128] = x[0]  = pcm[16 + 15];
	x[129] = x[1]  = pcm[16 + 7];
	x[130] = x[2]  = pcm[16 + 14];
	x[131] = x[3]  = pcm[16 + 8];
	x[132] = x[4]  = pcm[16 + 13];
	x[133] = x[5]  = pcm[16 + 9];
	x[134] = x[6]  = pcm[16 + 12];
	x[135] = x[7]  = pcm[16 + 10];
	x[136] = x[8]  = pcm[16 + 11];
	x[137] = x[9]  = pcm[16 + 3];
	x[138] = x[10] = pcm[16 + 6];
	x[139] = x[11] = pcm[16 + 0];
	x[140] = x[12] = pcm[16 + 5];
	x[141] = x[13] = pcm[16 + 1];
	x[142] = x[14] = pcm[16 + 4];
	x[143] = x[15] = pcm[16 + 2];

	x[144] = x[16] = pcm[0 + 15];
	x[145] = x[17] = pcm[0 + 7];
	x[146] = x[18] = pcm[0 + 14];
	x[147] = x[19] = pcm[0 + 8];
	x[148] = x[20] = pcm[0 + 13];
	x[149] = x[21] = pcm[0 + 9];
	x[150] = x[22] = pcm[0 + 12];
	x[151] = x[23] = pcm[0 + 10];
	x[152] = x[24] = pcm[0 + 11];
	x[153] = x[25] = pcm[0 + 3];
	x[154] = x[26] = pcm[0 + 6];
	x[155] = x[27] = pcm[0 + 0];
	x[156] = x[28] = pcm[0 + 5];
	x[157] = x[29] = pcm[0 + 1];
	x[158] = x[30] = pcm[0 + 4];
	x[159] = x[31] = pcm[0 + 2];

	/* Analyze blocks */
	sbc_analyze_eight_mmx(x + 24, out, analysis_consts_fixed8_simd_odd);
	out += out_stride;
	sbc_analyze_eight_mmx(x + 16, out, analysis_consts_fixed8_simd_even);
	out += out_stride;
	sbc_analyze_eight_mmx(x + 8, out, analysis_consts_fixed8_simd_odd);
	out += out_stride;
	sbc_analyze_eight_mmx(x + 0, out, analysis_consts_fixed8_simd_even);

	asm volatile ("emms\n");
}

static int check_mmx_support()
{
#ifdef __amd64__
	return 1; /* We assume that all 64-bit processors have MMX support */
#else
	int cpuid_feature_information;
	asm volatile (
		/* According to Intel manual, CPUID instruction is supported
		 * if the value of ID bit (bit 21) in EFLAGS can be modified */
		"pushf\n"
		"movl     (%%esp),   %0\n"
		"xorl     $0x200000, (%%esp)\n" /* try to modify ID bit */
		"popf\n"
		"pushf\n"
		"xorl     (%%esp),   %0\n"      /* check if ID bit changed */
		"jz       1f\n"
		"push     %%eax\n"
		"push     %%ebx\n"
		"push     %%ecx\n"
		"mov      $1,        %%eax\n"
		"cpuid\n"
		"pop      %%ecx\n"
		"pop      %%ebx\n"
		"pop      %%eax\n"
		"1:\n"
		"popf\n"
		: "=d" (cpuid_feature_information)
		:
		: "cc");
    return cpuid_feature_information & (1 << 23);
#endif
}

void sbc_init_primitives_mmx(struct sbc_encoder_state *state)
{
	if (check_mmx_support()) {
		state->sbc_analyze_4b_4s = sbc_analyze_4b_4s_mmx;
		state->sbc_analyze_4b_8s = sbc_analyze_4b_8s_mmx;
	}
}

#endif