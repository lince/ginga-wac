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
 * @file ElementaryState.cpp
 * @author Caio Viel
 * @date 23-07-10
 */

#include "../include/ElementaryState.h"

#include <ginga/player/PlayerState.h>
using namespace ::br::ufscar::lince::ginga::core::player;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace state {

const short IElementaryState::NONE = PlayerState::NONE;

const short IElementaryState::PLAY = PlayerState::PLAY;

const short IElementaryState::PAUSE = PlayerState::PAUSE;

const short IElementaryState::STOP = PlayerState::STOP;

inline short ElementaryState::getStatus() {
	return playerState->getStatus();
}

inline double ElementaryState::getMediaTime() {
	return playerState->getMediaTime();
}

inline int ElementaryState::getHight() {
	return playerState->getHight();
}

inline int ElementaryState::getWidh() {
	return playerState->getWidh();
}

inline bool ElementaryState::isPresented() {
	return playerState->isPresented();
}

inline bool ElementaryState::isVisible() {
	return playerState->isVisible();
}

inline string ElementaryState::getPropertyValue(string name) {
	return playerState->getPropertyValue(name);
}

inline vector<string>* ElementaryState::getPropertiesNames() {
	return playerState->getPropertiesNames();
}

inline string ElementaryState::getMrl() {
	return playerState->getMrl();
}

ElementaryState::~ElementaryState() {
	delete playerState;
}

ElementaryState::ElementaryState(PlayerState* playerState) {
	this->playerState = playerState;
}

string ElementaryState::getStatusName() {
	short status = playerState->getStatus();
	if (status == IElementaryState::PAUSE) {
		return "PAUSE";
	} else if (status == IElementaryState::PLAY) {
		return "PLAY";
	} else if (status == IElementaryState::STOP) {
		return "STOP";
	} else {
		return "NONE";
	}
}

}
}
}
}
}
}

