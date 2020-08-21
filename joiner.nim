# Just a very simple string joiner, im sure there are better ways

type
    StringJoiner* = ref object
        value: string
        delimiter*: string

proc append*(sj: StringJoiner, str: string) =
    if len(sj.value) == 0 :
        sj.value = str
    else:
        sj.value = sj.value & sj.delimiter & str

proc value*(sj: StringJoiner): string =
    return sj.value