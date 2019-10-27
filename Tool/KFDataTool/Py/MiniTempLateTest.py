from MiniTempLate import TempLate

#测试注释
text1 = """{# this is log #}"""

#测试变量
text2 = """this is var {{ varname }}"""

#测试变量2
text3 = """this is var {{ testobj.name }}"""

#测试if else 记得要加end
text4 = """
{% if varname == '这是一个变量名' %}
    if testif == true
{%else%}
    if testif == false
{%end%}
"""

#测试for 记得要加end
text5 = """
{% for index, str_item in testobj.arrlist%}  
    {{str_item}}
{%end%} 
"""

text6 = """
{% if this_is_none %}
    true
{% else %}
    false
{%end%}
"""

text7 = """
{% if testobj.this_is_none %}
    true
{% else %}
    false
{%end%}
"""

text8 = """
{% if testif %}
    true
{% elif testand %}
    我要打印这行字
{% else %}
    false
{%end%}
"""

# 多符号联合测试   not   and   or   !=   ==   <=   >=   >   <
# 直接判断string
# 直接判断num
# 测试string中 带关键字 带 " ' 符号
# .符号 前后多空格的情况
# [ ]符号内 多空格的情况
text9 = """
{% if      not  testif  and  not testif and varname ==   ' [这.是 一个.带  . 空格 带关键字 and  带 " 符号嵌套 " " 的变量] ' and testif != testand and testobj.  testbool    and 1 != testif and not not not testif and testobj[testobj.name] %}
    this is true
{%end%}
"""

text10 = """
{% for index, str_item in testobj.arrlist%}  
    {% if testand %}
        this is str
    {%end%} 
{%end%} 
"""

ctx = {'this_is_none': None, 'varname': ' [这.是 一个.带  . 空格 带关键字 and  带 " 符号嵌套 " " 的变量] ', 'testif': False, 'testand': True, 'testnot': False, 'testor': True, 'testobj': {'this_is_none': None, 'test1': {'test2': 'test3'},  'test3': False, 'testbool': True, 'name': "sex", 'sex': 'man', 'AAA': 1, 'BBB': "hello" , "arrlist":["abc","efg","333","333"]}, 'testArr': [{'name': "AAA", 'sex': 'man'}, {'name': "BBB", 'sex': 'man'}, {'name': "CCC", 'sex': 'man'}]}


print("1=========================")
print(TempLate(text1).render(ctx))
print("2=========================")
print(TempLate(text2).render(ctx))
print("3=========================")
print(TempLate(text3).render(ctx))
print("4=========================")
print(TempLate(text4).render(ctx))
print("5=========================")
print(TempLate(text5).render(ctx))
print("6=========================")
print(TempLate(text6).render(ctx))
print("7=========================")
print(TempLate(text7).render(ctx))
print("8=========================")
print(TempLate(text8).render(ctx))
print("9=========================")
print(TempLate(text9).render(ctx))
print("10=========================")
print(TempLate(text10).render(ctx))
print("end========================")


