def groupby(sequence, keyfun):
    d = {}
    for item in sequence:
        key = keyfun(item)
        if key not in d:
            d[key] = [item]
        else:
            d[key].append(item)

    return d

def splitsorted(sequence, keyfun):
    prev = None
    result = []
    for item in sequence:
        val = keyfun(item)
        if val != prev:
            result.append([])
            prev = val

        result[-1].append(item)

    return result
