import sys
import os.path

if __name__ == '__main__' and __package__ is None:
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))


from testcases import testcases
from runner import Runner, format_distribution


def test(file):
    for item in testcases():
        size, loops, distribution_name, numbers_distribution, separators_distribution, sign_distribution = item

        r = Runner(size, loops, numbers_distribution, separators_distribution, sign_distribution)
        clocks = r.run()

        file.write("%d;%d;%s;%s;%s;%s;%s\n" % (
            size,
            loops,
            distribution_name,
            format_distribution(numbers_distribution),
            format_distribution(separators_distribution),
            format_distribution(sign_distribution),
            str(clocks)))


def main():
    test(sys.stdout)


if __name__ == '__main__':
    main()
