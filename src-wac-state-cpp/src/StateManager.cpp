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
 * @file StateManager.cpp
 * @author Caio Viel
 * @date 23-07-10
 */

#include "../include/StateManager.h"
#include "../include/PresentationState.h"
#include "../include/ElementaryState.h"

#include <ginga/player/IPlayer.h>
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include <ginga/player/PlayerState.h>
using namespace ::br::ufscar::lince::ginga::core::player;

#include <map>
#include <string>
#include <vector>
using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace state {

StateManager::StateManager() {
	stateProviders = new map<string, IStateProvider*> ();
	contextProvider = NULL;
}

StateManager::~StateManager() {
	delete stateProviders;
}

StateManager* StateManager::_inst = NULL;

StateManager* StateManager::getInstance() {
	if (_inst == NULL) {
		_inst = new StateManager();
	}
	return _inst;
}

void StateManager::addStateProvider(string id, IStateProvider* stateProvider) {
	(*stateProviders)[id] = stateProvider;

}

IPresentationState* StateManager::getPresentationState() {
	PresentationState* presentationState = new PresentationState();
	map<string, IStateProvider*>::iterator i;
	IStateProvider* stateProvider;
	i = stateProviders->begin();

	if (i != stateProviders->end()) {
		string fullname = i->first;
		string privateBaseName =
				fullname.substr(0, fullname.find_first_of("/"));
		fullname = fullname.substr(fullname.find_first_of("/") + 1,
				fullname.size());
		string documentName = fullname.substr(0, fullname.find_first_of("/"));
		presentationState->setDcoumentName(documentName);
		presentationState->setPrivateBaseName(privateBaseName);
	} else {
		return NULL;
	}

	map<string, IElementaryState*>* stateMap =
			new map<string, IElementaryState*> ();
	map<string, string>* descMap = new map<string, string> ();
	map<string, string>* contextMap = new map<string, string>();

	while (i != stateProviders->end()) {
		string fullname = i->first;
		stateProvider = i->second;
		if (stateProvider != NULL) {
			string playerName = fullname.substr(
					fullname.find_first_of("/") + 1, fullname.size());
			playerName = playerName.substr(playerName.find_first_of("/") + 1,
					playerName.size());

			string descName = playerName.substr(playerName.find_last_of("/")
					+ 1, playerName.size());
			string mediaName = playerName.substr(0,
					playerName.find_last_of("/"));
			(*stateMap)[mediaName]
					= new ElementaryState(stateProvider->getPlayerState());
			(*descMap)[mediaName] = descName;
		}
		i++;
	}

	vector<string>* contextNames = contextProvider->getPropertyNames();
	vector<string>::iterator it2;
	it2 = contextNames->begin();
	while (it2 != contextNames->end()) {
		string name = (*it2);
		string value = contextProvider->getPropertyValue(name);
		(*contextMap)[name] =  value;
		it2++;
	}

	delete contextNames;

	presentationState->setStateMap(stateMap, descMap, contextMap);
	return presentationState;
}


void StateManager::setContextProvider(IContextProvider* contextProvider) {
	this->contextProvider = contextProvider;
}

}
}
}
}
}
}

extern "C" ::br::ufscar::lince::ginga::wac::state::IStateManager*
createStateManager() {

	return (::br::ufscar::lince::ginga::wac::state::StateManager::getInstance());
}

extern "C" void destroyStateManager(
		::br::ufscar::lince::ginga::wac::state::IStateManager* stateManager) {

	delete stateManager;
}

