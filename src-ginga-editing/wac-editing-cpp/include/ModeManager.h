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

#ifndef MODEMANAGER_H
#define MODEMANAGER_H

#include "IMode.h"
#include "IFormatterAdapter.h"
#include "ClientEditingManager.h"

#include <cstdlib>
#include <string>
#include <vector>

using namespace std;


namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace editing {

	/**
	 * Classe responsável manipular os modos de exibição do cliente e da emissora.
	 * Além de ser a responsável por manipular qual o modo atual, também é o responsável por
	 * testar os links por liberar ou não a execução dos elos.
	 */
	class ModeManager : public IMode {

	  private:
		/// Armazena a informação sobre estar ou não no modo cliente.
		bool clientMode;

		/// Instância única de ModeManager.
		static ModeManager* _instance;

		/// Constrói a instância única de ModeManager.
		ModeManager();

		/// Armazena todos os objetos iniciados por links adicionados pelo cliente.
		vector<IObjectMode*>* startedObjects;

		/// Interface pela qual se comunica com o scheduler para matar a execução de objetos do cliente quando necessário.
		ISchedulerAdapter* scheduler;

	   public:
		/**
		 * Obtém a instância única de ModeManager.
		 * @return Instância única de ModeManager.
		 */
		static IMode* getInstance();

		/**
		 * Força a entrada no de exibição do cliente.
		 * @return True se entrou no modo cliente; False se já estava no modo cliente.
		 */
		bool enterClientPresentationMode();

		/**
		 * Força a saida do modo de exibição do cliente.
		 * @return True se saiu do modo cliente; False se não estava no modo cliente.
		 */
		bool exitClientPresentationMode();

		/**
		 * Verifica se o determinado elo deve ou não ser disparado dependendo do modo de exibição atual.
		 * @param linkAction Elo que será testado quanto a ser ou não disparado.
		 * @return True se o elo deve ser disparado; False Caso contrário.
		 */
		bool runModeAction(ILinkAction* linkAction);

		/**
		 * Seta a instãncia de Scheculer que será utilizada para matar objetos.
		 * @param scheduler Instância de FormatterScheduler.
		 */
		void setScheduler(ISchedulerAdapter* scheduler);
	};

}
}
}
}
}
}


#endif
