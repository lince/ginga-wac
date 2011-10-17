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
* @file PrivateBaseManager.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/PrivateBaseManager.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace privateBase {

	PrivateBaseManager* PrivateBaseManager::_instance = NULL;

	PrivateBaseManager* PrivateBaseManager::getInstance() {
		if (_instance == NULL) {
			_instance = new PrivateBaseManager();
		}
		return _instance;
	}

	void PrivateBaseManager::setDefaultDocumentManager(
			DocumentManager* defaultDocManager) {

		if (defaultDocumentManager == NULL) {
			this->defaultDocumentManager = defaultDocManager;
		} else {
			string excepMen = "Tentativa de setar um DocumentManager padrão quando outro ";
			excepMen += "DocumentManager já foi defenido como padrão";
			InitializationException excep(excepMen);
			throw excep;
		}
	}

	PrivateBaseManager::PrivateBaseManager() {
		privateBases = new map<string, PrivateBase*>();
		documentManagers = new map<string, DocumentManager*>();
		this->defaultDocumentManager = NULL;
	}

	PrivateBaseManager::~PrivateBaseManager() {
		//iterar sobre os maps e dar clear e delete
		//dar clear e delete no default
	}

	PrivateBase* PrivateBaseManager::createPrivateBase(string id) {
		//necessidade de sincronização neste método?
		map<string, PrivateBase*>::iterator it;
		it = privateBases->find(id);
		if (it != privateBases->end()) {
			cout<<"PrivateBaseManager::createPrivateBase)"<<endl;
			cout<<"Trying to create a PrivateBase with PrivateBase id already used."<<endl;
			return it->second;
		}
		DocumentManager* newDoc = new DocumentManager(id);
		(*documentManagers)[id] = newDoc;
		PrivateBase* newPri = newDoc->getPrivateBaseContext();
		(*privateBases)[id] = newPri;
		return newPri;
	}

	PrivateBase* PrivateBaseManager::getPrivateBase(string id) {
		map<string, PrivateBase*>::iterator it;
		it = privateBases->find(id);
		if (it != privateBases->end()) {
			return it->second;
		}	
		return NULL;
	}

	bool PrivateBaseManager::deletePrivateBase(string id) {
		map<string, PrivateBase*>::iterator itPri ;
		map<string, DocumentManager*>::iterator itDoc;
		itDoc = documentManagers->find(id);
		if (itDoc == documentManagers->end()) {
			return false;
		}
		itPri = privateBases->find(id);
		if (privateBases->end() == itPri) {
			return false;
		}
		DocumentManager* docMng = itDoc->second;
		PrivateBase* privateBase = itPri->second;
		docMng->clear();
		delete docMng;
		delete privateBase;
		documentManagers->erase(id);
		privateBases->erase(id);
		return true;
	}
			
        vector<PrivateBase*>* PrivateBaseManager::getPrivateBases() {
		vector<PrivateBase*>* retValue = new vector<PrivateBase*>();
		if (defaultDocumentManager != NULL) {
			retValue->push_back(defaultDocumentManager->getPrivateBaseContext());
		}

		map<string, PrivateBase*>::iterator it;
		it = privateBases->begin();
		while (it != privateBases->end()) {
			PrivateBase* privateBase = it->second;
			if (privateBase != NULL) {
				retValue->push_back(privateBase);
			}
		}
		return retValue;
	}

	DocumentManager* PrivateBaseManager::getDefaultDocumentManager() {
		return defaultDocumentManager;
	}

	DocumentManager* PrivateBaseManager::getDocumentManagerByBaseId(string id) {
		map<string, DocumentManager*>::iterator it;
		it = documentManagers->find(id);
		if (it != documentManagers->end()) {
			DocumentManager* doc = it->second;
			return doc;
		}
		return NULL;
	}

}
}
}
}
}
