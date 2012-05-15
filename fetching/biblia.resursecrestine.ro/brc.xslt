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
                <xsl:apply-templates select=".//h:ul[@id='verslist']"/>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="h:ul[@id='verslist']">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="h:li[@class='versetTitlu']">
        <div class="heading">
            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="h:li[@class='versetWrapper']">
        <div class="par">
            <div class="verse">
                <xsl:attribute name="verse">
                    <xsl:value-of select="h:span/h:span[@class='numar-verset']"/>
                </xsl:attribute>
                <div class="verse-label">
                    <xsl:value-of select="h:span/h:span[@class='numar-verset']"/>
                </div>
                <xsl:apply-templates select="h:span/h:span[@class='continut-verset']"/>
            </div>
        </div>
    </xsl:template>

    <xsl:template match="h:span[@class='continut-verset']">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="h:li[@class='trimiteriText']"/>
    <xsl:template match="h:li[@id='verset-options']"/>

    <xsl:template match="*">
        <zzz>
            <xsl:copy/>
        </zzz>
    </xsl:template>
</xsl:stylesheet>
