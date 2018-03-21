ITEM_PATTERN = "/* %(ID)04x */ {%(FIRST_SKIP)s, %(FIRST_LENGTH)s, %(TOTAL_SKIP)s, %(ELEMENT_COUNT)s, %(ELEMENT_SIZE)s, {%(PSHUFB_PATTERN)s}}"
FILE_PATTERN = """
#pragma once

#include "block_info.h"

BlockInfo blocks[%(COUNT)d] = {
%(ITEMS)s
};
"""

class CPPWriter(object):
    def __init__(self, data):
        self.data = data
        pass

    def save(self, path):
        tmp = [self._render_item(item) for item in self.data]
        params = {
            'COUNT': len(tmp),
            'ITEMS': ',\n'.join(tmp),
        }

        text = FILE_PATTERN % params

        with open(path, 'wt') as f:
            f.write(text)


    def _render_item(self, block):
        params = {
            'ID'             : block.id,
            'FIRST_SKIP'     : block.first_skip,
            'FIRST_LENGTH'   : block.first_length,
            'TOTAL_SKIP'     : block.total_skip,
            'ELEMENT_COUNT'  : len(block.ranges),
            'ELEMENT_SIZE'   : block.element_size,
            'PSHUFB_PATTERN' : self._make_c_array(block.pshufb_pattern),
        }

        return ITEM_PATTERN % params


    def _make_c_array(self, numbers):
        return ','.join('0x%02x' % x for x in numbers)

