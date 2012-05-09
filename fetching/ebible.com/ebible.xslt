<?xml version="1.0"?>
<!DOCTYPE xsl:stylesheet>
<xsl:stylesheet
    version="1.0"
    xmlns:h="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    exclude-result-prefixes="h"
>

    <xsl:strip-space elements="*"/>

    <xsl:template match="h:html">
        <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
            </head>
            <body>
                <xsl:apply-templates select=".//h:div[@id='reading']"/>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="h:div[@id='reading']">
        <xsl:apply-templates match="."/>
    </xsl:template>


    <xsl:template match="h:p">
        <div class="par">
            <xsl:apply-templates />
        </div>
    </xsl:template>

    <xsl:template match="h:h1"/>

    <xsl:template match="h:h2">
        <div class="heading">
            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="h:h2/h:a">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="h:sup"/>

    <xsl:template match="h:span[@class='verse ']">
        <div class="verse">
            <xsl:attribute name="verse">
                <xsl:value-of select="preceding-sibling::h:sup[1]/text()"/>
            </xsl:attribute>
            <div class="verse-label">
                <xsl:value-of select="preceding-sibling::h:sup[1]/text()"/>
            </div>
            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="*">
        <zzz>
            <xsl:copy/>
        </zzz>
    </xsl:template>

</xsl:stylesheet>
