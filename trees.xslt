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
	<xsl:param name="parentname" />
	<xsl:param name="ndx" />
	<xsl:variable name="myname" ><xsl:value-of select="text()" /></xsl:variable>
		<xsl:variable name="bkg" >
			<xsl:if test='($ndx mod 2) = 1'>#bbbbaa;</xsl:if>
			<xsl:if test='($ndx mod 2) = 0'>#bbbbcc;</xsl:if>
		</xsl:variable>
	<div class="SubNest">
		<xsl:attribute name="style">background:<xsl:value-of select="$bkg" /></xsl:attribute>
		<table cellspacing="0" cellpadding="0" > 
				<xsl:if test="$ndx" ><tr><td><div class="Step">Step <xsl:value-of select="$ndx" />)</div></td></tr></xsl:if>
				<tr cellspacing="0" cellpadding="0" >
					<td colspan="2" border="0" cellspacing="0" cellpadding="0" >
						<div>
							<xsl:attribute name="style">display:inline;margin:10px;padding:px;font-weight:bold;font-size:10px;vertical-align:top;color:gold;background:<xsl:value-of select="@color" /></xsl:attribute>
							<xsl:value-of select="text()" />
						</div>
					</td>
				</tr>
				<tr> 
					<td width="50%" >
						<xsl:if test="not (LeftTree)">NIL</xsl:if>
						<xsl:for-each select="LeftTree" >
								<xsl:call-template name="subnest" >
										<xsl:with-param name="parentname"><xsl:value-of select="$myname" /></xsl:with-param>
								</xsl:call-template>
						</xsl:for-each>
					</td>
					<td width="50%" >
						<xsl:if test="not (RightTree)">NIL</xsl:if>
						<xsl:for-each select="RightTree" >
								<xsl:call-template name="subnest" >
										<xsl:with-param name="parentname"><xsl:value-of select="$myname" /></xsl:with-param>
								</xsl:call-template>
						</xsl:for-each>
					</td>
				</tr>
		</table>
	</div>
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

<xsl:template name="Source" >
	<xsl:param name="parentpos" />
		<xsl:variable name="bkg" >
			<xsl:if test='($parentpos mod 2) = 1'>#ababab;</xsl:if>
			<xsl:if test='($parentpos mod 2) = 0'>#cacaca;</xsl:if>
		</xsl:variable>
		<table class="source" width="100%">
			<xsl:attribute name="style">background:<xsl:value-of select="$bkg" /></xsl:attribute>
			<tr>
				<td class="sourceline" width="10px"><xsl:value-of select="$parentpos" /></td>
				<td class="sourceline" width="80px"><xsl:value-of select="@file" /></td>
				<td class="sourceline" width="20px"><xsl:value-of select="@line" /></td>
				<td class="sourceline" width="20px"><xsl:value-of select="@clrs" /></td>
				<td class="sourceline" width="500px"><xsl:value-of select="." /></td>
			</tr>
		</table>
</xsl:template>

<xsl:template name="insrt" >
	<xsl:variable name="id">insrt_<xsl:value-of select="generate-id()" /></xsl:variable>
	<div class="nodeframe" >
		<xsl:attribute name="id"><xsl:value-of select="$id" /></xsl:attribute>
		<div class="MainAction" >Insert <xsl:value-of select="key" /></div>
		<xsl:for-each select="subtree">
				<xsl:call-template name="subnest" ><xsl:with-param name="ndx"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template>
		</xsl:for-each>
		<xsl:for-each select="Source"><xsl:call-template name="Source" ><xsl:with-param name="parentpos"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template></xsl:for-each>
		<xsl:for-each select="subtree"><xsl:call-template name="subtree" /></xsl:for-each>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
		<script>
			setTimeout("LoadNumber('<xsl:value-of select='$id' />')",300)
		</script>
	</div>
</xsl:template>


<xsl:template name="done" >
	<xsl:variable name="id">done_<xsl:value-of select="generate-id()" /></xsl:variable>
	<div class="doneframe" >
		<xsl:for-each select="subtree">
				<xsl:call-template name="subnest" ><xsl:with-param name="ndx"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template>
		</xsl:for-each>
		<xsl:attribute name="id"><xsl:value-of select="$id" /></xsl:attribute>
		<xsl:for-each select="Source"><xsl:call-template name="Source" ><xsl:with-param name="parentpos"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template></xsl:for-each>
		<xsl:for-each select="subtree"><xsl:call-template name="subtree" /></xsl:for-each>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
	</div>
</xsl:template>

<xsl:template name="erse" >
	<div class="nodeframe" >
		<div class="MainAction" >Erase <xsl:value-of select="key" /></div>
		<xsl:for-each select="subtree">
				<xsl:call-template name="subnest" ><xsl:with-param name="ndx"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template>
		</xsl:for-each>
		<xsl:for-each select="Source"><xsl:call-template name="Source" ><xsl:with-param name="parentpos"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template></xsl:for-each>
		<xsl:for-each select="subtree"><xsl:call-template name="subtree" /></xsl:for-each>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
	</div>
</xsl:template>

<xsl:template name="Transplant" >
	<div class="nodeframe" >
		<div class="SubAction" >Transplant</div>
		<xsl:for-each select="subtree">
				<xsl:call-template name="subnest" ><xsl:with-param name="ndx"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template>
		</xsl:for-each>
		<xsl:for-each select="Source"><xsl:call-template name="Source" ><xsl:with-param name="parentpos"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template></xsl:for-each>
		<xsl:for-each select="subtree"><xsl:call-template name="subtree" /></xsl:for-each>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
	</div>
</xsl:template>

<xsl:template name="RotateLeft" >
	<div class="nodeframe" >
		<div class="SubAction" >RotateLeft</div>
		<xsl:for-each select="subtree">
				<xsl:call-template name="subnest" ><xsl:with-param name="ndx"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template>
		</xsl:for-each>
		<xsl:for-each select="Source"><xsl:call-template name="Source" ><xsl:with-param name="parentpos"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template></xsl:for-each>
		<xsl:for-each select="subtree"><xsl:call-template name="subtree" /></xsl:for-each>
		<xsl:for-each select="Item"><xsl:call-template name="Item" /></xsl:for-each>
	</div>
</xsl:template>

<xsl:template name="RotateRight" >
	<div class="nodeframe" >
		<div class="SubAction" >RotateRight</div>
		<xsl:for-each select="subtree">
				<xsl:call-template name="subnest" ><xsl:with-param name="ndx"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template>
		</xsl:for-each>
		<xsl:for-each select="Source"><xsl:call-template name="Source" ><xsl:with-param name="parentpos"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template></xsl:for-each>
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
		<xsl:attribute name="style">background:<xsl:value-of select="$bkg" /></xsl:attribute>
		<xsl:choose>
			<xsl:when test="name()='subtree'"><xsl:call-template name="subtree" /></xsl:when>
			<xsl:when test="@insrt='true'"><xsl:call-template name="insrt" /></xsl:when>
			<xsl:when test="@done='true'"><xsl:call-template name="done" /></xsl:when>
			<xsl:when test="@erse='true'"><xsl:call-template name="erse" /></xsl:when>
		</xsl:choose>
		<xsl:for-each select="Source"><xsl:call-template name="Source" ><xsl:with-param name="parentpos"><xsl:value-of select="position()" /></xsl:with-param></xsl:call-template></xsl:for-each>
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
 
