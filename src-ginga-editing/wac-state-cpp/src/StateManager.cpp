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
* @date 29-01-10
*/

#include "../include/StateManager.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace state {
    
    StateManager::StateManager() {
        players = new map <string, IPlayerWac*>();
    }
    
    StateManager* StateManager::_inst = NULL;
    
    StateManager* StateManager::getInstance() {
        if (_inst == NULL) {
            _inst = new StateManager();
        }
        return _inst;
    }

    void StateManager::addPlayerAdapter(string id, IPlayerWac* playerAd) {
        (*players)[id] = playerAd;
        
    }


	PresentationState* StateManager::getPresentationState() {
		PresentationState* presentationState = new PresentationState();
                map<string, IPlayerWac*>::iterator i;
        	IPlayerWac* player;
		i = players->begin();
                if (i != players->end()) {
			string fullname = i->first;
                        string privateBaseName = fullname.substr(0, fullname.find_first_of("/"));
                        fullname = fullname.substr(fullname.find_first_of("/")+1, fullname.size());
                        string documentName = fullname.substr(0, fullname.find_first_of("/"));
                        presentationState->setDcoumentName(documentName);
                        presentationState->setPrivateBaseName(privateBaseName);
                }
                
                map<string, PlayerState*>* stateMap = new map<string, PlayerState*>();
				map<string, string>* descMap = new map<string, string>();
		while (i != players->end()) {
			string fullname = i->first;
			IPlayerWac* player = i->second;
			if (player != NULL) {
                            string playerName = fullname.substr(fullname.find_first_of("/") +1, fullname.size());
                            playerName = playerName.substr(playerName.find_first_of("/") +1, playerName.size());

							string descName = playerName.substr(playerName.find_last_of("/") +1, playerName.size());
							string mediaName = playerName.substr(0,playerName.find_last_of("/"));
                            (*stateMap)[mediaName] = player->getPlayerState(); 
							(*descMap)[mediaName] = descName;
                        }
                         ++i;
             }
             presentationState->setStateMap(stateMap, descMap);
             presentationState->lockInstance();
             return presentationState;
        
        }
}
}
}
}
}
}


