/******************************************************************************
 * Xoroshiro256+ implementation from:
 *   http://xoshiro.di.unimi.it/xoshiro256plus.c
 * Splitmix64 implementation from:
 *   https://github.com/svaarala/duktape/blob/master/misc/splitmix64.c
 * 
 * Modified interface to avoid name clashes, ease intiailization and provide
 * an interface for generation of doubles.
 *
 * In *one* of your C files, define XOROSHIRO_IMPLEMENTATION before including
 * this header.
 *
 * // It should look like this:
 * ...
 * #define XOROSHIRO_IMPLEMENTATION
 * #include "xoroshiro.h"
 *
 * In all other C/C++ files, include normally
 *
 *
 * To the extent possible under law, the author has dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 * See <http://creativecommons.org/publicdomain/zero/1.0/>.
 *
 *****************************************************************************/

#ifndef __XOROSHIRO_H_
#define __XOROSHIRO_H_

#include <stdint.h>

/******************************************************************************
 * Written in 2015 by Sebastiano Vigna (vigna@acm.org)
 *
 * To the extent possible under law, the author has dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 * See <http://creativecommons.org/publicdomain/zero/1.0/>.
 *
 * This is a fixed-increment version of Java 8's SplittableRandom generator
 * See http://dx.doi.org/10.1145/2714064.2660195 and
 * http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html
 * It is a very fast generator passing BigCrush, and it can be useful if
 * for some reason you absolutely want 64 bits of state; otherwise, we
 * rather suggest to use a xoroshiro128+ (for moderately parallel
 * computations) or xorshift1024* (for massively parallel computations)
 * generator.
 *****************************************************************************/

extern uint64_t sm64_s; /* The state can be seeded with any value. */

void sm64_seed(uint64_t s);
uint64_t sm64_next();
double sm64_next_double();

#ifdef XOROSHIRO_IMPLEMENTATION

uint64_t sm64_s; /* The state can be seeded with any value. */

void sm64_seed(uint64_t s) {
  sm64_s = s;
}

uint64_t sm64_next() {
	uint64_t z = (sm64_s += UINT64_C(0x9E3779B97F4A7C15));
	z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
	z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
	return z ^ (z >> 31);
}

double sm64_next_double() {
  struct {
    uint64_t i64;
    double f64;
  } buf;
  buf.i64 = 0x3FF0000000000000ULL | (sm64_next() >> 12);
  return buf.f64 - 1.0;
}

#endif

/******************************************************************************
 * Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)
 * 
 * To the extent possible under law, the author has dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 * 
 * See <http://creativecommons.org/publicdomain/zero/1.0/>.
 * This is xoshiro256+ 1.0, our best and fastest generator for floating-point
 * numbers. We suggest to use its upper bits for floating-point
 * generation, as it is slightly faster than xoshiro256**. It passes all
 * tests we are aware of except for the lowest three bits, which might
 * fail linearity tests (and just those), so if low linear complexity is
 * not considered an issue (as it is usually the case) it can be used to
 * generate 64-bit outputs, too.
 * 
 * We suggest to use a sign test to extract a random Boolean value, and
 * right shifts to extract subsets of bits.
 * 
 * The state must be seeded so that it is not everywhere zero. If you have
 * a 64-bit seed, we suggest to seed a splitmix64 generator and use its
 * output to fill s.
 *****************************************************************************/

extern uint64_t xs256_s[4];

void xs256_seed(uint64_t s1, uint64_t s2, uint64_t s3, uint64_t s4);
uint64_t xs256_next();
double xs256_next_double();

/* This is the jump function for the generator. It is equivalent
   to 2^128 calls to next(); it can be used to generate 2^128
   non-overlapping subsequences for parallel computations. */
void xs256_jump();

/* This is the long-jump function for the generator. It is equivalent to
   2^192 calls to next(); it can be used to generate 2^64 starting points,
   from each of which jump() will generate 2^64 non-overlapping
   subsequences for parallel distributed computations. */
void xs256_long_jump();

#ifdef XOROSHIRO_IMPLEMENTATION

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

uint64_t xs256_s[4];

void xs256_seed(uint64_t s1, uint64_t s2, uint64_t s3, uint64_t s4) {
  xs256_s[0] = s1;
  xs256_s[1] = s2;
  xs256_s[2] = s3;
  xs256_s[3] = s4;
}

uint64_t xs256_next() {
	const uint64_t result_plus = xs256_s[0] + xs256_s[3];

	const uint64_t t = xs256_s[1] << 17;

	xs256_s[2] ^= xs256_s[0];
	xs256_s[3] ^= xs256_s[1];
	xs256_s[1] ^= xs256_s[2];
	xs256_s[0] ^= xs256_s[3];

	xs256_s[2] ^= t;

	xs256_s[3] = rotl(xs256_s[3], 45);

	return result_plus;
}

double xs256_next_double() {
  struct {
    uint64_t i64;
    double f64;
  } buf;
  buf.i64 = 0x3FF0000000000000ULL | (xs256_next() >> 12);
  return buf.f64 - 1.0;
}

void xs256_jump() {
	static const uint64_t JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	uint64_t s2 = 0;
	uint64_t s3 = 0;
	for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
		for(int b = 0; b < 64; b++) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= xs256_s[0];
				s1 ^= xs256_s[1];
				s2 ^= xs256_s[2];
				s3 ^= xs256_s[3];
			}
			xs256_next();	
		}
		
	xs256_s[0] = s0;
	xs256_s[1] = s1;
	xs256_s[2] = s2;
	xs256_s[3] = s3;
}

void xs256_long_jump() {
	static const uint64_t LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	uint64_t s2 = 0;
	uint64_t s3 = 0;
	for(int i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++)
		for(int b = 0; b < 64; b++) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= xs256_s[0];
				s1 ^= xs256_s[1];
				s2 ^= xs256_s[2];
				s3 ^= xs256_s[3];
			}
			xs256_next();	
		}
		
	xs256_s[0] = s0;
	xs256_s[1] = s1;
	xs256_s[2] = s2;
	xs256_s[3] = s3;
}

#endif

#endif
