import subprocess

EXECUTABLE = './bin/benchmark-cpuclocks'

class Runner(object):
    def __init__(self, size, loops, numbers_distribution, separators_distribution, sign_distribution):

        assert len(numbers_distribution) > 0
        assert len(separators_distribution) > 0
        assert len(sign_distribution) > 0

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

        return self.__parse_output(res)


    def __prepare_arguments(self):
        return (
            EXECUTABLE,
            '--size=%d'         % self.size,
            '--loops=%d'        % self.loops,
            '--num=%s'          % format_distribution(self.numbers_distribution),
            '--sep=%s'          % format_distribution(self.separators_distribution),
            '--sign=%s'         % format_distribution(self.sign_distribution),
        )


    def __parse_output(self, output):
        d = {}

        for line in output.splitlines():
            if 'cycle/op' not in line:
                continue

            # line = "scalar  :    14.503 cycle/op (best)   15.494 cycle/op (avg)"
            name, tmp = line.split(':')
            name = name.strip()
            tmp  = tmp.split()
            best = float(tmp[0])
            avg  = float(tmp[3])

            d[name] = (best, avg)

        return d


def format_distribution(dist):
    return ','.join(map(str, dist))
