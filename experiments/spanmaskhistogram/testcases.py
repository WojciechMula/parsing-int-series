from distribution import *

def testcases():
    sign_distribution = [1,1,1]

    for size in sizes:
        for separator_distribution in separator_distributions:
            for k in range(1, 8 + 1):
                numbers_distribution = single_digit_distribution(k)
                yield size, 'single', numbers_distribution, separator_distribution, sign_distribution

            for k in range(1, 8 + 1):
                numbers_distribution = uniform_distribution(k)
                yield size, 'uniform', numbers_distribution, separator_distribution, sign_distribution

            for k in range(1, 8 + 1):
                numbers_distribution = normal_distribution(k, 1.0)
                yield size, 'normal', numbers_distribution, separator_distribution, sign_distribution


sizes = [
    1024,
    4096,
    102400,
    1024000,
    10240000,
]

separator_distributions = [
    [1],            # single character
    [1,1,1,1,1,1]   # from 1 to 6 separators
]

