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
 * @file PresentationState.cpp
 * @author Caio Viel
 * @date 23-07-10
 */

#include <ginga/util/functions.h>
using namespace ::br::pucrio::telemidia::util;

#include <ginga/player/PlayerState.h>
using namespace ::br::ufscar::lince::ginga::core::player;

#include "../include/PresentationState.h"

#include <iostream>

using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace state {

PresentationState::PresentationState() {
	this->stateMap = NULL;
	this->descMap = NULL;
	this->contextMap = NULL;
}

PresentationState::~PresentationState() {
	if (descMap != NULL) {
		delete stateMap;
	}

	if (contextMap != NULL) {
		delete contextMap;
	}

	if (stateMap != NULL) {
		map<string, IElementaryState*>::iterator it;
		it = stateMap->begin();
		while (it != stateMap->end()) {
			IElementaryState* es = it->second;
			delete es;
			it++;
		}
		delete stateMap;
	}
}

IElementaryState* PresentationState::getElementaryState(string name) {
	if (stateMap != NULL) {
		map<string, IElementaryState*>::iterator it;
		it = stateMap->find(name);
		if (it != stateMap->end()) {
			return it->second;
		}
	}
	return NULL;
}

string PresentationState::getMediaDescriptor(string name) {
	if (descMap != NULL) {
		map<string, string>::iterator it;
		it = descMap->find(name);
		if (it != descMap->end()) {
			return it->second;
		}
	}
	return NULL;

}

vector<string>* PresentationState::getPlayersNames() {
	vector<string>* ret = new vector<string> ;

	map<string, IElementaryState*>::iterator i;
	string name;

	i = stateMap->begin();
	int count;
	while (i != stateMap->end()) {
		name = i->first;
		ret->push_back(name);
		++i;
	}
	return ret;
}

string PresentationState::getPresentationName() {
	return this->privateBaseName + "/" + this->documentName;
}

string PresentationState::getDocumentName() {
	return this->documentName;
}
string PresentationState::getPrivateBaseName() {
	return this->privateBaseName;
}

void PresentationState::setDcoumentName(string name) {
		this->documentName = name;

}

void PresentationState::setPrivateBaseName(string name) {
		this->privateBaseName = name;

}

void PresentationState::setStateMap(
			map<string, IElementaryState*>* nStateMap,
			map<string, string>* nDescMap,
			map<string, string>* nContextMap) {

		this->stateMap = nStateMap;
		this->descMap = nDescMap;
		this->contextMap = nContextMap;
}

string PresentationState::toString() {
	string ret = "----------------PresentationState----------------\n";
	ret += "PresentationName: " + this->getPresentationName() + "\n";

	map<string, string>::iterator it2;
	it2 = contextMap->begin();
	while (it2 != contextMap->end()) {
		string name = it2->first;
		string value = it2->second;
		ret += name + ": " + value + "\n";
		it2++;
	}

	ret += "Players	\t\tDescriptor \t\tTime \t\tState \t\tMRL \n";
	map<string, IElementaryState*>::iterator it;
	it = stateMap->begin();
	while (it != stateMap->end()) {
		string name = it->first;
		IElementaryState* elemState = it->second;
		string desc = this->getMediaDescriptor(name);
		string mrl = elemState->getMrl();
		double time = elemState->getMediaTime();
		string sStatus  = elemState->getStatusName();
		ret += name + " \t\t\t" + desc + " \t\t" + itos(time) +
				" \t\t" + sStatus + " \t\t" + mrl + "\n";
		it++;
	}
	return ret;
}

vector<string>* PresentationState::getContextPropertyNames() {
	vector<string>* ret = new vector<string>();
	map<string, string>::iterator it;
	it = contextMap->begin();
	while (it != contextMap->end()) {
		ret->push_back(it->first);
		it++;
	}
	return ret;
}

string PresentationState::getContextPropertyValue(string attributeId) {
	if (contextMap->count(attributeId) != 0) {
		return (*contextMap)[attributeId];
	}
	return "";
}

}
}
}
}
}
}
