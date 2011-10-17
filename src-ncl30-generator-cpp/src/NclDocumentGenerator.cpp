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
* @file NclDocumentGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/NclDocumentGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {
    
    string NclDocumentGenerator::generateCode() {
        string ret = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
        ret += "<ncl id=\"" + this->getId() +  "\" xmlns=\"http://www.ncl.org.br/NCL3.0/profileName\">\n";
        ret += "<head>\n";
		

		map<int, RegionBase*>* regionBases =NULL;
		regionBases = this->getRegionBases();
        if (regionBases != NULL) {
				map<int, RegionBase*>::iterator it;
				it = regionBases->begin();
				while (it != regionBases->end()) {
					RegionBase* regionBase = it->second;
					ret+= static_cast<RegionBaseGenerator*>(regionBase)->generateCode() + "\n";
					it++;
				}     
		}
        if (this->getDescriptorBase() != NULL) {
               ret+= static_cast<DescriptorBaseGenerator*>(this->getDescriptorBase())->generateCode() + "\n";
		}
        if (this->getConnectorBase() != NULL)
                ret += static_cast<ConnectorBaseGenerator*>(this->getConnectorBase())->generateCode() + "\n"; {
		}
        if (this->getTransitionBase() != NULL) {
                ret += static_cast<TransitionBaseGenerator*>(this->getTransitionBase())->generateCode() + "\n";
		}
        if (this->getRuleBase() != NULL) {
                ret += static_cast<RuleBaseGenerator*>(this->getRuleBase())->generateCode() + "\n";
		}

        //tratar meta informações (que eu não sei o quê é)
        //buscar infomarções nas especificações

        ret += "</head>\n";
		//<body>
        ret += static_cast<ContextNodeGenerator*>(this->getBody())->generateBodyCode((NclDocument*)this) + "\n";
		//</body>
        ret +=" </ncl>\n";
        return ret;  
    }   
}
}
}
}
}
