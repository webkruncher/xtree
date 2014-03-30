
		function innerGetObject(name)
		{
			if (document.layers){return document.layers[name];}
			else if (document.all){return document.all[name];}
			else if (document.getElementById){return document.getElementById(name);}
		}

		function GetObject(name)
		{
			o=innerGetObject(name)
			return o
		}

		function FindChildByName(parent,childname)
		{
			for (this.i=1;this.i<parent.childNodes.length; this.i++)
			{
				try
				{
					if (parent.childNodes[this.i].getAttribute("name")==childname) return parent.childNodes[this.i]
				} catch(e) {}
			}	
			return null
		}

		function FindChildByTag(parent,tagname,childname)
		{
			for (this.i=1;this.i<parent.childNodes.length; this.i++)
			{
				try
				{
					if (parent.childNodes[this.i])
					{
						if (parent.childNodes[this.i].getAttribute(tagname))
						{
							if (parent.childNodes[this.i].getAttribute(tagname)==childname) return parent.childNodes[this.i]
						}
					}
				} catch(e) {}
			}	
			return null
		}


		function LoadNumber(what)
		{
			this.numid=what+"_"+"number"
			this.style="background:blue;color:gray;display:inline;"
			this.obj=GetObject(what)
			this.Number=FindChildByName(this.obj,"Number");

			this.txt=document.createElement("div") 
			this.m=document.createTextNode(this.Number.innerHTML)
			this.txt.appendChild(this.m)
			this.txt.className="Number"
			this.obj.firstChild.appendChild(this.txt)
		}

		function AppendSubTree(sctx,node,x,y,w,h,d)
		{
			this.txt=node.getAttribute("value")
			this.depth=node.getAttribute("depth")
			if (node.getAttribute("color"))
				sctx.fillStyle=node.getAttribute("color")
			else
				sctx.fillStyle="green"
			sctx.fillText(this.txt+" ("+this.depth+")",x,y);
			this.left=FindChildByTag(node,"tag","LeftTree")
			this.L=x/d;
			this.R=(x+w)/2;
			this.Y=y+20;
			if (this.left) AppendSubTree(sctx,this.left,this.L,this.Y,w/2,h-20,d+1);
			this.right=FindChildByTag(node,"tag","RightTree")
			if (this.right) AppendSubTree(sctx,this.right,this.R,this.Y,w/2,h-20,d+1);
		}

		function LoadSubTree(id,x,y,w,h)
		{
//alert("Loading:"+id)
			this.items=GetObject("v_"+id)
			this.txt=this.items.getAttribute("value")
			this.depth=this.items.getAttribute("depth")
			this.canvas=GetObject(id)
			this.ctx = this.canvas.getContext("2d");
			this.ctx.font = "10px Arial";
			if (this.items.getAttribute("color"))
				this.ctx.fillStyle=this.items.getAttribute("color")
			else
				sctx.fillStyle="green"
			this.ctx.fillText(this.txt+" ("+this.depth+")",x,y);
			left=FindChildByTag(this.items,"tag","LeftTree")
			if (left) AppendSubTree(this.ctx,left,(x/2),(y+20),w/2,h,2);
			right=FindChildByTag(this.items,"tag","RightTree")
			if (right) AppendSubTree(this.ctx,right,((x+w)/2),y+10,w/2,h,2);
		}

