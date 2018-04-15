import subprocess
from os.path import exists

EXECUTABLE = './bin/statistics'
TMPPATH = '/dev/shm/stats-%d.csv'


class Runner(object):
    def __init__(self, id, size, numbers_distribution, separators_distribution, sign_distribution):

        assert len(numbers_distribution) > 0
        assert len(separators_distribution) > 0
        assert len(sign_distribution) > 0

        self.path   = TMPPATH % id
        self.size   = size
        self.numbers_distribution    = numbers_distribution
        self.separators_distribution = separators_distribution
        self.sign_distribution       = sign_distribution

    def run(self):
        if not exists(self.path):
            args = self.__prepare_arguments()
            proc = subprocess.Popen(args, stdout=subprocess.PIPE)
            res  = proc.communicate()[0]
            ret  = proc.wait()
            if ret != 0:
                print args
                print res
                raise RuntimeError("program failed")

        return self.path


    def __prepare_arguments(self):
        return (
            EXECUTABLE,
            '--size=%d'         % self.size,
            '--num=%s'          % format_distribution(self.numbers_distribution),
            '--sep=%s'          % format_distribution(self.separators_distribution),
            '--sign=%s'         % format_distribution(self.sign_distribution),
            '--histogram-file=%s' % self.path
        )


def format_distribution(dist):
    return ','.join(map(str, dist))
