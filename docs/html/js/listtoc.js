/* LISTTOC.JS */
window.onload = window_load;
var eCurrentA = null;

function MarkActive(eA)
{
	if ("object" == typeof(eA) && eA && "A" == eA.tagName.toUpperCase() && eA != eCurrentA)
    {
        MarkInActive();
        window.eCurrentA = eA;
        window.eCurrentA.className = "CURLIST";
    }
}

function MarkInActive()
{
    if ("object" == typeof(eCurrentA) && eCurrentA)
    {
        window.eCurrentA.className = "";
        window.eCurrentA  = null;
    }
}

function IMG_click(elIMG)
{

	if ("IMG" == elIMG.tagName.toUpperCase()) {
		elDiv = elIMG.parentNode;
            if (elDiv.className == "PARENT-HIDDEN") {
    	
            elIMG.setAttribute('src','./images/minus.gif');
            childs = elDiv.getElementsByTagName("DIV");
            childs[0].className = "SHOWN";
            elDiv.className = "PARENT-SHOWN";

            childs = childs[0].getElementsByTagName("DIV");
            childs = childs[0].getElementsByTagName("IMG");
            /* childs[0].setAttribute('src','./images/square.gif'); */
		}
		else if (elDiv.className == "PARENT-SHOWN") {
            childs = elDiv.getElementsByTagName("DIV");
			childs[0].className = "HIDDEN";
			elDiv.className = "PARENT-HIDDEN";
			elIMG.setAttribute('src','./images/plus.gif');
		}
	}
}

function A_click(elA)
{
    if ("A" == elA.tagName.toUpperCase()) {
   	    elDiv = elA.parentNode;

		if (elDiv.className == "PARENT-HIDDEN") {
			childs = elDiv.getElementsByTagName("IMG");
			IMG_click(childs[0]);
		}

		MarkActive(elA);
	}  
	return false;
}

function Navigate_URL(eLink)
{
    window.open(eLink.href,eLink.target);
}

function window_load()
{	
    MarkActive(document.links[0]);
}

