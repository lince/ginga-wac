/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Copyright (C) 2009 UFSCar/Lince, Todos os Direitos Reservados.

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.
 
Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
lince@dc.ufscar.br
http://www.ncl.org.br
http://www.ginga.org.br
http://lince.dc.ufscar.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright (C) 2009 UFSCar/Lince, Todos os Direitos Reservados.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.
 
You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
lince@dc.ufscar.br
http://www.ncl.org.br
http://www.ginga.org.br
http://lince.dc.ufscar.br
*******************************************************************************/

/**
* @file GeneratorUtil.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/GeneratorUtil.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {

	string GeneratorUtil::getColorString(Color* color) {
		int colorI = 0;
		colorI |= color->getR();
		colorI = colorI << 8;
		colorI |= color->getG();
		colorI = colorI << 8;
		colorI |= color->getB();
		string ret = "";
		
		if (colorI == Color::white) {
			ret = "white";
		} else if (colorI == Color::yellow) {
			ret = "yellow";
		} else if (colorI == Color::red) {
			ret = "red";
		} else if (colorI == Color::pink) {
			ret = "pink";
		} else if (colorI == Color::orange) {
			ret = "orange";
		} else if (colorI == Color::magenta) {
			ret = "magenta";
		} else if (colorI == Color::green) {
			ret = "green";
		} else if (colorI == Color::cyan) {
			ret = "cyan";
		} else if (colorI == Color::blue) {
			ret = "blue";
		} else if (colorI == Color::lightGray) {
			ret = "lightGray";
		} else if (colorI == Color::gray) {
			ret = "gray";
		} else if (colorI == Color::darkGray) {
			ret = "darkGray";
		} else if (colorI == Color::black) {
			ret = "black";
		} else if (colorI == Color::silver) {
			ret = "silver";
		} else if (colorI == Color::maroon) {
			ret = "maroon";
		} else if (colorI == Color::fuchsia) {
			ret = "fuchsia";
		} else if (colorI == Color::purple) {
			ret = "purple";
		}  else if (colorI == Color::lime) {
			ret = "lime";
		}  else if (colorI == Color::olive) {
			ret = "olive";
		}  else if (colorI == Color::navy) {
			ret = "navy";
		}  else if (colorI == Color::aqua) {
			ret = "aqua";
		}  else if (colorI == Color::teal) {
			ret = "teal";
		}

		return ret;
	}

	string GeneratorUtil::referNodeCodeGenerate(
			ReferNode* referNode, NclDocument* nclDocument) {

		string ret = "";
		Node* referredNode = (Node*)referNode->getReferredEntity();
		if (referredNode != NULL && 
				referredNode->instanceOf("ContextNode")) {

			ret += "<context id=\"" + referNode->getId() + "\" ";
			ret += "refer=\"" + referredNode->getId() + "\" ";
			vector<Anchor*>* anchors = referNode->getAnchors();
			vector<Anchor*>::iterator anchorIt;
			anchorIt = anchors->begin();
			if (anchorIt == anchors->end()) {
				ret += "/>\n";
			} else {
				ret += ">\n";
				while (anchorIt != anchors->end()) {
					Anchor* anchor = *anchorIt;
            		if (anchor->instanceOf("PropertyAnchor")) {
               			PropertyAnchor* propertyAnchor = (PropertyAnchor*) anchor;
               			ret += static_cast<PropertyAnchorGenerator*>(propertyAnchor)->generateCode() + "\n";
            		}
					anchorIt++; 
				}
				ret += " </context>\n";
			}
        } else if (referredNode != NULL && 
				referredNode->instanceOf("ContentNode")) {

			ret += "<media id=\"" + referNode->getId() + "\" ";
			ret += "refer=\"" + referredNode->getId() + "\" ";
			ret += "instance=\"" + referNode->getInstanceType() + "\" ";

			vector<Anchor*>* anchors = referNode->getAnchors();
			vector<Anchor*>::iterator anchorIt;
			anchorIt = anchors->begin();
			if (anchorIt == anchors->end()) {
				ret += "/>\n";
			} else {
				ret += ">\n";
				while (anchorIt != anchors->end()) {
					Anchor* anchor = *anchorIt;
               		ret += (static_cast<AnchorGenerator*>(anchor))->generateCode() + "\n";
					anchorIt++; 
				}
				ret += " </media>\n";
			}
        } else if (referredNode->instanceOf("SwitchNode")) {
			ret += "<switch id=\"" + referNode->getId() + "\" ";
			ret += "refer=\"" + referredNode->getId() + "\" />\n";		
        }
		return ret;
	}

}
}
}
}
}

