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
* @file IClientEditing.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef ICLIENTEDITING_H
#define ICLIENTEDITING_H

#include "ncl/components/Node.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/interfaces/InterfacePoint.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/switches/Rule.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl/connectors/Connector.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/transition/Transition.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include <cstdlib>
#include <string>
using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace editing {

/**
 * Interface utilizada pelas aplicações do cliente para utilizar os serviçõs do módulo ClientEditig.
 */
class IClientEditing {

	public:
		/**
		 * Destrói a instância de IClientEditing.
		 */
		virtual ~IClientEditing() {};

		/**
		 * Adiciona uma Região na Versão Cliente do Documento NCL em execução.
		 * @param regionId Identificador da Região onde a nova Região será adiconada.
		 * @param layoutRegion Instância que representa a Região que será adiconada.
		 * @return True se a região foi adicionada; False caso contrário.
		 */
		virtual bool addRegion(string regionId, 
			LayoutRegion* layoutRegion)=0;

		/**
		 * Adiciona uma Região na Versão Cliente do Documento NCL em execução.
		 * @param regionId Identificador da Região onde a nova Região será adiconada.
		 * @param xml Código xml da Região que será adiconada.
		 * @return True se a região foi adicionada; False caso contrário.
		 */
		virtual bool addRegion(string regionId, 
			string xml)=0;

		/**
		 * Adiciona uma Regra na Versão Cliente do Documento NCL em execução.
		 * @param rule Instância que representa a Regra que será adiconada.
		 * @return True se a regra foi adicionada; False caso contrário.
		 */
		virtual bool addRule(Rule* rule)=0;

		/**
		 * Adiciona uma Regra na Versão Cliente do Documento NCL em execução.
		 * @param xml Código xml da Regra que será adiconada.
		 * @return True se a regra foi adicionada; False caso contrário.
		 */
		virtual bool addRule(string xml)=0;

		/**
		 * Remove uma regra da Versão Cliente do Documento NCL em execução.
		 * @param ruleId Identificador da Regra que será removida.
		 * @return True se a regra for removida; False caso contrário.
		 */
		virtual bool removeRule(string ruleId)=0;

		/**
		 * Adiciona uma transição na Versão Cliente do Documento NCL em execução.
		 * @param transition Instância que representa a transição que será adiconada.
		 * @return True se a transição foi adicionada; False caso contrário.
		 */
		virtual bool addTransition(Transition* transition)=0;

		/**
		 * Adiciona uma transição na Versão Cliente do Documento NCL em execução.
		 * @param xml Código xml da transição que será adiconada.
		 * @return True se a transição foi adicionada; False caso contrário.
		 */
		virtual bool addTransition(string xml)=0;

		/**
		 * Remove uma transição da Versão Cliente do Documento NCL em execução.
		 * @param transitionId Identificador da transição que será removida.
		 * @return True se a transição for removida; False caso contrário.
		 */
		virtual bool removeTransition(string transitionId)=0;

		/**
		 * Adiciona um Conector na Versão Cliente do Documento NCL em execução.
		 * @param connector Instância que representa o conector que será adiconado.
		 * @return True se o conector foi adicionado; False caso contrário.
		 */
		virtual bool addConnector(Connector* connector)=0;

		/**
		 * Adiciona um Conector na Versão Cliente do Documento NCL em execução.
		 * @param xml Código xml do conector que será adiconado.
		 * @return True se o conector foi adicionado; False caso contrário.
		 */
		virtual bool addConnector(string xml)=0;

		/**
		 * Remove um conector da Versão Cliente do Documento NCL em execução.
		 * @param connectorId Identificador do conector que será removido.
		 * @return True se o conector for removido; False caso contrário.
		 */
		virtual bool removeConnector(string connectorId)=0;

		/**
		 * Adiciona um Descritor na Versão Cliente do Documento NCL em execução.
		 * @param descriptor Instância que representa o Descritor que será adiconado.
		 * @return True se o Descritor foi adicionado; False caso contrário.
		 */
		virtual bool addDescriptor(GenericDescriptor* descriptor)=0;

		/**
		 * Adiciona um Descritor na Versão Cliente do Documento NCL em execução.
		 * @param xml Código xml do Descritor que será adiconado.
		 * @return True se o Descritor foi adicionado; False caso contrário.
		 */
		virtual bool addDescriptor(string xml)=0;

		/**
		 * Remove um descritor da Versão Cliente do Documento NCL em execução.
		 * @param descriptorId Identificador do descritor que será removido.
		 * @return True se o descritor for removido; False caso contrário.
		 */
		virtual bool removeDescriptor(string descriptorId)=0;

		/**
		 * Adiciona um Nó à Versão Cliente do Documento NCL em execução.
		 * @param compositeId Idenitificador do nó onde este novo nó será adicionado.
		 * @param node Instância que representa o nó que será adiconado.
		 * @return True se o nó foi adicionado; False caso contrário.
		 */
		virtual bool addNode(string compositeId, Node* node)=0;

		/**
		 * Adiciona um Nó à Versão Cliente do Documento NCL em execução.
		 * @param compositeId Idenitificador do nó onde este novo nó será adicionado.
		 * @param xml Código xml do nó que será adiconado.
		 * @return True se o nó foi adicionado; False caso contrário.
		 */
		virtual bool addNode(string compositeId, string xml)=0;

		/**
		 * Adiciona uma Interface à Versão Cliente do Documento NCL em execução.
		 * @param nodeId Idenitificador do nó onde esta nova interface será adicionada.
		 * @param interface Instância que representa a interface que será adiconada.
		 * @return True se a interface foi adicionada; False caso contrário.
		 */
		virtual bool addInterface(string nodeId, 
			InterfacePoint* interface)=0;

		/**
		 * Adiciona uma Interface à Versão Cliente do Documento NCL em execução.
		 * @param nodeId Idenitificador do nó onde esta nova interface será adicionada.
		 * @param xml Código xml da interface que será adiconada.
		 * @return True se a interface foi adicionada; False caso contrário.
		 */
		virtual bool addInterface(string nodeId, 
			string xml)=0;

		/**
		 * Remove uma interface da Versão Cliente do Documento NCL em execução.
		 * @param nodeId Idenitificador do nó de onde esta interface será removida.
		 * @param descriptorId Identificador do descritor que será removido.
		 * @return True se o descritor for removido; False caso contrário.
		 */
		virtual bool removeInterface(string nodeId, 
			string interfaceId)=0;

		/**
		 * Adiciona um elo à Versão Cliente do Documento NCL em execução.
		 * @param compositeId Idenitificador do nó de contexto onde este novo elo será adicionado.
		 * @param link Instancia que representa o elo que será adiconado.
		 * @return True se o elo foi adicionado; False caso contrário.
		 */
		virtual bool addLink(string compositeId, Link* link)=0;

		/**
		 * Adiciona um elo à Versão Cliente do Documento NCL em execução.
		 * @param compositeId Idenitificador do nó de contexto onde este novo elo será adicionado.
		 * @param xml Código xml do elo que será adiconado.
		 * @return True se o elo foi adicionado; False caso contrário.
		 */
		virtual bool addLink(string compositeId, string xml)=0;

		/**
		 * Remove um elo da Versão Cliente do Documento NCL em execução.
		 * @param nodeId Idenitificador do nó de onde este elo será removido.
		 * @param descriptorId Identificador do elo que será removido.
		 * @return True se o elo for removido; False caso contrário.
		 */
		virtual bool removeLink(string compositeId, string linkId)=0;

		/**
		 * Retorno a instância que representa o documento NCl que esta sendo apresentado.
		 * @return Documento NCL que está sendo apresentado.
		 */
		virtual NclDocument* getCurrentDocument()=0;

		/**
		 * Permite desfazer as ultimas edições realizadas por aplicações do cliente.
		 * @param n Número de edições que serão desfeitas. Se omitido será desfeita uma edição.
		 * @return True se as alterações foram desfeitas com sucesso; False Caso contrário.
		 */
		virtual bool undoLastEditing(int n)=0;

};


}
}
}
}
}
}



#endif //ICLIENTEDITING_H
