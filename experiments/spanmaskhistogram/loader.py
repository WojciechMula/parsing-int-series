class Item(object):
    __slots__ = ('size', 'distribution_name', 'numbers_distribution',
                 'separators_distribution', 'sign_distribution', 'histogram')

def load(path):
    with open(path, 'rt') as f:
        for line in f:
            F = line.split(';')

            item = Item()
            item.size                    = int(F[0])
            item.distribution_name       = F[1]
            item.numbers_distribution    = eval(F[2])
            item.separators_distribution = eval(F[3])
            item.sign_distribution       = eval(F[4])
            item.histogram               = eval(F[5])

            yield item

