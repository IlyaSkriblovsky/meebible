# -*- coding: utf-8 -*-

lines = [line.strip().split(';') for line in open('UnicodeData.txt').readlines()]

by_code = dict((int(line[0], 16), line) for line in lines)


def decompose(code):
    try:
        line = by_code[code]
    except KeyError:
        print 'ABSENT CHAR: {0}'.format('{0:04x}'.format(code))
        return [code]

    if line[5] == '': return [code]

    parts = line[5].split(' ')
    if parts[0][0] == '<': parts = parts[1:]

    import operator
    return reduce(operator.__add__, (
        decompose(int(c, 16)) for c in parts
    ))


def is_mark(code):
    try:
        line = by_code[code]
    except KeyError:
        return False
    return line[2][0] == 'M'


def remove_marks(codes):
    return filter(lambda c: not is_mark(c), codes)


# for line in lines:
#     print '{0:06x} {1} {2}'.format(
#         int(line[0], 16),
#         line[2][0],
#         ' '.join('{0:04x}'.format(d) for d in decompose(int(line[0],16)))
#     )

# 00FC 0304
# 0076 0308 0304

text = u'Když přišel, začal vyptávat svých bratrů. Marosvásárhely. Ёлки зелёные. У Ови́да родился Иессе́й. ßæĲ ǖ'

print text.encode('utf-8')
print u''.join(u''.join(unichr(d) for d in remove_marks(decompose(ord(c)))) for c in text).encode('utf-8')


count = 0
by_type = {}
for code in by_code.keys():
    d = decompose(code)
    if len(d) != 1 or d[0] != code:
        count += 1
        cat = by_code[code][2]
        try:
            by_type[cat] += 1
        except KeyError: by_type[cat] = 1

print count
print by_type
