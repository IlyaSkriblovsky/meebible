$(function() {
    var slide_interval = 7000
    var timeout = null

    function slide() {
        var from_div = $('.screen.current')
        var to_div = $('.screen.current').next()
        if (to_div.size() == 0) to_div = $('.screen').first()

        var width = from_div.width()

        from_div.css('left', 0)
        to_div.css('left', width).addClass('visible')

        from_div.animate({left: -width}, {
            duration: 'normal',
            easing: 'swing',
            complete: function() {
                $(this).removeClass('visible current')
            }
        })
        to_div.animate({left: 0}, {
            duration: 'normal',
            easing: 'swing',
            complete: function() {
                $(this).addClass('current')
            }
        })
    }

    timeout = setInterval(slide, slide_interval)

    $('#screen-wrapper').mouseenter(function() {
        if (timeout) { clearInterval(timeout); timeout = null }
    })
    $('#screen-wrapper').mouseleave(function() {
        if (timeout) { clearInterval(timeout); timeout = null }
        timeout = setInterval(slide, slide_interval)
    })
})
