import sys
from writer import CPPWriter as Writer

SIMD_ELEMENT_SIZES = [1, 2, 4, 8, 16]

class DigitsSpan(object):
    # range: [first, last] - include the both ends
    def __init__(self, first, last):
        assert first <= last

        self.first = first
        self.last  = last

        assert self.digits() <= 16


    def digits(self):
        return self.last - self.first + 1


    def simd_size(self):
        d = self.digits()
        if d == 1:
            return 1
        if d == 2:
            return 2
        if d <= 4:
            return 4
        if d <= 8:
            return 8
        return 16;

DIGIT = 'd'
SPACE = '_'

class Parser(object):
    def __init__(self, number):
        assert number >= 0
        assert number < 65536

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
                # Note: a digits span which not ends within the chunk
                #       doesn't appear in the result (we don't know how to parse it)
                ranges.append(DigitsSpan(start, i - 1))

            prev = c

        return ranges


    def __convert_to_string(self, x):
        s = ''
        for i in xrange(16):
            if x & (1 << i):
                s += DIGIT
            else:
                s += SPACE

        return s


class Optimizer(object):
    def __init__(self, ranges):
        self.ranges = ranges


    def get_best(self):
        best = None
        best_size = None
        for element_size in SIMD_ELEMENT_SIZES:

            res = self.__pack(element_size)
            if res is None:
                continue

            if best is None or len(res) > len(best):
                best = res;
                best_size = element_size

        if best is not None:
            return (best_size, best)
        else:
            return None


    def __pack(self, element_size, vector_size = 16):
        max_size = vector_size / element_size
        result = []
        for r in self.ranges:
            if r.digits() <= element_size:
                result.append(r)
                if len(result) == max_size:
                    break
            else:
                break

        if len(result) > 0:
            return result
        else:
            return None


class BlockInfo(object):

    __slots__ = ("id", "first_skip", "first_length", "total_skip",
                 "ranges", "element_size", "pshufb_pattern")

    def __init__(self, number):
        self.id             = number
        self.first_skip     = 0
        self.first_length   = 0
        self.total_skip     = 0
        self.ranges         = []
        self.element_size   = 0
        self.pshufb_pattern = []

    def build_pshubf_mask(self):
        self.pshufb_pattern = [0x80] * 16
        for element, r in enumerate(self.ranges):
            index  = element * self.element_size
            index += self.element_size - r.digits() # align to "right" within the vector's element

            for i in xrange(r.first, r.last + 1):
                self.pshufb_pattern[index] = i
                index += 1

    def __str__(self):
        param = (
            self.id,
            self.first_skip,
            self.first_length,
            self.total_skip,
            self.element_size,
            self.ranges
        )

        return "<BlockInfo#%04x {first_skip=%d, first_length=%d, " \
               "total_skip=%d, element_size=%d, ranges=%s}>" % param


class Generator(object):
    def run(self):
        result = []
        for i in xrange(2**16):
            result.append(self.__get_structure(i))

        return result

    def __get_structure(self, number):
        parser = Parser(number)
        ranges = parser.get_ranges()

        opt = Optimizer(ranges)
        ret = opt.get_best()

        block = BlockInfo(number)
        if ret is not None:
            element_size, items = ret

            block.first_skip   = items[0].first
            block.first_length = items[0].digits()
            block.ranges       = items
            block.element_size = element_size

            block.total_skip = items[-1].last + 1
            try:
                image = parser.image
                while image[block.total_skip] == '_':
                    block.total_skip += 1
            except IndexError:
                pass

        block.build_pshubf_mask()

        return block


def main(path):
    gen  = Generator()
    data = gen.run()

    writer = Writer(data)
    writer.save(path)

if __name__ == '__main__':
    if len(sys.argv) < 1:
        print "Usage: script output-path"
        sys.exit(1)

    main(sys.argv[1])
