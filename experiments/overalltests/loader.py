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

