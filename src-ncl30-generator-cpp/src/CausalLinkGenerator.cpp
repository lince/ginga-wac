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
* @file CausalLinkGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/CausalLinkGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {
  
    string CausalLinkGenerator::generateCode(ConnectorBase* connectorBase) {
		HLoggerPtr logger = LoggerUtil::getLogger("br.ufscar.lince.ncl.generate.CausalLinkGenerator");
        string ret = "<link ";
        string id = this->getId();
        if (id!= "") {
            ret += "id=\"" + id + "\" ";
        }
        Connector* connector = this->getConnector();
        ret += "xconnector=\"";
        if (!connectorBase->containsConnector(connector)) {
			vector<Base*>* bases = connectorBase->getBases();
			if (bases != NULL) {
				bool find = false;
				for (int i=0; i < bases->size(); i++) {
					ConnectorBase* connectorBaseAux = (ConnectorBase*)(*bases)[i];
					if (connectorBaseAux->containsConnector(connector)) {
						ret += connectorBase->getBaseAlias((*bases)[i]) + "#";
						find = true;
						break;
					}
				}
				if (!find) {
					LoggerUtil_error(logger, "Connector não encontrado em nenhuma base!");
				}
			} else {
				LoggerUtil_error(logger, "Connector não encontrado em nenhuma base!");
			}
		}

        ret += connector->getId() + "\" >";
		vector<Parameter*>* parameters =  this->getParameters();
		if (parameters != NULL) {
			vector<Parameter*>::iterator itParam;
			itParam = parameters->begin();
			while (itParam != parameters->end()) {
				Parameter* parameter = *itParam;
            	ret += static_cast<ParameterGenerator*>(parameter)->generateCode("linkParam", "value");
				itParam++;
			}
		}

		vector<Bind*>* binds = this->getBinds();
		vector<Bind*>::iterator itBind;
		itBind = binds->begin();
		while (itBind != binds->end()) {
            Bind* bind = *itBind;
            ret += (static_cast<BindGenerator*>(bind))->generateCode() + "\n";
			itBind++;
		}
        ret += "</link>\n";
        return ret;
    }
}
}
}
}
}
