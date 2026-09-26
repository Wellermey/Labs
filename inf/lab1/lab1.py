from math import *
import re

functions = [
    "pow",
    "round",
    "sqrt",
    "abs",
    "sin",
    "cos",
    "tan",
    "log",
    "exp",
    "floor",
    "ceil",
    "fact",
    "log10",
]

oper = {
    "and": 0,
    "|": 1,
    "^": 2,
    "&": 3,
    "==": 4,
    "!=": 4,
    ">=": 5,
    "<=": 5,
    "<": 5,
    ">": 5,
    "<<": 6,
    ">>": 6,
    "+": 7,
    "-": 7,
    "*": 8,
    "/": 8,
    "m": 9,
}


def is_digit(n):
    return n.replace(".", "").replace("-", "").isdigit()


# Разбиваем на части
def split_raw(raw_str):
    regular = re.compile(
        r"""
        \d+(?:\.\d+)?|,|
        [a-z]\w*|
        >=|<=|==|!=|<<|>>|
        [\+\-\*/<>&^\()|]
        """,
        re.VERBOSE,
    )
    return re.findall(regular, raw_str)


# Убираем унарный минус
def remove_m(badtokens):
    tokens = []
    i = 0
    while i < len(badtokens):
        if badtokens[i] == "-" and (
            i == 0
            or badtokens[i - 1] == "("
            or badtokens[i - 1] == ","
            or badtokens[i - 1] in oper
        ):
            if is_digit(badtokens[i + 1]):
                tokens.append("-" + badtokens[i + 1])
                i += 1
            else:
                tokens.append("m")
        else:
            tokens.append(badtokens[i])
        i += 1
    return tokens


# В обратную польскую
def to_reverse_polish(tokens):
    output = []
    stack = []
    i = 0

    while i < len(tokens):
        if is_digit(tokens[i]):
            output.append(tokens[i])
        elif tokens[i] == "(":
            stack.append(tokens[i])
        elif tokens[i] == ")":
            while stack and stack[-1] != "(":
                output.append(stack.pop())
            stack.pop()
        elif tokens[i] in oper:
            while stack and stack[-1] in oper and oper[stack[-1]] >= oper[tokens[i]]:
                output.append(stack.pop())
            stack.append(tokens[i])
        elif tokens[i] in functions:
            stack.extend(["(", tokens[i]])
            i += 1
        elif tokens[i] == "max" or tokens[i] == "min":  #  считаем количество аргументов
            countc = 1  # ','
            counts = 1  # '()'
            j = i + 1
            while counts:
                j += 1
                if tokens[j] == "(":
                    counts += 1
                elif tokens[j] == ")":
                    counts -= 1
                elif tokens[j] == "," and counts == 1:
                    countc += 1
            stack.extend(["(", tokens[i] + str(countc)])
            i += 1

        i += 1
    while stack:
        output.append(stack.pop())
    return output


loper1 = {
    "sqrt": sqrt,
    "abs": fabs,
    "sin": sin,
    "cos": cos,
    "tan": tan,
    "log": log,
    "exp": exp,
    "floor": floor,
    "ceil": ceil,
    "fact": factorial,
    "log10": log10,
    "m": lambda a: -a,
}

loper2 = {
    "|": lambda a, b: int(a) | int(b),
    "^": lambda a, b: int(a) ^ int(b),
    "&": lambda a, b: int(a) & int(b),
    "==": lambda a, b: int(a) == int(b),
    "!=": lambda a, b: int(a) != int(b),
    ">=": lambda a, b: a >= b,
    "<=": lambda a, b: a <= b,
    "<": lambda a, b: a < b,
    ">": lambda a, b: a > b,
    "<<": lambda a, b: int(a) << int(b),
    ">>": lambda a, b: int(a) >> int(b),
    "+": lambda a, b: a + b,
    "-": lambda a, b: a - b,
    "*": lambda a, b: a * b,
    "/": lambda a, b: a / b,
    "and": lambda a, b: int(a) and int(b),
    "pow": lambda a, b: a**b,
    "round": lambda a, b: round(a, int(b)),
}


# Считаем обратную польскую
def calculate(tokens):
    stack = []

    for e in tokens:
        if is_digit(e):
            stack.append(e)
        elif e in loper1:
            stack.append(loper1[e](float(stack.pop())))
        elif e in loper2:
            a = float(stack.pop())
            b = float(stack.pop())
            stack.append(loper2[e](b, a))
        elif e[0:3] == "max":
            n = int(e[3:])
            maxel = max([float(stack.pop()) for i in range(n)])
            stack.append(maxel)
        elif e[0:3] == "min":
            n = int(e[3:])
            maxel = min([float(stack.pop()) for i in range(n)])
            stack.append(maxel)

    return stack[0]


# Считаем прямую польскую
def calculate2(tokens):
    stack = []

    for e in tokens[::-1]:
        if is_digit(e):
            stack.append(e)
        elif e in loper1:
            stack.append(loper1[e](float(stack.pop())))
        elif e in loper2:
            a = float(stack.pop())
            b = float(stack.pop())
            stack.append(loper2[e](a, b))
        elif e[0:3] == "max":
            n = int(e[3:])
            maxel = max([float(stack.pop()) for i in range(n)])
            stack.append(maxel)
        elif e[0:3] == "min":
            n = int(e[3:])
            maxel = min([float(stack.pop()) for i in range(n)])
            stack.append(maxel)

    return stack[0]


# Проверка корректность ввода
def parser(tokens):
    return True


raw_str = str(input("Ввод:"))
tokens = split_raw(raw_str)
new_tokens = remove_m(tokens)
if "(" in tokens:
    print("Результат:", calculate(to_reverse_polish(new_tokens)))
elif tokens[0] in oper or tokens[0] in functions:
    print("Результат:", calculate(tokens))
elif tokens[-1] in oper or tokens[-1] in functions:
    print("Результат:", calculate2(tokens))
else:
    print("Результат:", calculate(to_reverse_polish(tokens)))
