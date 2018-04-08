from distribution import *

def testcases():
    sign_distribution = [1,1,1]

    for size, loops in sizes:
        for separator_distribution in separator_distributions:
            for k in range(1, 8 + 1):
                numbers_distribution = uniform_distribution(k)
                yield size, loops, 'uniform', numbers_distribution, separator_distribution, sign_distribution

            for k in range(1, 8 + 1):
                numbers_distribution = normal_distribution(k, 1.0)
                yield size, loops, 'normal', numbers_distribution, separator_distribution, sign_distribution


sizes = [
    (4096,  10000),
    (65536, 1000),
]

separator_distributions = [
    [1],            # single character
    [1,1,1,1,1,1]   # from 1 to 6 separators
]

