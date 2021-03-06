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

#include "../../../include/ProceduralExecutionObject.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
	ProceduralExecutionObject::ProceduralExecutionObject(
			string id, Node* node) : ExecutionObject(id, node) {

		initializeProceduralObject();
	}

	ProceduralExecutionObject::ProceduralExecutionObject(
			string id, Node* node,
			GenericDescriptor* descriptor) : ExecutionObject(
					id, node, descriptor) {

		initializeProceduralObject();
	}

	ProceduralExecutionObject::ProceduralExecutionObject(
			string id, Node* node,
			CascadingDescriptor* descriptor) : ExecutionObject(
					id, node, descriptor) {

		initializeProceduralObject();
	}

	ProceduralExecutionObject::~ProceduralExecutionObject() {

	}

	void ProceduralExecutionObject::initializeProceduralObject() {
		typeSet.insert("ProceduralExecutionObject");
		currentEvent = NULL;
		preparedEvents = new map<string, FormatterEvent*>;
	}

	void ProceduralExecutionObject::setCurrentEvent(FormatterEvent* event) {
		if (!containsEvent(event)) {
			currentEvent = NULL;
		} else {
			currentEvent = event;
		}
	}

	bool ProceduralExecutionObject::prepare(
		    FormatterEvent* event, double offsetTime) {

		CompositeExecutionObject* parentObject;
		int size;
		EventTransition* transition;
		map<Node*, void*>::iterator i;
		double startTime = 0;
		ContentAnchor* contentAnchor;

		if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
			return false;
		}

		if (event->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)event)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				i = parentTable->begin();
				while (i != parentTable->end()) {
					parentObject = (CompositeExecutionObject*)(i->second);
					// register parent as a mainEvent listener
					event->addEventListener(parentObject);
					++i;
				}
				(*preparedEvents)[event->getId()] = event;
				return true;
			}
		}

		if (event->instanceOf("PresentationEvent")) {
			startTime = ((PresentationEvent*)event)->getBegin() + offsetTime;
			if (startTime > ((PresentationEvent*)event)->getEnd()) {
				return false;
			}
		}

		i = parentTable->begin();
		while (i != parentTable->end()) {
			parentObject = (CompositeExecutionObject*)(i->second);
			// register parent as a currentEvent listener
			event->addEventListener(parentObject);
			++i;
		}

		if (event == wholeContent && startTime == 0.0) {
			startTransitionIndex = 0;

		} else {
			size = transitionTable->size();
			startTransitionIndex = 0;
			while (startTransitionIndex < size) {
				transition = (*transitionTable)[startTransitionIndex];
				if (transition->getTime() >= startTime) {
					break;
				}

				if (transition->instanceOf("BeginEventTransition")) {
					transition->getEvent()->
						    setCurrentState(EventUtil::ST_OCCURRING);
				} else {
					transition->getEvent()->
						    setCurrentState(EventUtil::ST_SLEEPING);

					transition->getEvent()->incrementOccurrences();
				}
				startTransitionIndex++;
			}
		}

		FormatterEvent* auxEvent;
		AttributionEvent* attributeEvent;
		PropertyAnchor* attributeAnchor;
		int j;

		if (otherEvents != NULL) {
			size = otherEvents->size();
			for (j = 0; j < size; j++) {
				auxEvent = (*otherEvents)[j];
				if (auxEvent->instanceOf("AttributionEvent")) {
					attributeEvent = (AttributionEvent*)auxEvent;
					attributeAnchor = attributeEvent->getAnchor();
					if (attributeAnchor->getPropertyValue() != "") {
						attributeEvent->setValue(
							    attributeAnchor->getPropertyValue());
					}
				}
			}
		}

		this->offsetTime = startTime;
		currentTransitionIndex = startTransitionIndex;
		(*preparedEvents)[event->getId()] = event;
		return true;
	}

	bool ProceduralExecutionObject::start() {
		EventTransition* transition;
		ContentAnchor* contentAnchor;

		if (currentEvent == NULL ||
				preparedEvents->count(currentEvent->getId()) == 0) {

			return false;
		}

		/*
		 * TODO: follow the event state machine or start instruction behavior
		 * if (currentEvent->getCurrentState() == EventUtil::ST_PAUSED) {
			return resume();
		}*/

		if (currentEvent->getCurrentState() != EventUtil::ST_SLEEPING) {
			return false;
		}

		if (currentEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)currentEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				currentEvent->start();
				return true;
			}
		}

		while ((unsigned int)currentTransitionIndex < 
			    transitionTable->size()) {

			transition = (*transitionTable)[currentTransitionIndex];
			if (transition->getTime() <= offsetTime) {
				if (transition->instanceOf("BeginEventTransition")) {
					transition->getEvent()->start();
				}
				currentTransitionIndex++;
			} else {
				break;
			}
		}

		return true;
	}

	bool ProceduralExecutionObject::stop() {
		EventTransition* transition;
		ContentAnchor* contentAnchor;
		vector<EventTransition*>::iterator i;

		if (currentEvent == NULL ||
				currentEvent->getCurrentState() == EventUtil::ST_SLEEPING ||
				preparedEvents->count(currentEvent->getId()) == 0) {

			return false;
		}

		if (currentEvent->instanceOf("PresentationEvent")) {
			i = transitionTable->begin();
			while (i != transitionTable->end()) {
				transition = *i;
				if (transition->getTime() >
						((PresentationEvent*)currentEvent)->getEnd()) {

					transition->getEvent()->setCurrentState(
						    EventUtil::ST_SLEEPING);

				} else if (transition->instanceOf("EndEventTransition")) {
					transition->getEvent()->stop();
				}
				++i;
			}

		} else if (currentEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)currentEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				currentEvent->stop();
			}
		}

		currentTransitionIndex = startTransitionIndex;
		pauseCount = 0;
		return true;
	}

	bool ProceduralExecutionObject::abort() {
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		if (currentEvent == NULL ||
				currentEvent->getCurrentState() == EventUtil::ST_SLEEPING ||
				preparedEvents->count(currentEvent->getId()) == 0) {

			return false;
		}

		evs = getEvents();
		if (evs != NULL) {
			i = evs->begin();
			while (i != evs->end()) {
				if ((*i)->getCurrentState() != EventUtil::ST_SLEEPING) {
					(*i)->abort();
				}
				++i;
			}
			delete evs;
			evs = NULL;
		}

		currentTransitionIndex = startTransitionIndex;
		pauseCount = 0;
		return true;
	}

	bool ProceduralExecutionObject::pause() {
		FormatterEvent* event;
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		if (currentEvent == NULL ||
				currentEvent->getCurrentState() != EventUtil::ST_OCCURRING ||
				preparedEvents->count(currentEvent->getId()) == 0) {

			return false;
		}

		evs = getEvents();
		if (evs != NULL) {
			if (pauseCount == 0) {
				i = evs->begin();
				while (i != evs->end()) {
					event = *i;
					if (event->getCurrentState() == EventUtil::ST_OCCURRING) {
						event->pause();
					}
					++i;
				}
			}
			delete evs;
			evs = NULL;
		}

		pauseCount++;
		return true;
	}

	bool ProceduralExecutionObject::resume() {
		FormatterEvent* event;
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		if (pauseCount == 0) {
			return false;

		} else {
			pauseCount--;
			if (pauseCount > 0) {
				return false;
			}
		}

		evs = getEvents();
		if (evs != NULL) {
			if (pauseCount == 0) {
				i = evs->begin();
				while (i != evs->end()) {
					event = *i;
					if (event->getCurrentState() == EventUtil::ST_PAUSED) {
						event->resume();
					}
					++i;
				}
			}
			delete evs;
			evs = NULL;
		}

		return true;
	}

	bool ProceduralExecutionObject::unprepare() {
		if (currentEvent == NULL ||
				currentEvent->getCurrentState() != EventUtil::ST_SLEEPING ||
				preparedEvents->count(currentEvent->getId()) == 0) {

			return false;
		}

		if (currentEvent->instanceOf("AnchorEvent") &&
				((AnchorEvent*)currentEvent)->getAnchor() != NULL &&
				((AnchorEvent*)currentEvent)->getAnchor()->instanceOf(
						"LambdaAnchor")) {

			FormatterEvent* event;
			vector<FormatterEvent*>* evs;
			vector<FormatterEvent*>::iterator i;

			evs = getEvents();
			if (evs != NULL) {
				i = evs->begin();
				while (i != evs->end()) {
					event = *i;
					if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
						event->stop();
					}
					++i;
				}
				delete evs;
				evs = NULL;
			}
		}

		map<Node*, void*>::iterator i;
		CompositeExecutionObject* parentObject;

		i = parentTable->begin();
		while (i != parentTable->end()) {
			parentObject = (CompositeExecutionObject*)(i->second);
			// unregister parent as a currentEvent listener
			currentEvent->removeEventListener(parentObject);
			++i;
		}

		preparedEvents->erase(preparedEvents->find(currentEvent->getId()));
		currentEvent = NULL;
		return true;
	}
}
}
}
}
}
}
}
