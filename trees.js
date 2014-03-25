
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

