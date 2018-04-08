import sys
import os.path

if __name__ == '__main__' and __package__ is None:
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from table import Table
from loader import load
from utils import groupby
from report_writer import RestWriter
from prettyprint import *


class Report(object):

    def __init__(self, path):
        with open(path, 'rt') as f:
            self.raw_data = load(f)

        # group by separators distribution
        bysep = lambda item: item.sep_distribution

        self.report = []
        for sep, collection in groupby(self.raw_data, bysep).iteritems():
            ret = self.split_by_distribution(collection)
            self.report.append((
                get_separator_title(sep),
                ret
            ))


    def get(self):
        return self.report


    def split_by_distribution(self, collection):
        result = []

        bynum = lambda item: (item.distribution_name)
        tmp = groupby(collection, bynum)
        for distribution_name, collection in tmp.iteritems():
            res = self.split_by_parameters(distribution_name, collection)
            result.append((
                get_distribution_title(distribution_name),
                res
            ))

        return result
 

    def split_by_parameters(self, distribution_name, collection):
        byparam = lambda item: item.num_distribution

        result = []
        for key, collection in groupby(collection, byparam).iteritems():
            table = self.prepare_table(collection)
            ret   = get_num_distribution_parameters(distribution_name, key)
            result.append((
                ret.title,
                table,
                ret.weight
            ))

        result.sort(key=lambda row: row[-1])

        return [item[:2] for item in result]


    def prepare_table(self, procedures):

        keyfun = lambda item: (item.size, item.loops)
        tmp = groupby(procedures, keyfun)

        data = []
        for (size, loops), items in tmp.iteritems():
            def get_time(procedure):
                for item in items:
                    if item.procedure == procedure:
                        return item.time

                raise KeyError("Procedure '%s' not found" % procedure)

            data.append((
                size,
                loops,
                get_time("scalar"),
                get_time("sse"),
                get_time("sse-unrolled"),
            ))

        data.sort(key=lambda t: t[0]) # sort by size

        t = Table()
        t.add_header([("input", 2), "scalar", ("SSE", 2), ("SSE unrolled", 2)])
        t.add_header(["size [B]", "loops", "time [us]", "time [us]", "speed-up", "time [us]", "speed-up"])

        for item in data:
            t0 = item[2]
            t1 = item[3]
            t2 = item[4]
            if t0 < 10 and t1 < 10 and t2 < 10:
                # don't fool people when all measurements are single-digit numbers
                speedup_sse = '---'
                speedup_sse_unrolled = '---'
            else:
                speedup_sse = '%0.2f' % (float(t0)/t1)
                speedup_sse_unrolled = '%0.2f' % (float(t0)/t2)

            t.add_row([
                '{:,}'.format(item[0]),
                '%d' % item[1],
                '%d' % item[2],
                '%d' % item[3],
                speedup_sse,
                '%d' % item[4],
                speedup_sse_unrolled,
            ])
        
        return t




def main():
    report = Report(sys.argv[1])
    writer = RestWriter(sys.stdout, report.get())
    try:
        restsection = sys.argv[2]
    except IndexError:
        restsection = "-~#"

    writer.write(restsection)

if __name__ == '__main__':
    main()
