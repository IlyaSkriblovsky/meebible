<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xsl:stylesheet>
<xsl:stylesheet
    version="1.0"
    xmlns:h="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>
    <xsl:template match="h:html">
        <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
            </head>
            <body layout="indented">
                <xsl:apply-templates select=".//h:span[@class='versetext']"/>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="h:span[@class='versetext']">
        <div class="par">
            <div class="verse">
                <xsl:attribute name="verse">
                    <xsl:value-of select="substring-after(@id, '-')"/>
                </xsl:attribute>
                <div class="verse-label">
                    <xsl:value-of select="substring-after(@id, '-')"/>
                </div>
                <xsl:apply-templates/>
            </div>
        </div>
    </xsl:template>

    <xsl:template match="h:span[@class='versenum']"></xsl:template>
    <xsl:template match="*[@class='footnote']"></xsl:template>

    <xsl:template match="h:a">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="*">
        <zzz>
            <xsl:copy/>
        </zzz>
    </xsl:template>

</xsl:stylesheet>
