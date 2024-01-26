import subprocess
from os.path import exists

EXECUTABLE = './bin/benchmark-hwevents'
LOOPS = 3


class Runner(object):
    def __init__(self, size, numbers_distribution, separators_distribution, sign_distribution):

        assert len(numbers_distribution) > 0
        assert len(separators_distribution) > 0
        assert len(sign_distribution) > 0

        self.size   = size
        self.numbers_distribution    = numbers_distribution
        self.separators_distribution = separators_distribution
        self.sign_distribution       = sign_distribution

    def run(self):
        args = self.__prepare_arguments()
        proc = subprocess.Popen(args, stdout=subprocess.PIPE)
        res  = proc.communicate()[0]
        ret  = proc.wait()
        if ret != 0:
            print(args)
            print(res)
            raise RuntimeError("program failed")

        return self.__parse_output(res)


    def __prepare_arguments(self):
        return (
            EXECUTABLE,
            '--size=%d'         % self.size,
            '--num=%s'          % format_distribution(self.numbers_distribution),
            '--sep=%s'          % format_distribution(self.separators_distribution),
            '--sign=%s'         % format_distribution(self.sign_distribution),
            '--loops=%d'        % LOOPS,
            '--csv-output'
        )


    def __parse_output(self, output):
        return list(map(int, (s.strip() for s in output.split(b','))))


def format_distribution(dist):
    return ','.join(map(str, dist))
