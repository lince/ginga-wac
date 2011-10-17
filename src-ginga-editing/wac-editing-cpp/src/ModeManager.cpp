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
* @file ModeManager.h
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/ModeManager.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace editing {

	ModeManager* ModeManager::_instance = NULL;

	IMode* ModeManager::getInstance() {
		if (_instance == NULL) {
			_instance = new ModeManager();
		}
		return _instance;
	}

	ModeManager::ModeManager() {
		startedObjects = new vector<IObjectMode*>();
		scheduler = NULL;
		clientMode = false;
	}

	bool ModeManager::enterClientPresentationMode() {
		if (clientMode == true) {
			return false;
		} else {
			clientMode = true;
			return true;
		}
	}

	bool ModeManager::exitClientPresentationMode() {
		if (clientMode == true) {
			clientMode = false;
			return true;
		} else {
			return false;
		}

	}

	bool ModeManager::runModeAction(ILinkAction* linkAction) {
		//esta implementação é apenas um teste.
		int type = linkAction->getLinkType();
		string linkId = linkAction->getLinkId();
		if (clientMode) {
			//TODO registre objeto iniciado
			return true;
		} else if (linkAction->getLinkType() == IFormatterAdapter::CLIENT_EDITING) {
			//Link irá ser descartado, pois não estamos no modo cliente
			return false;
		} else if (linkId.find(ClientEditingManager::CLIENT_LINK_PREFIX) != -1) {
			cout<<"ModeManager::runModeAction Vamo sair por aqui"<<endl;
			//Link irá ser descartado, pois não estamos no modo cliente
			return false;
		} else {
			//Caso da variavel interna
			//TODO registre objeto iniciado
			return true;
		}
		return true;
	}

	void ModeManager::setScheduler(ISchedulerAdapter* scheduler) {
		this->scheduler = scheduler;

	}

}
}
}
}
}
}


