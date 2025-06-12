document.body.onclick = function() {
    var target = event.target

    while (target  &&  ! isVerseDiv(target))
        target = target.parentNode

    if (! target)
        return

    var verse = parseInt(target.getAttribute('verse'))

    toggleVerse(verse)
}


var selectedVerses = {}

function isVerseDiv(div)
{
    return div.hasOwnProperty('className') && div.className.split(' ').indexOf('verse') != -1
}

function selectVerse(verse, dontNotify)
{
    selectedVerses[verse] = true

    var divs = verseDivs(verse)
    for (var i = 0; i < divs.length; i++)
        divs[i].className = 'verse selected'

    if (! dontNotify)
        bibleView.verseSelectionChanged(selectedVersesList())
}

function unselectVerse(verse, dontNotify)
{
    selectedVerses[verse] = false

    var divs = verseDivs(verse)
    for (var i = 0; i < divs.length; i++)
        divs[i].className = 'verse'

    if (! dontNotify)
        bibleView.verseSelectionChanged(selectedVersesList())
}

function verseSelected(verse)
{
    return selectedVerses[verse] == true
}

function toggleVerse(verse)
{
    if (verseSelected(verse))
        unselectVerse(verse)
    else
        selectVerse(verse)
}

function clearSelection(dontNotify)
{
    for (var verse in selectedVerses)
        if (selectedVerses[verse])
            unselectVerse(verse, true)

    if (! dontNotify)
        bibleView.verseSelectionChanged(selectedVersesList())
}

function selectVerses(list)
{
    clearSelection(true)

    for (var i in list)
        selectVerse(list[i], true)

    bibleView.verseSelectionChanged(selectedVersesList())
}

function selectedVersesList()
{
    var result = []
    for (var verse in selectedVerses)
        if (selectedVerses[verse])
            result.push(parseInt(verse))
    return result
}




function verseDivs(verse)
{
    var result = []
    var allDivs = document.getElementsByTagName('div')
    for (var i = 0; i < allDivs.length; i++)
        if (allDivs[i].getAttribute('verse') == verse)
            result.push(allDivs[i])
    return result
}


var digitUp = {'0':'⁰','1':'¹','2':'²','3':'³','4':'⁴','5':'⁵','6':'⁶','7':'⁷','8':'⁸','9':'⁹'}
function numberSup(str) {
    var out = ""
    for (var i = 0; i < str.length; i++)
        out += digitUp[str[i]] ? digitUp[str[i]] : str[i]
    return out
}


function selectedText(withVerseNumbers)
{
    var result = ''

    var selected = []
    for (var verse in selectedVerses)
        if (selectedVerses[verse])
            selected.push(verse)

    selected.sort()

    var prev = null

    for (var j = 0; j < selected.length; j++)
    {
        var verse = parseInt(selected[j])

        if (prev != null  &&  verse != prev + 1)
            result += ' [...]'

        var divs = verseDivs(verse)
        for (var i = 0; i < divs.length; i++)
        {
            result += ' '

            var divtext = divs[i].innerText

            if (divs[i].firstElementChild && divs[i].firstElementChild.className == "verse-label")
            {
                var withoutVerseNumber = divtext.substr(divs[i].firstElementChild.innerText.length)
                if (withVerseNumbers)
                    result += /*numberSup*/(divs[i].firstElementChild.innerText) + ' ' + withoutVerseNumber
                else
                    result += withoutVerseNumber
            }
            else
                result += divtext
        }

        prev = verse
    }

    return result.trim()
}



function verseOffset(verse)
{
    var divs = verseDivs(verse)
    return divs[0].offsetTop - 20
}


/*
var currentMatchIndex = 0
function highlightMatch(matchIndex)
{
    var span

    span = document.getElementById('match' + currentMatchIndex)
    if (span) span.className = "match"

    currentMatchIndex = matchIndex

    span = document.getElementById('match' + currentMatchIndex)
    if (span)
    {
        span.className = "match current"
        return span.offsetTop
    }
    else
        return 0;
}
*/

var currentMatch = null
function highlightMatch(matchIndex)
{
    if (currentMatch)
        currentMatch.className = 'match'
    currentMatch = null

    var spans = document.getElementsByTagName('span')
    var count = 0
    for (var i = 0; i < spans.length; i++)
    {
        if (spans[i].className == 'match')
        {
            if (count == matchIndex)
            {
                currentMatch = spans[i];
                break;
            }
            count++
        }
    }

    if (currentMatch)
    {
        currentMatch.className = 'match current'
        return currentMatch.offsetTop
    }
    else
        return 0
}



function hideAllHighlights()
{
    document.body.className = "hide-matches"
}



function setFontSize(factor)
{
    document.body.style.fontSize = factor + 'px'
}

function setFontName(name)
{
    document.body.style.fontFamily = name
}

function setLineSpacing(spacing, preserveScroll)
{
    document.body.style.lineHeight = spacing
}

function setInverted(inverted)
{
    document.body.setAttribute('inverted', inverted ? 'true' : 'false')
}
