import sys
import os.path

if __name__ == '__main__' and __package__ is None:
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from loader import load
from utils import groupby
from average_writer import RestWriter
from table import Table
from prettyprint import *

class Report(object):
    def __init__(self, path):
        with open(path, 'rt') as f:
            self.raw_data = load(f)

        bydistribution = lambda item: item.distribution_name

        bysep = lambda item: (item.sep_distribution, item.distribution_name)

        self.report = []
        for (sep, distribution_name), collection in groupby(self.raw_data, bysep).iteritems():
            ret = self.prepare_table(collection)
            self.report.append((
                get_separator_title(sep),
                get_distribution_title(distribution_name),
                ret
            ))

    def get(self):
        return self.report


    def prepare_table(self, procedures):

        keyfun = lambda item: (item.size, item.loops, item.num_distribution)
        tmp = groupby(procedures, keyfun)

        data = {}
        for (size, loops, _), items in tmp.iteritems():
            def get_time(procedure):
                for item in items:
                    if item.procedure == procedure:
                        return item.time

                raise KeyError("Procedure '%s' not found" % procedure)

            t0 = get_time("scalar")
            t1 = get_time("sse")
            t2 = get_time("sse-block")

            if t0 < 10 and t1 < 10 and t2 < 10:
                # don't fool people when all measurements are single-digit numbers
                continue

            speedup_sse = float(t0)/t1
            speedup_sse_block = float(t0)/t2

            key = (size, loops)
            if key not in data:
                data[key] = [[], []]
            
            data[key][0].append(speedup_sse)
            data[key][1].append(speedup_sse_block)

        t = Table()
        t.add_header([("input", 2), ("SSE speed-up", 3), ("SSE block speed-up", 3)])
        t.add_header(["size [B]", "loops", "min", "avg", "max", "min", "avg", "max"])

        def stats(numbers):
            s = sum(numbers)
            n = len(numbers)
            return min(numbers), s/n, max(numbers)

        for size, loops in sorted(data, key=lambda t: t[0]):
            
            key = size, loops

            sse = stats(data[key][0])
            sse_block = stats(data[key][1])

            t.add_row([
                '{:,}'.format(size),
                '%d' % loops,

                '%0.2f' % sse[0],
                '%0.2f' % sse[1],
                '%0.2f' % sse[2],

                '%0.2f' % sse_block[0],
                '%0.2f' % sse_block[1],
                '%0.2f' % sse_block[2],
            ])
        
        return t


if __name__ == '__main__':
    rep = Report(sys.argv[1])
    wrt = RestWriter(sys.stdout, rep.get())
    try:
        sep = sys.argv[2]
    except IndexError:
        sep = '~'
        
    wrt.write(sep)
