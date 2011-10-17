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
 * @file IStateManager.h
 * @author Caio Viel
 * @date 23-07-10
 */

#ifndef _ISTATEMANAGER_H
#define _ISTATEMANAGER_H

#include "IPresentationState.h"
#include "IStateProvider.h"
#include "IContextProvider.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace state {

/**
 * Está classe virtual é utilizada como interface para acessar os
 * serviços providos pelo componente de StateManager.
 */
class IStateManager {
public:
	/**
	 * Este método permite registrar uma instância capaz de informar parte do estado de
	 * uma apresentação NCL.
	 * @param id Identificador do provedor de estados.
	 * @param stateProvider A instância do provedor de estados da aplicação
	 */
	virtual void addStateProvider(string id, IStateProvider* stateProvider)=0;

	/**
	 * Este método gera um objeto que contém o estado da apresentação NCL ativa atual.
	 * @return Uma instância de IPresentationState que contém o estado atual da apresentação NCL.
	 */
	virtual IPresentationState* getPresentationState()=0;

	/**
	 * Este método permite registrar a instância responsável por fornecer o contexto da
	 * apresentação NCL.
	 * @param contextProvider A instância que fornecerá o contexto da apresentação NCL.
	 */
	virtual void setContextProvider(IContextProvider* contextProvider)=0;

	/**
	 * Destrutor Genérico.
	 */
	virtual ~IStateManager() {
	}
};

}
}
}
}
}
}

typedef ::br::ufscar::lince::ginga::wac::state::IStateManager
		* StateManagerCreator();

typedef void StateManagerDestroyer(
		::br::ufscar::lince::ginga::wac::state::IStateManager* stateManager);

#endif	/* _ISTATEMANAGER_H */
