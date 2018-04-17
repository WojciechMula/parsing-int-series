import sys
import os.path

if __name__ == '__main__' and __package__ is None:
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from testcases import testcases
from runner import Runner, format_distribution

def main():

    for item in testcases():
        size, distribution_name, numbers_distribution, separators_distribution, sign_distribution = item

        r = Runner(size, numbers_distribution, separators_distribution, sign_distribution)
        result = r.run()
        print "%d;%s;%s;%s;%s;%s" % (
            size,
            distribution_name,
            numbers_distribution,
            separators_distribution,
            sign_distribution,
            result
        )


if __name__ == '__main__':
    main()
