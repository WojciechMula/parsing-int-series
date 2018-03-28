SPACE = '_'
DIGIT = 'd'

class Cost(object):
    def __init__(self):
        self.compare = 0
        self.multiplication = 0
        self.add_sub = 0
        self.bit_and = 0
        self.pack = 0
        self.movemask = 0
        self.load = 0

        self.store = 0

    def value(self):
        c = 1.00 * self.compare + \
            1.00 * self.multiplication + \
            1.00 * self.add_sub + \
            1.00 * self.bit_and + \
            1.00 * self.pack + \
            1.00 * self.movemask + \
            1.00 * self.load
        
        if self.store > 0:
            return c/float(self.store)
        else:
            return c
            
    def __str__(self):
        return '%0.2f' % self.value()

def scalar_cost(bi):
    cost = Cost()

    prev = SPACE
    for c in bi.image:
        if c == SPACE:
            cost.compare += 1 # char in separators
            if prev == DIGIT:
                # end of digits span
                cost.store += 1
        else:
            # tmp = x - '0'
            cost.add_sub += 1
            # if x > 9 then invalid char
            pass
            # else
            # result = 10 * result + tmp
            cost.multiplication += 1
            cost.add_sub += 1
            
        prev = c

    # unlike SIMD algorithm, the last range is considered
    if prev == DIGIT:
        cost.store += 1

    return cost


def SSE_cost(bi):
    cost = Cost()

    # simd code always validate whole input
    cost.compare  += 3
    cost.bit_and  += 3
    cost.movemask += 2

    if bi.element_size == 1:
        cost.add_sub += 1
        cost.load += len(bi.spans)

    elif bi.element_size == 2:
        cost.add_sub += 1
        cost.multiplication += 1
        cost.load += len(bi.spans)

    elif bi.element_size == 4:
        cost.add_sub += 1
        cost.multiplication += 2
        cost.pack += 1
        cost.load += len(bi.spans)

    elif bi.element_size == 8:
        cost.add_sub += 1
        cost.multiplication += 3
        cost.pack += 1
        cost.load += len(bi.spans)

    else:
        cost.compare = 100000

    cost.store = len(bi.spans)

    return cost


def is_profitable(bi):
    if len(bi.spans) == 0:
        return False

    scalar = scalar_cost(bi)
    sse    = SSE_cost(bi)

    return sse.cost() < scalar.cost()

