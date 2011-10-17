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
* @file ContextNodeGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/ContextNodeGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {
        
    string ContextNodeGenerator::generateBodyCode(NclDocument* nclDocument) {
        string ret = "<body ";
        ret += interCodeGenerate(nclDocument);
        ret += "</body>\n";
        return ret;
    }

	
    
    string ContextNodeGenerator::generateCode(NclDocument* nclDocument) {
        string ret = "<context ";
        ret  += interCodeGenerate(nclDocument);
        ret += "</context>\n";
        return ret;
    }    
    
    string ContextNodeGenerator::interCodeGenerate(NclDocument* nclDocument) {
        string ret = "id=\"" +this->getId() + "\" >";
		
		vector<Port*>* ports = this->getPorts();
		vector<Port*>::iterator itPort;
		itPort = ports->begin();
		while (itPort != ports->end()) {
			Port* port = *itPort;
            ret += static_cast<PortGenerator*>(port)->generateCode() + "\n";
			itPort++;
		}

        vector<Node*>* nodes = this->getNodes();
		vector<Node*>::iterator itNode;
		itNode = nodes->begin();
		while (itNode != nodes->end()) {
			Node* node = *itNode;
			if (node->instanceOf("ReferNode")) {
				ReferNode* referNode = (ReferNode*)node;
				ret += GeneratorUtil::referNodeCodeGenerate(referNode, nclDocument);
			} else if (node->instanceOf("ContextNode")) {
                ContextNode * contextNode = (ContextNode*)node;
                ret+= static_cast<ContextNodeGenerator*>(contextNode)->generateCode(nclDocument) + "\n";
            } else if (node->instanceOf("ContentNode")) {
                ContentNode* contentNode = (ContentNode*)node;
                ret+= static_cast<ContentNodeGenerator*>(contentNode)->generateCode() + "\n";
            } else if (node->instanceOf("SwitchNode")) {
                SwitchNode* switchNode = (SwitchNode*)node;
                ret+= static_cast<SwitchNodeGenerator*>(switchNode)->generateCode(nclDocument) + "\n";
            }
			itNode++;
		}

		vector<Link*>* links = this->getLinks();
		vector<Link*>::iterator itLinks;
		itLinks = links->begin();
		while (itLinks != links->end()) {
			CausalLink* link = (CausalLink*)*itLinks;
            ret+= static_cast<CausalLinkGenerator*>(link)->generateCode(nclDocument->getConnectorBase()) + "\n";
			itLinks++;
		}
        
		vector<Anchor*>* anchors = this->getAnchors();
		vector<Anchor*>::iterator itAnchor;
		itAnchor = anchors->begin();
		while (itAnchor != anchors->end()) {
			Anchor* anchor = *itAnchor;
			if (anchor->instanceOf("PropertyAnchor")) {
                PropertyAnchor* propertyAnchor = (PropertyAnchor*) anchor;
                ret += static_cast<PropertyAnchorGenerator*>(propertyAnchor)->generateCode() + "\n";
            } 
			itAnchor++;
		}      

        return ret;
    }
    
}
}
}
}
}
