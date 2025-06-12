.pragma library

var groups = [
    {
        color: "#111119",
        books: ["ge", "ex", "le", "nu", "de"]
    },
    {
        color: "#111911",
        books: ["jos", "jg", "ru", "1sa", "2sa", "1ki", "2ki", "1ch", "2ch", "ezr", "ne", "es"]
    },
    {
        color: "#191111",
        books: ["job", "ps", "pr", "ec", "ca"],
    },
    {
        color: "#191119",
        books: ["isa", "jer", "la", "eze", "da"],
    },
    {
        color: "#111919",
        books: ["ho", "joe", "am", "ob", "jon", "mic", "na", "hab", "zep", "hag", "zec", "mal"]
    },
    {
        color: "#191111",
        books: ["mt", "mr", "lu", "joh"],
    },
    {
        color: "#111911",
        books: ["ac"],
    },
    {
        color: "#111119",
        books: ["ro", "1co", "2co", "ga", "eph", "php", "col", "1th", "2th", "1ti", "2ti", "tit", "phm", ],
    },
    {
        color: "#111919",
        books: ["heb", "jas", "1pe", "2pe", "1jo", "2jo", "3jo", "jude"]
    },
    {
        color: "#191119",
        books: ["re"]
    }
]

function bookColor(bookCode)
{
    for (var i = 0; i < groups.length; i++)
        if (groups[i].books.indexOf(bookCode) != -1)
            return groups[i].color

    return "transparent"
}
