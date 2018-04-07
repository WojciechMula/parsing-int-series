from distribution import *

def testcases():
    sign_distribution = [1,1,1]

    for procedure in procedures:
        for size, loops in sizes:
            for separator_distribution in separator_distributions:
                for k in range(1, 8 + 1):
                    numbers_distribution = single_digit_distribution(k)
                    yield procedure, size, loops, 'single', numbers_distribution, separator_distribution, sign_distribution

                for k in range(1, 8 + 1):
                    numbers_distribution = uniform_distribution(k)
                    yield procedure, size, loops, 'uniform', numbers_distribution, separator_distribution, sign_distribution

                for k in range(1, 8 + 1):
                    numbers_distribution = normal_distribution(k, 1.0)
                    yield procedure, size, loops, 'normal', numbers_distribution, separator_distribution, sign_distribution


procedures = [
    'scalar',
    'sse',
    'sse-unrolled'
]

sizes = [
    (1024,      10000),
    (4096,      10000),
    (102400,     1000),
    (1024000,     100),
    (10240000,     10),
]

separator_distributions = [
    [1],            # single character
    [1,1,1,1,1,1]   # from 1 to 6 separators
]

