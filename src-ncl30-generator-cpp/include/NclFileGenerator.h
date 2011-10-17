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
* @file NclFileGenerator.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef _NCLFILEGENERATOR_H
#define _NCLFILEGENERATOR_H

#include <string>
using namespace std;

#include <ginga/ncl/NclDocument.h>
using namespace ::br::pucrio::telemidia::ncl;

#include <ginga/linceutil/LoggerUtil.h>
using namespace br::ufscar::lince::util;

#include "generables/NclDocumentGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {

	class NclFileGenerator {

		private:
			/// Instância responsável por tratar as mensagens de log da classe.
			HLoggerPtr logger;

		protected:
			/// Instância que representa o documento NCL cujo arquivo XML será gerado.
			NclDocument* nclDocument;
			/// String com o nome do arquivo a ser salvo.
			string fileName;
		
		public:
			/**
			 * Constrói uma instancia de NclFileGenerator.
			 * @param doc A instância que representa o documento NCL cujo arquivo será gerado.
			 * @param fileName String contento o nome do arquivo que será gerado.
			 */
			NclFileGenerator(NclDocument* doc, string fileName);

			/**
			 * Destrói a instancia de NclFileGenerator.
			 */
			~NclFileGenerator();

			/**
			 * Gerá o documento NCL.
			 * @return Verdadeiro se conseguiu Gerar; Falso caso contrário.
			 */
			bool generateFile();

	};

}
}
}
}
}

#endif //_NCLFILEGENERATOR_H
