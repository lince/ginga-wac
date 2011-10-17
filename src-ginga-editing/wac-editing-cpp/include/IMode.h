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
* @file IMode.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef IMODE_H
#define IMODE_H

#include "ISchedulerAdapter.h"
#include "ILinkAction.h"


namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace editing {

	/**
	 * Interface que permite a manipulação dos modos de exibição.
	 */
	class IMode{
		
	   public:
		/**
		 * Força a entrada no de exibição do cliente.
		 * @return True se entrou no modo cliente; False se já estava no modo cliente.
		 */
		virtual bool enterClientPresentationMode()=0;

		/**
		 * Força a saida do modo de exibição do cliente.
		 * @return True se saiu do modo cliente; False se não estava no modo cliente.
		 */
		virtual bool exitClientPresentationMode()=0;

		/**
		 * Verifica se o determinado elo deve ou não ser disparado dependendo do modo de exibição atual.
		 * @param linkAction Elo que será testado quanto a ser ou não disparado.
		 * @return True se o elo deve ser disparado; False Caso contrário.
		 */
		virtual bool runModeAction(ILinkAction* linkAction)=0;

		/**
		 * Seta a instãncia de Scheculer que será utilizada para matar objetos.
		 * @param scheduler Instância de FormatterScheduler.
		 */
		virtual void setScheduler(ISchedulerAdapter* scheduler)=0;
	};

}
}
}
}
}
}


#endif
