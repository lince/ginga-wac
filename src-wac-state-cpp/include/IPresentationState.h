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
 * @file IPresentationState.h
 * @author Caio Viel
 * @date 23-07-10
 */

#ifndef IPRESENTATIONSTATE_H_
#define IPRESENTATIONSTATE_H_

#include <map>
#include <string>
#include <vector>
using namespace std;

#include "IElementaryState.h"

namespace br{
namespace ufscar{
namespace lince{
namespace ginga{
namespace wac{
namespace state{

/**
 * Esta classe genérica serve de interface como as apliações que desejam obter informações
 * sobre o estado da aplicação.
 */
class IPresentationState {
public:
	~IPresentationState() {};

	/**
	 * Retorna o nome de todos os players da apresentação atual.
	 * @return Instância de vector contendo todos os nomes dos players da apresentação atual.
	 */
	virtual vector<string>* getPlayersNames()=0;

	/**
	 * Retorna o estado de um determinado Player.
	 * @param name Nome do player o qual se deseja obter o estado.
	 * @return Instância de PlayerStateWac com o estado atual do player.
	 */
	virtual IElementaryState* getElementaryState(string name)=0;

	/**
	 * Retorna o Descritor associado ao nó de mídia de um player.
	 * @param Nome do Player relacionado ao nó de mídia.
	 * @return Nome do descritor associado ao nó de mídia.
	 */
	virtual string getMediaDescriptor(string name)=0;

	/**
	 * Retorna o nome completo da apresentação.
	 * @return String contendo o nome completo da apresentação;
	 */
	virtual string getPresentationName()=0;

	/**
	 * Retorna o nome do documento NCL.
	 * @return String contendo o nome do documento NCL.
	 */
	virtual string getDocumentName()=0;

	/**
	 * Retorna o nome da base privada.
	 * @return String contendo o nome da base privada.
	 */
	virtual string getPrivateBaseName()=0;

	/**
	 * Retorna uma string contento o resumo das principais informações relativas ao estado da
	 * apresentação presentados pelo objeto.
	 * @return String com o resumo do estado da apresentaçaõ.
	 */
	virtual string toString()=0;

	/**
	 * Este método retorna todos os nomes das propriedades relativas ao contexto da apresentação.
	 * @param Um vetor contento o nome das propriedades relativas ao contexto.
	 */
	virtual vector<string>* getContextPropertyNames()=0;

	/**
	 * Este método retorna o valor de uma determinada propriedade do contexto da apresentação
	 * através de uma string.
	 * @param attributeId O identificador da propriedade do contexto.
	 * @return O valor da propriedade representada através de uma string
	 */
	virtual string getContextPropertyValue(string attributeId)=0;
};


}
}
}
}
}
}


#endif /* IPRESENTATIONSTATE_H_ */
