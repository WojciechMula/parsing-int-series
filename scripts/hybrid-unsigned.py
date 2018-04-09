from hybrid import GeneratorBase

class GenerateUnsingedParser(GeneratorBase):

    def before(self):
        self.lines.append('case 0x%02x:' % self.number)

    def after(self):
        self.lines.append('break;')

    def empty(self):
        pass

    def full(self):
        l = self.lines

        l.append("if (has_last) {")
        l.append("   val = %s;" % self.expression(self.span, "val"))
        l.append("} else {")
        l.append("   val = %s;" % self.expression(self.span))
        l.append("}")
        l.append("has_last = true;")

    def finalize_previous(self):
        l = self.lines

        l.append("if (has_last) {")
        l.append("   has_last = false;")
        l.append("    *output++ = val;")
        l.append("}")

    def first_continuation(self):
        l = self.lines

        l.append("if (has_last) {")
        l.append("   val = %s;" % self.expression(self.span, "val"))
        l.append("   has_last = false;")
        l.append("} else {")
        l.append("   val = %s;" % self.expression(self.span))
        l.append("}")
        l.append("*output++ = val;")

    def whole(self):
        self.lines.append("*output++ = %s;" % self.expression(self.span))

    def last(self):
        l = self.lines

        l.append("val = %s;" % self.expression(self.span))
        l.append("has_last = true;")

    def expression(self, span, arg = None):
        result = "scalar::convert<%d>(data" % span.digits()
        if span.first != 0:
            result += ' + %d' % span.first;

        if arg is not None:
            result += ', %s' % arg

        result += ')'
        return result


if __name__ == '__main__':
    gen = GenerateUnsingedParser()
    for line in gen.get():
        print line

