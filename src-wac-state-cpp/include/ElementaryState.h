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
 * @file ElementaryState.h
 * @author Caio Viel
 * @date 23-07-10
 */

#ifndef _ELEMENTARYSTATE_H
#define _ELEMENTARYSTATE_H

#include <string>
#include <vector>
using namespace std;

#include <ginga/player/PlayerState.h>
using namespace ::br::ufscar::lince::ginga::core::player;

#include "IElementaryState.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace state {

/**
 * Esta classe generica representa o Estado de um Player em um determinado momento, servindo de interface
 * com as aplicações que desejam obter os estados do player.
 * Futuramente, ela pode ser espandida para armazenar mais informações referentes ao estado do player que não podem
 * ser obtidas diretamente da módulo players, mas sim da classe PlayerAdapter do módulo Formatter.
 */
class ElementaryState: public IElementaryState {
private:
	PlayerState* playerState;

public:
	/**
	 * Este método retorna uma número que representa o estado do player
	 * @return Um número que representa o estado do player
	 */
	virtual short getStatus();

	/**
	 * Este método retorna o tempo atual do player, considerando o inicio como sendo 0.
	 * @return O tempo atual do player.
	 */
	virtual double getMediaTime();

	/**
	 * Este método retorna a altura da imagem exibida pelo player.
	 * @return Altura da Imagem exibida pelo player
	 */
	virtual int getHight();

	/**
	 * Este método retorna a largura da imagem exibida pelo player.
	 * @return Largura da Imagem exibida pelo player
	 */
	virtual int getWidh();

	/**
	 * Este permite saber se a apresentação do player já foi encerrada.
	 * @return True se a apresentação já foi encerrada; False caso contrário.
	 */
	virtual bool isPresented();

	/**
	 * Este permite saber a mídia exibida pelo player está visivel.
	 * @return True se está visivel; False caso contrário.
	 */
	virtual bool isVisible();

	/**
	 * Este método permite obter o valor de uma propriedade do player.
	 * @param name Nome da propriedade a ser obtida
	 * @return Valor da propriedade armazenada em um string.
	 */
	virtual string getPropertyValue(string name);

	/**
	 * Este método permite otbter todos os nomes das propriedades possuidas pelo player.
	 * @return Vetor contento todos os nomes das propriedades.
	 */
	virtual vector<string>* getPropertiesNames();

	/**
	 * Este método permite obter o MRL da mídia executada pelo player.
	 * @return Mrl da mídia executada pelo player.
	 */
	virtual string getMrl();

	/**
	 * Este método retorna um string que representa o estado atual do player.
	 * Os nomes possíveis de serem retornados são: 'NOME', 'PLAY', 'PAUSE' e 'STOP'
	 * @return String com o nome do estado atual do player.
	 */
	virtual string getStatusName();

	/**
	 * Destrutor
	 */
	~ElementaryState();

	/**
	 * Construtor.
	 * @param playerState Instância que contém as informações relativas ao estado dos players.
	 */
	ElementaryState(PlayerState* playerState);

};

}
}
}
}
}
}

#endif	/* _ELEMENTARYSTATE_H */

