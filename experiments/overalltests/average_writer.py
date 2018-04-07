import sys

class RestWriter(object):
    def __init__(self, file, report):
        self.file   = file
        self.report = report


    def write(self, restsection):

        assert len(restsection) >= 1

        for separator, distribution, table in self.report:
            header = '%s (%s)' % (distribution, separator)
            self.write_header(header, restsection[0], 80)
            self.file.write('\n')
            self.file.write(str(table))


    def write_header(self, title, char, width = 80):
        f = self.file
        f.write('\n')
        f.write('\n')
        f.write("%s\n" % title)
        f.write(char * max(len(title), width))
        f.write('\n')

