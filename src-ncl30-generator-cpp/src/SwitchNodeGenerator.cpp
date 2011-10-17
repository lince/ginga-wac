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
* @file SwitchNodeGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/SwitchNodeGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {

	string SwitchNodeGenerator::generateCode(NclDocument* nclDocument) {
		string ret = "<switch id=\"" + this->getId() + "\" >";

		Node* defaultNode = this->getDefaultNode();
		if (defaultNode != NULL) {
			ret += "<defaultComponent component=\"" + defaultNode->getId() + "\" />";
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
                SwitchNode* switchNode = (SwitchNode*) node;
                ret+= static_cast<SwitchNodeGenerator*>(switchNode)->generateCode(nclDocument) + "\n";
            }
			itNode++;
		}

		unsigned int numRules = this->getNumRules();
		if (numRules > 0) {
			for (int index = 0; index < numRules; index++) {
				Rule* rule = this->getRule(index);
				Node* node = this->getNode(rule);
				ret += "<bindRule rule=\"" + rule->getId() + "\" ";
				ret += "constituent=\"" + descriptor->getId() + "\" />\n"; 
			}
		}

		vector<Port*>* ports = this->getPorts();
		vector<Port*>::iterator itPorts;
		itPorts = ports->begin();
		while (itPorts != ports->end()) {
			Port* port = *itPorts;
			if (port->instanceOf("SwitchPort")) {
				SwitchPort* switchPort = (SwitchPort*) port;
				ret += (static_cast<SwitchPortGenerator*>(switchPort))->generateCode() + "\n";
			}
		}
		ret += "</switch>\n";
		return ret;		
	}
    
}
}
}
}
}
