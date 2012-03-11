<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xsl:stylesheet>
<xsl:stylesheet
    version="1.0"
    xmlns:h="http://wwww.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    exclude-result-prefixes="h"
>

    <xsl:template match="html">
        <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
            </head>
            <body layout="indented">
                <xsl:apply-templates select=".//div[@id='version_primary']/div"/>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="div[@id='version_primary']/div">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="h2">
        <div class="heading">
            <xsl:value-of select="."/>
        </div>
    </xsl:template>

    <xsl:template match="p">
            <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="span[starts-with(@class, 'verse ')]">
        <div class="par">
            <div class="verse">
                <xsl:attribute name="verse">
                    <xsl:value-of select="substring-after(substring-after(@class, '_'), '_')"/>
                </xsl:attribute>
                <div class="verse-label">
                    <xsl:value-of select="substring-after(substring-after(@class, '_'), '_')"/>
                </div>
                <xsl:apply-templates/>
            </div>
        </div>
    </xsl:template>

    <xsl:template match="span[@class='vsmallcaps']">
        <span class="upper smallcaps">
            <xsl:apply-templates/>
        </span>
    </xsl:template>

    <xsl:template match="br">
        <br/>
    </xsl:template>

    <xsl:template match="hr">
        <hr/>
    </xsl:template>

    <xsl:template match="h1"></xsl:template>
    <xsl:template match="strong"></xsl:template>
    <xsl:template match="span[@class='trans']"></xsl:template>

    <xsl:template match="*">
        <zzz/>
    </xsl:template>

</xsl:stylesheet>
