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

#include "../../include/io/InputManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	InputManager::InputManager() : Thread() {
		cm = IComponentManager::getCMInstance();

		eventListeners = new map<IInputEventListener*, set<int>*>;
		toAddEventListeners = new map<IInputEventListener*, set<int>*>;
		toRemoveEventListeners = new set<IInputEventListener*>;

		proceduralListeners = new set<IInputEventListener*>;
		toAddProcListeners = new set<IInputEventListener*>;
		toRemoveProcListeners = new set<IInputEventListener*>;

		lastEventTime = 0;
		eventBuffer = NULL;

		if (cm != NULL) {
			eventBuffer = ((EventBufferCreator*)(
					cm->getObject("EventBuffer")))();
		}

		running = true;
		notifying = false;
		notifyingProc = false;

		pthread_mutex_init(&addMutex, NULL);
		pthread_mutex_init(&removeMutex, NULL);

		pthread_mutex_init(&procMutex, NULL);
		pthread_mutex_init(&addProcMutex, NULL);
		pthread_mutex_init(&removeProcMutex, NULL);

		initializeInputIntervalTime();
		Thread::start();
	}

	InputManager::~InputManager() {
		if (_instance != NULL) {
			delete _instance;
			_instance = NULL;
		}
	}

	void InputManager::initializeInputIntervalTime() {
		string strVar;
		ifstream fis;

		imperativeIntervalTime = 0;
		declarativeIntervalTime = 0;

		strVar = "/usr/local/lib/ginga/files/system/config/input.cfg";
		fis.open(strVar.c_str(), ifstream::in);
		if (!fis.is_open()) {
			cout << "InputManager: can't open input file:";
			cout << strVar.c_str() << endl;
			return;
		}

		while (fis.good()) {
			fis >> strVar;
			if (strVar == "imperative.inputIntervalTime" && fis.good()) {
				fis >> strVar;
				if (strVar == "=" && fis.good()) {
					fis >> strVar;
					if (strVar != "") {
						imperativeIntervalTime = stof(strVar);
					}
				}
			} else if (strVar == "declarative.inputIntervalTime" &&
					fis.good()) {

				fis >> strVar;
				if (strVar == "=" && fis.good()) {
					fis >> strVar;
					if (strVar != "") {
						declarativeIntervalTime = stof(strVar);
					}
				}
			}
		}
		fis.close();

		cout << "InputManager::initializeInputIntervalTime imperative = '";
		cout << imperativeIntervalTime << "' declarative = '";
		cout << declarativeIntervalTime << "'" << endl;
	}

	void InputManager::release() {
		running = false;
		lock();
		notifying = true;
		cout << "InputManager::release" << endl;
		if (eventBuffer != NULL) {
			eventBuffer->wakeUp();
		}

		if (eventListeners != NULL) {
			eventListeners->clear();
			delete eventListeners;
			eventListeners = NULL;
		}

		pthread_mutex_lock(&addMutex);
		if (toAddEventListeners != NULL) {
			toAddEventListeners->clear();
			delete toAddEventListeners;
			toAddEventListeners = NULL;
		}
		pthread_mutex_unlock(&addMutex);
		pthread_mutex_destroy(&addMutex);

		pthread_mutex_lock(&removeMutex);
		if (toRemoveEventListeners != NULL) {
			toRemoveEventListeners->clear();
			delete toRemoveEventListeners;
			toRemoveEventListeners = NULL;
		}
		pthread_mutex_unlock(&removeMutex);
		pthread_mutex_destroy(&removeMutex);

		pthread_mutex_lock(&procMutex);
		notifyingProc = true;

		if (proceduralListeners != NULL) {
			proceduralListeners->clear();
			delete proceduralListeners;
			proceduralListeners = NULL;
		}

		pthread_mutex_lock(&addProcMutex);
		if (toAddProcListeners != NULL) {
			toAddProcListeners->clear();
			delete toAddProcListeners;
			toAddProcListeners = NULL;
		}
		pthread_mutex_unlock(&addProcMutex);
		pthread_mutex_destroy(&addProcMutex);

		pthread_mutex_lock(&removeProcMutex);
		if (toRemoveProcListeners != NULL) {
			toRemoveProcListeners->clear();
			delete toRemoveProcListeners;
			toRemoveProcListeners = NULL;
		}
		pthread_mutex_unlock(&removeProcMutex);
		pthread_mutex_destroy(&removeProcMutex);

		pthread_mutex_unlock(&procMutex);
		pthread_mutex_destroy(&procMutex);

		if (eventBuffer != NULL) {
			delete eventBuffer;
			eventBuffer = NULL;
		}
	}

	InputManager* InputManager::_instance = 0;

	InputManager* InputManager::getInstance() {
		if (InputManager::_instance == NULL) {
			InputManager::_instance = new InputManager();
		}
		return InputManager::_instance;
	}

	void InputManager::addInputEventListener(
		    IInputEventListener* listener, set<int>* events) {

		map<IInputEventListener*, set<int>*>::iterator i;

		if (notifying) {
			pthread_mutex_lock(&addMutex);
			(*toAddEventListeners)[listener] = events;
			pthread_mutex_unlock(&addMutex);

		} else {
			lock();
			i = eventListeners->find(listener);
			if (i != eventListeners->end()) {
				if (i->second != NULL) {
					delete i->second;
				}
				eventListeners->erase(i);
			}
			(*eventListeners)[listener] = events;
			unlock();
		}
	}

	void InputManager::removeInputEventListener(
		    IInputEventListener* listener) {

		map<IInputEventListener*, set<int>*>::iterator i;

		if (notifying) {
			pthread_mutex_lock(&removeMutex);
			toRemoveEventListeners->insert(listener);
			pthread_mutex_unlock(&removeMutex);

		} else {
			lock();
			i = eventListeners->find(listener);
			if (i != eventListeners->end()) {
				if (i->second != NULL) {
					delete i->second;
				}
				eventListeners->erase(i);
			}
			unlock();
		}
	}

	bool InputManager::dispatchEvent(IInputEvent* inputEvent) {
		map<IInputEventListener*, set<int>*>::iterator i, j;
		set<IInputEventListener*>::iterator k;
		set<int>* evs;
		int keyCode;

		notifying = true;
		lock();
		notifying = true;

		pthread_mutex_lock(&removeMutex);
		k = toRemoveEventListeners->begin();
		while (k != toRemoveEventListeners->end()) {
			j = eventListeners->find(*k);
			if (j != eventListeners->end()) {
				if (j->second != NULL) {
					delete j->second;
				}
				eventListeners->erase(j);
			}
			++k;
		}
		toRemoveEventListeners->clear();
		pthread_mutex_unlock(&removeMutex);

		pthread_mutex_lock(&addMutex);
		i = toAddEventListeners->begin();
		while (i != toAddEventListeners->end()) {
			j = eventListeners->find(i->first);
			if (j != eventListeners->end()) {
				if ((i->second != j->second) && j->second != NULL) {
					delete j->second;
				}
				eventListeners->erase(j);
			}
			(*eventListeners)[i->first] = i->second;
			++i;
		}
		toAddEventListeners->clear();
		pthread_mutex_unlock(&addMutex);

		if (eventListeners->empty() || inputEvent == NULL) {
			cout << "InputManager::dispatchEvent Warning!";
			cout << "no listeners found or inputEvent is null"<< endl;
			unlock();
			notifying = false;
			return true;
		}

		keyCode = inputEvent->getKeyCode();
		i = eventListeners->begin();
		while (i != eventListeners->end()) {
			evs = i->second;
			if (evs != NULL) {
				if (evs->count(keyCode) != 0) {
					//return false means an event with changed keySymbol
					if (!i->first->userEventReceived(inputEvent)) {
						unlock();
						notifying = false;
						return false;
					}
				}

			} else if (!i->first->userEventReceived(inputEvent)) {
				unlock();
				notifying = false;
				return false;
			}
			++i;
		}
		unlock();
		notifying = false;
		return true;
	}

	bool InputManager::dispatchProceduralEvent(IInputEvent* inputEvent) {
		set<IInputEventListener*>::iterator i, j, k;

		notifyingProc = true;
		pthread_mutex_lock(&procMutex);

		pthread_mutex_lock(&removeProcMutex);
		k = toRemoveProcListeners->begin();
		while (k != toRemoveProcListeners->end()) {
			j = proceduralListeners->find(*k);
			if (j != proceduralListeners->end()) {
				proceduralListeners->erase(j);
			}
			++k;
		}
		toRemoveProcListeners->clear();
		pthread_mutex_unlock(&removeProcMutex);

		pthread_mutex_lock(&addProcMutex);
		k = toAddProcListeners->begin();
		while (k != toAddProcListeners->end()) {
			proceduralListeners->insert(*k);
			++k;
		}
		toAddProcListeners->clear();
		pthread_mutex_unlock(&addProcMutex);

		if (proceduralListeners->empty() || inputEvent == NULL) {
			pthread_mutex_unlock(&procMutex);
			notifyingProc = false;
			return true;
		}

		i = proceduralListeners->begin();
		while (i != proceduralListeners->end()) {
			if (*i != NULL) {
				if (!(*i)->userEventReceived(inputEvent)) {
					pthread_mutex_unlock(&procMutex);
					notifyingProc = false;
					return false;
				}
			}
			++i;
		}
		pthread_mutex_unlock(&procMutex);
		notifyingProc = false;
		return true;
	}

	void InputManager::addProceduralInputEventListener(
		    IInputEventListener* listener) {

		if (notifyingProc) {
			pthread_mutex_lock(&addProcMutex);
			toAddProcListeners->insert(listener);
			pthread_mutex_unlock(&addProcMutex);

		} else {
			pthread_mutex_lock(&procMutex);
			proceduralListeners->insert(listener);
			pthread_mutex_unlock(&procMutex);
		}
	}

	void InputManager::removeProceduralInputEventListener(
		    IInputEventListener* listener) {

		set<IInputEventListener*>::iterator i;

		if (notifyingProc) {
			pthread_mutex_lock(&removeProcMutex);
			toRemoveProcListeners->insert(listener);
			pthread_mutex_unlock(&removeProcMutex);

		} else {
			pthread_mutex_lock(&procMutex);
			i = proceduralListeners->find(listener);
			if (i != proceduralListeners->end()) {
				proceduralListeners->erase(i);
			}
			pthread_mutex_unlock(&procMutex);
		}
	}

	void InputManager::postEvent(IInputEvent* event) {
		eventBuffer->postEvent(event);
	}

	void InputManager::postEvent(int keyCode) {
		IInputEvent* ie;

		if (cm != NULL) {
			ie = ((InputEventCreator*)(cm->getObject(
					"InputEvent")))(NULL, keyCode);

			postEvent(ie);

		} else {
			cout << "InputManager::postEvent Warning! cm is NULL" << endl;
		}
	}

	void InputManager::run() {
		cerr<<"InputManager Run"<<endl;
		/*IInputEvent* inputEvent;
		int pLastCode = -1;
		int lastCode = -1;
		double pTimeStamp = 0;

		while (running && eventBuffer != NULL) {
			eventBuffer->waitEvent();
			if (!running) {
				break;
			}

			inputEvent = eventBuffer->getNextEvent();
			while (inputEvent != NULL) {
				if (inputEvent->isPressedType() && ((getCurrentTimeMillis() -
						timeStamp) >= declarativeIntervalTime ||
								lastCode != inputEvent->getKeyCode())) {

					lastCode = inputEvent->getKeyCode();
					timeStamp = getCurrentTimeMillis();
					if (!dispatchEvent(inputEvent)) {
						delete inputEvent;
						inputEvent = eventBuffer->getNextEvent();
						continue;
					}
				}

				if ((inputEvent->isKeyType() && ((getCurrentTimeMillis() -
						pTimeStamp) >= imperativeIntervalTime ||
								pLastCode != inputEvent->getKeyCode())) ||
								inputEvent->isUserClass()) {

					pLastCode = inputEvent->getKeyCode();
					pTimeStamp = getCurrentTimeMillis();
					dispatchProceduralEvent(inputEvent);
				}

				delete inputEvent;
				inputEvent = eventBuffer->getNextEvent();
			}
		}*/
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::IInputManager*
		createInputManager() {

	return (::br::pucrio::telemidia::ginga::core::system::io::
			InputManager::getInstance());
}

extern "C" void destroyInputManager(
		::br::pucrio::telemidia::ginga::core::system::io::IInputManager* im) {

	im->release();
}
