Scripts purpose
--------------------------------------------------------------------------------

``microbenchamarks/*.py`` --- run all implementations (scalar and vectorized)
on rather small inputs and count how many **CPU cycles** are needed to complete
conversion. Subdirectory ``results`` contains produced files from some computers.

It uses ``bin/benchmark-cpuclocks`` utility; makefile targets
``microbenchmarks.txt`` and ``microbenchmarks.rst``.

--------------------------------------------------------------------------------

``speedup-comparison/report.py`` --- from ``microbenchmarks.txt`` produces a
summary speedup array (min/avg/max) for all methods and input
size/distribution. Usage::

    $ python speedup-comparison/report.py microbenchmarks/results/file.txt > file.rst

The result of this script is shown in article__.

__ http://0x80.pl/articles/simd-parsing-int-sequences.html#core-i7-results

--------------------------------------------------------------------------------

``overalltests/*.py`` --- run scalar and SSE implementations on small and huge
input, measure **wall clock** of algorithms.  Subdirectory ``results`` contains
produced files from some computers.

It uses ``bin/benchmark`` utility; makefile targets ``overall.txt``,
``report-overall.rst`` (all numbers are shown), ``report-overall-short.rst``
(just min/mav/max speedup is shown).

--------------------------------------------------------------------------------

``hwevents/*.py`` --- runs SSE searches  and counts hardware events: branch
taken & misses and cache references & misses.

It uses ``bin/benchmark-hwevents`` utility; makefile target: ``hwevents.txt``.

--------------------------------------------------------------------------------

``spanmaskhistogram/*.py`` --- for different input sizes and input data
distrubutions it gets following parameters of **SSE procedure**:

* ``span_pattern`` statistics usage  (runs ``bin/statistics``);
* running time in CPU clocks (from ``measurements.txt``);
* brach and cache events count (from ``hwevents.txt``).

All these data is collated in single a table; makefile target
``spanmaskhistogram.rst``.

The result of this script is shown in article__.

__ http://0x80.pl/articles/simd-parsing-int-sequences.html#sse-conversion-runtime-analysis

