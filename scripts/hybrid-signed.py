from hybrid import GeneratorBase

class GenerateSingedParser(GeneratorBase):

    def before(self):
        self.lines.append('case 0x%02x:' % self.number)

    def after(self):
        self.lines.append('break;')

    def empty(self):
        pass

    def full(self):
        l = self.lines

        l.append("if (prev != none) {")
        l.append("   val = %s;" % self.expression(self.span, "val"))
        l.append("} else {")
        l.append("   val = %s;" % self.expression(self.span))
        l.append("}")
        l.append("prev = has_value;")

    def finalize_previous(self):
        l = self.lines

        l.append("if (prev == has_value) {")
        l.append("    *output++ = (negative) ? -val : val;")
        l.append("    prev = none;")
        l.append("} else if (prev == has_sign) {")
                      # there was a sole sign at the end of the previous block
        l.append('    throw std::runtime_error("wrong syntax");')
        l.append("}")

    def first_continuation(self):
        l = self.lines

        l.append("if (prev == has_value) {")
        l.append("   val = %s;" % self.expression(self.span, "val"))
        l.append("   *output++ = (negative) ? -val : val;")
        l.append("   prev = none;")
        l.append("} else if (prev == has_sign) {")
        l.append("   val = %s;" % self.expression(self.span, "0"))
        l.append("   *output++ = (negative) ? -val : val;")
        l.append("   prev = none;")
        l.append("} else {")
        l.append("   *output++ = %s;" % self.expression(self.span))
        l.append("}")

    def whole(self):
        self.lines.append("*output++ = %s;" % self.expression(self.span))

    def last(self):
        l = self.lines
        span = self.span

        if span.digits() == 1:
            # just one character
            l.append("if (data[%d] == '+') {" % span.first)
            l.append("  prev = has_sign;")
            l.append("  negative = false;")
            l.append("} else if (data[%d] == '-') {" % span.first)
            l.append("  prev = has_sign;")
            l.append("  negative = true;")
            l.append("} else {")
            l.append("  val = %s;" % self.expression(span, "0"))
            l.append("  prev = has_value;")
            l.append("  negative = false;")
            l.append("}")
            
        else:
            l.append("if (data[%d] == '+') {" % span.first)
            l.append("  val = %s;" % self.invocation(span.digits() - 1, span.first + 1))
            l.append("  negative = false;")
            l.append("} else if (data[%d] == '-') {" % span.first)
            l.append("  val = %s;" % self.invocation(span.digits() - 1, span.first + 1))
            l.append("  negative = true;")
            l.append("} else {")
            l.append("  val = %s;" % self.expression(span, "0"))
            l.append("  negative = false;")
            l.append("}")
            l.append("prev = has_value;")

    
    def invocation(self, digits, offset, arg = None):
        result = "hybrid_signed::convert<%d>(data" % digits
        if offset != 0:
            result += ' + %d' % offset
        
        if arg is not None:
            result += ', %s' % arg

        result += ')'
        return result


    def expression(self, span, arg = None):
        return self.invocation(span.digits(), span.first, arg)


if __name__ == '__main__':
    gen = GenerateSingedParser()
    for line in gen.get():
        print(line)

