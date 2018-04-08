class RestWriter(object):
    def __init__(self, file, report):
        self.file   = file
        self.report = report


    def write(self, restsection):

        assert len(restsection) >= 2

        for section, collection in self.report:
            self.write_header(section, restsection[0], 80)

            for subsection, table in collection:
                self.file.write('\n')
                self.file.write("**%s**\n" % subsection)
                self.file.write('\n')
                self.file.write(str(table))
                self.file.write('\n')


    def write_header(self, title, char, width):
        f = self.file
        f.write('\n')
        f.write('\n')
        f.write("%s\n" % title)
        f.write(char * max(len(title), width))
        f.write('\n')

