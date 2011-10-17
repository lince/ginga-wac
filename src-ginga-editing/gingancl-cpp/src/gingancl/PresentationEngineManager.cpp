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

#include "../include/PresentationEngineManager.h"

struct event {
	PresentationEngineManager* p;
	int code;
#if HAVE_TUNER
	Tuner* tuner;
#endif
};

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
	PresentationEngineManager::PresentationEngineManager(
		    Formatter* formatter, bool standAlone) {

		this->formatter = formatter;
		this->formatter->addFormatterListener(this);

		set<int>* keys;
		keys = new set<int>;

		keys->insert(CodeMap::KEY_F9);
		keys->insert(CodeMap::KEY_F10);
		keys->insert(CodeMap::KEY_PLAY);

		keys->insert(CodeMap::KEY_F11);
		keys->insert(CodeMap::KEY_STOP);

		keys->insert(CodeMap::KEY_F12);
		keys->insert(CodeMap::KEY_PAUSE);

#if HAVE_TUNER
		this->tuner = NULL;
		keys->insert(CodeMap::KEY_PAGE_UP);
		keys->insert(CodeMap::KEY_PAGE_DOWN);
		keys->insert(CodeMap::KEY_CHANNEL_UP);
		keys->insert(CodeMap::KEY_CHANNEL_DOWN);
#endif

		InputManager::getInstance()->addInputEventListener(this, keys);

		standAloneApp = standAlone;
		currentFile = "";
		currentDocument = NULL;
		isLocalNcl = true;

		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&condition, NULL);
	}

	PresentationEngineManager::~PresentationEngineManager() {
		close();
	}

	void PresentationEngineManager::close() {
		stopPresentation();
		if (formatter != NULL) {
			delete formatter;
			formatter = NULL;
		}

		pthread_mutex_unlock(&mutex);
		pthread_mutex_destroy(&mutex);

		pthread_cond_destroy(&condition);
	}

	bool PresentationEngineManager::getIsLocalNcl() {
		return this->isLocalNcl;
	}

#if HAVE_TUNER
	void PresentationEngineManager::setIsLocalNcl(bool isLocal, Tuner* tuner) {
		this->tuner = tuner;
#else
	void PresentationEngineManager::setIsLocalNcl(bool isLocal) {
#endif
		this->isLocalNcl = isLocal;
	}

	void PresentationEngineManager::openNclFile(string fname) {
		if (currentDocument != NULL) {
			formatter->removeDocument(currentDocument->getId());
		}

		if (isAbsolutePath(fname)) {
			currentFile = fname;

		} else {
			currentFile = getCurrentPath() + fname;
		}

		double time;
		time = getCurrentTimeMillis();
		currentDocument = formatter->addDocument(currentFile);
		time = getCurrentTimeMillis() - time;

		wclog << "compile time: " << time << " ms" << endl;
	}

#if HAVE_GINGAJ
	void PresentationEngineManager::openNclDocument(
			string docUri, int x, int y, int w, int h) {

		cout << "PresentationEngineManager::openNclDocument docUri '";
		cout << docUri << "' x = '" << x << "', y = '" << y << "', w = '";
		cout << w << "', h = '" << h << "'" << endl;
	}
#endif

#if HAVE_DSMCC
	void PresentationEngineManager::objectMounted(
			string ior, string clientUri, string name) {

		cout << "PresentationEngineManager::objectMounted ior '" << ior;
		cout << "' clientUri '" << clientUri << "'" << endl;
		GingaLocatorFactory::getInstance()->addClientLocation(
				ior, clientUri, name);
	}

	void PresentationEngineManager::receiveStreamEvent(StreamEvent* event) {
		string payload;

		cout << "PresentationEngineManager::receiveStreamEvent " << endl;
		if (event->getEventName() == "gingaEditingCommands") {
			payload = (string)(char*)(event->getEventData());
			if (formatter != NULL) {
				formatter->nclEdit(payload);
			}
		}
	}
#endif

	void PresentationEngineManager::pausePressed() {
		if (currentDocument != NULL) {
			if (paused) {
				formatter->resumeDocument(currentDocument->getId());
				paused = false;
			} else {
				formatter->pauseDocument(currentDocument->getId());
				paused = true;
			}
		}
	}

	void PresentationEngineManager::startPresentation() {
		if (currentDocument != NULL) {
			formatter->startDocument(currentDocument->getId(), "");
			paused = false;

		} else {
			wclog << "PresentationEngineManager::startPresentation warning!";
			wclog << " Trying to start the presentation of a NULL document.";
			wclog << endl;
		}
	}

	void PresentationEngineManager::runTVProgram() {

	}

	void PresentationEngineManager::stopPresentation() {
		wclog << "PresentationEngineManager::stopPresentation" << endl;
		if (currentDocument != NULL) {
			formatter->stopDocument(currentDocument->getId());
		}
	}

	void PresentationEngineManager::presentationCompleted(string documentId) {
		cout << "PresentationEngineManager::presentationCompleted" << endl;
		if (isLocalNcl) {
			ButtonPlayer::getInstance()->stop();
			::usleep(900000);
			std::abort();
		}

		if (formatter != NULL) {
			//TODO: how to delete formater since it notify me that the document
			// presentation is completed
			delete formatter;
			formatter = NULL;
			ButtonPlayer::getInstance()->release();
			::usleep(100000); //waiting release of used objects
			OutputManager::getInstance()->clearWidgetPools();
		}
		unlockConditionSatisfied();
	}

	bool PresentationEngineManager::userEventReceived(InputEvent* ev) {
		struct event* evR;

		evR = new struct event;
		evR->p = this;
		evR->code = ev->getKeyCode();
#if HAVE_TUNER
		evR->tuner = this->tuner;
#endif

		pthread_t notifyThreadId_;
		pthread_create(
				&notifyThreadId_,
				0, PresentationEngineManager::eventReceived, (void*)evR);

		pthread_detach(notifyThreadId_);
		return true;
	}

	void* PresentationEngineManager::eventReceived(void* ptr) {
		struct event* ev;
		PresentationEngineManager* p;

		ev = (struct event*)ptr;
		const int code = ev->code;
		p = ev->p;
#if HAVE_TUNER
		Tuner* t;
		t = ev->tuner;
#endif

		if (code == CodeMap::KEY_STOP || code == CodeMap::KEY_F11) {
			ButtonPlayer::getInstance()->stop();
			if (p->getIsLocalNcl()) {
				::usleep(900000);
				std::abort();
			}

			p->stopPresentation();

		} else if (code == CodeMap::KEY_PAUSE || code == CodeMap::KEY_F12) {
			ButtonPlayer::getInstance()->pause();
			p->pausePressed();

		} else if (code == CodeMap::KEY_F10) {
			cerr<<"PresentationEngineManager::eventReceived()"<<endl;
			cerr<<"F10 pressionado"<<endl;
			//NclDocumentGenerator* generator = NclGenerator::getGenerableInstance(currentDocument);
			//cout<<generator->generateCode();
			eu sou um erro de compilação
		} else if (code == CodeMap::KEY_F9 {
			cerr<<"PresentationEngineManager::eventReceived()"<<endl;
			cerr<<"F9 pressionado"<<endl;
			//NclDocumentGenerator* generator = NclGenerator::getGenerableInstance(currentDocument);
			//cout<<generator->generateCode();

#if HAVE_TUNER
		} else if (t != NULL) {
			if (code == CodeMap::KEY_CHANNEL_UP ||
					code == CodeMap::KEY_PAGE_UP) {

				t->channelUp();

			} else if (code == CodeMap::KEY_CHANNEL_DOWN ||
					code == CodeMap::KEY_PAGE_DOWN) {

				t->channelDown();
			}
#endif
		}

		return NULL;
	}

	void PresentationEngineManager::addRegion(string location) {
		if (location != "" && formatter != NULL) {
			//TODO choose the parent region
			formatter->addRegion(currentDocument->getId(), NULL, location);
		}
	}

	void PresentationEngineManager::removeRegion(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeRegion(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addRegionBase(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addRegionBase(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeRegionBase(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeRegionBase(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addRule(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addRule(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeRule(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeRule(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addRuleBase(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addRuleBase(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeRuleBase(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeRuleBase(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addConnector(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addConnector(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeConnector(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeConnector(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addConnectorBase(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addConnectorBase(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeConnectorBase(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeConnectorBase(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addTransition(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addTransition(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeTransition(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeTransition(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addTransitionBase(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addTransitionBase(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeTransitionBase(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeTransitionBase(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addDescriptor(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addDescriptor(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeDescriptor(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeDescriptor(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addDescriptorBase(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addDescriptorBase(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeDescriptorBase(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeDescriptorBase(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addImportBase(string id, string location) {
		if (location != "" && id != "" && formatter != NULL) {
			formatter->addImportBase(currentDocument->getId(), id, location);
		}
	}

	void PresentationEngineManager::removeImportBase(
		    string id, string location) {

		if (id != "" && location != "" && formatter != NULL) {
			formatter->removeImportBase(currentDocument->getId(), id, location);
		}
	}

	void PresentationEngineManager::addImportedDocumentBase(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addImportedDocumentBase(
				    currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeImportedDocumentBase(string id) {
		if (id != "" && formatter != NULL) {
			formatter->removeImportedDocumentBase(currentDocument->getId(), id);
		}
	}

	void PresentationEngineManager::addImportNCL(string location) {
		if (location != "" && formatter != NULL) {
			formatter->addImportNCL(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::removeImportNCL(string location) {
		if (location != "" && formatter != NULL) {
			formatter->removeImportNCL(currentDocument->getId(), location);
		}
	}

	void PresentationEngineManager::addNode(string id, string location) {
		if (location != "" && id != "" && formatter != NULL) {
			formatter->addNode(currentDocument->getId(), id, location);
		}
	}

	void PresentationEngineManager::removeNode(
		    string compositionId, string nodeId) {

		if (compositionId != "" && nodeId != "" && formatter != NULL) {
    		formatter->removeNode(
    			    currentDocument->getId(), compositionId, nodeId);
  		}
  	}

	void PresentationEngineManager::addInterface(string id, string location) {
		if (location != "" && id != "" && formatter != NULL) {
			formatter->addInterface(currentDocument->getId(), id, location);
		}
	}

	void PresentationEngineManager::removeInterface(
		    string nodeId, string interfaceId) {

		if (nodeId != "" && interfaceId != "" && formatter != NULL) {
			formatter->removeInterface(
				    currentDocument->getId(), nodeId, interfaceId);
		}
	}

	void PresentationEngineManager::addLink(string id, string location) {
		if (location != "" && id != "" && formatter != NULL) {
			formatter->addLink(currentDocument->getId(), id, location);
		}
	}

	void PresentationEngineManager::removeLink(
		    string compositionId, string linkId) {

		if (compositionId != "" && linkId != "" && formatter != NULL) {
			formatter->removeLink(
				    currentDocument->getId(), compositionId, linkId);
		}
	}

	void PresentationEngineManager::setPropertyValue(
		    string nodeId, string attributeId, string value) {

		if (nodeId != "" && attributeId != "" &&
				value != "" && formatter != NULL) {

			formatter->setPropertyValue(
				    currentDocument->getId(), nodeId, attributeId, value);
		}
	}

	void PresentationEngineManager::waitUnlockCondition() {
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&condition, &mutex);
		pthread_mutex_unlock(&mutex);
	}

	void PresentationEngineManager::unlockConditionSatisfied() {
		pthread_cond_signal(&condition);
	}
}
}
}
}
}
