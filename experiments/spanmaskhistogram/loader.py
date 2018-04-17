from hwevents_loader import load as load_hwevents

def load(spanmaskhistogram, hwevents):

    hwevents_dict = {}
    for item in load_hwevents(hwevents):
        key = (item.size, item.numbers_distribution, item.separators_distribution, item.sign_distribution)
        hwevents_dict[key] = item.hwevents
    
    for item in load_histogram(spanmaskhistogram):
        key = (item.size, item.numbers_distribution, item.separators_distribution, item.sign_distribution)
        item.hwevents = hwevents_dict[key]

        yield item


class Item(object):
    __slots__ = ('size', 'distribution_name', 'numbers_distribution',
                 'separators_distribution', 'sign_distribution', 'histogram', 'hwevents')

def load_histogram(path):
    with open(path, 'rt') as f:
        for line in f:
            F = line.split(';')

            item = Item()
            item.size                    = int(F[0])
            item.distribution_name       = F[1]
            item.numbers_distribution    = tuple(eval(F[2]))
            item.separators_distribution = tuple(eval(F[3]))
            item.sign_distribution       = tuple(eval(F[4]))
            item.histogram               = eval(F[5])

            yield item

