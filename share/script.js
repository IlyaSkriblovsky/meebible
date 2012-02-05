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

function selectVerse(verse)
{
    selectedVerses[verse] = true

    var divs = verseDivs(verse)
    for (var i = 0; i < divs.length; i++)
        divs[i].className = 'verse selected'
}

function unselectVerse(verse)
{
    selectedVerses[verse] = false

    var divs = verseDivs(verse)
    for (var i = 0; i < divs.length; i++)
        divs[i].className = 'verse'
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

function verseDivs(verse)
{
    var result = []
    var allDivs = document.getElementsByTagName('div')
    for (var i = 0; i < allDivs.length; i++)
        if (allDivs[i].getAttribute('verse') == verse)
            result.push(allDivs[i])
    return result
}

function selectedVersesList()
{
    var result = []
    for (var verse in selectedVerses)
        if (selectedVerses[verse])
            result.push(verse)
    return result.join(',')
}

function selectedText()
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

        var divs = verseDivs(verse)
        var text = ''
        for (var i = 0; i < divs.length; i++)
            text += divs[i].innerText

        if (prev != null  &&  verse != prev + 1)
            result += ' [...] '

        result += text.replace(/^\d+/, '$& ') + ' '

        prev = verse
    }

    return result
}

function clearSelection()
{
    for (var verse in selectedVerses)
        if (selectedVerses[verse])
            unselectVerse(verse)
}



function verseOffset(verse)
{
    var divs = verseDivs(verse)
    return divs[0].offsetTop - 20
}


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

function hideAllHighlights()
{
    document.body.className += " hide-matches"
}



function setFontSize(factor)
{
    document.body.style.fontSize = factor + 'px'
}

function setLineSpacing(spacing, preserveScroll)
{
    document.body.style.lineHeight = spacing
}

function setInverted(inverted)
{
    document.body.className = inverted ? 'inverted' : ''
}
