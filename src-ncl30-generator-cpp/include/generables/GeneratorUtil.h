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
* @file GeneratorUtil.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef _GENERATORUTIL_H
#define _GENERATORUTIL_H

#include "util/Color.h"
#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/reuse/ReferNode.h"
using namespace ::br::pucrio::telemidia::ncl::reuse;

#include "ncl/interfaces/PropertyAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include <string>
using namespace std;

#include "PropertyAnchorGenerator.h"
#include "AnchorGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {

class GeneratorUtil {
	public:

		/**
		 * Gera o código XML da entidade NCL Color.
		 * @param color O objeto que representa a cor cujo código deve ser gerado.
	 	 * @return Uma string contendo o código NCL gerado.
		 */
		static string getColorString(Color* color);

		/**
		 * Gera o código XML de nós que referenciam outros nós.
		 * @param referNode O nó de referência que cujo código será gerado.
		 * @param nclDocument O documento NCL a que o nó de referência pertence.
		 * @return Uma string contendo o código NCL gerado.
		 */
		static string referNodeCodeGenerate(ReferNode* referNode, 
				NclDocument* nclDocument);
};

}
}
}
}
}

#endif //_GENERATORUTIL_H

