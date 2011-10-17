/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

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

#include "../include/gfx/GingaNclGfx.h"

#include "system/io/IDeviceManager.h"
#include "system/io/IGingaLocatorFactory.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
	Formatter::Formatter(string id) {
		string deviceName = "default";
		int deviceNumber, w, h;
		IComponentManager* cm = IComponentManager::getCMInstance();
		IDeviceManager* dm = NULL;
		pm = NULL;

		if (cm != NULL) {
			dm = ((DeviceManagerCreator*)(cm->getObject("DeviceManager")))();
			pm = ((PrefetchManagerCreator*)(
					cm->getObject("PrefetchManager")))();
		}

		if (dm != NULL) {
			deviceNumber = dm->createDevice(deviceName);

			w = dm->getDeviceWidth();
			h = dm->getDeviceHeight();

			DeviceLayout::getInstance()->addDevice(deviceName, w, h);

			GingaNclGfx::show(0, 0, w, h);
		}

		initializeContextProxy();

		playerManager = new PlayerAdapterManager();
		compileErrors = NULL;

		this->compiler = new FormatterConverter(this->ruleAdapter);
		scheduler = new FormatterScheduler(
			    playerManager, ruleAdapter, compiler);

		scheduler->addSchedulerListener(this);
		compiler->setScheduler(scheduler);

//#if HAVE_PRIVATEBASE
		clientPrivateBaseId = "client";
		broadcasterPrivateBaseId = id;
		documentManager = new DocumentManagerAdapter(broadcasterPrivateBaseId);
		privateBaseManager = PrivateBaseManager::getInstance();
		privateBaseManager->createPrivateBase(clientPrivateBaseId);
		
//#endif

//#if HAVE_WAC
		ClientEditingManager* clientEditingManager = NULL;
		clientEditingManager = ClientEditingManager::getInstance();
		clientEditingManager->setIFormatterAdpater(this);
//#endif

		documentEvents = new map<string, FormatterEvent*>;
		documentEntryEvents = new map<string, vector<FormatterEvent*>*>;
		formatterListeners = new set<IFormatterListener*>;

		playerManager->setNclEditListener((INclEditListener*)this);
	}

	Formatter::~Formatter() {
		//PresentationContext::getInstance()->save();
		if (playerManager != NULL) {
			playerManager->clear();
			delete playerManager;
			playerManager = NULL;
		}

		if (documentManager != NULL) {
			delete documentManager;
			documentManager = NULL;
		}

//#if HAVE_PRIVATEBASE
		if (privateBaseManager != NULL) {
			delete privateBaseManager;
			privateBaseManager = NULL;
		}
//#endif

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

		if (pm != NULL) {
			pm->release();
			delete pm;
			pm = NULL;
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

	void* Formatter::addDocument(string docLocation) {
		NclDocument* docAdded = NULL;

		if (docLocation.length() > 7 &&
				docLocation.substr(0, 7) == "http://") {

			if (pm == NULL) {
				GingaNclGfx::release();
				return NULL;
			}

			if (!pm->hasIChannel()) {
				return NULL;
			}

			docLocation = pm->createDocumentPrefetcher(docLocation);
			solveRemoteNclDeps(docLocation, true);

		} else {
			solveRemoteNclDeps(docLocation, false);
		}

		documentManager->getPrivateBaseContext();
		docAdded = documentManager->addDocument(docLocation);
		if (docAdded != NULL && prepareDocument(docAdded->getId())) {
//#IF HAVE_WAC
			addDocument(docLocation, clientPrivateBaseId);
			return docAdded;
//#ENDIF
		}

		GingaNclGfx::release();
		return NULL;
	}

//#if HAVE_PRIVATEBASE
	string Formatter::adaptDocumentId(string documentId) {
		if (documentId.find("/") == string::npos) {
			return clientPrivateBaseId + "/" + documentId;
		}
		return documentId;
	}
		
	void* Formatter::addDocument(string docLocation, string baseId) {
		NclDocument* docAdded = NULL;

		if (docLocation.length() > 7 &&
				docLocation.substr(0, 7) == "http://") {

			if (pm == NULL) {
				GingaNclGfx::release();
				return NULL;
			}

			if (!pm->hasIChannel()) {
				return NULL;
			}

			docLocation = pm->createDocumentPrefetcher(docLocation);
			solveRemoteNclDeps(docLocation, true);

		} else {
			solveRemoteNclDeps(docLocation, false);
		}

		docAdded = documentManager->addDocument(docLocation, baseId);
		if (baseId != "") {
			baseId = baseId + "/";
		}
		if (docAdded != NULL && prepareDocument(baseId + docAdded->getId())) {
			return docAdded;
		}

		GingaNclGfx::release();
		return NULL;
	}
//#endif

	bool Formatter::removeDocument(string documentId) {
		NclDocument* document;

		if (documentEvents->count(documentId) != 0) {
			stopDocument(documentId);
		}

		document = documentManager->removeDocument(documentId);
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

		nclDocument = documentManager->getDocument(documentId);
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
			delete ports;
			return NULL;
		}

//#IF HAVE_WAC
//Coloquei o documentId na chamada do getPrivateBaseContext
		contextPerspective = new NodeNesting(
			    documentManager->getPrivateBaseContext(documentId));
//#ENDIF
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

		delete ports;

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

		cout<<"Adicionando o documento com o id :"<<documentId<<endl;
		(*documentEvents)[documentId] = documentEvent;
		(*documentEntryEvents)[documentId] = entryEvents;

		/*time = getCurrentTimeMillis() - time;
		cout << "Formatter::processDocument - document process time: " << time;
		cout << endl;*/
		return true;
	}

	bool Formatter::prepareDocument(string documentId) {
		NclDocument* doc;
		string src, docLocation;
		bool isRemoteDoc;

		DescriptorBase* db;
		vector<GenericDescriptor*>* descs;

		ContextNode* body;
		vector<Node*>* nodes;

		body = getDocumentContext(documentId);
		if (body == NULL || pm == NULL) {
			// document has no body
			cout << "Formatter::prepareDocument warning! Doc '" << documentId;
			cout << "': without body!" << endl;
			return false;
		}

		docLocation = documentManager->getDocumentLocation(documentId);
		if (docLocation == "NULL") {
			return false;
		}

		isRemoteDoc = pm->hasRemoteLocation(docLocation);

		//solving remote descriptors URIs
		doc = documentManager->getDocument(documentId);
		if (doc != NULL) {
			db = doc->getDescriptorBase();
			if (db != NULL) {
				descs = db->getDescriptors();
				if (descs != NULL) {
					solveRemoteDescriptorsUris(docLocation, descs, isRemoteDoc);
				}
			}
		}

		//solving remote nodes URIs
		//cout << "Formatter::prepareDocument '" << docLocation;
		//cout << "'" << endl;
		nodes = body->getNodes();
		if (nodes != NULL) {
			solveRemoteNodesUris(docLocation, nodes, isRemoteDoc);
		}

		if (pm != NULL) {
			pm->getScheduledContents();
		}

		return true;
	}

	void Formatter::solveRemoteDescriptorsUris(string docLocation,
			vector<GenericDescriptor*>* descs, bool isRemoteDoc) {

		string src;
		GenericDescriptor* desc;
		vector<GenericDescriptor*>::iterator i;
		FocusDecoration* fd;

		i = descs->begin();
		while (i != descs->end()) {
			desc = *i;
			if (desc->instanceOf("Descriptor")) {
				fd = ((Descriptor*)desc)->getFocusDecoration();

			} else {
				fd = NULL;
			}

			if (fd != NULL) {
				src = fd->getFocusSelSrc();
				//cout << "Formatter::prepareDocument old ";
				//cout << "focusSelSrc='" << src << "'" << endl;
				if (src != "") {
					src = solveRemoteSourceUri(docLocation, src);
					fd->setFocusSelSrc(src);
				}
				//cout << "Formatter::prepareDocument new ";
				//cout << "focusSelSrc='" << src << "'" << endl;

				src = fd->getFocusSrc();
				//cout << "Formatter::prepareDocument old ";
				//cout << "focusSrc='" << src << "'" << endl;
				if (src != "") {
					src = solveRemoteSourceUri(docLocation, src);
					fd->setFocusSrc(src);
				}
				//cout << "Formatter::prepareDocument new ";
				//cout << "focusSrc='" << src << "'" << endl;
			}
			++i;
		}
	}

	void Formatter::solveRemoteNodesUris(
			string docLocation, vector<Node*>* nodes, bool isRemoteDoc) {

		CompositeNode* node;
		ContextNode* parent;
		vector<Node*>* childs;
		vector<Node*>::iterator i;
		Content* content;
		string src, mime;

		i = nodes->begin();
		while (i != nodes->end()) {
			if ((*i)->instanceOf("ContextNode")) {
				parent = (ContextNode*)(*i);
				childs = parent->getNodes();
				if (childs != NULL) {
					solveRemoteNodesUris(docLocation, childs, isRemoteDoc);
				}

			} else if ((*i)->instanceOf("CompositeNode")) {
				node = (CompositeNode*)(*i);
				childs = node->getNodes();
				if (childs != NULL) {
					solveRemoteNodesUris(docLocation, childs, isRemoteDoc);
				}

			} else {
				content = ((NodeEntity*)((*i)->getDataEntity()))->getContent();
				if (content != NULL && content->instanceOf(
						"ReferenceContent")) {

					src = ((ReferenceContent*)content)->
							getCompleteReferenceUrl();

					if (src != "") {
						mime = src.substr(
								src.find_last_of(".") + 1,
								src.length() - (src.find_last_of(".") + 1));

						if (mime == "ht" || mime == "html" ||
								mime == "htm" || mime == "xhtml") {

							src = updatePath(src);

						} else if (mime == "lua") {
							src = updatePath(solveRemoteSourceUri(
									docLocation, updatePath(src)));

							solveRemoteLuaDeps(docLocation, src, isRemoteDoc);

						} else {
							src = updatePath(solveRemoteSourceUri(
									docLocation, src));
						}

						((ReferenceContent*)content)->setReference(src);
					}
				}
			}
			++i;
		}
	}

	void Formatter::solveRemoteNclDeps(string docLocation, bool isRemoteDoc) {
		string docRoot, nclDep;
		ifstream fis;

		docRoot = docLocation.substr(0, docLocation.find_last_of("/"));
		fis.open(docLocation.c_str(), ifstream::in);
		if (!fis.is_open()) {
			cout << "Formatter::solveRemoteNclDeps Warning! Can't solve ncl";
			cout << " deps on '" << docLocation << "'" << endl;
			return;
		}

		while (fis.good()) {
			fis >> nclDep;
			if (nclDep.find("documentURI") != std::string::npos) {
				while (nclDep.find("\"") == std::string::npos && fis.good()) {
					fis >> nclDep;
				}

				if (nclDep.find("\"") != std::string::npos &&
						nclDep.find_first_of("\"") !=
							nclDep.find_last_of("\"")) {

					nclDep = nclDep.substr(
							nclDep.find_first_of("\"") + 1,
							nclDep.find_last_of("\"") -
							(nclDep.find_first_of("\"") + 1));

					if ((!isRemoteDoc &&
							nclDep.find("http://") == std::string::npos) ||
							nclDep == "") {

						continue;
					}

					nclDep = solveRemoteSourceUri(docLocation, nclDep);
					if (pm != NULL) {
						pm->getScheduledContent(nclDep);

					} else {
						cout << "Formatter::solveRemoteNclDeps Warning! ";
						cout << "PrefetchManager is NULL" << endl;
						break;
					}
				}
			}
		}

		fis.close();
	}

	void Formatter::solveRemoteLuaDeps(
			string docLocation, string src, bool isRemoteDoc) {

		string clientLuaDepsSrc, clientLuaDepsRoot, luaDep;
		ifstream fis;

		if (!isRemoteDoc && src.find("http://") == std::string::npos) {
			return;
		}

		clientLuaDepsSrc = src.substr(0, src.find_last_of(".")) + ".deps";
		clientLuaDepsSrc = solveRemoteSourceUri(docLocation, clientLuaDepsSrc);
		clientLuaDepsRoot = clientLuaDepsSrc.substr(
				0, clientLuaDepsSrc.find_last_of("/"));

		if (pm != NULL) {
			pm->getScheduledContent(clientLuaDepsSrc);

		} else {
			cout << "Formatter::solveRemoteLuaDeps Warning! PrefetchManager ";
			cout << "is NULL" << endl;
		}

		fis.open(clientLuaDepsSrc.c_str(), ifstream::in);
		if (!fis.is_open()) {
			cout << "Formatter::solveRemoteLuaDeps Warning! Can't solve lua";
			cout << " deps on '" << clientLuaDepsSrc << "'" << endl;
			return;
		}

		while (fis.good()) {
			fis >> luaDep;
			luaDep = clientLuaDepsRoot + "/" + luaDep;
			solveRemoteSourceUri(docLocation, luaDep);
		}

		fis.close();
	}

	string Formatter::solveRemoteSourceUri(string localDocUri, string src) {
		return pm->createSourcePrefetcher(localDocUri, src);
	}

	bool Formatter::startDocument(string documentId, string interfaceId) {
		vector<FormatterEvent*>* entryEvents;
		FormatterEvent* documentEvent;

//#if HAVE_WAC
		documentId = adaptDocumentId(documentId);
		NclDocument* document = documentManager->getDocument(documentId);
		ClientEditingManager* clientEditingManager = NULL;
		clientEditingManager = ClientEditingManager::getInstance();
		clientEditingManager->setCurrentDocument(document);
//#endif

		//cout << "Formatter::startDocument docId: '" << documentId;
		//cout << "', interfaceId: '" << interfaceId << "'." << endl;
		if (compileDocument(documentId, interfaceId)) {
			if (documentEvents->count(documentId) != 0) {
				GingaNclGfx::release();
				documentEvent = (*documentEvents)[documentId];
				entryEvents = (*documentEntryEvents)[documentId];
				scheduler->startDocument(documentEvent, entryEvents);
				//cout << "Formatter::startDocument return true" << endl;
				return true;
			}
		}

		GingaNclGfx::release();
		return false;
	}

	bool Formatter::stopDocument(string documentId) {
		FormatterEvent* documentEvent;

//#if HAVE_WAC
		documentId = adaptDocumentId(documentId);
//#endif

		if (pm != NULL) {
			pm->releaseContents();
		}

		if (documentEvents->count(documentId) == 0) {
			return false;
		}

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
//#if HAVE_WAC
		documentId = adaptDocumentId(documentId);
//#endif

		if (documentEvents->count(documentId) == 0) {
			return false;
		}

		FormatterEvent* documentEvent;

		documentEvent = (*documentEvents)[documentId];
		scheduler->pauseDocument(documentEvent);
		return true;
	}

	bool Formatter::resumeDocument(string documentId) {
//#if HAVE_WAC
		documentId = adaptDocumentId(documentId);
//#endif

		if (documentEvents->count(documentId) == 0) {
			return false;
		}

		FormatterEvent* documentEvent;

		documentEvent = (*documentEvents)[documentId];
		scheduler->resumeDocument(documentEvent);
		return true;
	}

	void Formatter::addFormatterListener(IFormatterListener* listener) {
		if (formatterListeners->count(listener) != 0) {
			cout << "Formatter::addFormatterListener Warning! Trying to ";
			cout << "add same listener twice" << endl;
		}
		formatterListeners->insert(listener);
	}

	void Formatter::removeFormatterListener(IFormatterListener* listener) {
		set<IFormatterListener*>::iterator i;

		i = formatterListeners->find(listener);
		if (i != formatterListeners->end()) {
			formatterListeners->erase(i);
		}
	}

	void Formatter::presentationCompleted(IFormatterEvent* documentEvent) {
		set<IFormatterListener*>::iterator i;
		string documentId;
		IFormatterListener* listener;

		documentId = ((ExecutionObject*)documentEvent->
			    getExecutionObject())->getDataObject()->getId();

		cout << "Formatter::presentationCompleted for '" << documentId;
		cout << "'" << endl;

		PresentationContext::getInstance()->save();

		if (pm != NULL) {
			pm->releaseContents();
		}

		documentEvent->removeEventListener((IEventListener*)this);
		if (documentEvents->count(documentId) != 0) {
			i = formatterListeners->begin();
			while (i != formatterListeners->end()) {
				listener = *i;
				listener->presentationCompleted(documentId);
				++i;
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

		IComponentManager* cm = IComponentManager::getCMInstance();
		IGingaLocatorFactory* glf = NULL;

		if (cm != NULL) {
			glf = ((GingaLocatorFactoryCreator*)(cm->getObject(
					"GingaLocatorFactory")))();
		}

		args = split(editingCommand, ",");
		i = args->begin();
		arg = *i;
		++i;

		cout << "Formatter::nclEdit(" << editingCommand << "): '";
		cout << args->size() << "'" << endl;

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
					if ((*i).find("\"") != std::string::npos) {
						uri = (*i).substr((*i).find_first_of("\"") + 1,
								(*i).find_last_of("\"") -
										((*i).find_first_of("\"") + 1));

						++i;
						ior = (*i).substr((*i).find_first_of("\"") + 1,
								(*i).find_last_of("\"") -
										((*i).find_first_of("\"") + 1));

					} else {
						uri = trim(*i);
						++i;
						ior = trim(*i);
					}

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
					if (glf != NULL) {
						glf->createLocator(uri, ior);
					}

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
				document = (NclDocument*)(addDocument(xmlDoc));

			} else {
				cout << "Formatter::nclEdit calling ";
				cout << "getLocation '" << docUri << "' for ior '";
				cout << docIor << "'" << endl;

				if (glf != NULL) {
					uri = glf->getLocation(docUri);
					uName = glf->getName(docIor);
					document = (NclDocument*)(addDocument(uri + uName));
				}
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

//#if HAVE_WAC
	LayoutRegion* Formatter::addRegion(
		    string documentId, string regionId, string xmlRegion, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			LayoutRegion* added1 = documentManager->addRegion(
				documentBroadCasterId, regionId, xmlRegion);

			LayoutRegion* added2 = documentManager->addRegion(
				documentClientId, regionId, xmlRegion);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}

		} else {
			return documentManager->addRegion(documentId, regionId, xmlRegion);
		}
	}

	LayoutRegion* Formatter::removeRegion(string documentId, string regionId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			LayoutRegion* added1 = documentManager->removeRegion(
				documentBroadCasterId, regionId);

			LayoutRegion* added2 = documentManager->removeRegion(
				documentClientId, regionId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeRegion(documentId, regionId);
		}
	}

	RegionBase* Formatter::addRegionBase(
		    string documentId, string xmlRegionBase, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			RegionBase* added1 = documentManager->addRegionBase(
				documentBroadCasterId, xmlRegionBase);

			RegionBase* added2 = documentManager->addRegionBase(
				documentClientId, xmlRegionBase);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addRegionBase(documentId, xmlRegionBase);
		}
	}

	RegionBase* Formatter::removeRegionBase(
		    string documentId, string regionBaseId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			RegionBase* added1 = documentManager->addRegionBase(
				documentBroadCasterId, regionBaseId);

			RegionBase* added2 = documentManager->addRegionBase(
				documentClientId, regionBaseId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeRegionBase(documentId, regionBaseId);
		}
	}

	Rule* Formatter::addRule(string documentId, string xmlRule, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			Rule* added1 = documentManager->addRule(
				documentBroadCasterId, xmlRule);

			Rule* added2 = documentManager->addRule(
				documentClientId, xmlRule);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addRule(documentId, xmlRule);
		}
	}

	Rule* Formatter::removeRule(string documentId, string ruleId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			Rule* added1 = documentManager->removeRule(
				documentBroadCasterId, ruleId);

			Rule* added2 = documentManager->removeRule(
				documentClientId, ruleId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeRule(documentId, ruleId);
		}
	}

	RuleBase* Formatter::addRuleBase(string documentId, string xmlRuleBase, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			RuleBase* added1 = documentManager->addRuleBase(
				documentBroadCasterId, xmlRuleBase);

			RuleBase* added2 = documentManager->addRuleBase(
				documentClientId, xmlRuleBase);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addRuleBase(documentId, xmlRuleBase);
		}
	}

	RuleBase* Formatter::removeRuleBase(string documentId, string ruleBaseId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			RuleBase* added1 = documentManager->removeRuleBase(
				documentBroadCasterId, ruleBaseId);

			RuleBase* added2 = documentManager->removeRuleBase(
				documentClientId, ruleBaseId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeRuleBase(documentId, ruleBaseId);
		}
	}

	Transition* Formatter::addTransition(
		    string documentId, string xmlTransition, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			Transition* added1 = documentManager->addTransition(
				documentBroadCasterId, xmlTransition);

			Transition* added2 = documentManager->addTransition(
				documentClientId, xmlTransition);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addTransition(documentId, xmlTransition);
		}
	}

	Transition* Formatter::removeTransition(
		    string documentId, string transitionId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			Transition* added1 = documentManager->removeTransition(
				documentBroadCasterId, transitionId);

			Transition* added2 = documentManager->removeTransition(
				documentClientId, transitionId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeTransition(documentId, transitionId);
		}
	}

	TransitionBase* Formatter::addTransitionBase(
		    string documentId, string xmlTransitionBase, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			TransitionBase* added1 = documentManager->addTransitionBase(
				documentBroadCasterId, xmlTransitionBase);

			TransitionBase* added2 = documentManager->addTransitionBase(
				documentClientId, xmlTransitionBase);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addTransitionBase(
			    documentId, xmlTransitionBase);
		}
	}

	TransitionBase* Formatter::removeTransitionBase(
		    string documentId, string transitionBaseId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			TransitionBase* added1 = documentManager->removeTransitionBase(
				documentBroadCasterId, transitionBaseId);

			TransitionBase* added2 = documentManager->removeTransitionBase(
				documentClientId, transitionBaseId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeTransitionBase(
				    documentId, transitionBaseId);
		}
	}

	Connector* Formatter::addConnector(string documentId, string xmlConnector, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			Connector* added1 = documentManager->addConnector(
				documentBroadCasterId, xmlConnector);

			Connector* added2 = documentManager->addConnector(
				documentClientId, xmlConnector);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addConnector(documentId, xmlConnector);
		}
	}

	Connector* Formatter::removeConnector(
		    string documentId, string connectorId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			Connector* added1 = documentManager->removeConnector(
				documentBroadCasterId, connectorId);

			Connector* added2 = documentManager->removeConnector(
				documentClientId, connectorId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeConnector(documentId, connectorId);
		}
	}

	ConnectorBase* Formatter::addConnectorBase(
		    string documentId, string xmlConnectorBase, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			ConnectorBase* added1 = documentManager->addConnectorBase(
				documentBroadCasterId, xmlConnectorBase);

			ConnectorBase* added2 = documentManager->addConnectorBase(
				documentClientId, xmlConnectorBase);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addConnectorBase(
				    documentId, xmlConnectorBase);
		}
	}

	ConnectorBase* Formatter::removeConnectorBase(
		    string documentId, string connectorBaseId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			ConnectorBase* added1 = documentManager->removeConnectorBase(
				documentBroadCasterId, connectorBaseId);

			ConnectorBase* added2 = documentManager->removeConnectorBase(
				documentClientId, connectorBaseId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeConnectorBase(
				    documentId, connectorBaseId);
		}
	}

	GenericDescriptor* Formatter::addDescriptor(
		    string documentId, string xmlDescriptor, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			GenericDescriptor* added1 = documentManager->addDescriptor(
				documentBroadCasterId, xmlDescriptor);

			GenericDescriptor* added2 = documentManager->addDescriptor(
				documentClientId, xmlDescriptor);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addDescriptor(documentId, xmlDescriptor);
		}
	}

	GenericDescriptor* Formatter::removeDescriptor(
		    string documentId, string descriptorId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			GenericDescriptor* added1 = documentManager->removeDescriptor(
				documentBroadCasterId, descriptorId);

			GenericDescriptor* added2 = documentManager->removeDescriptor(
				documentClientId, descriptorId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeDescriptor(documentId, descriptorId);
		}
	}

	DescriptorBase* Formatter::addDescriptorBase(
		    string documentId, string xmlDescriptorBase, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			DescriptorBase* added1 = documentManager->addDescriptorBase(
				documentBroadCasterId, xmlDescriptorBase);

			DescriptorBase* added2 = documentManager->addDescriptorBase(
				documentClientId, xmlDescriptorBase);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addDescriptorBase(
				    documentId, xmlDescriptorBase);
		}
	}

	DescriptorBase* Formatter::removeDescriptorBase(
		    string documentId, string descriptorBaseId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			DescriptorBase* added1 = documentManager->removeDescriptorBase(
				documentBroadCasterId, descriptorBaseId);

			DescriptorBase* added2 = documentManager->removeDescriptorBase(
				documentClientId, descriptorBaseId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeDescriptorBase(
				    documentId, descriptorBaseId);
		}
	}

	Base* Formatter::addImportBase(
		    string documentId, string docBaseId, string xmlImportBase, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			Base* added1 = documentManager->addImportBase(
				documentBroadCasterId, docBaseId, xmlImportBase);

			Base* added2 = documentManager->addImportBase(
				documentClientId, docBaseId, xmlImportBase);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}	
		} else {
			return documentManager->addImportBase(
				    documentId, docBaseId, xmlImportBase);
		}
	}

	Base* Formatter::removeImportBase(
		    string documentId, string docBaseId, string documentURI, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			Base* added1 = documentManager->removeImportBase(
				documentBroadCasterId, docBaseId, documentURI);

			Base* added2 = documentManager->removeImportBase(
				documentClientId, docBaseId, documentURI);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeImportBase(
				    documentId, docBaseId, documentURI);
		}
	}

	NclDocument* Formatter::addImportedDocumentBase(
		    string documentId, string xmlImportedDocumentBase, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			NclDocument* added1 = documentManager->addImportedDocumentBase(
				documentBroadCasterId, xmlImportedDocumentBase);

			NclDocument* added2 = documentManager->addImportedDocumentBase(
				documentClientId, xmlImportedDocumentBase);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addImportedDocumentBase(
				    documentId, xmlImportedDocumentBase);
		}
	}

	NclDocument* Formatter::removeImportedDocumentBase(
		    string documentId, string importedDocumentBaseId, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			NclDocument* added1 = documentManager->removeImportedDocumentBase(
				documentBroadCasterId, importedDocumentBaseId);

			NclDocument* added2 = documentManager->removeImportedDocumentBase(
				documentClientId, importedDocumentBaseId);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeImportedDocumentBase(
				    documentId, importedDocumentBaseId);
		}
	}

	NclDocument* Formatter::addImportNCL(
		    string documentId, string xmlImportNCL, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			NclDocument* added1 = documentManager->addImportNCL(
				documentBroadCasterId, xmlImportNCL);

			NclDocument* added2 = documentManager->addImportNCL(
				documentClientId, xmlImportNCL);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addImportNCL(documentId, xmlImportNCL);
		}
	}

	NclDocument* Formatter::removeImportNCL(
		    string documentId, string documentURI, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			NclDocument* added1 = documentManager->removeImportNCL(
				documentBroadCasterId, documentURI);

			NclDocument* added2 = documentManager->removeImportNCL(
				documentClientId, documentURI);

			if (added1 != NULL && added2 != NULL) {
				return added1;
			} else {
				return NULL;
			}
		} else {
			return documentManager->removeImportNCL(documentId, documentURI);
		}
	}
//#endif

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

//#if HAVE_WAC
	Node* Formatter::addNode(
		    string documentId, string compositeId, string xmlNode, 
			int editingType) {

		Node* node;

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			Node* added1 = documentManager->addNode(
				documentBroadCasterId, compositeId, xmlNode);

			Node* added2 = documentManager->addNode(
				documentClientId, compositeId, xmlNode);

			if (added1 != NULL && added2 != NULL) {
				node = added2;
			} else {
				node = NULL;
			}
		} else {
			node = documentManager->addNode(documentId, compositeId, xmlNode);
		}

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
		    string documentId, string compositeId, string nodeId,
			int editingType) {

		NclDocument* document;
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
		    string documentId, string nodeId, string xmlInterface, 
			int editingType) {

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			InterfacePoint* added1 = documentManager->addInterface(
				documentBroadCasterId, nodeId, xmlInterface);

			InterfacePoint* added2 = documentManager->addInterface(
				documentClientId, nodeId, xmlInterface);

			if (added1 != NULL && added2 != NULL) {
				return added2;
			} else {
				return NULL;
			}
		} else {
			return documentManager->addInterface(
				    documentId, nodeId, xmlInterface);
		}

	}
//#endif

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

//#if HAVE_WAC
	InterfacePoint* Formatter::removeInterface(
		    string documentId, string nodeId, string interfaceId, 
			int editingType) {

		NclDocument* document;
		Node* node;
		InterfacePoint* interfacePoint;

		document = documentManager->getDocument(documentId);
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
		    string documentId, string compositeId, string xmlLink, 
			int editingType) {

		Link* link;
		NclDocument* document;
		ContextNode* contextNode;

		if (editingType == IFormatterAdapter::BROADCASTER_EDITING) {
			string documentBroadCasterId = broadcasterPrivateBaseId + "/" + documentId;
			string documentClientId = clientPrivateBaseId + "/" + documentId;

			Link* added1 = documentManager->addLink(
				documentBroadCasterId, compositeId, xmlLink);

			Link* added2 = documentManager->addLink(
				documentClientId, compositeId, xmlLink);

			if (added1 != NULL && added2 != NULL) {
				link  = added2;
			} else {
				link = NULL;
			}
		} else {
			link = documentManager->addLink(documentId, compositeId, xmlLink);
		}

		if (link != NULL) {
			document = documentManager->getDocument(documentId);
			contextNode = (ContextNode*)(document->getNode(compositeId));
			if (link->instanceOf("CausalLink")) {
				compiler->addCausalLink(contextNode, (CausalLink*)link, editingType); //modificada
			}
		}
		return link;
	}


	void Formatter::removeLink(LinkComposition* composition, Link* link) {
		CompositeExecutionObject* compositeObject = NULL;

		if (composition->instanceOf("CompositeNode")) {
			
			//Este código não funciona, dá erro. Problemas provavelmente esta no cast.
			/*compositeObject = (CompositeExecutionObject*)(compiler->
				    hasExecutionObject((CompositeNode*)composition, NULL));*/

			if (compositeObject != NULL) {
				compositeObject->removeNcmLink(link);
			}
		}
		composition->removeLink(link);
	}


	Link* Formatter::removeLink(
		    string documentId, string compositeId, string linkId, 
			int editingType) {

		NclDocument* document;
		Node* node;
		ContextNode* contextNode;
		Link* link;

		document = documentManager->getDocument(documentId);
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
//#endif

	bool Formatter::setPropertyValue(
		    string documentId, string nodeId, string propertyId, string value) {

		NclDocument* document;
		Node* node;
		Anchor* anchor;
		NodeNesting* perspective;
		ExecutionObject* executionObject;
		FormatterEvent* event;
		LinkAssignmentAction* setAction;

		document = documentManager->getDocument(documentId);
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

		} catch (ObjectCreationForbiddenException* exc) {
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

	void Formatter::pushEPGEventToEPGFactory(map<string, string> t) {
		playerManager->pushEPGEventToEPGFactory(t);
	}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::ncl::IFormatter* createFormatter(
		string id) {

	return (new ::br::pucrio::telemidia::ginga::ncl::Formatter(id));
}

extern "C" void destroyFormatter(
		::br::pucrio::telemidia::ginga::ncl::IFormatter* f) {

	delete f;
}
