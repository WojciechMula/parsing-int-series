from hybrid import Parser

def get_shifts():
    for number in range(2**8):
        parser = Parser(number)
        ranges = parser.get_ranges()
        if len(ranges) == 0:
            yield 0
            continue
        
        last = ranges[-1]
        if last.last == 7:
            yield last.digits()
        else:
            yield 0


def main():
    shifts = list(get_shifts())
    shifts = map(str, shifts)
    print("static int shift[256] = {%s};" % (','.join(shifts)))


if __name__ == '__main__':
    main()
