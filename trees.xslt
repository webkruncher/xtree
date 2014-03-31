<?xml version="1.0" encoding="UTF-8" ?>

<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" version="5.0" encoding="UTF-8" indent="yes" />


<xsl:template match="key" >
	<div class="Key"><xsl:value-of select="." /></div>
</xsl:template>

<xsl:template name="subnode" >
		<div style="display:none" > 
			<xsl:attribute name="tag"><xsl:value-of select="name()" /></xsl:attribute>
			<xsl:attribute name="value"><xsl:value-of select="text()" /></xsl:attribute>
			<xsl:attribute name="color"><xsl:value-of select="@color" /></xsl:attribute>
			<xsl:attribute name="depth"><xsl:value-of select="@depth" /></xsl:attribute>
				<xsl:value-of select="name()" />:<xsl:value-of select="text()" />
		</div>
</xsl:template>

<xsl:template name="subnest" >
		<table width="100%;border:1px;">
				<tr>
					<td colspan="2">
						<xsl:attribute name="style">font-weight:bold;font-size:10px;vertical-align:top;align:center;width:140px;color:#aaaaaa;background:<xsl:value-of select="@color" /></xsl:attribute><xsl:value-of select="text()" />
					</td>
				</tr>
				<tr>
					<td>
						<xsl:attribute name="style">font-size:8px;vertical-align:top;align:center;width:140px;color:black;background:#777777;</xsl:attribute>
						<xsl:if test="not (LeftTree)">NIL</xsl:if>
						<xsl:for-each select="LeftTree" ><xsl:value-of select="text()" /><xsl:call-template name="subnest" /></xsl:for-each>
					</td>
					<td>
						<xsl:attribute name="style">font-size:8px;vertical-align:top;align:center;width:140px;color:black;background:#777777;</xsl:attribute>
						<xsl:if test="not (RightTree)">NIL</xsl:if>
						<xsl:for-each select="RightTree" ><xsl:value-of select="text()" /><xsl:call-template name="subnest" /></xsl:for-each>
					</td>
				</tr>
		</table>
</xsl:template>

<xsl:template name="up"><xsl:value-of select="name()" />_<xsl:for-each select="*"><xsl:call-template name="up" /></xsl:for-each></xsl:template>

<xsl:template name="subtree" >
		<xsl:variable name="id"><xsl:call-template name="up" /><xsl:value-of select="generate-id()" /></xsl:variable>
		<div style="display:none;font-size:0.7em;"> 
			<xsl:attribute name="id">v_<xsl:value-of select="$id" /></xsl:attribute>
			<xsl:attribute name="value"><xsl:value-of select="text()" /></xsl:attribute>
			<xsl:attribute name="color"><xsl:value-of select="@color" /></xsl:attribute>
			<xsl:attribute name="depth"><xsl:value-of select="@depth" /></xsl:attribute>
			<xsl:for-each select="*" ><xsl:call-template name="subnode" /></xsl:for-each>
		</div>
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
	<xsl:variable name="id">insrt_<xsl:value-of select="generate-id()" /></xsl:variable>
	<div class="nodeframe" >
		<xsl:attribute name="id"><xsl:value-of select="$id" /></xsl:attribute>
		<div class="MainAction" >Insert <xsl:value-of select="key" /></div>
		<xsl:for-each select="subtree">
			<div>
				Step:<xsl:value-of select="position()" /><br />
				<xsl:call-template name="subnest" />
			</div>
		</xsl:for-each>
		<xsl:for-each select="subtree"><xsl:call-template name="subtree" /></xsl:for-each>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
		<script>
			setTimeout("LoadNumber('<xsl:value-of select='$id' />')",300)
		</script>
	</div>
</xsl:template>

<xsl:template name="done" >
	<xsl:variable name="id">done_<xsl:value-of select="generate-id()" /></xsl:variable>
	<div class="nodeframe" >
		<div class="MainAction" >Done <xsl:value-of select="key" /></div>
		<xsl:for-each select="subtree">
			<div>
				Step:<xsl:value-of select="position()" /><br />
				<xsl:call-template name="subnest" />
			</div>
		</xsl:for-each>
		<xsl:attribute name="id"><xsl:value-of select="$id" /></xsl:attribute>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
	</div>
</xsl:template>

<xsl:template name="erse" >
	<div class="nodeframe" >
		<div class="MainAction" >Erase <xsl:value-of select="key" /></div>
		<xsl:for-each select="subtree">
			<div>
				Step:<xsl:value-of select="position()" /><br />
				<xsl:call-template name="subnest" />
			</div>
		</xsl:for-each>
	</div>
</xsl:template>

<xsl:template name="Transplant" >
	<div class="nodeframe" >
		<div class="SubAction" >Transplant</div>
		<xsl:for-each select="subtree">
			<div>
				Step:<xsl:value-of select="position()" /><br />
				<xsl:call-template name="subnest" />
			</div>
		</xsl:for-each>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
	</div>
</xsl:template>

<xsl:template name="RotateLeft" >
	<div class="nodeframe" >
		<div class="SubAction" >RotateLeft</div>
		<xsl:for-each select="subtree">
			<div>
				Step:<xsl:value-of select="position()" /><br />
				<xsl:call-template name="subnest" />
			</div>
		</xsl:for-each>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
	</div>
</xsl:template>

<xsl:template name="RotateRight" >
	<div class="nodeframe" >
		<div class="SubAction" >RotateRight</div>
		<xsl:for-each select="subtree">
			<div>
				Step:<xsl:value-of select="position()" /><br />
				<xsl:call-template name="subnest" />
			</div>
		</xsl:for-each>
		<xsl:for-each select="subtree"><xsl:call-template name="subtree" /></xsl:for-each>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
	</div>
</xsl:template>

<xsl:template name="Item" >
		<xsl:variable name="bkg" >
			<xsl:if test='(position() mod 2) = 1'>#ababab;</xsl:if>
			<xsl:if test='(position() mod 2) = 0'>#cacaca;</xsl:if>
		</xsl:variable>
	<div>
		<xsl:choose>
			<xsl:when test="name()='subtree'"><xsl:call-template name="subtree" /></xsl:when>
			<xsl:when test="@insrt='true'"><xsl:call-template name="insrt" /></xsl:when>
			<xsl:when test="@done='true'"><xsl:call-template name="done" /></xsl:when>
			<xsl:when test="@erse='true'"><xsl:call-template name="erse" /></xsl:when>
		</xsl:choose>
		<xsl:for-each select="RotateLeft"><xsl:call-template name="RotateLeft" /></xsl:for-each>
		<xsl:for-each select="RotateRight"><xsl:call-template name="RotateRight" /></xsl:for-each>
		<xsl:for-each select="Transplant"><xsl:call-template name="Transplant" /></xsl:for-each>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
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
 
