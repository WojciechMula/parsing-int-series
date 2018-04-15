import sys
import os.path

if __name__ == '__main__' and __package__ is None:
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from testcases import sizes, distributions
from runner import Runner, format_distribution
from report import Report
from report_writer import RestWriter

def main():
    paths = {}
    id = 0
    for item in distributions():
        size, distribution_name, numbers_distribution, separators_distribution, sign_distribution = item

        r = Runner(id, size, numbers_distribution, separators_distribution, sign_distribution)
        paths[id] = r.run()
        id += 1

    report = Report()
    id = 0
    for item in distributions():
        size, distribution_name, numbers_distribution, separators_distribution, sign_distribution = item
        path = paths[id]
        id += 1

        report.add(size, distribution_name, numbers_distribution, separators_distribution, sign_distribution, path)

    rep = report.get()
    with open(sys.argv[1], 'wt') as f:
        writer = RestWriter(f, rep)
        writer.write('-')



if __name__ == '__main__':
    main()
