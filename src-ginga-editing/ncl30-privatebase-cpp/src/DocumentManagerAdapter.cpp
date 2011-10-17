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
* @file DocumentManagerAdapter.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/DocumentManagerAdapter.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace privateBase {

	//construtores destrutores
	DocumentManagerAdapter::DocumentManagerAdapter(string defaultPrivateBaseId) {
		logger = LoggerUtil::getLogger("br.ufscar.lince.ncl30.privatebase.DocumentManagerAdapter");

		pbm = PrivateBaseManager::getInstance();
		defaultDocumentManager = new DocumentManager(defaultPrivateBaseId);
		this->defaultPrivateBaseId = defaultPrivateBaseId;
		pbm->setDefaultDocumentManager(defaultDocumentManager);

		LoggerUtil_info(logger, "DocumentManagerAdapter criado.");
	}

	DocumentManagerAdapter::~DocumentManagerAdapter() {
		//TODO
	}

	bool DocumentManagerAdapter::resolveCompositeDocumentId(string compositeDocumentId, 
			string& privateBaseId, string& documentId) {

		int pos = compositeDocumentId.find('/');
		if (pos > 0) {
			privateBaseId = compositeDocumentId.substr(0, pos);
			documentId = compositeDocumentId.substr(pos +1, 
					compositeDocumentId.size() - pos);

			return true;
		} else {
			privateBaseId = "";
			documentId = compositeDocumentId;
			return false;
		}
	}

	//Métodos adicionais criados apra manipulação de bases
	NclDocument* DocumentManagerAdapter::addDocument(string location, string baseId) {
		if (baseId != "client") {
			pbm->createPrivateBase(baseId);
		}

		DocumentManager* docManager = pbm->getDocumentManagerByBaseId(
				baseId);

		if (docManager == NULL) {
			LoggerUtil_error(logger, "Erro tentando adicionar documento visivel na base privada '"<<baseId<<"'.");
		}

		return docManager->addDocument(location);
	}

	NclDocument* DocumentManagerAdapter::addVisibleDocument(string location, string baseId) {
		if (baseId != "client") {
			pbm->createPrivateBase(baseId);
		}

		DocumentManager* docManager = pbm->getDocumentManagerByBaseId(
				baseId);

		if (docManager == NULL) {
			LoggerUtil_error(logger, "Erro tentando adicionar documento na base privada '"<<baseId<<"'.");
		}

		return docManager->addVisibleDocument(location);
	}

	vector<NclDocument*>* DocumentManagerAdapter::getDocuments(string baseId) {

		DocumentManager* docManager = pbm->getDocumentManagerByBaseId(
				baseId);

		if (docManager == NULL) {
			LoggerUtil_error(logger, "Erro tentando obter os documento da base privada '"<<baseId<<"'.");
		}

		return docManager->getDocuments();
	}

	void DocumentManagerAdapter::clear(string baseId) {

		DocumentManager* docManager = pbm->getDocumentManagerByBaseId(
				baseId);

		if (docManager == NULL) {
			LoggerUtil_error(logger, "Erro tentando limpar os documento da base privada '"<<baseId<<"'.");
		}

		return docManager->clear();
	}

	//Métodos herdados para manipulação de documentos
	NclDocument* DocumentManagerAdapter::addDocument(string location) {
		defaultDocumentManager->getPrivateBaseContext();

		return defaultDocumentManager->addDocument(location);
	}

	NclDocument* DocumentManagerAdapter::addVisibleDocument(string location) {
		return defaultDocumentManager->addVisibleDocument(location);
	}

	void DocumentManagerAdapter::clear() {
		defaultDocumentManager->clear();
	}

	vector<NclDocument*>* DocumentManagerAdapter::getDocuments() {
		return defaultDocumentManager->getDocuments();
	}

	string DocumentManagerAdapter::getDocumentLocation(string compositeDocId) {
		string baseId, documentId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->getDocumentLocation(documentId);

	}

	NclDocument* DocumentManagerAdapter::getDocument(string compositeDocId) {
		string baseId, documentId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->getDocument(documentId);
	}

	NclDocument* DocumentManagerAdapter::removeDocument(string compositeDocId) {
		string baseId, documentId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeDocument(documentId);
	}


	//Métodos herdados para edição ao vivo de documentos
	LayoutRegion* DocumentManagerAdapter::addRegion(
		     string compositeDocId, string regionId, string xmlRegion) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}

		return documentManager->addRegion(documentId, regionId, xmlRegion);
	}

	LayoutRegion* DocumentManagerAdapter::removeRegion(
			string compositeDocId, string regionId) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}

		return documentManager->removeRegion(documentId, regionId);

	}

	RegionBase* DocumentManagerAdapter::addRegionBase(
			string compositeDocId, string xmlRegionBase) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}

		return documentManager->addRegionBase(documentId, xmlRegionBase);
	}

	RegionBase* DocumentManagerAdapter::removeRegionBase(
		    string compositeDocId, string regionBaseId) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}

		return documentManager->removeRegionBase(documentId, regionBaseId);

	}

	Rule* DocumentManagerAdapter::addRule(string compositeDocId, string xmlRule) {
		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}

		return documentManager->addRule(documentId, xmlRule);
	}

	Rule* DocumentManagerAdapter::removeRule(string compositeDocId, string ruleId) {
		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}

		return documentManager->removeRule(documentId, ruleId);
	}


	RuleBase* DocumentManagerAdapter::addRuleBase(
			string compositeDocId, string xmlRuleBase) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}

		return documentManager->addRuleBase(documentId, xmlRuleBase);
	}

	RuleBase* DocumentManagerAdapter::removeRuleBase(
		string compositeDocId, string ruleBaseId) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeRuleBase(documentId, ruleBaseId);
	}

	Transition* DocumentManagerAdapter::addTransition(
			string compositeDocId, string xmlTransition) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addTransition(documentId, xmlTransition);

	}

	Transition* DocumentManagerAdapter::removeTransition(
			string compositeDocId, string transitionId) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeTransition(documentId, transitionId);
	}

	TransitionBase* DocumentManagerAdapter::addTransitionBase(
		    string compositeDocId, string xmlTransitionBase) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addTransitionBase(documentId, xmlTransitionBase);
	}

	TransitionBase* DocumentManagerAdapter::removeTransitionBase(
		    string compositeDocId, string transitionBaseId) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeTransitionBase(documentId, transitionBaseId);
	}

	Connector* DocumentManagerAdapter::addConnector(
			string compositeDocId, string xmlConnector) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addConnector(documentId, xmlConnector);

	}

	Connector* DocumentManagerAdapter::removeConnector(
			string compositeDocId, string connectorId) {
		
		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeConnector(documentId, connectorId);
	}

	ConnectorBase* DocumentManagerAdapter::addConnectorBase(
		    string compositeDocId, string xmlConnectorBase) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addConnectorBase(documentId, xmlConnectorBase);
	}

	ConnectorBase* DocumentManagerAdapter::removeConnectorBase(
		    string compositeDocId, string connectorBaseId) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeConnectorBase(documentId, connectorBaseId);
	}

	GenericDescriptor* DocumentManagerAdapter::addDescriptor(
		    string compositeDocId, string xmlDescriptor) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addDescriptor(documentId, xmlDescriptor);
	}

	GenericDescriptor* DocumentManagerAdapter::removeDescriptor(
		    string compositeDocId, string descriptorId) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeDescriptor(documentId, descriptorId);
	}

	DescriptorBase* DocumentManagerAdapter::addDescriptorBase(
		    string compositeDocId, string xmlDescriptorBase) {
		
		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addDescriptorBase(documentId, xmlDescriptorBase);
	}

	DescriptorBase* DocumentManagerAdapter::removeDescriptorBase(
			string compositeDocId, string descriptorBaseId) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeDescriptorBase(documentId, descriptorBaseId);
	}

	Base* DocumentManagerAdapter::addImportBase(
		    string compositeDocId, string docBaseId, string xmlImportBase) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addImportBase(documentId, docBaseId, xmlImportBase);
	}

	Base* DocumentManagerAdapter::removeImportBase(
		    string compositeDocId, string docBaseId, string documentURI) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeImportBase(documentId, docBaseId, documentURI);

	}

	NclDocument* DocumentManagerAdapter::addImportedDocumentBase(
		    string compositeDocId, string xmlImportedDocumentBase) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addImportedDocumentBase(documentId, xmlImportedDocumentBase);
	}

	NclDocument* DocumentManagerAdapter::removeImportedDocumentBase(
		    string compositeDocId, string importedDocumentBaseId) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeImportedDocumentBase(documentId, importedDocumentBaseId);
	}

	NclDocument* DocumentManagerAdapter::addImportNCL(string compositeDocId, string xmlImportNCL) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addImportNCL(documentId, xmlImportNCL);
	}

	NclDocument* DocumentManagerAdapter::removeImportNCL(string compositeDocId, string documentURI) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->removeImportNCL(documentId, documentURI);
	}

	Node* DocumentManagerAdapter::addNode(string compositeDocId, string compositeId, string xmlNode) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addNode(documentId, compositeId, xmlNode);
	}

	InterfacePoint* DocumentManagerAdapter::addInterface(
		    string compositeDocId, string nodeId, string xmlInterface) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addInterface(documentId, nodeId, xmlInterface);
	}

	Link* DocumentManagerAdapter::addLink(string compositeDocId, string compositeId, string xmlLink) {

		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}
		return documentManager->addLink(documentId, compositeId, xmlLink);
	}

	PrivateBase* DocumentManagerAdapter::getPrivateBaseContext() {

		defaultDocumentManager->getPrivateBaseContext();
	}

	PrivateBase* DocumentManagerAdapter::getPrivateBaseContext(string compositeDocId) {
		string documentId, baseId;
		DocumentManager* documentManager;

		if (resolveCompositeDocumentId(
			compositeDocId, baseId, documentId)) {
			
			documentManager = pbm->getDocumentManagerByBaseId(
									baseId);
		} else {
			documentManager = defaultDocumentManager;
		}

		return documentManager->getPrivateBaseContext();
	}
}
}
}
}
}

