import sys
import os.path

if __name__ == '__main__' and __package__ is None:
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from table import Table
from loader import load_file as load, procedures, reference_procedure
from utils import groupby
from writer import RestWriter
from prettyprint import *


class Report(object):
    def __init__(self, path):
        with open(path, 'rt') as f:
            data = list(load(f))

        bysize = lambda item: item.size
        data = groupby(data, bysize)
        self.report = []
        for size in sorted(data):
            collection = data[size]
            sortby = lambda item: (item.distribution_name, item.num_distribution, item.sep_distribution)
            collection.sort(key=sortby)

            res = []
            for item in collection:
                title = self.get_title(item)
                table = self.prepare_table(item)

                res.append((title, table))

            self.report.append((
                'Input size %d bytes' % size,
                res
            ))

    def get(self):
        return self.report


    def get_title(self, item):
        return '%s --- %s' % (item.get_num_distribution_title(), item.get_sep_distribution_title())


    def prepare_table(self, item):
        table = Table()
        table.add_header(["", ("cycles per one input byte", 2), "speed-up"])
        table.add_header(["procedure", "min", "avg", "(min)"])

        refmin, refavg = item.cycles[reference_procedure]

        for key in procedures:
            try:
                min, avg = item.cycles[key]
            except KeyError:
                continue

            table.add_row([
                key,
                '%5.3f' % min,
                '%5.3f' % avg,
                '%0.2f' % (refmin/min)
            ])

        return table


def main():
    report = Report(sys.argv[1])
    writer = RestWriter(sys.stdout, report.get())
    try:
        restsection = sys.argv[2]
    except IndexError:
        restsection = '-~'

    writer.write(restsection)


if __name__ == '__main__':
    main()
