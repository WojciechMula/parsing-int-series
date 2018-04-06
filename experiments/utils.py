def groupby(sequence, keyfun):
    d = {}
    for item in sequence:
        key = keyfun(item)
        if key not in d:
            d[key] = [item]
        else:
            d[key].append(item)

    return d

