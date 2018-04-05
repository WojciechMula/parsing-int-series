from generator import Generator
from table import Table

if __name__ == '__main__':
    gen = Generator()

    conversion = {}

    for bi in gen.run():
        size = bi.element_size
        k    = len(bi.spans)

        if size not in conversion:
            conversion[size] = []

        conversion[size].append(k)

    table = Table()
    table.add_header(["element size", "occurances", "%", "avg", "max", "utilization"])

    def get_capacity(element_size):
        if element_size == 1:
            return 16
        elif element_size == 2:
            return 8
        elif element_size == 4:
            return 4
        elif element_size == 8:
            return 2
        else:
            return 1


    for element_size in sorted(conversion.keys()):
        list = conversion[element_size]
        n = len(list)
        avg = sum(list)/float(n)
        utilization = 100 * avg/get_capacity(element_size)

        table.add_row([
            '%d' % element_size,
            '%d' % n,
            '%0.1f%%' % (100 * n/65536.0),
            '%0.2f' % avg,
            '%d' % max(list),
            '%0.1f%%' % utilization,
        ])

    print table
