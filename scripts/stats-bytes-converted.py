from generator import Generator
from table import Table

if __name__ == '__main__':
    gen = Generator()

    freq = {}
    for bi in gen.run():
        k = bi.total_skip

        freq[k] = freq.get(k, 0) + 1

    table = Table()
    table.add_header(["bytes processed", "patterns", "%", "cumulative %"])

    cumulative = 0
    for total_skip in sorted(freq.keys()):
        count = freq[total_skip]
        cumulative += count

        table.add_row([
            '%d' % total_skip,
            '%d' % count,
            '%0.2f%%' % (100 * count/65536.0),
            '%0.2f%%' % (100 * cumulative/65536.0),
        ])

    print(table)
