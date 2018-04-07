class Item(object):
    __slots__ = ("procedure",
                 "size",
                 "loops",
                 "distribution_name",
                 "num_distribution",
                 "sep_distribution",
                 "sign_distribution",
                 "time")


def load(file):
    L = []
    for line in file:   
        F = line.split(';')

        L.append(Item())
        item = L[-1]

        item.procedure          = F[0]
        item.size               = int(F[1])
        item.loops              = int(F[2])
        item.distribution_name  = F[3]
        item.num_distribution   = F[4]
        item.sep_distribution   = F[5]
        item.sign_distribution  = F[6]
        item.time               = int(F[7])

    return L


def get_distribution_title(distribution_name):
    if distribution_name == 'single':
        return 'Fixed length'
    elif distribution_name == 'normal':
        return 'Guassian distribution'
    elif distribution_name == 'uniform':
        return 'Uniform distribution'
    else:
        assert False


def get_separator_title(sep_distribution):
    if sep_distribution == '1':
        separator = 'Single separator character'
    else:
        k = len(sep_distribution.split(','))
        separator = '1 .. %d separator characters' % k

    return separator

