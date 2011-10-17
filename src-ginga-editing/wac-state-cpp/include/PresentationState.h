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
* @file PresentationState.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef _PRESENTATIONSTATE_H
#define	_PRESENTATIONSTATE_H

#include <map>
#include <string>
#include <vector>

using namespace std;

#include "player/PlayerState.h"
using namespace ::br::ufscar::lince::ginga::core::player;

#include "PlayerStateWac.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace state {
    
	/**
	 * Representa o estado da apresentação como um todo em um dado momento.
	 * Esta classe agrupa o estado de todos os players de uma apresentação NCL além de outras informações mais gerais da apresentação, 
	 * representado o estado de uma apresentação NCL em um dado momento.
	 */
    class PresentationState {
        private:
			/// Variavel utilizada para verificar se a instância já foi finalizada e não pode mais ser alterada.
            bool imutable;

		    /// Mapeia o nome dos nós de mídia nos estados dos players relacionados as mídias.
            map<string, PlayerState*>* stateMap;

			/// Mapeia o nome dos nós de mídia com os descritores de cada um.
			map<string, string>* descMap;

			/// Contém o nome completo do arquivo NCL da apresentação.
            string documentName;

			/// Contém o nome da base privada onde o documento se encontra.
            string privateBaseName;
           //another informations...
            
        public:
			/**
			 * Constrói uma instância de PresentationState.
			 */
            PresentationState();

			/**
			 * Destrói a instância de PresentationState.
			 */
	    	~PresentationState();

			/**
			 * Trava a instância, impedindo que ela seja modificada.
			 */
            void lockInstance(); 

			/**
			 * Seta os mapas que contém os estados dos players da apresentação.
			 * @param nStateMap Mapa que relaciona os nomes dos players com seus estados.
			 * @param nDescMap Mapa que relaciona os nomes dos players com seus descritores.
			 */
            void setStateMap(map<string, PlayerState*>* nStateMap,
					map<string, string>* nDescMap);

			/**
			 * Seta o nome do documento NCL.
			 * @param name Nome do documento NCL.
			 */
            void setDcoumentName(string name);

			/**
			 * Seta o nome da base privada.
			 * @param name Nome da base privada.
			 */
            void setPrivateBaseName(string name);
            
			/**
			 * Retorna o nome de todos os players da apresentação atual.
			 * @return Instância de vector contendo todos os nomes dos players da apresentação atual.
			 */
            vector<string>* getPlayersNames();

			/**
			 * Retorna o estado de um determinado Player.
			 * @param name Nome do player o qual se deseja obter o estado.
			 * @return Instância de PlayerStateWac com o estado atual do player.
			 */
            PlayerStateWac* getPlayerState(string name);

			/**
			 * Retorna o Descritor associado ao nó de mídia de um player.
			 * @param Nome do Player relacionado ao nó de mídia.
			 * @return Nome do descritor associado ao nó de mídia.
			 */
			string getMediaDescriptor(string name);

			/**
			 * Retorna o nome completo da apresentação.
			 * @return String contendo o nome completo da apresentação;
			 */
            string getPresentationName();

			/**
			 * Retorna o nome do documento NCL.
			 * @return String contendo o nome do documento NCL.
			 */
            string getDocumentName();

			/**
			 * Retorna o nome da base privada.
			 * @return String contendo o nome da base privada.
			 */
            string getPrivateBaseName();
    };
  
}
}
}
}
}
}

#endif	/* _PRESENTATIONSTATE_H */
