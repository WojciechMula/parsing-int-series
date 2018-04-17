import sys
import os.path

if __name__ == '__main__' and __package__ is None:
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from table import Table
from utils import splitsorted
from prettyprint import *
from loader import load
from report_writer import RestWriter


class Report(object):

    def __init__(self):
        self.report = None
        self.tmp = []
        self.prev_size = None

    def add(self, item):
        if item.size != self.prev_size:
            self.prev_size = item.size
            self.tmp.append((item.size, []))

        title = '%s, %s' % (
            get_num_distribution_parameters(item.distribution_name, item.numbers_distribution).title,
            get_separator_title(item.separators_distribution))

        self.tmp[-1][1].append((item.distribution_name, title, item.histogram, item.hwevents, item.cycles))


    def get(self):
        if self.report is None:
            self.report = []
            for size, statistics in self.tmp:
                title = 'Input size {:,d} bytes'.format(size)
                self.report.append((title, self.prepare_table(statistics)))

        return self.report


    def prepare_table(self, stats):

        t = Table()
        t.add_header(["parameters", ("distinct span masks count", 5), ("cycles per byte", 2), ("branches", 3), ("cache references", 3)])
        t.add_header(["", "< 25%", "< 50%", "< 75%", "< 95%", "100%", "min", "avg", "taken", "mispredicted", "ratio", "count", "missed", "ratio"])

        splitted = splitsorted(stats, lambda item: item[0])

        for subarray in splitted:
            distribution_name = subarray[0][0]
            title = get_distribution_title(distribution_name)
            t.add_row([(title, 14)])

            for distribution_name, parameters, histogram, hwevents, cycles in subarray:

                row = [parameters]

                # histogram
                weights = [0.25, 0.50, 0.75, 0.95, 1.00]
                tmp = self.process_histogram(histogram, weights)
                for w in weights:
                    row.append('%d' % tmp[w])

                # cycles
                if cycles is None:
                    row.append('')
                    row.append('')
                else:
                    row.append('%0.3f' % cycles[0])
                    row.append('%0.3f' % cycles[1])

                # hwevents
                row.append('%d' % hwevents.branches)
                row.append('%d' % hwevents.branch_misses)
                row.append('%0.2f%%' % (100.0 * hwevents.get_branch_miss_ratio()))
                row.append('%d' % hwevents.cache_references)
                row.append('%d' % hwevents.cache_misses)
                row.append('%0.2f%%' % (100.0 * hwevents.get_cache_miss_ratio()))

                t.add_row(row)

        return t


    def process_histogram(self, list, weights):

        assert(len(weights) > 0)

        get_count = lambda item: item[1]
        list.sort(key=get_count)
        total = sum(get_count(item) for item in list)

        result = {}
        for w in weights:
            result[w] = 0

        cumulative = 0
        for k, (mask, count) in enumerate(list):
            cumulative += count
            proc = cumulative/float(total)
            for w in result:
                if proc <= w:
                    result[w] = k + 1

        return result


def main():
    report = Report()

    spanmaskhistogram   = sys.argv[1]
    hwevents            = sys.argv[2]
    microbenchmarks     = sys.argv[3]
    output              = sys.argv[4]
    restseparator       = sys.argv[5]

    for item in load(spanmaskhistogram, hwevents, microbenchmarks):
        report.add(item)

    data = report.get()

    with open(output, 'wt') as f:
        writer = RestWriter(f, data)
        writer.write(restseparator)


if __name__ == '__main__':
    main()

