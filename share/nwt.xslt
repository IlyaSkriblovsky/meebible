<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xsl:stylesheet>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:template match="head">
    </xsl:template>

    <xsl:template match="body">
        <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
            </head>
            <body>
                <xsl:apply-templates />
            </body>
        </html>
    </xsl:template>

    <xsl:template match="p[@class='site']">
    </xsl:template>

    <xsl:template match="div[@class='topnavigation']">
    </xsl:template>

    <xsl:template match="div[@class='title']">
    </xsl:template>

    <xsl:template match="div[@class='footer']">
    </xsl:template>


    <xsl:template match="h3">
    <!--
        <h3><xsl:value-of select="."/></h3>
    -->
    </xsl:template>

    <xsl:template match="p">
        <div class="par">
            <xsl:apply-templates />
        </div>
    </xsl:template>

    <xsl:template match="span[contains(@id, '-')]">
        <div class="verse">
            <xsl:attribute name="verse"><xsl:value-of select="substring-after(substring-before(@id, '-'), 'vs')"/></xsl:attribute>
            <xsl:attribute name="part"><xsl:value-of select="substring-after(@id, '-')"/></xsl:attribute>

            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="span[contains(@id, 'vs') and not(contains(@id, '-'))]">
        <div class="verse">
            <xsl:attribute name="verse">
                <xsl:value-of select="substring-after(@id, 'vs')"/>
            </xsl:attribute>
            <div class="verse-label"><xsl:value-of select="substring-after(@id, 'vs')"/></div>

            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="a[@class='vsAnchor']">
    </xsl:template>

    <xsl:template match="i">
        <i><xsl:apply-templates/></i>
    </xsl:template>

    <xsl:template match="span[@class='small']">
        <span class="smallcaps"><xsl:apply-templates/></span>
    </xsl:template>

    <!-- Second level poetry lines -->
    <xsl:template match="span[@class='z']">
        <span class="z"><xsl:apply-templates/></span>
    </xsl:template>


    <!-- Poetry superscripts -->
    <xsl:template match="p[@class='w']">
        <div class="superscript"><xsl:apply-templates/></div>
    </xsl:template>
</xsl:stylesheet>
