import re


class Maker(object):
    STEP = 4

    def __init__(self, step):
        self.code = []
        self.now_step = step

    def __str__(self):
        source = ""
        for item in self.code:
            source += str(item)
        return source

    def add_line(self, line):
        self.code.extend([" " * self.now_step, line, "\n"])

    def add_raw(self, line):
        self.code.extend([" " * self.now_step, line])

    def get_session(self):
        session = Maker(self.now_step)
        self.code.append(session)
        return session

    def indent(self):
        self.now_step += self.STEP

    def dedent(self):
        self.now_step -= self.STEP

    def get_python(self):
        python_source = str(self)
        python_object = {}
        exec(python_source, python_object)
        return python_object


class TempLate(object):

    def __init__(self, text):
        self.all_vars = set()
        self.loop_vars = set()

        code = Maker(0)

        code.add_line("def render_function(context):")
        code.add_line("    def do_dots(value, *dots):")
        code.add_line("        for dot in dots:")
        code.add_line("            try:")
        code.add_line("                value = getattr(value, dot)")
        code.add_line("            except AttributeError:")
        code.add_line("                try:")
        code.add_line("                    value = value[dot]")
        code.add_line("                except KeyError:")
        code.add_line("                    return None")
        code.add_line("            if callable(value):")
        code.add_line("                value = value()")
        code.add_line("        return value")
        code.indent()
        var_maker = code.get_session()
        code.add_line("result = []")

        buffer = []

        line_arr = str(text).split('\n')
        i = len(line_arr) - 1
        while i >= 0:
            if line_arr[i].strip() == '':
                line_arr.pop()
                i = len(line_arr) - 1
            else:
                break

        line_arr.reverse()
        i = len(line_arr) - 1
        while i >= 0:
            if line_arr[i].strip() == '':
                line_arr.pop()
                i = len(line_arr) - 1
            else:
                break
        line_arr.reverse()

        def flush():
            if len(buffer) == 1:
                code.add_line("result.append(%s)" % buffer[0])
            elif len(buffer) > 1:
                code.add_line("result.extend([%s])" % ", ".join(buffer))
            del buffer[:]

        def make_if(_temp_arr):
            mark_arr = ['if', 'elif', 'not', 'and', 'or', '!=', '==', '<=', '>=', '>', '<']
            _i = 0
            arr_max = len(_temp_arr)
            _s = ''
            while _i < arr_max:
                temp = _temp_arr[_i]
                _s = _s + temp + ' '
                if temp in mark_arr:
                    _i = _i + 1
                    sub_total = ''
                    ops = 0
                    while _i < arr_max:
                        sub = _temp_arr[_i]
                        if sub not in mark_arr or ops != 0:
                            temp_strip = sub.strip()
                            if temp_strip.endswith("'") and ops == 1:
                                ops = 0
                            elif temp_strip.endswith('"') and ops == 2:
                                ops = 0
                            elif temp_strip.startswith("'") and ops == 0:
                                ops = 1
                            elif temp_strip.startswith('"') and ops == 0:
                                ops = 2
                            sub_total = sub_total + sub + ' '
                            _i = _i + 1
                        else:
                            break
                    _s = _s + self.split_expressionession(sub_total)
                else:
                    _i = _i + 1
            code.add_line(_s + ':')
        for str_index, line_str in enumerate(line_arr):
            test_strip = line_str.strip()
            if test_strip.startswith('{%') and test_strip.endswith('%}'):
                line_str = test_strip

            str_arr = re.split(r"(?s)({{.*?}}|{%.*?%}|{#.*?#})", str(line_str))
            jump_line = False
            for index, str_item in enumerate(str_arr):
                if str_item.startswith('{#'):
                    continue
                elif str_item.startswith('{{'):
                    expressionession = self.split_expressionession(str_item[2:-2].strip())
                    buffer.append("str(%s)" % expressionession)
                elif str_item.startswith('{%'):
                    jump_line = True
                    flush()
                    temp_arr = str_item[2:-2].strip().split(' ')
                    if temp_arr[0] == 'if':
                        make_if(temp_arr)
                        code.indent()
                    elif temp_arr[0] == 'elif':
                        code.dedent()
                        make_if(temp_arr)
                        code.indent()
                    elif temp_arr[0] == 'else':
                        code.dedent()
                        code.add_line("else:")
                        code.indent()
                    elif temp_arr[0] == 'for':
                        temp_arr[0] = temp_arr[0].strip()
                        self.loop_vars.add(temp_arr[1])
                        temp_arr[1] = 'temp_' + temp_arr[1]
                        for key, value in enumerate(temp_arr):
                            if value == 'in':
                                self.loop_vars.add(temp_arr[key - 1])
                                temp_arr[key-1] = 'temp_' + temp_arr[key-1]
                                break
                        s = temp_arr[0]
                        i = 1
                        while i < len(temp_arr)-1:
                            s = s + ' ' + temp_arr[i]
                            i = i + 1
                        code.add_line(s + " enumerate(%s):" % (self.split_expressionession(temp_arr[len(temp_arr)-1])))
                        code.indent()
                    elif temp_arr[0].startswith('end'):
                        code.dedent()
                        '''
                        code.add_line("temp_result_end_str = result[len(result) - 1]")
                        code.add_line("if temp_result_end_str.endswith('\\n') and temp_result_end_str.strip() == '':")
                        code.add_line("    result.pop()")
                        '''
                    else:
                        print("unknow type" + temp_arr[0])
                else:
                    if str_item:
                        buffer.append(repr(str_item))
            flush()
            if not jump_line:
                code.add_line("result.append('\\n')")

        for var_name in self.all_vars - self.loop_vars:
            var_maker.add_line("temp_%s = do_dots(context, '%s')" % (var_name, var_name))

        code.add_line("return ''.join(result)")
        code.dedent()
        self.render_function = code.get_python()['render_function']

    def split_expressionession(self, expression):
        def is_number(test_s):
            try:
                float(test_s)
                return True
            except ValueError:
                pass
            try:
                import unicodedata
                unicodedata.numeric(test_s)
                return True
            except (TypeError, ValueError):
                pass
            return False

        if is_number(expression):
            return str(expression)
        first = expression.find('[')
        end = expression.rfind(']')
        temp_str = expression.strip()
        if temp_str.startswith("'") and temp_str.endswith("'") or temp_str.startswith("'") and temp_str.endswith("'"):
            return expression
        if first != -1 and end != -1:
            temp_arr = expression[first+1:end]
            att_name = expression[:first]
            self.all_vars.add(att_name)
            code = self.split_expressionession(temp_arr)
            return "do_dots(temp_%s, %s)" % (att_name, code)
        elif "." in expression:
            dots = expression.split(".")
            code = self.split_expressionession(dots[0])
            lower_str = expression[len(dots[0]) + 1:]
            if "." in lower_str:
                args = self.split_expressionession(lower_str)
                return "do_dots(%s, %s)" % (code, args)
            else:
                first_str = ''
                end_str = ''
                for i, ch in enumerate(lower_str):
                    if ch == ' ':
                        first_str = first_str + ch
                    else:
                        break
                for i, ch in enumerate(lower_str[::-1]):
                    if ch == ' ':
                        end_str = end_str + ch
                    else:
                        break
                args = first_str + repr(lower_str.strip())
                return "do_dots(%s, %s)" % (code, args) + end_str
        elif expression.strip() == '':
            return expression
        else:
            strip = expression.strip()
            if strip.startswith('"') and strip.endswith('"') or strip.startswith("'") and strip.endswith("'"):
                return expression
            self.all_vars.add(expression.strip())

            s = ''
            b = True
            for i, ch in enumerate(expression):
                if b and ch != ' ':
                    b = False
                    s = s + 'temp_' + ch
                    continue
                s = s + ch
            return s

    def render(self, context):
        return self.render_function(context)
