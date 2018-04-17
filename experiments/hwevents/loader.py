class HWEvents(object):
    __slots__ = ('branches', 'branch_misses', 'cache_references', 'cache_misses')

    def get_branch_miss_ratio(self):
        return float(self.branch_misses) / self.branches

    def get_cache_miss_ratio(self):
        return float(self.cache_misses) / self.cache_references


class Item(object):
    __slots__ = ('size', 'distribution_name', 'numbers_distribution',
                 'separators_distribution', 'sign_distribution', 'hwevents')

def load(path):
    with open(path, 'rt') as f:
        for line in f:
            F = line.split(';')

            item = Item()
            item.size                    = int(F[0])
            item.distribution_name       = F[1]
            item.numbers_distribution    = tuple(eval(F[2]))
            item.separators_distribution = tuple(eval(F[3]))
            item.sign_distribution       = tuple(eval(F[4]))

            tmp = eval(F[5])
            hwevents = HWEvents()
            hwevents.branches         = tmp[0]
            hwevents.branch_misses    = tmp[1]
            hwevents.cache_references = tmp[2]
            hwevents.cache_misses     = tmp[3]
            item.hwevents = hwevents

            yield item

