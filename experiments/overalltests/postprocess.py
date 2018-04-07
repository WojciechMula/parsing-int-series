import sys
import os.path

if __name__ == '__main__' and __package__ is None:
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from report import Report
from report_writer import RestWriter


def main():
    report = Report(sys.argv[1])
    writer = RestWriter(sys.stdout, report.get())
    writer.write(sys.argv[2])

if __name__ == '__main__':
    main()
