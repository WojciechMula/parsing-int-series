class DigitsSpan(object):
    # range: [first, last] - include the both ends
    def __init__(self, first, last):
        assert first <= last

        self.first = first
        self.last  = last

        assert self.digits() <= 8

    def digits(self):
        return self.last - self.first + 1

    def __str__(self):
        return "<%d, %d>" % (self.first, self.last)

    __repr__ = __str__


DIGIT = 'd'
SPACE = '_'

class Parser(object):
    def __init__(self, number):
        assert number >= 0
        assert number < 256

        self.number = number
        self.image = self.__convert_to_string(number)


    def get_ranges(self):
        prev  = SPACE
        start = None
        ranges = []
        for i, c in enumerate(self.image):
            if c == prev:
                continue

            if c == DIGIT: # transition
                start = i
            else:
                ranges.append(DigitsSpan(start, i - 1))
                start = None

            prev = c

        if start is not None:
            ranges.append(DigitsSpan(start, 7))

        return ranges


    def __convert_to_string(self, x):
        s = ''
        for i in range(8):
            if x & (1 << i):
                s += DIGIT
            else:
                s += SPACE

        return s


EMPTY                   = 1
FULL                    = 2
FINALIZE_PREVIOUS       = 3
FIRST_CONTINUATION      = 4
WHOLE                   = 5
LAST                    = 6

def tokenize(number):
    parser = Parser(number)
    ranges = parser.get_ranges()
    if number == 0x00:
        assert len(ranges) == 0
        yield (EMPTY, [])

    elif number == 0xff:
        assert len(ranges) == 1
        yield (FULL, ranges[0])

    else:
        if ranges[0].first > 0:
            yield (FINALIZE_PREVIOUS, None)
        for r in ranges:
            if r.first == 0:
                yield (FIRST_CONTINUATION, r)
            elif r.last == 7:
                yield (LAST, r)
            else:
                yield (WHOLE, r)


class GeneratorBase(object):
    def __init__(self):
        self.span   = None
        self.number = None

    def get(self):

        self.begin()

        self.lines = []
        for number in range(2**8):
            self.number = number
            self.before()
            for (kind, span) in tokenize(number):
                self.span = span

                if kind == EMPTY:
                    self.empty()
                elif kind == FULL:
                    self.full()
                elif kind == FINALIZE_PREVIOUS:
                    self.finalize_previous()
                elif kind == FIRST_CONTINUATION:
                    self.first_continuation()
                elif kind == WHOLE:
                    self.whole()
                elif kind == LAST:
                    self.last()
                else:
                    assert False

            #for
            self.after()
        #for

        self.end()

        return self.lines


    def begin(self):
        pass


    def end(self):
        pass


    def before(self):
        pass


    def after(self):
        pass

