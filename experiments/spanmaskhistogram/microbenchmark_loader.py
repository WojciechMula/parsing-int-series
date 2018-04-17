import prettyprint

class Item(object):
    __slots__ = (
        "size",
        "iterations",
        "distribution_name",
        "num_distribution",
        "sep_distribution",
        "sign_distribution",
        "cycles",
    )

    def get_num_distribution_title(self):
        return '%s (%s)' % (prettyprint.get_distribution_title(self.distribution_name), \
               prettyprint.get_num_distribution_parameters(self.distribution_name, self.num_distribution).title)

    def get_num_distribution_weight(self):
        return prettyprint.get_num_distribution_parameters(self.distribution_name, self.num_distribution).weight

    def get_sep_distribution_title(self):
        return prettyprint.get_separator_title(self.sep_distribution)


procedures = (
    'scalar',
    'scalar (std)',
    'scalar (hybrid)',
    'SSE',
    'SSE (block)',
    'SSE (simplified)',
)

reference_procedure = 'scalar'

assert reference_procedure in procedures

def load(path):
    with open(path, 'rt') as f:
        for item in load_file(f):
            yield item


def load_file(file):

    for line in file:

        item = Item()
        F = line.split(';')

        item.size = int(F[0])
        item.iterations = int(F[1])
        item.distribution_name = F[2]

        def get_tuple(string):
            tmp = eval(string)
            if type(tmp) is int:
                return (1,)
            else:
                assert type(tmp) is tuple
                return tmp

        item.num_distribution  = get_tuple(F[3])
        item.sep_distribution  = get_tuple(F[4])
        item.sign_distribution = get_tuple(F[5])
        item.cycles = eval(F[6])

        yield item

