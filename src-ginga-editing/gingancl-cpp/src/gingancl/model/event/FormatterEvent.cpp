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

#include "../../../include/FormatterEvent.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
	FormatterEvent::FormatterEvent(string id, void* executionObject) {
		this->id = id;
		currentState = EventUtil::ST_SLEEPING;
		occurrences = 0;
		priorityListeners = new set<EventListener*>;
		listeners = new set<EventListener*>;
		this->executionObject = executionObject;
		typeSet.insert("FormatterEvent");
		pthread_mutex_init(&mutex, NULL);
	}

	FormatterEvent::~FormatterEvent() {
		wclog << endl << "FormatterEvent::~FormatterEvent '" << id.c_str();
		wclog << "'" << endl << endl;
		pthread_mutex_lock(&mutex);
		this->executionObject = NULL;

		// TODO, avoid to leave a link bind with an inconsistent event

		// After TODO
		if (priorityListeners != NULL) {
			priorityListeners->clear();
			delete priorityListeners;
			priorityListeners = NULL;
		}

		if (listeners != NULL) {
			listeners->clear();
			delete listeners;
			listeners = NULL;
		}
		pthread_mutex_unlock(&mutex);
		pthread_mutex_destroy(&mutex);
	}

	bool FormatterEvent::instanceOf(string s) {
		if (typeSet.empty()) {
			return false;
		} else {
			return (typeSet.find(s) != typeSet.end());
		}
	}

	void FormatterEvent::setId(string id) {
		this->id = id;
	}

	void FormatterEvent::addEventListener(EventListener* listener) {
		pthread_mutex_lock(&mutex);
		if (listener->isPriorityListener()) {
			priorityListeners->insert(listener);
		} else {
			listeners->insert(listener);
		}
		pthread_mutex_unlock(&mutex);
	}

	bool FormatterEvent::containsEventListener(EventListener* listener) {
		pthread_mutex_lock(&mutex);
		if (listeners->count(listener) != 0 ||
				priorityListeners->count(listener) != 0) {

			return true;
		}
		pthread_mutex_unlock(&mutex);
		return false;
	}

	short FormatterEvent::getNewState(short transition) {
		switch (transition) {
			case EventUtil::TR_STOPS:
				return EventUtil::ST_SLEEPING;

			case EventUtil::TR_STARTS:
			case EventUtil::TR_RESUMES:
				return EventUtil::ST_OCCURRING;

			case EventUtil::TR_PAUSES:
				return EventUtil::ST_PAUSED;

			case EventUtil::TR_ABORTS:
				return ST_ABORTED;

			default:
				return -1;
		}
	}

	short FormatterEvent::getTransition(short newState) {
		switch (currentState) {
			case EventUtil::ST_SLEEPING:
				switch (newState) {
					case EventUtil::ST_OCCURRING:
						return EventUtil::TR_STARTS;
					default:
						return -1;
				}

			case EventUtil::ST_OCCURRING:
				switch (newState) {
					case EventUtil::ST_SLEEPING:
						return EventUtil::TR_STOPS;
					case EventUtil::ST_PAUSED:
						return EventUtil::TR_PAUSES;
					case ST_ABORTED:
						return EventUtil::TR_ABORTS;
					default:
						return -1;
				}

			case EventUtil::ST_PAUSED:
				switch (newState) {
					case EventUtil::ST_OCCURRING:
						return EventUtil::TR_RESUMES;
					case EventUtil::ST_SLEEPING:
						return EventUtil::TR_STOPS;
					case ST_ABORTED:
						return EventUtil::TR_ABORTS;
					default:
						return -1;
				}

			default:
				return -1;
		}
	}

	bool FormatterEvent::abort() {
		switch (currentState) {
			case EventUtil::ST_OCCURRING:
			case EventUtil::ST_PAUSED:
				return changeState(ST_ABORTED, EventUtil::TR_ABORTS);

			default:
				return false;
		}
	}

	bool FormatterEvent::start() {
		wclog << "FormatterEvent::start for '" << id.c_str() << "'" << endl;
		switch (currentState) {
			case EventUtil::ST_SLEEPING:
				return changeState(
					    EventUtil::ST_OCCURRING, EventUtil::TR_STARTS);
			default:
				return false;
		}
	}

	bool FormatterEvent::stop() {
		wclog << "FormatterEvent::stop for '" << id.c_str() << "'" << endl;
		switch (currentState) {
			case EventUtil::ST_OCCURRING:
			case EventUtil::ST_PAUSED:
				return changeState(EventUtil::ST_SLEEPING, EventUtil::TR_STOPS);
			default:
				return false;
		}
	}

	bool FormatterEvent::pause() {
		wclog << "FormatterEvent::pause for '" << id.c_str() << "'" << endl;
		switch (currentState) {
			case EventUtil::ST_OCCURRING:
				return changeState(EventUtil::ST_PAUSED, EventUtil::TR_PAUSES);

			default:
				return false;
		}
	}

	bool FormatterEvent::resume() {
		wclog << "FormatterEvent::resume for '" << id.c_str() << "'" << endl;
		switch (currentState) {
			case EventUtil::ST_PAUSED:
				return changeState(
					    EventUtil::ST_OCCURRING, EventUtil::TR_RESUMES);

			default:
				return false;
		}
	}

	void FormatterEvent::setCurrentState(short newState) {
		currentState = newState;
	}

	bool FormatterEvent::changeState(short newState, short transition) {
		set<EventListener*>::iterator i;
		set<EventListener*>* listenersClone, *pListenersClone;
		short previousState;

		if (transition == EventUtil::TR_STOPS) {
			occurrences++;
		}

		previousState = currentState;
		currentState = newState;

		wclog << "FormatterEvent::changeState(" << this << ") for '";
		wclog << getId().c_str() << "' listeners->size = '";
		wclog << listeners->size() << "'" << endl;
		pthread_mutex_lock(&mutex);
		pListenersClone = new set<EventListener*>(
				priorityListeners->begin(), priorityListeners->end());

		listenersClone = new set<EventListener*>(
				listeners->begin(), listeners->end());

		//listenersClone = new set<EventListener*>(listeners);
		pthread_mutex_unlock(&mutex);

		i = pListenersClone->begin();
		while (i != pListenersClone->end()) {
			if (*i != NULL) {
				((EventListener*)(*i))->eventStateChanged(
					    (void*)this, transition, previousState);
			}
			++i;
		}

		i = listenersClone->begin();
		while (i != listenersClone->end()) {
			if (*i != NULL) {
				((EventListener*)(*i))->eventStateChanged(
					    (void*)this, transition, previousState);
			}
			++i;
		}

		pListenersClone->clear();
		delete pListenersClone;
		pListenersClone = NULL;

		listenersClone->clear();
		delete listenersClone;
		listenersClone = NULL;

		if (currentState == ST_ABORTED) {
			currentState = EventUtil::ST_SLEEPING;
		}

		wclog << "FormatterEvent::changeState(" << this << ") for '";
		wclog << getId().c_str() << "' all done, return true" << endl;
		return true;
	}

	void FormatterEvent::clearEventListeners() {
		pthread_mutex_lock(&mutex);
		priorityListeners->clear();
		listeners->clear();
		pthread_mutex_unlock(&mutex);
	}

	/*int compareTo(Object object) {

	}*/

	short FormatterEvent::getCurrentState() {
		return currentState;
	}

	void* FormatterEvent::getExecutionObject() {
		return executionObject;
	}

	void FormatterEvent::setExecutionObject(void* object) {
		executionObject = object;
	}

	string FormatterEvent::getId() {
		return id;
	}

	long FormatterEvent::getOccurrences() {
		return occurrences;
	}

	bool FormatterEvent::removeEventListener(EventListener* listener) {
		bool removed = false;

		set<EventListener*>::iterator i;
		pthread_mutex_lock(&mutex);
		i = priorityListeners->find(listener);
		if (i != priorityListeners->end()) {
			priorityListeners->erase(i);
			removed = true;
		}

		i = listeners->find(listener);
		if (i != listeners->end()) {
			listeners->erase(i);
			removed = true;
		}

		pthread_mutex_unlock(&mutex);
		return removed;
	}

	string FormatterEvent::getStateName(short state) {
		switch (state) {
			case EventUtil::ST_OCCURRING:
				return "occurring";

			case EventUtil::ST_PAUSED:
				return "paused";

			case EventUtil::ST_SLEEPING:
				return "sleeping";

			default:
				return "";
		}
	}
}
}
}
}
}
}
}
