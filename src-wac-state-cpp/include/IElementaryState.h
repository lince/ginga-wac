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
 * @file IElementaryState.h
 * @author Caio Viel
 * @date 23-07-10
 */

#ifndef _IELEMENTARYSTATE_H
#define _IELEMENTARYSTATE_H

#include <string>
#include <vector>

using namespace std;

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
class IElementaryState {
public:
	///Constante que representa o estado "nenhum" do player
	static const short NONE;

	///Constante que representa o estado "em execução" do player
	static const short PLAY;

	///Constante que representa o estado "pausado" do player
	static const short PAUSE;

	///Constante que representa o estado "parado" do player
	static const short STOP;

	/**
	 * Este método retorna uma número que representa o estado do player
	 * @return Um número que representa o estado do player
	 */
	virtual short getStatus()=0;

	/**
	 * Este método retorna o tempo atual do player, considerando o inicio como sendo 0.
	 * @return O tempo atual do player.
	 */
	virtual double getMediaTime()=0;

	/**
	 * Este método retorna a altura da imagem exibida pelo player.
	 * @return Altura da Imagem exibida pelo player
	 */
	virtual int getHight()=0;

	/**
	 * Este método retorna a largura da imagem exibida pelo player.
	 * @return Largura da Imagem exibida pelo player
	 */
	virtual int getWidh()=0;

	/**
	 * Este permite saber se a apresentação do player já foi encerrada.
	 * @return True se a apresentação já foi encerrada; False caso contrário.
	 */
	virtual bool isPresented()=0;

	/**
	 * Este permite saber a mídia exibida pelo player está visivel.
	 * @return True se está visivel; False caso contrário.
	 */
	virtual bool isVisible()=0;

	/**
	 * Este método permite obter o valor de uma propriedade do player.
	 * @param name Nome da propriedade a ser obtida
	 * @return Valor da propriedade armazenada em um string.
	 */
	virtual string getPropertyValue(string name)=0;

	/**
	 * Este método permite otbter todos os nomes das propriedades possuidas pelo player.
	 * @return Vetor contento todos os nomes das propriedades.
	 */
	virtual vector<string>* getPropertiesNames()=0;

	/**
	 * Este método permite obter o MRL da mídia executada pelo player.
	 * @return Mrl da mídia executada pelo player.
	 */
	virtual string getMrl()=0;

	/**
	 * Este método retorna um string que representa o estado atual do player.
	 * Os nomes possíveis de serem retornados são: 'NOME', 'PLAY', 'PAUSE' e 'STOP'
	 * @return String com o nome do estado atual do player.
	 */
	virtual string getStatusName()=0;

	/**
	 * Destrutor genérico
	 */
	virtual ~IElementaryState() {};
};

}
}
}
}
}
}

#endif	/* _IELEMENTARYSTATE_H */
