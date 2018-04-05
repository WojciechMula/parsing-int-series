from table import Table


class Item(object):
    __slots__ = ("procedure",
                 "size",
                 "loops",
                 "distribution_name",
                 "num_distribution",
                 "sep_distribution",
                 "sign_distribution",
                 "time")


class Report(object):

    def __init__(self, path):
        with open(path, 'rt') as f:
            self.raw_data = self.load(f)

        # group by separators distribution
        bysep = lambda item: item.sep_distribution

        self.report = []
        for sep, collection in groupby(self.raw_data, bysep).iteritems():
            ret = self.split_by_distribution(collection)
            self.report.append((
                self.separator_title(sep),
                ret
            ))


    def get(self):
        return self.report


    def load(self, file):
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


    def separator_title(self, sep_distribution):
        if sep_distribution == '1':
            separator = 'Single separator character'
        else:
            k = len(sep_distribution.split(','))
            separator = '1 .. %d separator characters' % k

        return separator


    def split_by_distribution(self, collection):
        result = []

        bynum = lambda item: (item.distribution_name)
        tmp = groupby(collection, bynum)
        for distribution_name, collection in tmp.iteritems():
            res = self.split_by_parameters(distribution_name, collection)
            result.append((
                self.distribution_title(distribution_name),
                res
            ))

        return result
 

    def distribution_title(self, distribution_name):
        if distribution_name == 'single':
            return 'Fixed length'
        elif distribution_name == 'normal':
            return 'Guassian distribution'
        elif distribution_name == 'uniform':
            return 'Uniform distribution'
        else:
            assert False


    def split_by_parameters(self, distribution_name, collection):
        byparam = lambda item: item.num_distribution

        result = []
        for key, collection in groupby(collection, byparam).iteritems():
            table = self.prepare_table(collection)
            n, title = self.parameters_title(distribution_name, key)
            result.append((
                title,
                table,
                n
            ))

        result.sort(key=lambda row: row[-1])

        return [item[:2] for item in result]


    def prepare_table(self, procedures):

        keyfun = lambda item: (item.size, item.loops)
        tmp = groupby(procedures, keyfun)

        data = []
        for (size, loops), items in tmp.iteritems():
            def get_time(procedure):
                for item in items:
                    if item.procedure == procedure:
                        return item.time

                raise KeyError("Procedure '%s' not found" % procedure)

            data.append((
                size,
                loops,
                get_time("scalar"),
                get_time("sse"),
                get_time("sse-unrolled"),
            ))

        data.sort(key=lambda t: t[0]) # sort by size

        t = Table()
        t.add_header([("input", 2), "scalar", ("SSE", 2), ("SSE unrolled", 2)])
        t.add_header(["size [B]", "loops", "time [us]", "time [us]", "speed-up", "time [us]", "speed-up"])

        for item in data:
            t0 = item[2]
            t1 = item[3]
            t2 = item[4]
            if t0 < 10 and t1 < 10 and t2 < 10:
                # don't fool people when all measurements are single-digit numbers
                speedup_sse = '---'
                speedup_sse_unrolled = '---'
            else:
                speedup_sse = '%0.2f' % (float(t0)/t1)
                speedup_sse_unrolled = '%0.2f' % (float(t0)/t2)

            t.add_row([
                '{:,}'.format(item[0]),
                '%d' % item[1],
                '%d' % item[2],
                '%d' % item[3],
                speedup_sse,
                '%d' % item[4],
                speedup_sse_unrolled,
            ])
        
        return t


    def parameters_title(self, distribution_name, num_distribution):

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
            return (n, format_count(n, 'digit'))

        elif distribution_name == 'normal':
            def get_max():
                return distr.index(max(distr)) + 1

            n = get_max()
            return (n, "max at %d digit" % n)
        elif distribution_name == 'uniform':
            n = len(distr)
            return (n, "1 .. %s" % format_count(n, 'digit'))
        else:
            assert False


def groupby(sequence, keyfun):
    d = {}
    for item in sequence:
        key = keyfun(item)
        if key not in d:
            d[key] = [item]
        else:
            d[key].append(item)

    return d

