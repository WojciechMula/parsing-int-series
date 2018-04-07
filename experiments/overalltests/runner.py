import subprocess

EXECUTABLE = './bin/benchmark'


class Runner(object):
    def __init__(self, procedure, size, loops, numbers_distribution, separators_distribution, sign_distribution):

        assert len(numbers_distribution) > 0
        assert len(separators_distribution) > 0
        assert len(sign_distribution) > 0

        self.procedure = procedure
        self.size      = size
        self.loops     = loops
        self.numbers_distribution    = numbers_distribution
        self.separators_distribution = separators_distribution
        self.sign_distribution       = sign_distribution

    def run(self):
        args = self.__prepare_arguments()
        proc = subprocess.Popen(args, stdout=subprocess.PIPE)
        res  = proc.communicate()[0]
        ret  = proc.wait()
        if ret != 0:
            print args
            print res
            raise RuntimeError("program failed")

        d = self.__parse_output(res)
        return d['time']


    def __prepare_arguments(self):
        return (
            EXECUTABLE,
            '--procedure=%s'    % self.procedure,
            '--size=%d'         % self.size,
            '--loops=%d'        % self.loops,
            '--num=%s'          % format_distribution(self.numbers_distribution),
            '--sep=%s'          % format_distribution(self.separators_distribution),
            '--sign=%s'         % format_distribution(self.sign_distribution),
        )


    def __parse_output(self, output):
        d = {}

        for line in output.splitlines():
            tmp = line.split(':')
            try:
                key   = tmp[0].strip()
                value = tmp[1].strip()

                d[key] = value
            except IndexError:
                continue

            try:
                value = int(value.split()[0])
                d[key] = value
            except ValueError:
                pass

        return d


def format_distribution(dist):
    return ','.join(map(str, dist))
