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
* @file EditingCommand.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef EDITINGCOMMAND_H
#define EDITINGCOMMAND_H

#include <string>
#include <cstdlib>

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

using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace editing {

/**
 * Classe que encapsula comandos de edição ao vivo do usuário
 */
class EditingCommand {
	protected:
		/// Armazena a entidade NCL removida ou adicionada.
		Entity* entity;

		/// Tipo de edição feita.
		int type;

		/// Identificador do comando de edição.
		string commandId;
	public:
		/**
		 * Constrói uma instância de EditingCommand
		 */
		EditingCommand();

		/**
		 * Constrói uma instância de EditingCommand com alguns atributos setados.
		 * @param entity Entidade NCL adicionada ou removida.
		 * @param id Identificador do comando de edição.
		 */
		EditingCommand(Entity* entity, string id);

		/**
		 * Constrói uma instância de EditingCommand com alguns atributos setados.
		 * @param id Identificador do comando de edição.
		 * @param type Tipo de comando de edição.
		 */
		EditingCommand(string id, int type);

		/**
		 * Destrói a instância de EditingCommand.
		 */
		~EditingCommand();

		/**
		 * Seta a entidade NCL adicionada ou removida.
		 * @param entity Entidade NCL adicionada ou removida.
		 */
		void setEntity(Entity* entity);

		/**
		 * Obtem a entidade NCL adicionada ou removida pelo comando de edição.
		 * @return A entidade NCL.
		 */
		Entity* getEntity();

		/**
		 * Obtém o tipo de comando de edição.
		 * @return Tipo do comando do edição.
		 */
		int getEntityType();

		/**
		 * Seta o Identificador do comando de edição.
		 * @param id Identificador do comando de edição.
		 */
		void setCommandId(string id);

		/**
		 * Obtém o identificador do comando de edição.
		 * @return Identificador do comando de edição.
		 */
		string getCommandId();

	public:
		/// Constate que simboliza Comando de Edição Nulo.
		static const int NONE;

		/// Constante que simboliza Comando de Adicionar Regra
		static const int ADD_RULE;

		/// Constante que simboliza Comando de Remover Regra
		static const int REMOVE_RULE;

		/// Constante que simboliza Comando de Adicionar Transição
		static const int ADD_TRANSITION;

		/// Constante que simboliza Comando de Remover Transição
		static const int REMOVE_TRANSITION;

		/// Constante que simboliza Comando de Adicionar Conector
		static const int ADD_CONNECTOR;

		/// Constante que simboliza Comando de Remover Conector
		static const int REMOVE_CONNECTOR;

		/// Constante que simboliza Comando de Adicionar Descritor
		static const int ADD_DESCRIPTOR;

		/// Constante que simboliza Comando de Remover Descritor
		static const int REMOVE_DESCRIPTOR;

		/// Constante que simboliza Comando de Adicionar Nó
		static const int ADD_NODE;

		/// Constante que simboliza Comando de Remover Nó
		static const int REMOVE_NODE;

		/// Constante que simboliza Comando de Adicionar Interface
		static const int ADD_INTERFACE;

		/// Constante que simboliza Comando de Remover Interface
		static const int REMOVE_INTERFACE;

		/// Constante que simboliza Comando de Adicionar Elo
		static const int ADD_LINK;

		/// Constante que simboliza Comando de Remover Elo
		static const int REMOVE_LINK;

		/// Constante que simboliza Comando de Adicionar Região
		static const int ADD_REGION;

		/// Constante que simboliza Comando de Remover Região
		static const int REMOVE_REGION;
};

}
}
}
}
}
}


#endif //EDITINGCOMMAND_H
