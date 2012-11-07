#!/usr/bin/python

from xml.dom import minidom
import simplejson
import sys
import argparse


argparser = argparse.ArgumentParser(description = "Converts translations from JSON format to Qt's *.ts files")
argparser.add_argument("json_filename", metavar = "JSON-file", help = "Input JSON file")
argparser.add_argument("out_basename", metavar = "out-basename", help = "Output file prefix")
argparser.add_argument("-m", "--show-missing", action = "store_const", const = True, default = False, help = "Show missing messages")
argparser.add_argument("-l", "--orig-language", metavar = "orig_language", help = "Treat original strings as translation to that language")

args = argparser.parse_args()


data = simplejson.loads(open(args.json_filename).read())

langs = set()

for context in data:
    for message in data[context]:
        for lang in message:
            langs.add(lang)

langs.remove('__')

if args.orig_language:
    langs.add(args.orig_language)


print('Found languages: {0}'.format(', '.join(langs)))

docs = {}
for lang in langs:
    docs[lang] = doc = minidom.Document()
    ts = doc.createElement('TS')
    ts.setAttribute('version', '2.0')
    ts.setAttribute('language', lang)
    doc.appendChild(ts)


counters = dict((lang, 0) for lang in langs)
counters['__'] = 0

missing = dict((lang, []) for lang in langs)


for context in data:
    context_tags = {}
    for lang in langs:
        ctx = docs[lang].createElement('context')
        name = docs[lang].createElement('name')
        name.appendChild(docs[lang].createTextNode(context))
        ctx.appendChild(name)
        docs[lang].firstChild.appendChild(ctx)
        context_tags[lang] = ctx

    for message in data[context]:
        source_text = message['__']

        if args.orig_language:
            message[args.orig_language] = message['__']

        for lang in message:
            counters[lang] += 1

            if lang == '__': continue

            doc = docs[lang]
            msg = doc.createElement('message')
            source = doc.createElement('source')
            source.appendChild(doc.createTextNode(source_text))
            trans = doc.createElement('translation')
            trans.appendChild(doc.createTextNode(message[lang]))
            msg.appendChild(source)
            msg.appendChild(trans)
            context_tags[lang].appendChild(msg)

        for lang in langs:
            if lang not in message:
                missing[lang].append(source_text)



print('Total messages: {0}'.format(counters['__']))


for lang in langs:
    if missing[lang]:
        if args.show_missing:
            print('MISSING for {0}:'.format(lang))
            for text in missing[lang]:
                print(u'\t{0}'.format(text))
        else:
            print('MISSING: {0} messages for {1}'.format(len(missing[lang]), lang))


for lang in langs:
    with open('{0}_{1}.ts'.format(args.out_basename, lang), 'w') as f:
        f.write(docs[lang].toprettyxml().encode('utf-8'))
