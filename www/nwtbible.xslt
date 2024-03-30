<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xsl:stylesheet>
<xsl:stylesheet
    version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>
    <xsl:template match="html">
        <html>
            <xsl:apply-templates/>
        </html>
    </xsl:template>

    <xsl:template match="head">
        <head>
            <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>

            <script src="script.js"></script>

            <style>
                .par {
                    margin-bottom: 10px;
                }

                .verse {
                    display: inline;
                }

                .z {
                    margin-left: 22px;
                }

                .superscript {
                    margin-top:    1em;
                    margin-bottom: 1em;
                    text-align: center;
                    font-size: 90%;
                    font-style: italic;
                }

            </style>
        </head>
    </xsl:template>

    <xsl:template match="body">
        <body>
            <xsl:apply-templates/>
        </body>
    </xsl:template>

    <xsl:template match="div[@class='par']">
        <div class="par">
            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="div[@class='verse']">
        <div class="verse">
            <xsl:attribute name="id">vs<xsl:value-of select="@verse"/></xsl:attribute>
            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="div[@class='verse-label']">
        <a class="vsAnchor">
            <xsl:attribute name="name">bk<xsl:value-of select="."/></xsl:attribute>
            <xsl:value-of select="."/>
        </a>
        <xsl:text> </xsl:text>
    </xsl:template>

    <xsl:template match="span[@class='smallcaps']">
        <span class="small">
            <xsl:apply-templates/>
        </span>
    </xsl:template>

    <xsl:template match="br"><br/></xsl:template>

    <xsl:template match="em"><em><xsl:apply-templates/></em></xsl:template>
    <xsl:template match="i"><i><xsl:apply-templates/></i></xsl:template>
    <xsl:template match="b"><b><xsl:apply-templates/></b></xsl:template>

    <xsl:template match="div[@class='superscript']">
        <div class="superscript">
            <xsl:apply-templates/>
        </div>
    </xsl:template>
</xsl:stylesheet>
