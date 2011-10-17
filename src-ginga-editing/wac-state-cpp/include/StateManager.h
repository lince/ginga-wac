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
* @file StateManager.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef _STATEMANAGER_H
#define	_STATEMANAGER_H

#include <map>
#include <string>
#include <iostream>
#include <cstdlib>
using namespace std;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "player/PlayerState.h"
using namespace ::br::ufscar::lince::ginga::core::player;


#include "PresentationState.h"
#include "IPlayerWac.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace state {

class StateManager {
	/**
 	 * Classe responsável por manter e informar o estado da apresentação.
	 * Esta classe é constantemente informada das mudanças topológicas nos players da apresentação NCL em execução,
	 * o que permite a ela gerar informar o estada da apresentação NCL como um todo.
	 */
    private:
		/// Instância única de StateManager.
        static StateManager* _inst;

		/// Mapa que associa os players com seus nomes.
		 map<string, IPlayerWac*>* players;

		/**
		 * Constrói a instância de StateManager.
		 */
        StateManager();
       
        
    public:
		/**
		 * Permite obter acesso a instância única de StateManager.
		 * @return Instância única de StateManager.
		 */ 
        static StateManager* getInstance();

		/**
		 * Adiciona um player a lista de players monitorados pela StateManager.
		 * @param id Identificador do Player dentro da Maquina de Apresentação NCL.
		 * @param playerAd Instância de PlayerAdapter que será monitorado por StateManager.
		 */
        void addPlayerAdapter(string id, IPlayerWac* playerAd);

		/**
		 * Retorna o estado da apresentação no atual momento.
		 * @return instância de PresentationState contento o estado atual da apresentação.
		 */
        PresentationState* getPresentationState();
};

}
}
}
}
}
}


#endif	/* _STATEMANAGER_H */


