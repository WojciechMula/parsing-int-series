import sys
import os.path

from table import Table
from utils import splitsorted
from prettyprint import *


class Report(object):

    def __init__(self):
        self.report = None
        self.tmp = []
        self.prev_size = None

    def add(self, size, distribution_name, numbers_distribution, separators_distribution, sign_distribution, path):
        if size != self.prev_size:
            self.prev_size = size
            self.tmp.append((size, []))

        title = '%s, %s' % (
            get_num_distribution_parameters(distribution_name, numbers_distribution).title,
            get_separator_title(separators_distribution))

        self.tmp[-1][1].append((distribution_name, title, path))


    def get(self):
        if self.report is None:
            self.report = []
            for size, statistics in self.tmp:
                title = 'Input size {:,d} bytes'.format(size)
                self.report.append((title, self.prepare_table(statistics)))

        return self.report


    def prepare_table(self, stats):

        t = Table()
        t.add_header(["parameters", ("distinct span masks count", 5)])
        t.add_header(["", "< 25%", "< 50%", "< 75%", "< 95%", "100%"])

        splitted = splitsorted(stats, lambda item: item[0])

        for subarray in splitted:
            distribution_name = subarray[0][0]
            title = get_distribution_title(distribution_name)
            t.add_row([(title, 6)])

            for distribution_name, parameters, path in subarray:
                histogram = self.parse_file(path)
                weights = [0.25, 0.50, 0.75, 0.95, 1.00]
                tmp = self.process_histogram(histogram, weights)
                row = [parameters]
                for w in weights:
                    row.append('%d' % tmp[w])

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


    def parse_file(self, path):
        res = []
        with open(path, 'rt') as f:
            for line in f:
                F = line.split(',')
                mask  = int(F[0].strip(), 16)
                count = int(F[1].strip())

                res.append((mask, count))

        return res
