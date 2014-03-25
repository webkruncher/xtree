<?xml version="1.0" encoding="UTF-8" ?>

<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" encoding="UTF-8" indent="yes" />


<xsl:template match="key" >
	<div class="Key"><xsl:value-of select="." /></div>
</xsl:template>

<xsl:template match="Number" >
	<div style="display:none;" name="Number"><xsl:value-of select="." /></div>
</xsl:template>

<xsl:template match="Source" >
	<div class="source" >
		<xsl:if test="@file!='./tree.h'"><xsl:value-of select="@file" />:</xsl:if>
		<xsl:value-of select="@line" />;
		<xsl:if test="@clrs" ><xsl:value-of select="@clrs" />;</xsl:if>
		<xsl:value-of select="." />
	</div>
</xsl:template>

<xsl:template name="insrt" >
	<xsl:variable name="id"><xsl:value-of select="generate-id()" /></xsl:variable>
	<div class="nodeframe" >
		<xsl:attribute name="id"><xsl:value-of select="$id" /></xsl:attribute>
		<div class="MainAction" >Insert</div>
		<xsl:apply-templates />
		<script>
			setTimeout("LoadNumber('<xsl:value-of select='$id' />')",300)
		</script>
	</div>
</xsl:template>

<xsl:template name="erse" >
	<div class="nodeframe" >
		<div class="MainAction" >Erase</div>
		<xsl:apply-templates />
	</div>
</xsl:template>

<xsl:template match="Transplant" >
	<div class="nodeframe" >
		<div class="SubAction" >Transplant</div>
		<xsl:apply-templates />
	</div>
</xsl:template>

<xsl:template match="RotateLeft" >
	<div class="nodeframe" >
		<div class="SubAction" >RotateLeft</div>
		<xsl:apply-templates />
	</div>
</xsl:template>

<xsl:template match="RotateRight" >
	<div class="nodeframe" >
		<div class="SubAction" >RotateRight</div>
		<xsl:apply-templates />
	</div>
</xsl:template>

<xsl:template name="Item" >
		<xsl:variable name="bkg" >
			<xsl:if test='(position() mod 2) = 1'>#ababab;</xsl:if>
			<xsl:if test='(position() mod 2) = 0'>#cacaca;</xsl:if>
		</xsl:variable>
	<div>
		<xsl:attribute name="style">background:<xsl:value-of select="$bkg" /></xsl:attribute>
		<xsl:choose>
			<xsl:when test="@insrt='true'"><xsl:call-template name="insrt" /></xsl:when>
			<xsl:when test="@erse='true'"><xsl:call-template name="erse" /></xsl:when>
		</xsl:choose>
	</div>
</xsl:template>

<xsl:template match="/" >
	<html>
		<link rel="stylesheet" type="text/css" href="./trees.css" />
		<script src="./trees.js"></script>
		<xsl:for-each select="Tree/Item">
				<xsl:call-template name="Item" />
		</xsl:for-each>
	</html>
</xsl:template>

</xsl:stylesheet>
 
