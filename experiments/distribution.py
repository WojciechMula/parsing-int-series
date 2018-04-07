import math


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

