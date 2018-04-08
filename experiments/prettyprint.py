class Parameters(object):
    def __init__(self, weight, title):
        self.weight = weight
        self.title  = title

def get_num_distribution_parameters(distribution_name, num_distribution):

    distr = map(int, num_distribution.split(','))
    def format_count(count, noun):
        if count == 1:
            return '%d %s' % (count, noun)
        else:
            return '%d %ss' % (count, noun)

    if distribution_name == 'single':
        def get_fixed():
            return distr.index(1) + 1

        n = get_fixed()
        return Parameters(n, format_count(n, 'digit'))

    elif distribution_name == 'normal':
        def get_max():
            return distr.index(max(distr)) + 1

        n = get_max()
        return Parameters(n, "max at %d digit" % n)

    elif distribution_name == 'uniform':
        n = len(distr)
        return Parameters(n, "1 .. %s" % format_count(n, 'digit'))
    
    assert False


def get_distribution_title(distribution_name):
    if distribution_name == 'single':
        return 'Fixed length'
    elif distribution_name == 'normal':
        return 'Gaussian distribution'
    elif distribution_name == 'uniform':
        return 'Uniform distribution'
    
    assert False


def get_separator_title(sep_distribution):
    if sep_distribution == '1':
        separator = 'single separator character'
    else:
        k = len(sep_distribution.split(','))
        separator = '1 .. %d separator characters' % k

    return separator

