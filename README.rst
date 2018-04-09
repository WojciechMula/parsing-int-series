================================================================================
                Parsing series of integers with SIMD
================================================================================

Sample programs for article (to be published). Parsers extract integer numbers
from strings. A number can be prepended by a sign character. The numbers are
separated by arbitrary sequences of separator chars. All other characters
are invalid and the parsers detects them and raise exception.

This repository contains:

* scalar reference implementation;
* two variants of SSE parsers; there are also separate variants designed
  solely of parsing unsigned numbers;
* scalar hybrid that combines ideas from SIMD parsing with scalar
  conversion procedures.


Requires: C++11 compiler (tested with GCC 7.3) and Python 2.7.

Usage
--------------------------------------------------------------------------------

Type ``make`` to build all programs.

Type ``make run-tests`` to build all unit tests and then run them.
Some tests are time consuming, be patient.

Type ``make microbenchmarks.rst`` to run benchmarks.

Type ``make report-overall.rst`` to run performance benchmarks.


Programs
--------------------------------------------------------------------------------

There are several programs available in ``bin`` subdirectory.

* ``benchmark`` --- test performance of given procedure
* ``benchmark-cpuclocks`` --- measure performance of all procedures; display
  CPU clocks
* ``benchmark-all`` --- compare performance of different procedures
* ``compare-singed`` and ``comapre-unsigned`` --- are used to
  validate if parsers produces the same results as the reference
  parser
* ``compare-avx512`` --- the same as above, but tests only
  AVX512BW implementation
* ``statistics`` --- gather execution statistics from SSE parsers

Apart from these programs, there are several ``verify_*`` executables
that runs various unit tests; they are invoked by ``make run-tests``.

Common arguments
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All the programs generates random data which is then parsed,
following command line arguments can be used to control data
characteristics:

    --size=NUMBER         input size (in bytes)
    --loops=NUMBER        how many times a test must be repeated [default: 1]
    --seed=NUMBER         seed for random number generator [default: 0]
    --num=DISTRIBUTION    distribution of lengths of numbers
    --sep=DISTRIBUTION    distribution of lengths of gaps between numbers [default: '1']
    --separators=string   list of separator characters [default: ",; "]
    --sign=DISTRIBUTION   distribution of sign in front of number [default: '1']

``DISTRIBUTION`` is a list of weights separated with commas, which defines
distribution of items.

In case of ``--num`` it's the count of decimal digits in a random number.
For instance ``--num=1,1,1,1`` will produce one-, two-, three- or four-digit
numbers with the same probability; ``--num=0,0,0,1,5,1`` will produce four-,
five- or six-digit numbers, but five-digits numbers with probability 5/7.

In case of ``--sep`` it is the distribution of numbers of separator characters
between the generated numbers. The default ``--sep=1`` means there's always
exactly one character; ``--sep=0,0,1,1,1`` would put form 2 to 4 separator chars.

The ``--sign`` defines distribution of set: no-character-sign, '+' and '-'.
Thus the default ``--sign=1`` forces just unsigned numbers; ``--sign=0,0,1``
will force all numbers negative.


TODO
--------------------------------------------------------------------------------

Important:

* Make scalar hybrid parse signed numbers.
* Complete AVX512 implementation to handle scalar fallback.

Minor:

* Update inline help in ``benchmark``. It'd require redesign of the base class.

License
--------------------------------------------------------------------------------

BSD
