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
        for i in xrange(8):
            if x & (1 << i):
                s += DIGIT
            else:
                s += SPACE

        return s


def generate_case(number, ranges):
    result = []

    result.append("// %s" % ranges)

    if number == 0:
        return result

    def conv_fun(span, arg = None):
        result = "scalar::convert<%d>(data" % span.digits()
        if span.first != 0:
            result += ' + %d' % span.first;

        if arg is not None:
            result += ', %s' % arg

        result += ')'
        return result

    if number == 0xff:
        span = ranges[0]
        result.append("if (has_last) {")
        result.append("   val = %s;" % conv_fun(span, "val"))
        result.append("} else {")
        result.append("   val = %s;" % conv_fun(span))
        result.append("}")
        result.append("has_last = true;");
        return result

    def generate_continuation():
        result.append("if (has_last) {")
        result.append("   has_last = false;")
        result.append("    *output++ = val;")
        result.append("}")

    def generate_first(span):
        result.append("if (has_last) {")
        result.append("   val = %s;" % conv_fun(span, "val"))
        result.append("   has_last = false;")
        result.append("} else {")
        result.append("   val = %s;" % conv_fun(span))
        result.append("}")
        result.append("*output++ = val;")

    def generate_last(span):
        result.append("val = %s;" % conv_fun(span))
        result.append("has_last = true;")

    def generate_middle(span):
        result.append("*output++ = %s;" % conv_fun(span))

    if len(ranges) == 1:
        span = ranges[0]
        if span.first == 0:
            generate_first(span)
            return result

        generate_continuation();
        if span.last == 7:
            generate_last(span)
        else:
            generate_middle(span)

        return result

    first  = ranges[0]
    middle = ranges[1:-1]
    last   = ranges[-1]

    if first.first == 0:
        generate_first(first)
    else:
        generate_continuation();
        generate_middle(first)

    for span in middle:
        generate_middle(span)

    if last.last == 7:
        generate_last(last)
    else:
        generate_middle(last)

    return result


def generate_case_cpp(number, ranges):
    result = []

    result.append("case 0x%02x:" % number)
    indent = " " * 4
    for line in generate_case(number, ranges):
        result.append(indent + line)

    result.append(indent + "break;")

    return '\n'.join(result)


def main():
    for i in xrange(2**8):
        parser = Parser(i)
        print generate_case_cpp(i, parser.get_ranges())


if __name__ == '__main__':
    main()

