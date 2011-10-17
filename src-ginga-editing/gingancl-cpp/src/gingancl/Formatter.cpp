/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware 
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob 
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free 
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM 
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU 
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do 
GNU versao 2 para mais detalhes. 

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto 
com este programa; se nao, escreva para a Free Software Foundation, Inc., no 
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

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
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "../include/Formatter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
	Formatter::Formatter(string id, NclDocumentManager* docManager) {
		initializeContextProxy();
		playerManager = new PlayerAdapterManager();
		compileErrors = NULL;

		this->compiler = new FormatterConverter(this->ruleAdapter);
		scheduler = new FormatterScheduler(
			    playerManager, ruleAdapter, compiler);

		scheduler->addSchedulerListener(this);
		compiler->setScheduler(scheduler);

		nclDocumentManager = docManager;
		documentEvents = new map<string, FormatterEvent*>;
		documentEntryEvents = new map<string, vector<FormatterEvent*>*>;
		formatterListeners = new vector<FormatterListener*>;

		playerManager->setNclEditListener((NclEditListener*)this);
	}

	Formatter::~Formatter() {
		//PresentationContext::getInstance()->save();
		wclog << "Formatter::~Formatter" << endl;
		if (playerManager != NULL) {
			delete playerManager;
			playerManager = NULL;
		}

		if (nclDocumentManager != NULL) {
			delete nclDocumentManager;
			nclDocumentManager = NULL;
		}

		if (documentEvents != NULL) {
			//TODO: check if we can delete events too
			delete documentEvents;
			documentEvents = NULL;
		}

		if (documentEntryEvents != NULL) {
			//TODO: check if we can delete events too
			delete documentEntryEvents;
			documentEntryEvents = NULL;
		}

		if (ruleAdapter != NULL) {
			delete ruleAdapter;
			ruleAdapter = NULL;
		}

		if (scheduler != NULL) {
			delete scheduler;
			scheduler = NULL;
		}

		if (compiler != NULL) {
			delete compiler;
			compiler = NULL;
		}

		if (compileErrors != NULL) {
			delete compileErrors;
			compileErrors = NULL;
		}

		if (formatterListeners != NULL) {
			delete formatterListeners;
			formatterListeners = NULL;
		}
	}

	void Formatter::initializeContextProxy() {
		ruleAdapter = new RuleAdapter();
	}

	vector<string>* Formatter::getCompileErrors() {
		if (compileErrors == NULL) {
			return new vector<string>;

		} else {
			return compileErrors;
		}
	}

	NclDocument* Formatter::addDocument(string docLocation) {
		return nclDocumentManager->addDocument(docLocation);
	}

	bool Formatter::removeDocument(string documentId) {
		wclog << "Formatter::removeDocument" << endl;
		NclDocument* document;

		if (documentEvents->count(documentId) != 0) {
			stopDocument(documentId);
		}

		document = nclDocumentManager->removeDocument(documentId);
		if (document != NULL) {
			return true;

		} else {
			return false;
		}
	}

	ContextNode* Formatter::getDocumentContext(string documentId) {
		NclDocument* nclDocument;

		if (documentEvents->count(documentId) != 0) {
			return NULL;
		}

		nclDocument = nclDocumentManager->getDocument(documentId);
		if (nclDocument == NULL) {
			return NULL;
		}

		return nclDocument->getBody();
	}

	void Formatter::setDepthLevel(int level) {
		compiler->setDepthLevel(level);
	}

	int Formatter::getDepthLevel() {
		return compiler->getDepthLevel();
	}

	vector<FormatterEvent*>* Formatter::processDocument(
		    string documentId, string interfaceId) {

		vector<FormatterEvent*>* entryEvents;
		vector<Port*>* ports;
		int i, size;
		ContextNode* context;
		Port* port;
		NodeNesting* contextPerspective;
		FormatterEvent* event;

		port = NULL;

		// look for the entry point perspective
		context = getDocumentContext(documentId);
		if (context == NULL) {     
			// document has no body
			cout << "Formatter::processDocument warning! Doc '" << documentId;
			cout << "': without body!" << endl;
			return NULL;
		}

		ports = new vector<Port*>;
		if (interfaceId == "") {
			size = context->getNumPorts();
			for (i = 0; i < size; i++) {
				port = context->getPort(i);
				if (port != NULL && port->getEndInterfacePoint() != NULL &&
					    port->getEndInterfacePoint()->
					    	    instanceOf("ContentAnchor")) {

					ports->push_back(port);
				}
			}

		} else {
			port = context->getPort(interfaceId);
			if (port != NULL) {
				ports->push_back(port);
			}
		}

		if (ports->empty()) {
			// interfaceId not found
			cout << "Formatter::processDocument warning! Doc '" << documentId;
			cout << "': without interfaces" << endl;
			return NULL;
		}

		contextPerspective = new NodeNesting(
			    nclDocumentManager->getPrivateBaseContext());

		contextPerspective->insertAnchorNode(context);

		entryEvents = new vector<FormatterEvent*>;
		size = ports->size();
		for (i = 0; i < size; i++) {
			port = (*ports)[i];
			event = compiler->insertContext(contextPerspective, port);
			if (event != NULL) {
				entryEvents->push_back(event);
			}
		}

		if (entryEvents->empty()) {
			cout << "Formatter::processDocument warning! Doc '" << documentId;
			cout << "': without entry events" << endl;
			return NULL;
		}

		return entryEvents;
	}

	vector<FormatterEvent*>* Formatter::getDocumentEntryEvent(
		    string documentId) {

		if (documentEntryEvents->count(documentId) != 0) {
			return (*documentEntryEvents)[documentId];

		} else {
			return NULL;
		}
	}

	bool Formatter::compileDocument(string documentId, string interfaceId) {
		vector<FormatterEvent*>* entryEvents;
		FormatterEvent* event;
		ExecutionObject* executionObject;
		CompositeExecutionObject* parentObject;
		FormatterEvent* documentEvent;
		double time;

		time = getCurrentTimeMillis();

		if (documentEvents->count(documentId) != 0) {
			return true;
		}

		entryEvents = processDocument(documentId, interfaceId);
		if (entryEvents == NULL) {
			return false;
		}

		event = (*entryEvents)[0];
		executionObject = (ExecutionObject*)(event->getExecutionObject());
		parentObject = (CompositeExecutionObject*)(executionObject->
			    getParentObject());

		if (parentObject != NULL) {
			while (parentObject->getParentObject() != NULL) {
				executionObject = parentObject;
				parentObject = (CompositeExecutionObject*)(parentObject->
					    getParentObject());
			}
			documentEvent = executionObject->getWholeContentPresentationEvent();

		} else {
			documentEvent = event;
		}

		(*documentEvents)[documentId] = documentEvent;
		(*documentEntryEvents)[documentId] = entryEvents;

		time = getCurrentTimeMillis() - time;
		wclog << "Formatter::processDocument - document process time: " << time;
		wclog << endl;
		return true;
	}

	bool Formatter::startDocument(string documentId, string interfaceId) {
		vector<FormatterEvent*>* entryEvents;
		FormatterEvent* documentEvent;

		cout << "Formatter::startDocument docId: '" << documentId;
		cout << "', interfaceId: '" << interfaceId << "'." << endl;
		if (compileDocument(documentId, interfaceId)) {
			if (documentEvents->count(documentId) != 0) {
				documentEvent = (*documentEvents)[documentId];
				entryEvents = (*documentEntryEvents)[documentId];
				scheduler->startDocument(documentEvent, entryEvents);
				cout << "Formatter::startDocument return true" << endl;
				return true;
			}
		}

		cout << "Formatter::startDocument return false" << endl;
		return false;
	}

	bool Formatter::stopDocument(string documentId) {
		if (documentEvents->count(documentId) == 0) {
			return false;
		}

		FormatterEvent* documentEvent;

		documentEvent = (*documentEvents)[documentId];
		scheduler->stopDocument(documentEvent);
		if (documentEvents != NULL && documentEvents->count(documentId) != 0) {
			documentEvents->erase(documentEvents->find(documentId));
		}

		if (documentEntryEvents != NULL) {
			if (documentEntryEvents->count(documentId) != 0) {
				documentEntryEvents->erase(
						documentEntryEvents->find(documentId));
			}
		}

		return true;
	}

	bool Formatter::pauseDocument(string documentId) {
		if (documentEvents->count(documentId) == 0) {
			return false;
		}

		FormatterEvent* documentEvent;

		documentEvent = (*documentEvents)[documentId];
		scheduler->pauseDocument(documentEvent);
		return true;
	}

	bool Formatter::resumeDocument(string documentId) {
		if (documentEvents->count(documentId) == 0) {
			return false;
		}

		FormatterEvent* documentEvent;

		documentEvent = (*documentEvents)[documentId];
		scheduler->resumeDocument(documentEvent);
		return true;
	}

	void Formatter::addFormatterListener(FormatterListener* listener) {
		vector<FormatterListener*>::iterator i;

		for (i = formatterListeners->begin();
			    i != formatterListeners->end(); ++i) {

			if (*i == listener) {
				return;
			}
		}

		formatterListeners->push_back(listener);
	}

	void Formatter::removeFormatterListener(FormatterListener* listener) {
		vector<FormatterListener*>::iterator i;

		for (i = formatterListeners->begin();
			    i != formatterListeners->end(); ++i) {

			if (*i == listener) {
				formatterListeners->erase(i);
				return;
			}
		}
	}

	void Formatter::presentationCompleted(FormatterEvent* documentEvent) {
		int i, size;
		string documentId;
		FormatterListener* listener;

		documentId = ((ExecutionObject*)documentEvent->
			    getExecutionObject())->getDataObject()->getId();

		cout << "Formatter::presentationCompleted for '" << documentId;
		cout << "'" << endl;
		documentEvent->removeEventListener((EventListener*)this);
		if (documentEvents->count(documentId) != 0) {
			size = formatterListeners->size();
			for (i = 0; i < size; i++) {
				listener = (*formatterListeners)[i];
				listener->presentationCompleted(documentId);
			}

			if (documentEvents != NULL && !documentEvents->empty()) {
				if (documentEvents->count(documentId) != 0) {
					documentEvents->erase(documentEvents->find(documentId));
				}
			}

			if (documentEntryEvents != NULL && !documentEntryEvents->empty()) {
				if (documentEntryEvents->count(documentId) != 0) {
					documentEntryEvents->erase(
							documentEntryEvents->find(documentId));
				}
			}
		}
	}

	bool Formatter::nclEdit(string editingCommand) {
		NclDocument* document;
		vector<string>* args;
		vector<string>::iterator i;
		string arg, uri, ior, docUri, docIor, uName, docId;

		args = split(editingCommand, ",");
		i = args->begin();
		arg = *i;
		++i;

		cout << "Formatter::nclEdit(" << editingCommand << "): '" << args->size() << "'" << endl;

		//parse command
		if (arg == EC_OPEN_BASE) {
			

		} else if (arg == EC_ACTIVATE_BASE) {
			

		} else if (arg == EC_DEACTIVATE_BASE) {
			

		} else if (arg == EC_SAVE_BASE) {
			

		} else if (arg == EC_CLOSE_BASE) {
			

		} else if (arg == EC_ADD_DOCUMENT) {
			string xmlDoc = "";
			cout << "Formatter::nclEdit addDocument(" << arg << ")" << endl;
			docUri = "";
			docIor = "";
			while (i != args->end()) {
				if ((*i).find("x-sbtvd://") != std::string::npos) {
					uri = (*i).substr((*i).find_first_of("\"") + 1,
							(*i).find_last_of("\"") -
									((*i).find_first_of("\"") + 1));

					++i;
					ior = (*i).substr((*i).find_first_of("\"") + 1,
							(*i).find_last_of("\"") -
									((*i).find_first_of("\"") + 1));

					if (uri.find("x-sbtvd://") != std::string::npos) {
						uri = uri.substr(uri.find("x-sbtvd://") + 10,
								uri.length() - (uri.find("x-sbtvd://") + 10));
					}

					if (docUri == "") {
						docUri = uri;
						docIor = ior;
					}

					cout << "Formatter::nclEdit ";
					cout << " command '" << arg << "' creating locator ";
					cout << "uri '" << uri << "', ior '" << ior;
					cout << "'" << endl;
					GingaLocatorFactory::getInstance()->createLocator(uri, ior);

				} else {
					uri = *i;
					ior = uri;
					if (docUri == "") {
						docIor = uri;
						docUri = uri;
					}

					++i;
					if (i != args->end()) {
						xmlDoc = *i;
					}
				}

				++i;
			}

			if (docUri == docIor) {
				cout << "Formatter::nclEdit calling addDocument(";
				cout << xmlDoc << ")" << endl;
				document = addDocument(xmlDoc);

			} else {
				cout << "Formatter::nclEdit calling ";
				cout << "getLocation '" << docUri << "' for ior '";
				cout << docIor << "'" << endl;
	
				uri = GingaLocatorFactory::getInstance()->getLocation(docUri);
				uName = GingaLocatorFactory::getInstance()->getName(docIor);
				document = addDocument(uri + uName);
			}

			if (document != NULL) {
				cout << "Formatter::nclEdit document added '";
				cout << document->getId() << "'" << endl;
				if (docUri != docIor) {
					startDocument(document->getId(), "");
				}
			} else {
				cout << "Formatter::nclEdit Warning! doc is NULL " << endl;
			}

		} else if (arg == EC_REMOVE_DOCUMENT) {
			cout << "Formatter::nclEdit removeDocument" << endl;
			removeDocument(*i);

		} else if (arg == EC_START_DOCUMENT) {
			cout << "Formatter::nclEdit startDocument" << endl;
			string interfaceId;
			docId = *i;
			++i;

			if (args->size() == 2) {
				startDocument(docId, "");

			} else if (args->size() == 3) {
				interfaceId = *i;
				startDocument(docId, interfaceId);

			} else {
				cout << "Formatter::nclEdit warning! cant startDocument";
				cout << " wrong number of params: " << args->size() << endl;
			}

		} else if (arg == EC_STOP_DOCUMENT) {
			stopDocument(*i);

		} else if (arg == EC_PAUSE_DOCUMENT) {
			pauseDocument(*i);

		} else if (arg == EC_RESUME_DOCUMENT) {
			resumeDocument(*i);

		} else if (arg == EC_ADD_REGION) {
			cout << "Formatter::nclEdit addRegion" << endl;
			string regionId, xmlRegion;
			LayoutRegion* region;

			if (args->size() == 4) {
				docId = *i;
				++i;
				regionId = *i;
				++i;
				xmlRegion = *i;
				region = addRegion(docId, regionId, xmlRegion);
				if (region != NULL) {
					cout << "Formatter::nclEdit region added '";
					cout << region->getId() << "'" << endl;
				} else {
					cout << "Formatter::nclEdit warning! cant addregion doc='";
					cout << docId << "' regionId='" << regionId << "' xml='";
					cout << xmlRegion << "'" << endl;
				}

			} else {
				cout << "Formatter::nclEdit warning! cant addRegion";
				cout << " wrong number of params: " << args->size() << endl;
			}

		} else if (arg == EC_REMOVE_REGION) {
			string regionId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				regionId = *i;
				removeRegion(docId, regionId);
			}

		} else if (arg == EC_ADD_REGION_BASE) {
			string xmlRegionBase;
			RegionBase* regionBase;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlRegionBase = *i;
				regionBase = addRegionBase(docId, xmlRegionBase);
				if (regionBase != NULL) {
					cout << "Formatter::nclEdit regionBase added '";
					cout << regionBase->getId() << "'" << endl;

				} else {
					cout << "Formatter::nclEdit warning! cant addrgbase doc='";
					cout << docId << "' xml='" << xmlRegionBase << "'" << endl;
				}

			} else {
				cout << "Formatter::nclEdit warning! cant addRegionBase";
				cout << " wrong number of params: " << args->size() << endl;
			}

		} else if (arg == EC_REMOVE_REGION_BASE) {
			string baseId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				baseId = *i;
				removeRegionBase(docId, baseId);
			}

		} else if (arg == EC_ADD_RULE) {
			string xmlRule;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlRule = *i;
				addRule(docId, xmlRule);
			}

		} else if (arg == EC_REMOVE_RULE) {
			string ruleId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				ruleId = *i;
				removeRule(docId, ruleId);
			}

		} else if (arg == EC_ADD_RULE_BASE) {
			string xmlRuleBase;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlRuleBase = *i;
				addRuleBase(docId, xmlRuleBase);
			}

		} else if (arg == EC_REMOVE_RULE_BASE) {
			string baseId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				baseId = *i;
				removeRuleBase(docId, baseId);
			}

		} else if (arg == EC_ADD_CONNECTOR) {
			string xmlConnector;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlConnector = *i;
				addConnector(docId, xmlConnector);
			}

		} else if (arg == EC_REMOVE_CONNECTOR) {
			string connectorId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				connectorId = *i;
				removeConnector(docId, connectorId);
			}

		} else if (arg == EC_ADD_CONNECTOR_BASE) {
			cout << "Formatter::nclEdit addConnectorBase" << endl;
			string xmlConnectorBase;
			ConnectorBase* connBase;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlConnectorBase = *i;
				connBase = addConnectorBase(docId, xmlConnectorBase);
				if (connBase != NULL) {
					cout << "Formatter::nclEdit connBase added '";
					cout << connBase->getId() << "'" << endl;
				} else {
					cout << "Formatter::nclEdit warning! cant addconnBs doc='";
					cout << docId << "' xmlConnBs='" << xmlConnectorBase;
					cout << "'" << endl;
				}

			} else {
				cout << "Formatter::nclEdit warning! cant addConnBase";
				cout << " wrong number of params: " << args->size() << endl;
			}

		} else if (arg == EC_REMOVE_CONNECTOR_BASE) {
			string baseId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				baseId = *i;
				removeConnectorBase(docId, baseId);
			}

		} else if (arg == EC_ADD_DESCRIPTOR) {
			cout << "Formatter::nclEdit addDescriptor" << endl;
			string xmlDescriptor;
			GenericDescriptor* desc;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlDescriptor = *i;
				desc = addDescriptor(docId, xmlDescriptor);
				if (desc != NULL) {
					cout << "Formatter::nclEdit descriptor added '";
					cout << desc->getId() << "'" << endl;
				} else {
					cout << "Formatter::nclEdit warning! cant addDesc doc='";
					cout << docId << "' xmlDesc='" << xmlDescriptor;
					cout << "'" << endl;
				}

			} else {
				cout << "Formatter::nclEdit warning! cant addDesc";
				cout << " wrong number of params: " << args->size() << endl;
			}

		} else if (arg == EC_REMOVE_DESCRIPTOR) {
			string descId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				descId = *i;
				removeDescriptor(docId, descId);
			}

		} else if (arg == EC_ADD_DESCRIPTOR_SWITCH) {
			string xmlDesc;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlDesc = *i;
				addDescriptor(docId, xmlDesc);
			}

		} else if (arg == EC_REMOVE_DESCRIPTOR_SWITCH) {
			string descId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				descId = *i;
				removeDescriptor(docId, descId);
			}

		} else if (arg == EC_ADD_DESCRIPTOR_BASE) {
			cout << "Formatter::nclEdit addDescriptorBase" << endl;
			string xmlDescBase;
			DescriptorBase* dBase;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlDescBase = *i;
				dBase = addDescriptorBase(docId, xmlDescBase);
				if (dBase != NULL) {
					cout << "Formatter::nclEdit dBase added '";
					cout << dBase->getId() << "'" << endl;
				} else {
					cout << "Formatter::nclEdit warning! cant adddBase doc='";
					cout << docId << "' xmlDescBase='";
					cout << xmlDescBase << "'" << endl;
				}

			} else {
				cout << "Formatter::nclEdit warning! cant addDescBase";
				cout << " wrong number of params: " << args->size() << endl;
			}

		} else if (arg == EC_REMOVE_DESCRIPTOR_BASE) {
			string baseId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				baseId = *i;
				removeDescriptorBase(docId, baseId);
			}

		} else if (arg == EC_ADD_TRANSITION) {
			string xmlTrans;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlTrans = *i;
				addTransition(docId, xmlTrans);
			}

		} else if (arg == EC_REMOVE_TRANSITION) {
			string transId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				transId = *i;
				removeTransition(docId, transId);
			}

		} else if (arg == EC_ADD_TRANSITION_BASE) {
			string xmlTransBase;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlTransBase = *i;
				addTransitionBase(docId, xmlTransBase);
			}

		} else if (arg == EC_REMOVE_TRANSITION_BASE) {
			string baseId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				baseId = *i;
				removeTransitionBase(docId, baseId);
			}

		} else if (arg == EC_ADD_IMPORT_BASE) {
			string docBaseId, xmlImportBase;

			if (args->size() == 4) {
				docId = *i;
				++i;
				docBaseId = *i;
				++i;
				xmlImportBase = *i;
				addImportBase(docId, docBaseId, xmlImportBase);
			}

		} else if (arg == EC_REMOVE_IMPORT_BASE) {
			string docBaseId;

			if (args->size() == 4) {
				docId = *i;
				++i;
				docBaseId = *i;
				++i;
				docUri = *i;
				removeImportBase(docId, docBaseId, docUri);
			}

		} else if (arg == EC_ADD_IMPORTED_DOCUMENT_BASE) {
			string xmlImportedBase;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlImportedBase = *i;
				addImportedDocumentBase(docId, xmlImportedBase);
			}

		} else if (arg == EC_REMOVE_IMPORTED_DOCUMENT_BASE) {
			string baseId;

			if (args->size() == 3) {
				docId = *i;
				++i;
				baseId = *i;
				removeImportedDocumentBase(docId, baseId);
			}

		} else if (arg == EC_ADD_IMPORT_NCL) {
			string xmlImportNcl;

			if (args->size() == 3) {
				docId = *i;
				++i;
				xmlImportNcl = *i;
				addImportNCL(docId, xmlImportNcl);
			}

		} else if (arg == EC_REMOVE_IMPORT_NCL) {
			if (args->size() == 3) {
				docId = *i;
				++i;
				docUri = *i;
				removeImportNCL(docId, docUri);
			}

		} else if (arg == EC_ADD_NODE) {
			cout << "Formatter::nclEdit addNode" << endl;
			string compositeId, xmlNode;
			Node* n;

			if (args->size() == 4) {
				docId = *i;
				++i;
				compositeId = *i;
				++i;
				xmlNode = *i;
				n = addNode(docId, compositeId, xmlNode);
				if (n != NULL) {
					cout << "Formatter::nclEdit node added '";
					cout << n->getId() << "'" << endl;
				} else {
					cout << "Formatter::nclEdit warning! cant addnode doc='";
					cout << docId << "' compositeId='" << compositeId;
					cout << "' xmlNode='";
					cout << xmlNode << "'" << endl;
				}

			} else {
				cout << "Formatter::nclEdit warning! cant addNode";
				cout << " wrong number of params: " << args->size() << endl;
			}

		} else if (arg == EC_REMOVE_NODE) {
			string compositeId, nodeId;

			if (args->size() == 4) {
				docId = *i;
				++i;
				compositeId = *i;
				++i;
				nodeId = *i;
				removeNode(docId, compositeId, nodeId);
			}

		} else if (arg == EC_ADD_INTERFACE) {
			cout << "Formatter::nclEdit addInterface" << endl;
			string nodeId, xmlInterface;
			InterfacePoint* intPoint;

			if (args->size() == 4) {
				docId = *i;
				++i;
				nodeId = *i;
				++i;
				xmlInterface = *i;
				intPoint = addInterface(docId, nodeId, xmlInterface);
				cout << "Formatter::nclEdit addInterf doc='";
				cout << docId << "' nodeId='" << nodeId << "' xml='";
				cout << xmlInterface << "'" << endl;
				if (intPoint != NULL) {
					cout << "Formatter::nclEdit interface added '";
					cout << intPoint->getId() << "'" << endl;

				} else {
					cout << "Formatter::nclEdit warning! cant addInterf doc='";
					cout << docId << "' nodeId='" << nodeId << "' xml='";
					cout << xmlInterface << "'" << endl;
				}

			} else {
				cout << "Formatter::nclEdit warning! cant addInterface";
				cout << " wrong number of params: " << args->size() << endl;
			}

		} else if (arg == EC_REMOVE_INTERFACE) {
			string nodeId, interfaceId;

			if (args->size() == 4) {
				docId = *i;
				++i;
				nodeId = *i;
				++i;
				interfaceId = *i;
				removeInterface(docId, nodeId, interfaceId);
			}

		} else if (arg == EC_ADD_LINK) {
			string compositeId, xmlLink;

			if (args->size() == 4) {
				docId = *i;
				++i;
				compositeId = *i;
				++i;
				xmlLink = *i;
				addLink(docId, compositeId, xmlLink);
			}

		} else if (arg == EC_REMOVE_LINK) {
			string compositeId, linkId;

			if (args->size() == 4) {
				docId = *i;
				++i;
				compositeId = *i;
				++i;
				linkId = *i;
				removeLink(docId, compositeId, linkId);
			}

		} else if (arg == EC_SET_PROPERTY_VALUE) {
			string nodeId, propId, value;

			if (args->size() == 5) {
				docId = *i;
				++i;
				nodeId = *i;
				++i;
				propId = *i;
				++i;
				value = *i;
				setPropertyValue(docId, nodeId, propId, value);
			}
		}

		delete args;
		args = NULL;
	}

	LayoutRegion* Formatter::addRegion(
		    string documentId, string regionId, string xmlRegion) {

		return nclDocumentManager->addRegion(documentId, regionId, xmlRegion);
	}

	LayoutRegion* Formatter::removeRegion(string documentId, string regionId) {
		return nclDocumentManager->removeRegion(documentId, regionId);
	}

	RegionBase* Formatter::addRegionBase(
		    string documentId, string xmlRegionBase) {

		return nclDocumentManager->addRegionBase(documentId, xmlRegionBase);
	}

	RegionBase* Formatter::removeRegionBase(
		    string documentId, string regionBaseId) {

		return nclDocumentManager->removeRegionBase(documentId, regionBaseId);
	}

	Rule* Formatter::addRule(string documentId, string xmlRule) {
		return nclDocumentManager->addRule(documentId, xmlRule);
	}

	Rule* Formatter::removeRule(string documentId, string ruleId) {
		return nclDocumentManager->removeRule(documentId, ruleId);
	}

	RuleBase* Formatter::addRuleBase(string documentId, string xmlRuleBase) {
		return nclDocumentManager->addRuleBase(documentId, xmlRuleBase);
	}

	RuleBase* Formatter::removeRuleBase(string documentId, string ruleBaseId) {
		return nclDocumentManager->removeRuleBase(documentId, ruleBaseId);
	}

	Transition* Formatter::addTransition(
		    string documentId, string xmlTransition) {

		return nclDocumentManager->addTransition(documentId, xmlTransition);
	}

	Transition* Formatter::removeTransition(
		    string documentId, string transitionId) {

		return nclDocumentManager->removeTransition(documentId, transitionId);
	}

	TransitionBase* Formatter::addTransitionBase(
		    string documentId, string xmlTransitionBase) {

		return nclDocumentManager->addTransitionBase(
			    documentId, xmlTransitionBase);
	}

	TransitionBase* Formatter::removeTransitionBase(
		    string documentId, string transitionBaseId) {

		return nclDocumentManager->removeTransitionBase(
			    documentId, transitionBaseId);
	}

	Connector* Formatter::addConnector(string documentId, string xmlConnector) {
		return nclDocumentManager->addConnector(documentId, xmlConnector);
	}

	Connector* Formatter::removeConnector(
		    string documentId, string connectorId) {

		return nclDocumentManager->removeConnector(documentId, connectorId);
	}

	ConnectorBase* Formatter::addConnectorBase(
		    string documentId, string xmlConnectorBase) {

		return nclDocumentManager->addConnectorBase(
			    documentId, xmlConnectorBase);
	}

	ConnectorBase* Formatter::removeConnectorBase(
		    string documentId, string connectorBaseId) {

		return nclDocumentManager->removeConnectorBase(
			    documentId, connectorBaseId);
	}

	GenericDescriptor* Formatter::addDescriptor(
		    string documentId, string xmlDescriptor) {

		return nclDocumentManager->addDescriptor(documentId, xmlDescriptor);
	}

	GenericDescriptor* Formatter::removeDescriptor(
		    string documentId, string descriptorId) {

		return nclDocumentManager->removeDescriptor(documentId, descriptorId);
	}

	DescriptorBase* Formatter::addDescriptorBase(
		    string documentId, string xmlDescriptorBase) {

		return nclDocumentManager->addDescriptorBase(
			    documentId, xmlDescriptorBase);
	}

	DescriptorBase* Formatter::removeDescriptorBase(
		    string documentId, string descriptorBaseId) {

		return nclDocumentManager->removeDescriptorBase(
			    documentId, descriptorBaseId);
	}

	Base* Formatter::addImportBase(
		    string documentId, string docBaseId, string xmlImportBase) {

		return nclDocumentManager->addImportBase(
			    documentId, docBaseId, xmlImportBase);
	}

	Base* Formatter::removeImportBase(
		    string documentId, string docBaseId, string documentURI) {

		return nclDocumentManager->removeImportBase(
			    documentId, docBaseId, documentURI);
	}

	NclDocument* Formatter::addImportedDocumentBase(
		    string documentId, string xmlImportedDocumentBase) {

		return nclDocumentManager->addImportedDocumentBase(
			    documentId, xmlImportedDocumentBase);
	}

	NclDocument* Formatter::removeImportedDocumentBase(
		    string documentId, string importedDocumentBaseId) {

		return nclDocumentManager->removeImportedDocumentBase(
			    documentId, importedDocumentBaseId);
	}

	NclDocument* Formatter::addImportNCL(
		    string documentId, string xmlImportNCL) {

		return nclDocumentManager->addImportNCL(documentId, xmlImportNCL);
	}

	NclDocument* Formatter::removeImportNCL(
		    string documentId, string documentURI) {

		return nclDocumentManager->removeImportNCL(documentId, documentURI);
	}

	void Formatter::processInsertedReferNode(ReferNode* referNode) {
		NodeEntity* nodeEntity;
		ExecutionObject* executionObject;
		CompositeExecutionObject* parentObject;
		int depthLevel;
		string instanceType;

		nodeEntity = (NodeEntity*)(referNode->getDataEntity());
		instanceType = referNode->getInstanceType();
		if (nodeEntity->instanceOf("ContentNode") &&
				instanceType == "instSame") {

			executionObject = compiler->hasExecutionObject(nodeEntity, NULL);
			if (executionObject != NULL) {
				parentObject = compiler->addSameInstance(
					    executionObject, referNode);

				if (parentObject != NULL) {
					depthLevel = compiler->getDepthLevel();
					if (depthLevel > 0) {
						depthLevel = depthLevel - 1;
					}

					compiler->compileExecutionObjectLinks(
						    executionObject,
						    referNode,
						    parentObject,
						    depthLevel);
				}

			} else {
				cout << "Formatter::processInsertedReferNode referred object";
				cout << " is NULL for '";
				cout << nodeEntity->getId() << "' instType = '";
				cout << instanceType << "'" << endl;
			}

		} else {
			cout << "Formatter::processInsertedReferNode referred object";
			cout << " for '" << nodeEntity->getId() << "' instType = '";
			cout << instanceType << "'" << endl;
		}
	}

	void Formatter::processInsertedComposition(CompositeNode* composition) {
		vector<Node*>* nodes;
		
		nodes = composition->getNodes();
		if (nodes != NULL) {
			vector<Node*>::iterator i;
			i = nodes->begin();
			while (i != nodes->begin()) {
				if ((*i)->instanceOf("ReferNode")) {
					processInsertedReferNode((ReferNode*)(*i));

				} else if ((*i)->instanceOf("CompositeNode")) {
					processInsertedComposition((CompositeNode*)(*i));
				}
			}
		}
	}

	Node* Formatter::addNode(
		    string documentId, string compositeId, string xmlNode) {

		Node* node;

		node = nclDocumentManager->addNode(documentId, compositeId, xmlNode);
		if (node == NULL) {
			return NULL;
		}

		if (node->instanceOf("ReferNode")) {
			processInsertedReferNode((ReferNode*)node);

		} else if (node->instanceOf("CompositeNode")) {
			// look for child nodes with refer and newInstance=false
			processInsertedComposition((CompositeNode*)node);
		}

		return node;
	}

	Node* Formatter::removeNode(
		    string documentId, string compositeId, string nodeId) {

		NclDocument* document;

		document = nclDocumentManager->getDocument(documentId);
		if (document == NULL) {
			// document does not exist
			return NULL;
		}

		Node* parentNode;

		parentNode = document->getNode(compositeId);
		if (parentNode == NULL || !(parentNode->instanceOf("ContextNode"))) {
			// composite node (compositeId) does exist or is not a context node
			return NULL;
		}

		Node* node;
		CompositeNode* compositeNode;

		compositeNode = (ContextNode*)parentNode;
		node = compositeNode->getNode(nodeId);
		if (node == NULL) {
			// node (nodeId) is not a compositeId child node
			return NULL;
		}

		ExecutionObject* executionObject;
		vector<Anchor*>* nodeInterfaces;
		vector<Anchor*>* anchors;
		vector<Anchor*>::iterator it;
		vector<Port*>* ports;
		vector<Port*>::iterator j;
		int i, size;
		InterfacePoint* nodeInterface;

		// remove all node interfaces
		nodeInterfaces = new vector<Anchor*>;

		anchors = node->getAnchors();
		if (anchors != NULL) {
			it = anchors->begin();
			while (it != anchors->end()) {
				nodeInterfaces->push_back(*it);
				++it;
			}
		}

		if (node->instanceOf("CompositeNode")) {
			ports = ((CompositeNode*)node)->getPorts();
			if (ports != NULL) {
				j = ports->begin();
				while (j != ports->end()) {
					nodeInterfaces->push_back((Anchor*)(*j));
				}
			}
		}

		size = nodeInterfaces->size();
		for (i = 0; i < size; i++) {
			nodeInterface = (*nodeInterfaces)[i];
			removeInterface(node, nodeInterface);
		}

		// remove the execution object
		executionObject = compiler->hasExecutionObject(node, NULL);
		if (executionObject != NULL) {
			if (node->instanceOf("ReferNode") &&
				    ((ReferNode*)node)->getInstanceType() != "new") {

				// remove the object entry
				compiler->removeExecutionObject(
					    executionObject, (ReferNode*)node);

			} else {
				// remove the whole execution object
				if (executionObject->getMainEvent() != NULL) {
					scheduler->stopEvent(executionObject->getMainEvent());
				}
				compiler->removeExecutionObject(executionObject);
			}
		}

		compositeNode->removeNode(node);
		return node;
	}

	InterfacePoint* Formatter::addInterface(
		    string documentId, string nodeId, string xmlInterface) {

		return nclDocumentManager->addInterface(
			    documentId, nodeId, xmlInterface);

	}

	void Formatter::removeInterfaceMappings(
		    Node* node,
		    InterfacePoint* interfacePoint,
		    CompositeNode* composition) {

		vector<Port*>* portsToBeRemoved;
		vector<Port*>* ports;
		vector<Port*>* mappings;
		vector<Port*>::iterator j, k;

		int i, size;
		Port* port;
		Port* mapping;

		portsToBeRemoved = new vector<Port*>;

		ports = composition->getPorts();
		if (ports != NULL) {
			j = ports->begin();
			while (j != ports->end()) {
				port = *j;

				if (port->instanceOf("SwitchPort")) {
					mappings = ((SwitchPort*)port)->getPorts();
					if (mappings != NULL) {
						k = mappings->begin();
						while (k != mappings->end()) {
							mapping = (Port*)(*k);
							if (mapping->getNode() == node &&
								    mapping->getInterfacePoint() ==
								    interfacePoint) {

								portsToBeRemoved->push_back(port);
								break;
							}
							++k;
						}
					}

				} else if (port->getNode() == node &&
							port->getInterfacePoint() == interfacePoint) {

					portsToBeRemoved->push_back(port);
				}
				++j;
			}
		}

		size = portsToBeRemoved->size();
		for (i = 0; i < size; i++) {
			port = (Port*)((*portsToBeRemoved)[i]);
			removeInterface(composition, port);
		}
	}

	void Formatter::removeInterfaceLinks(
		    Node* node,
		    InterfacePoint* interfacePoint,
		    LinkComposition* composition) {

		vector<Link*>* linksToBeRemoved;
		vector<Link*>* links;
		vector<Link*>::iterator j;
		vector<Bind*>* binds;
		vector<Bind*>::iterator k;

		Link* link;
		Bind* bind;
		int i, size;

		linksToBeRemoved = new vector<Link*>;
		links = composition->getLinks();
		if (links != NULL) {
			j = links->begin();
			while (j != links->end()) {
				link = (Link*)(*j);
				// verify if node and interface point participate in link
				binds = link->getBinds();
				if (binds != NULL) {
					k = binds->begin();
					while (k != binds->end()) {
						bind = (Bind*)(*k);
						if (bind->getNode() == node && 
								bind->getInterfacePoint() == interfacePoint) {

							linksToBeRemoved->push_back(link);
							break;
						}
						++k;
					}
				}
				++j;
			}
		}

		size = linksToBeRemoved->size();
		for (i = 0; i < size; i++) {
			link = (Link*)((*linksToBeRemoved)[i]);
			removeLink(composition, link);
		}
	}

	void Formatter::removeInterface(
		    Node* node, InterfacePoint* interfacePoint) {

		CompositeNode* parentNode;

		parentNode = (CompositeNode*)(node->getParentComposition());
		removeInterfaceMappings(node, interfacePoint, parentNode);

		if (parentNode != NULL && parentNode->instanceOf("LinkComposition")) {
			removeInterfaceLinks(
				    node, interfacePoint, (LinkComposition*)parentNode);
		}

		if (interfacePoint->instanceOf("Anchor")) {
			node->removeAnchor((Anchor*)interfacePoint);

		} else {
			((CompositeNode*)node)->removePort((Port*)interfacePoint);
			//TODO verify if a special treatment is necessary for switch ports
		}
	}

	InterfacePoint* Formatter::removeInterface(
		    string documentId, string nodeId, string interfaceId) {

		NclDocument* document;
		Node* node;
		InterfacePoint* interfacePoint;

		document = nclDocumentManager->getDocument(documentId);
		if (document == NULL) {
			// document does not exist
			return NULL;
		}

		node = document->getNode(nodeId);
		if (node == NULL) {
			// node (nodeId) does not exist
			return NULL;
		}

		interfacePoint = node->getAnchor(interfaceId);
		if (interfacePoint == NULL && node->instanceOf("CompositeNode")) {
			interfacePoint = ((CompositeNode*)node)->getPort(interfaceId);
		}

		if (interfacePoint == NULL) {
			// interface (interfaceId) does not exist or
			// does not pertain to node
			return NULL;
		}

		removeInterface(node, interfacePoint);
		return interfacePoint;
	}

	Link* Formatter::addLink(
		    string documentId, string compositeId, string xmlLink) {

		Link* link;
		NclDocument* document;
		ContextNode* contextNode;

		link = nclDocumentManager->addLink(documentId, compositeId, xmlLink);
		if (link != NULL) {
			document = nclDocumentManager->getDocument(documentId);
			contextNode = (ContextNode*)(document->getNode(compositeId));
			if (link->instanceOf("CausalLink")) {
				compiler->addCausalLink(contextNode, (CausalLink*)link);
			}
		}
		return link;
	}

	void Formatter::removeLink(LinkComposition* composition, Link* link) {
		CompositeExecutionObject* compositeObject;

		if (composition->instanceOf("CompositeNode")) {
			compositeObject = (CompositeExecutionObject*)(compiler->
				    hasExecutionObject((CompositeNode*)composition, NULL));

			if (compositeObject != NULL) {
				compositeObject->removeNcmLink(link);
			}
		}
		composition->removeLink(link);
	}

	Link* Formatter::removeLink(
		    string documentId, string compositeId, string linkId) {

		NclDocument* document;
		Node* node;
		ContextNode* contextNode;
		Link* link;

		document = nclDocumentManager->getDocument(documentId);
		if (document == NULL) {
			// document does not exist
			return NULL;
		}

		node = document->getNode(compositeId);
		if (node == NULL || !(node->instanceOf("ContextNode"))) {
			// composite node (compositeId) does exist or is not a context node
			return NULL;
		}

		contextNode = (ContextNode*)node;
		link = contextNode->getLink(linkId);
		if (link == NULL) {
			// link (linkId) is not a nodeId child link
			return NULL;
		}

		removeLink(contextNode, link);
		return link;
	}

	bool Formatter::setPropertyValue(
		    string documentId, string nodeId, string propertyId, string value) {

		NclDocument* document;
		Node* node;
		Anchor* anchor;
		NodeNesting* perspective;
		ExecutionObject* executionObject;
		FormatterEvent* event;
		LinkAssignmentAction* setAction;

		document = nclDocumentManager->getDocument(documentId);
		if (document == NULL) {
			// document does not exist
			return false;
		}

		node = document->getNode(nodeId);
		if (node == NULL) {
			// node (nodeId) does exist
			return false;
		}

		anchor = node->getAnchor(propertyId);
		if (!(anchor->instanceOf("PropertyAnchor"))) {
			// interface (interfaceId) is not a property
			return false;
		}

		perspective = new NodeNesting(node->getPerspective());
		try {
			executionObject = compiler->getExecutionObject(
				    perspective, NULL, compiler->getDepthLevel());

			wclog << "Formatter::setPropertyValue compiler->getExecutionObject";
			wclog << ": '" << executionObject->getId().c_str() << "'" << endl;

		} catch (ObjectCreationForbiddenException* exc) {
			wclog << "Formatter::setPropertyValue catched ";
			wclog << "ObjectCreationForbiddenException" << endl;
			return false;
		}

		event = compiler->getEvent(
			    executionObject, anchor, EventUtil::EVT_ATTRIBUTION, "");

		if (event == NULL || !(event->instanceOf("AttributionEvent"))) {
			return false;
		}

		setAction = new LinkAssignmentAction(
			    (AttributionEvent*)event,
				SimpleAction::ACT_SET,
				value);

		((LinkActionListener*)scheduler)->runAction(setAction);
		return true;
	}

}
}
}
}
}
