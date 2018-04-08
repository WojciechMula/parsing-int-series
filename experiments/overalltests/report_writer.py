class RestWriter(object):
    def __init__(self, file, report):
        self.file   = file
        self.report = report


    def write(self, restsection):

        assert len(restsection) >= 3

        for separator, collection1 in self.report:
            self.write_header(separator, restsection[0], 80)
            for distribution, collection2 in collection1:
                self.write_header(distribution, restsection[1], 50)
                for parameters, table in collection2:
                    self.write_header(parameters, restsection[2], 40)
                    self.file.write('\n')
                    self.file.write(str(table))


    def write_header(self, title, char, width = 80):
        f = self.file
        f.write('\n')
        f.write('\n')
        f.write("%s\n" % title)
        f.write(char * max(len(title), width))
        f.write('\n')

