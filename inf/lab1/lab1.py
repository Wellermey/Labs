import math
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

loper1 = {
    "sqrt": math.sqrt,
    "abs": math.fabs,
    "sin": math.sin,
    "cos": math.cos,
    "tan": math.tan,
    "log": math.log,
    "exp": math.exp,
    "floor": math.floor,
    "ceil": math.ceil,
    "fact": lambda a: math.factorial(int(a)),
    "log10": math.log10,
    "m": lambda a: -a,
}

loper2 = {
    "|": lambda a, b: int(a) | int(b),
    "^": lambda a, b: int(a) ^ int(b),
    "&": lambda a, b: int(a) & int(b),
    "==": lambda a, b: a == b,
    "!=": lambda a, b: a != b,
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
    "and": lambda a, b: bool(a) and bool(b),
    "pow": lambda a, b: a**b,
    "round": lambda a, b: round(a, int(b)),
}


def is_digit(n):
    try:
        float(n)
        return True
    except ValueError:
        return False


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
        elif tokens[i] == ",":
            while stack and stack[-2] != "(":
                output.append(stack.pop())

        i += 1
    while stack:
        output.append(stack.pop())
    return output


# Считаем обратную польскую
def calculate(tokens, flag):
    stack = []

    for e in tokens:
        if is_digit(e):
            stack.append(e)
        elif e in loper1:
            try:
                stack.append(loper1[e](float(stack.pop())))
            except Exception as error:
                raise ValueError(error)
        elif e in loper2:
            try:
                if flag == "reverse":
                    a = float(stack.pop())
                    b = float(stack.pop())
                else:
                    b = float(stack.pop())
                    a = float(stack.pop())
                stack.append(loper2[e](b, a))
            except Exception as error:
                raise ValueError(error)
        elif e[0:3] == "max":
            n = int(e[3:])
            maxel = max([float(stack.pop()) for i in range(n)])
            stack.append(maxel)
        elif e[0:3] == "min":
            n = int(e[3:])
            maxel = min([float(stack.pop()) for i in range(n)])
            stack.append(maxel)
    if len(stack) != 1:
        raise ValueError("Некорректное выражение")
    return stack[0]


# Проверка корректности ввода
def parser(tokens):
    if not tokens:
        return False

    brackets = 0
    for token in tokens:
        if token == "(":
            brackets += 1
        elif token == ")":
            brackets -= 1
            if brackets < 0:
                return False
    if brackets != 0:
        return False

    all_functions = set(functions) | {"max", "min"}
    all_oper = set(oper.keys()) | set(functions) | {"max", "min"}

    for i in range(len(tokens)):
        curr = tokens[i]
        prev = tokens[i - 1] if i > 0 else None
        next = tokens[i + 1] if i < len(tokens) - 1 else None

        if (
            not is_digit(curr)
            and curr not in oper
            and curr not in all_functions
            and curr not in {"(", ")", ","}
        ):
            return False

        if (
            curr in all_functions
            and next != "("
            and prev
            and not is_digit(prev)
            and prev != ")"
        ):
            return False

        if curr == ",":
            if prev is None or next is None or prev == "(" or next == ")":
                return False
            if prev in oper or next in oper:
                return False
        if (
            is_digit(curr)
            and next
            and is_digit(next)
            and "(" in tokens
            and not tokens[0] in all_oper
            and not tokens[-1] in all_oper
        ):
            return False

    return True


def check_notation(tokens, new_tokens):
    attempts = []
    if not "(" in tokens:
        if tokens[0] in oper or tokens[0] in (set(functions) | {"min", "max"}):
            attempts.append(lambda: calculate(tokens, "straight"))
        if tokens[-1] in oper or tokens[-1] in (set(functions) | {"min", "max"}):
            attempts.append(lambda: calculate(tokens, "reverse"))
    for attempt in attempts:
        try:
            return attempt()
        except Exception:
            continue
    return calculate(to_reverse_polish(new_tokens), "reverse")


raw_str = input("Ввод:")
tokens = split_raw(raw_str)
new_tokens = remove_m(tokens)
if "".join(tokens) == "".join(raw_str.split()) and parser(new_tokens):
    try:
        func = check_notation(tokens, new_tokens)
        print("Результат:", func)
    except Exception as error:
        print(f"Ошибка: {error}")
else:
    print("Некорректный ввод")
