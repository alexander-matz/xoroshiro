# Xoroshiro

Xoroshiro256+ implementation from:
  http://xoshiro.di.unimi.it/xoshiro256plus.c
Splitmix64 implementation from:
  https://github.com/svaarala/duktape/blob/master/misc/splitmix64.c

Modified interface to avoid name clashes, ease intiailization and provide
an interface for generation of doubles.

In *one* of your C files, define `XOROSHIRO_IMPLEMENTATION` before including
this header.

```
// It should look like this:
...
#define XOROSHIRO_IMPLEMENTATION
#include "xoroshiro.h"

In all other C/C++ files, include normally
```

# Interface

```
// Splitmix64
void sm64_seed(uint64_t s);
uint64_t sm64_next();
double sm64_next_double();

// Xoroshiro256+
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
```

# License

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.
See <http://creativecommons.org/publicdomain/zero/1.0/>.
