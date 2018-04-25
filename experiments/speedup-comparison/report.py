import sys
import os.path

if __name__ == '__main__' and __package__ is None:
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from loader import load_file, procedures as all_procedures, reference_procedure
from utils import groupby
from prettyprint import *
from table import Table

procedures = [proc for proc in all_procedures if proc != reference_procedure]

def calculate_speeups(item):
    speedup = {}
    ref = float(item.cycles[reference_procedure][0])
    for proc in procedures:
        val = item.cycles[proc][0]
        speedup[proc] = ref/val

    return speedup


def statistics(array):
    n = len(array)
    assert n > 0
    return (min(array), sum(array)/n, max(array))


def calculate_speedup_statistics(collection):
    speedups = {}
    for proc in procedures:
        speedups[proc] = []

    for item in collection:
        speedup = calculate_speeups(item)
        for proc in procedures:
            speedups[proc].append(speedup[proc])

    size = collection[0].size
    name = collection[0].distribution_name
    result = {}
    for proc in procedures:
        array = speedups[proc]
        if len(array) == 0:
            continue

        result[proc] = statistics(array)

    return (size, name, result)


def main(path):
    with open(path, 'rt') as f:
        keyfun = lambda item: (item.size, item.distribution_name)
        data = groupby(load_file(f), keyfun)

    header1 = [("", 3), ("speedup over %s procedure" % reference_procedure, len(procedures) * 3)]
    header2 = [("", 3)]
    header3 = ["size [B]", "distribution", "samples"]
    for proc in procedures:
        header2.append((proc, 3))
        header3.extend(["min", "avg", "max"])
 
    table = Table()
    table.add_header(header1)
    table.add_header(header2)
    table.add_header(header3)

    for key in sorted(data):
        collection = data[key]
        size, name, stats = calculate_speedup_statistics(collection)

        row = []
        row.append('%d' % size)
        row.append(get_distribution_title(name))
        row.append('%d' % len(collection))

        for proc in procedures:
            row.append('%0.2f' % stats[proc][0])
            row.append('%0.2f' % stats[proc][1])
            row.append('%0.2f' % stats[proc][2])

        table.add_row(row)

    
    print table

if __name__ == '__main__':
    main(sys.argv[1])
