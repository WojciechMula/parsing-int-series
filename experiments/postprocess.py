import sys
from report import Report
from writer import RestWriter


def main():
    report = Report(sys.argv[1])
    writer = RestWriter(sys.stdout, report.get())
    writer.write(sys.argv[2])

if __name__ == '__main__':
    main()
