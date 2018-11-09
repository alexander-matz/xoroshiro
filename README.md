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

# License

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.
See <http://creativecommons.org/publicdomain/zero/1.0/>.
