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
* @file PrivateBaseManager.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef _PRIVATEBASEMANAGER
#define _PRIVATEBASEMANAGER

#include <map>
#include <vector>
#include <string>
using namespace std;

#include "ncl/components/PrivateBase.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "converter/DocumentManager.h"
using namespace ::br::pucrio::telemidia::converter;

#include <ginga/linceutil/LoggerUtil.h>
#include <ginga/linceutil/InitializationException.h>
using namespace br::ufscar::lince::util;

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace privateBase {

	/** Manipula e associa as PrivateBases com os DocumentManagers relacionados.
	 *
	 *	Cada PrivataBase é relacionada com um DocumentManager que gerencia os
	 *	os documentos contidos dentro daquela base. A PrivateBase padrão que é
	 *	criada sempre pelo Ginga é tratada de forma especial.
	 */
	
	class PrivateBaseManager {
		private:
			/// Instância única de PrivateBaseManager.
			static PrivateBaseManager* _instance;
			
			/**
			 * Constrói uma instância de PrivateBaseManager.
			 */
			PrivateBaseManager();

		private:
			/// Mapa que relaciona as Bases Privadas com seus Identificadores.
			map<string, PrivateBase*>* privateBases;

			/// Mapa que relaciona os Identificadores das Bases Privadas com seus DocumentManager.
			map<string, DocumentManager*>* documentManagers;

			/// DocumentManager da base privada padrão.
			DocumentManager* defaultDocumentManager;
		
		public:

			/**
			 * Garante acesso a instância única de PrivateBaseManager
			 * @return Instância única de PrivateBaseManager.
			 */
			static PrivateBaseManager* getInstance();

			/**
			 * Seta o DocumentManager da Base Privada Padrão.
			 * @param defaultDocManager DocumentManager a ser setado.
			 */
			void setDefaultDocumentManager(DocumentManager* defaultDocManager);

			/**
			 * Destrói a instância única de PrivateBaseManager.
			 */
			~PrivateBaseManager();

			/** 
			 * Cria uma PrivateBase com um determinado identificado.
			 * @param id Identicador da nova PrivateBase
			 * @return A PrivateBase criada se tudo ocorreu bem. NULL caso contrário.
			 */
			PrivateBase* createPrivateBase(string id);

			/**
			 * Retorna a Base private
			 * @param id Identicador da nova PrivateBase
			 * @return A PrivateBase com o id informado Caso o PrivateBase com determinado id não exista, retorna NULL
			 */
        	PrivateBase* getPrivateBase(string id);

			/** 
			 * Deleta a PrivateBase e o DocumentManager relacinado a ela.
			 * @param id Identicador da PrivateBase
			 * @return true se a PrivateBase foi excluído com suceddo. False caso contrário.
			 */ 
        	bool deletePrivateBase(string id);

			/**
			 * Retorna um vetor contento todas as PrivateBases (incluindo a padrão)
			 * @return vetor com todas as PrivateBases existentes
			 */
        	vector<PrivateBase*>* getPrivateBases();

			/**
			 * Retorna o DocumentManager da PrivateBase padrão
			 * @return DocumentManager da PrivateBase padrão
			 */
			DocumentManager* getDefaultDocumentManager();

			/** 
			 * Retorna o DocumentManager de uma PrivateBase
			 * @param id O identificador de uma PrivateBase
			 * @return DocumentManager Retorna o DocumentManager da PrivateBase com id informado se ela existir. NULL caso contrário.
			*/
			DocumentManager* getDocumentManagerByBaseId(string id);
	};

}
}
}
}
}

#endif /*_PRIVATEBASEMANAGER*/
