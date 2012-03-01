<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xsl:stylesheet>
<xsl:stylesheet
    version="1.0"
    xmlns:h="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    exclude-result-prefixes="h"
>
    <xsl:template match="h:body">
        <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
            </head>
            <body>
                <xsl:apply-templates/>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="h:head"></xsl:template>

    <xsl:template match="h:div[@id='header']"></xsl:template>
    <xsl:template match="h:div[@id='large-footer']"></xsl:template>
    <xsl:template match="h:div[@id='small-footer']"></xsl:template>
    <xsl:template match="h:h1"></xsl:template>
    <xsl:template match="h:div[@class='chapter-actions']"></xsl:template>
    <xsl:template match="h:p[@class='chapter-copyright']"></xsl:template>

    <xsl:template match="h:div[@class='content']">
        <xsl:apply-templates select=".//h:span[@class='verse']|.//h:h2|.//h:p"/>
    </xsl:template>

    <xsl:template match="h:p">
        <p><xsl:apply-templates/></p>
    </xsl:template>

    <!--
    <xsl:template match="h:p">
        <p>
            <xsl:for-each select="following-sibling::h:span[generate-id(preceding-sibling::h:p[1]) = generate-id(current())]">
                <xsl:apply-templates/>
            </xsl:for-each>
        </p>
    </xsl:template>
    -->

    <xsl:template match="h:h2">
        <div class="heading">
            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="h:span[@class='verse']">
        <div class="verse">
            <xsl:attribute name="verse">
                <xsl:value-of select="h:strong/h:a" />
            </xsl:attribute>

            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="h:strong[@class='verseno']">
        <div class="verse-label">
            <xsl:value-of select="h:a" />
        </div>
    </xsl:template>

    <xsl:template match="h:span[@class='vsmallcaps']">
        <span class='smallcaps upper'>
            <xsl:apply-templates/>
        </span>
    </xsl:template>

    <xsl:template match="h:br"><br/></xsl:template>
    <xsl:template match="h:b"><b/></xsl:template>
    <xsl:template match="h:strong"><strong/></xsl:template>
    <xsl:template match="h:i"><i/></xsl:template>
</xsl:stylesheet>
