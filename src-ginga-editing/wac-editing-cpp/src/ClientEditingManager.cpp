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
* @file ClientEditingManager.h
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/ClientEditingManager.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace editing {

	ClientEditingManager* ClientEditingManager::_instance = NULL;

	const string ClientEditingManager::CLIENT_LINK_PREFIX = "_WAC__";

	ClientEditingManager::ClientEditingManager() {
		formatter = NULL;
		editingCommands = new list<EditingCommand*>();
		mapCommands = new map<string, EditingCommand*>();
		currentDocument = NULL;
		broadcasterPrivateBaseId = "formatter";
		clientPrivateBaseId = "client";
		logger = LoggerUtil::getLogger("br.ufscar.lince.ginga.wac.editing.ClientEditingManager");
	}

	ClientEditingManager* ClientEditingManager::getInstance() {
		if (_instance == NULL) {
			_instance = new ClientEditingManager();
		}
		return _instance;
	}

	ClientEditingManager::~ClientEditingManager() {
		list<EditingCommand*>::iterator it;
		it = editingCommands->begin();
		while (it != editingCommands->end()) {
			delete *it;
			it++;
		}
		delete editingCommands;
		delete mapCommands;
	}

	NclDocument* ClientEditingManager::getCurrentDocument() {
		return currentDocument;
	}

	void ClientEditingManager::setCurrentDocument(NclDocument* 
			nclDocument) {

		this->currentDocument = nclDocument;
		this->documentPath = getDocumentPath();
	}

	bool ClientEditingManager::addRegion(string regionId, 
			LayoutRegion* layoutRegion) {

		string compositeDocumentIdCli = clientPrivateBaseId + "/" +
				currentDocument->getId();

		LayoutRegionGenerator* generator = 
			NclGenerator::getGenerableInstance(layoutRegion);

		string code = generator->generateCode();

		LayoutRegion* added = formatter->addRegion(compositeDocumentIdCli, regionId, 
			code, IFormatterAdapter::CLIENT_EDITING);

		if (added != NULL) {
			string id = regionId + "/" +  added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_REGION);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);

			LoggerUtil_info(logger, "Região '"<<added->getId()<<"' adicionada no documento '"<<
					compositeDocumentIdCli<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar a nova região ao documento '"<<
					compositeDocumentIdCli<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::addRegion(string regionId, 
			string xml) {

		string compositeDocumentIdCli = clientPrivateBaseId + "/" + 
			currentDocument->getId();


		LayoutRegion* added = formatter->addRegion(compositeDocumentIdCli, regionId, 
			xml, IFormatterAdapter::CLIENT_EDITING);


		if (added != NULL) {
			string id = regionId + "/" +  added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_REGION);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Região '"<<added->getId()<<"' adicionada no documento '"<<
					compositeDocumentIdCli<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar a nova região ao documento '"<<
					compositeDocumentIdCli<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::addRule(Rule* rule) {
		//!TODO implementar o geração de código para Rules

		return false;

	}

	bool ClientEditingManager::addRule(string xml) {
		//!TODO implementar o geração de código para Rules

		return false;

	}

	bool ClientEditingManager::removeRule(string ruleId) {
		//!TODO implementar o geração de código para Rules
		return false;

	}

	bool ClientEditingManager::addTransition(Transition* transition) {
		//!TODO implementar o geração de código para Rules
		return false;
	}

	bool ClientEditingManager::addTransition(string xml) {
		//!TODO implementar o geração de código para Rules
		return false;
	}

	bool ClientEditingManager::removeTransition(string transitionId) {
		//!TODO implementar o geração de código para Rules
		return false;
	}

	bool ClientEditingManager::addConnector(Connector* connector) {
		if (!connector->instanceOf("CausalConnector")) {
			return false;
		}

		string compositeDocumentIdCli = clientPrivateBaseId + "/" + 
			currentDocument->getId();


		CausalConnectorGenerator* generator = 
			NclGenerator::getGenerableInstance(
			(CausalConnectorGenerator*)connector);

		string code = generator->generateCode();

		Connector* added = formatter->addConnector(compositeDocumentIdCli, 
			code, IFormatterAdapter::CLIENT_EDITING);


		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_CONNECTOR);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Conector '"<<added->getId()<<"' adicionado no documento '"<<
					compositeDocumentIdCli<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar o novo conector ao documento '"<<
					compositeDocumentIdCli<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::addConnector(string xml) {
		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		Connector* added = formatter->addConnector(compositeDocumentId, 
			xml, IFormatterAdapter::CLIENT_EDITING);

		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_CONNECTOR);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Conector '"<<added->getId()<<"' adicionado no documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar o novo conector ao documento '"<<
					compositeDocumentId<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::removeConnector(string connectorId) {

		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();


		Connector* added = formatter->removeConnector(compositeDocumentId, 
			connectorId, IFormatterAdapter::CLIENT_EDITING);

		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::REMOVE_CONNECTOR);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Conector '"<<added->getId()<<"' removido do documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível removor o conector do documento '"<<
					compositeDocumentId<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::addDescriptor(GenericDescriptor* descriptor) {

		if (!descriptor->instanceOf("Descriptor")) {
			return false;
		}

		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		DescriptorGenerator* generator = 
			NclGenerator::getGenerableInstance((Descriptor*) descriptor);

		GenericDescriptor* added = formatter->addDescriptor(compositeDocumentId, 
			generator->generateCode(), IFormatterAdapter::CLIENT_EDITING);

		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_DESCRIPTOR);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Descritor '"<<added->getId()<<"' adicionado no documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar o novo descritor ao documento '"<<
					compositeDocumentId<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::addDescriptor(string xml) {

		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		GenericDescriptor* added = formatter->addDescriptor(compositeDocumentId, 
			xml, IFormatterAdapter::CLIENT_EDITING);

		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_DESCRIPTOR);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Descritor '"<<added->getId()<<"' adicionado no documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar o novo descritor ao documento '"<<
				compositeDocumentId<<"'."<<endl);
		return false;
	}


	bool ClientEditingManager::removeDescriptor(string descriptorId) {
		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		GenericDescriptor* added = formatter->removeDescriptor(compositeDocumentId, 
			descriptorId, IFormatterAdapter::CLIENT_EDITING);

		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::REMOVE_DESCRIPTOR);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Descritor '"<<added->getId()<<"' removido do documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível remover o descritor do documento '"<<
				compositeDocumentId<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::addNode(string compositeId, Node* node) {
		if (!node->instanceOf("Node")) {
			return false;
		}
		Node* added = NULL;

		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		if (node->instanceOf("ContextNode")) {
			ContextNodeGenerator* generator = 
				NclGenerator::getGenerableInstance((ContextNode*) node);

			string code = generator->generateCode(currentDocument);

			added = formatter->addNode(compositeDocumentId, compositeId, 
				code, IFormatterAdapter::CLIENT_EDITING);

		} else if (node->instanceOf("ContentNode")) {
			ContentNodeGenerator* generator = 
				NclGenerator::getGenerableInstance((ContentNode*) node);

			string code = generator->generateCode();
			added = formatter->addNode(compositeDocumentId, compositeId,
				code, IFormatterAdapter::CLIENT_EDITING);
		}

		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_NODE);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Nó '"<<added->getId()<<"' adicionado ao documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar o nó ao documento '"<<
				compositeDocumentId<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::addNode(string compositeId, string xml) {
		Node* added = NULL;

		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		added = formatter->addNode(compositeDocumentId, compositeId,
			xml, IFormatterAdapter::CLIENT_EDITING);

		LoggerUtil_info(logger, "Nó adicionado pelo formatter '"<<
				compositeDocumentId<<"'."<<endl);		

		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_NODE);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Nó '"<<added->getId()<<"' adicionado ao documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar o nó ao documento '"<<
				compositeDocumentId<<"'."<<endl);
		return false;
	}

	void ClientEditingManager::setIFormatterAdpater(IFormatterAdapter* formatter) {
		this->formatter = formatter;
	}

	bool ClientEditingManager::addInterface(string nodeId, 
			InterfacePoint* interface) {

		InterfacePoint* added = NULL;

		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		if (interface->instanceOf("Anchor")) {
			AnchorGenerator* generator = 
				NclGenerator::getGenerableInstance((Anchor*) interface);

			string code = generator->generateCode();
			added = formatter->addInterface(compositeDocumentId, nodeId, 
				code, IFormatterAdapter::CLIENT_EDITING);

		} else {
			return false;
		}

		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_INTERFACE);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Interface '"<<added->getId()<<"' adicionada ao documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar a interface ao documento '"<<
				compositeDocumentId<<"'."<<endl);
		return false;

	}


	bool ClientEditingManager::addInterface(string nodeId, 
			string xml) {

		InterfacePoint* added = NULL;

		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		added = formatter->addInterface(compositeDocumentId, nodeId, 
			xml, IFormatterAdapter::CLIENT_EDITING);

		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_INTERFACE);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Interface '"<<added->getId()<<"' adicionada ao documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar a interface ao documento '"<<
				compositeDocumentId<<"'."<<endl);
		return false;

	}

	bool ClientEditingManager::removeInterface(string nodeId, 
			string interfaceId) {
		return false;

	}

	bool ClientEditingManager::addLink(string compositeId, Link* link) {
		if (!link->instanceOf("CausalLink")) {
			return false;
		}
		Link* added = NULL;

		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		if (link->instanceOf("CausalLink")) {
			CausalLinkGenerator* generator = 
				NclGenerator::getGenerableInstance((CausalLink*) link);

			
			string code = generator->generateCode(currentDocument->getConnectorBase());

			added = formatter->addLink(compositeDocumentId, compositeId, 
				code, IFormatterAdapter::CLIENT_EDITING);

		}

		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_LINK);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Elo '"<<added->getId()<<"' adicionado ao documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar o elo ao documento '"<<
				compositeDocumentId<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::addLink(string compositeId, string xml) {
		Link* added = NULL;

		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		added = formatter->addLink(compositeDocumentId, compositeId, 
			xml, IFormatterAdapter::CLIENT_EDITING);


		if (added != NULL) {
			string id = added->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_LINK);

			command->setEntity(added);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Elo '"<<added->getId()<<"' adicionado ao documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível adicionar o elo ao documento '"<<
				compositeDocumentId<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::removeLink(string compositeId, string linkId) {
		Link* removed = NULL;

		string compositeDocumentId = clientPrivateBaseId + "/" + 
			currentDocument->getId();

		removed = formatter->removeLink(compositeDocumentId, compositeId, linkId, 
			IFormatterAdapter::CLIENT_EDITING);


		if (removed != NULL) {
			string id = removed->getId();
			EditingCommand* command = new EditingCommand(id, 
				EditingCommand::ADD_LINK);

			command->setEntity(removed);
			(*mapCommands)[id] = command;
			editingCommands->push_back(command);
			LoggerUtil_info(logger, "Elo '"<<removed->getId()<<"' removido do documento '"<<
					compositeDocumentId<<"'."<<endl);
			return true;
		}
		LoggerUtil_info(logger, "Não foi possível remover o elo do documento '"<<
				compositeDocumentId<<"'."<<endl);
		return false;
	}

	bool ClientEditingManager::undoLastEditing(int n) {
		return false;

	}


	void ClientEditingManager::setClientPrivateBaseId(string id) {
		this->clientPrivateBaseId = id;
	}	

	void ClientEditingManager::setBroadcasterPrivateBaseId(string id) {
		this->broadcasterPrivateBaseId = id;
	}

	bool ClientEditingManager::saveCurrentFile(string fileName) {
		NclFileGenerator* fileGenerator = NULL;
		fileGenerator = new NclFileGenerator(currentDocument, fileName);
		return fileGenerator->generateFile();
	}

	string ClientEditingManager::getCurrentDocumentPath() {
		return documentPath;
	}
}
}
}
}
}
}
