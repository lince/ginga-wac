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

#include "../include/EditingCommand.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace editing {

	EditingCommand::EditingCommand() {
		entity = NULL;
		type = NONE;
		commandId = "";
	}

	EditingCommand::EditingCommand(Entity* e, string id) {
		this->entity = e;
		commandId = id;
		setEntity(e);
	}

	EditingCommand::EditingCommand(string id, int t) {
		entity = NULL;
		commandId = id;
		this->type = t;
	}

	EditingCommand::~EditingCommand() {
		//não faz nada
	}

	void  EditingCommand::setEntity(Entity* e){
		this->entity = e;
		if (entity->instanceOf("Node")) {
			type = ADD_NODE;
		} else if (entity->instanceOf("Rule")) {
			type = ADD_RULE;
		} else if (entity->instanceOf("Transition")) {
			type = ADD_TRANSITION;
		} else if (entity->instanceOf("Connector")) {
			type = ADD_CONNECTOR;
		} else if (entity->instanceOf("GenericDescriptor")) {
			type = ADD_DESCRIPTOR;
		} else if (entity->instanceOf("InterfacePoint")) {
			type = ADD_INTERFACE;
		} else if (entity->instanceOf("Link")) {
			type = ADD_LINK;
		} else {
			type = NONE;
		}
	}

	Entity* EditingCommand::getEntity() {
		return entity;
	}

	int EditingCommand::getEntityType() {
		return type;
	}

	void  EditingCommand::setCommandId(string id) {
		this->commandId = id;
	}

	string EditingCommand::getCommandId() {
		return commandId;
	}

	const int EditingCommand::NONE=-1;
	const int EditingCommand::ADD_RULE=1;
	const int EditingCommand::REMOVE_RULE=2;
	const int EditingCommand::ADD_TRANSITION=3;
	const int EditingCommand::REMOVE_TRANSITION=4;
	const int EditingCommand::ADD_CONNECTOR=5;
	const int EditingCommand::REMOVE_CONNECTOR=6;
	const int EditingCommand::ADD_DESCRIPTOR=7;
	const int EditingCommand::REMOVE_DESCRIPTOR=8;
	const int EditingCommand::ADD_NODE=9;
	const int EditingCommand::REMOVE_NODE=10;
	const int EditingCommand::ADD_INTERFACE=11;
	const int EditingCommand::REMOVE_INTERFACE=12;
	const int EditingCommand::ADD_LINK=13;
	const int EditingCommand::REMOVE_LINK=14;
	const int EditingCommand::ADD_REGION=15;
	const int EditingCommand::REMOVE_REGION=16;

}
}
}
}
}
}
