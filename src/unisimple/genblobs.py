# -*- coding: utf-8 -*-

ONLY_16BIT = True

lines = [line.strip().split(';') for line in open('UnicodeData.txt').readlines() + open('Additional.txt').readlines()]

if ONLY_16BIT:
    lines = filter(lambda l: int(l[0], 16) <= 0xffff, lines)

by_code = dict((int(line[0], 16), line) for line in lines)

char_class = dict((c, l[2][0]) for c, l in by_code.items())


def decompose(code):
    try:
        line = by_code[code]
    except KeyError:
        # print 'ABSENT CHAR: {0}'.format('{0:04x}'.format(code))
        return [code]

    if line[13] != '': return decompose(int(line[13], 16))

    if line[5] == '': return [code]

    parts = line[5].split(' ')
    if parts[0][0] == '<': parts = parts[1:]

    if ONLY_16BIT:
        if len(filter(lambda p: int(p, 16) > 0xffff, parts)) > 0:
            return [code]

    import operator
    return reduce(operator.__add__, (
        decompose(int(c, 16)) for c in parts
    ))


def is_mark(code):
    try:
        return char_class[code] == 'M'
    except KeyError:
        return False


def remove_marks(codes):
    return filter(lambda c: not is_mark(c), codes)


replacements = dict(
    (c, u''.join(unichr(d) for d in remove_marks(decompose(c))))
    for c in by_code.keys()
)


with open('gen_CharClasses.inc.cpp', 'w') as f:
    prev_class = ''
    for c in sorted(char_class.keys()):
        cc = char_class[c]
        if cc != prev_class:
            f.write("{{ 0x{0:04x}, UCHAR_CLASS_{1} }},\n".format(c, cc))
            prev_class = cc



rtexts = set(replacements[c] for c in by_code.keys() if replacements[c] != unichr(c))
print sum(len(rt) for rt in rtexts)
rtbylen = {}
for rt in rtexts:
    try:
        rtbylen[len(rt)].append(rt)
    except KeyError:
        rtbylen[len(rt)] = [rt]


blob = u''
for l in reversed(sorted(rtbylen.keys())):
    for rt in rtbylen[l]:
        if blob.find(rt) == -1:
            blob += rt
print len(blob)

with open('gen_Simplify_blob.inc.cpp', 'w') as f:
    partlen = 10
    s = 0
    while s < len(blob):
        part = blob[s:s+partlen]
        s += partlen
        f.write('{0},\n'.format(
            ', '.join(
                '0x{0:04x}'.format(ord(c))
                for c in part
            )
        ))


with open('gen_SimplifyDesc.inc.cpp', 'w') as f:
    for c in sorted(by_code.keys()):
        r = replacements[c]
        if len(r) == 0 or r == unichr(c): continue

        blob_start = blob.find(r)
        if blob_start == -1: raise Exception('??')

        f.write(u'{{ 0x{0:04x}, {1}, {2} }}, // {3:04X} -> {4}\n'.format(
            c, blob_start, len(r), c, u' '.join('{0:04X}'.format(ord(rc)) for rc in r)
        ).encode('utf-8'))


# text = u'Když přišel, Marosvásárhely. Ёлки зелёные. У Ови́да родился Иессе́й. ẞßÆæĲ ǖ ÆæÐđØøÞþßæĐðøþŁł'
# print text.encode('utf-8')
# print u''.join(u''.join(unichr(d) for d in remove_marks(decompose(ord(c)))) for c in text).encode('utf-8')
