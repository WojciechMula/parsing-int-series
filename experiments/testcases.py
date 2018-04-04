import math

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


def single_digit_distribution(digit, count = 8):
    tmp = [0] * count
    tmp[digit - 1] = 1

    return tmp


def uniform_distribution(count):
    return [1] * count


def normal_distribution(mu, sigma, count = 8):
    tmp = [0] * count

    def gauss(x):
        c = 1.0/(sigma * math.sqrt(2 * math.pi))
        return c * math.exp(-(x - mu)**2/(2 * sigma**2))

    for x in xrange(0, count):
        tmp[x] = gauss(float(x + 1))

    m = max(tmp)

    for x in xrange(0, count):
        tmp[x] = int(1000 * tmp[x]/m)

    return tmp


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

