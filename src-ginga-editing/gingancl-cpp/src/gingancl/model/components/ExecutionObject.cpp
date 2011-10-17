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

#include "../../../include/ExecutionObject.h"
#include "../../../include/CompositeExecutionObject.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
	ExecutionObject::ExecutionObject(string id, Node* node) {
		initializeExecutionObject(id, node, NULL);
	}

	ExecutionObject::ExecutionObject(
		    string id,
		    Node* node,
		    GenericDescriptor* descriptor) {

		initializeExecutionObject(
			    id, node, new CascadingDescriptor(descriptor));
	}

	ExecutionObject::ExecutionObject(
		    string id,
		    Node* node,
		    CascadingDescriptor* descriptor) {

		initializeExecutionObject(id, node, descriptor);
	}

	ExecutionObject::~ExecutionObject() {
		lock();
		map<string, FormatterEvent*>::iterator i;
		FormatterEvent* event;

		map<Node*, Node*>::iterator j;
		Node* parentNode;
		CompositeExecutionObject* parentObject;

		if (transitionTable != NULL) {
			transitionTable->clear();
			delete transitionTable;
			transitionTable = NULL;
		}

		wholeContent = NULL;

		lockEvents();
		if (events != NULL) {
			i = events->begin();
			while (i != events->end()) {
				event = i->second;
				if (event != NULL) {
					delete event;
					event = NULL;
				}
				++i;
			}

			events->clear();
			delete events;
			events = NULL;
		}
		unlockEvents();

		if (presentationEvents != NULL) {
			presentationEvents->clear();
			delete presentationEvents;
			presentationEvents = NULL;
		}

		if (selectionEvents != NULL) {
			selectionEvents->clear();
			delete selectionEvents;
			selectionEvents = NULL;
		}

		if (otherEvents != NULL) {
			otherEvents->clear();
			delete otherEvents;
			otherEvents = NULL;
		}

		if (nodeParentTable != NULL) {
			j = nodeParentTable->begin();
			while (j != nodeParentTable->end()) {
				parentNode = j->second;
				if (parentTable->count(parentNode) != 0) {
					parentObject = (CompositeExecutionObject*)
						((*parentTable)[parentNode]);

					parentObject->removeExecutionObject(this);
				}
				++j;
			}

			nodeParentTable->clear();
			delete nodeParentTable;
			nodeParentTable = NULL;
		}

		if (parentTable != NULL) {
			parentTable->clear();
			delete parentTable;
			parentTable = NULL;
		}

		if (descriptor != NULL) {
			delete descriptor;
			descriptor = NULL;
		}
		unlock();
		pthread_mutex_destroy(&mutex);
		pthread_mutex_destroy(&mutexEvent);
	}

	void ExecutionObject::initializeExecutionObject(
			    string id, Node* node, CascadingDescriptor* descriptor) {

		typeSet.insert("ExecutionObject");

		this->id = id;
		this->dataObject = node;

		this->wholeContent = NULL;
		this->startTime = infinity();
		this->descriptor = NULL;

		this->nodeParentTable = new map<Node*, Node*>;
		this->parentTable = new map<Node*, void*>;

		this->isItCompiled = false;

		this->events = new map<string, FormatterEvent*>;
		this->presentationEvents = new vector<FormatterEvent*>;
		this->selectionEvents = new set<SelectionEvent*>;
		this->otherEvents = new vector<FormatterEvent*>;
		this->pauseCount = 0;
		this->transitionTable = new vector<EventTransition*>;
		this->mainEvent = NULL;

		this->descriptor = descriptor;
		pthread_mutex_init(&mutex, NULL);
		pthread_mutex_init(&mutexEvent, NULL);
	}

	bool ExecutionObject::instanceOf(string s) {
		if(typeSet.empty())
			return false;
		else
			return (typeSet.find(s) != typeSet.end());
	}

	int ExecutionObject::compareToUsingId(ExecutionObject* object) {
		return id.compare(object->getId());
	}

	Node* ExecutionObject::getDataObject() {
		return dataObject;
	}

	CascadingDescriptor* ExecutionObject::getDescriptor() {
		return descriptor;
	}

	string ExecutionObject::getId() {
		return id;
	}

	void* ExecutionObject::getParentObject() {
		return getParentObject(dataObject);
	}

	void* ExecutionObject::getParentObject(Node* node) {
		Node* parentNode;

		if (nodeParentTable->count(node) != 0) {
			parentNode = (*nodeParentTable)[node];
			if (parentTable->count(parentNode) != 0) {
				return (*parentTable)[parentNode];
			}
		}
		return NULL;
	}

	void ExecutionObject::addParentObject(
		    void* parentObject, Node* parentNode) {

		addParentObject(dataObject, parentObject, parentNode);
	}

	void ExecutionObject::addParentObject(
		    Node* node,
		    void* parentObject,
		    Node* parentNode) {

		(*nodeParentTable)[node] = parentNode;
		(*parentTable)[parentNode] = parentObject;
	}

	void ExecutionObject::setDescriptor(
		    CascadingDescriptor* cascadingDescriptor) {

		this->descriptor = cascadingDescriptor;
	}

	void ExecutionObject::setDescriptor(GenericDescriptor* descriptor) {
		CascadingDescriptor* cascade;
		cascade = new CascadingDescriptor(descriptor);
		this->descriptor = cascade;
	}

	string ExecutionObject::toString() {
		return id;
	}

	bool ExecutionObject::addEvent(FormatterEvent* event) {
		lockEvents();
		if (events->count(event->getId()) != 0) {
			unlockEvents();
			return false;
		}

		(*events)[event->getId()] = event;
		unlockEvents();
		if (event->instanceOf("PresentationEvent")) {
			addPresentationEvent((PresentationEvent*)event);

		} else if (event->instanceOf("SelectionEvent")) {
			selectionEvents->insert(((SelectionEvent*)event));

		} else {
			otherEvents->push_back(event);
		}

		return true;
	}

	void ExecutionObject::addPresentationEvent(PresentationEvent* event) {
		PresentationEvent* auxEvent;
		double begin, auxBegin, end;
		int posBeg, posEnd, posMid;
		BeginEventTransition* beginTransition;
		EndEventTransition* endTransition;

		if ((event->getAnchor())->instanceOf("LambdaAnchor")) {
			presentationEvents->insert(presentationEvents->begin(), event);
			wholeContent = (PresentationEvent*)event;

			beginTransition = new BeginEventTransition(0, event);
			transitionTable->insert(transitionTable->begin(), beginTransition);
			if (event->getEnd() >= 0) {
				endTransition = new EndEventTransition(
					    event->getEnd(), event, beginTransition);

				transitionTable->push_back(endTransition);
			}

		} else {
			begin = event->getBegin();

			// undefined events are not inserted into transition table
			if (PresentationEvent::isUndefinedInstant(begin)) {
				return;
			}

			posBeg = 0;
			posEnd = presentationEvents->size() - 1;
			while (posBeg <= posEnd) {
				posMid = (posBeg + posEnd) / 2;
				auxEvent = (PresentationEvent*)((*presentationEvents)[posMid]);
				auxBegin = auxEvent->getBegin();
				if (begin < auxBegin) {
					posEnd = posMid - 1;

				} else if (begin > auxBegin) {
					posBeg = posMid + 1;

				} else {
					posBeg = posMid + 1;
					break;
				}
			}

			// TODO: verificar se eh (posBeg - 1) ou posBeg
			presentationEvents->insert(
				    (presentationEvents->begin() + posBeg), event);

			beginTransition = new BeginEventTransition(begin, event);
			addEventTransition(beginTransition);
			end = event->getEnd();

			if (!PresentationEvent::isUndefinedInstant(end)) {
				endTransition = new EndEventTransition(
					    end, event, beginTransition);

				addEventTransition(endTransition);
			}
		}
	}

	void ExecutionObject::addEventTransition(EventTransition* transition) {
		int beg, end, pos;
		EventTransition* auxTransition;

		// binary search
		beg = 0;
		end = transitionTable->size() - 1;
		while (beg <= end) {
			pos = (beg + end) / 2;
			auxTransition = (*transitionTable)[pos];
			switch (transition->compareTo(auxTransition)) {
				case 0:
					// entrada corresponde a um evento que ja' foi inserido
					return;

				case -1:
					end = pos - 1;
					break;

				case 1:
					beg = pos + 1;
					break;
			}
		}
		// TODO: verificar se eh (beg - 1) ou beg
		transitionTable->insert((transitionTable->begin() + beg), transition);
	}

	void ExecutionObject::removeEventTransition(PresentationEvent* event) {
		int i, size;
		vector<EventTransition*>::iterator j;
		EventTransition* transition;
		EventTransition* endTransition;

		size = transitionTable->size();
		for (i = 0; i < size; i++) {
			transition = (*transitionTable)[i];
			if (transition->getEvent() == event) {
				if (transition->instanceOf("BeginEventTransition") &&
					    ((BeginEventTransition*)transition)->
					    getEndTransition() != NULL) {

					endTransition = ((BeginEventTransition*)transition)->
						    getEndTransition();

					for (j = transitionTable->begin();
						    j != transitionTable->end(); ++j) {

						if (*j == endTransition) {
							transitionTable->erase(j);
							break;
						}
					}
				}

				for (j = transitionTable->begin();
					    j != transitionTable->end(); ++j) {

					if (*j == transition) {
						transitionTable->erase(j);
						break;
					}
				}
				break;
			}
		}
	}

	int ExecutionObject::compareTo(ExecutionObject* object) {
		int ret;

		ret = compareToUsingStartTime(object);
		if (ret == 0)
			return compareToUsingId(object);
		else
			return ret;
	}

	int ExecutionObject::compareToUsingStartTime(ExecutionObject* object) {
		double thisTime, otherTime;

		thisTime = startTime;
		otherTime = (object->getExpectedStartTime());
		if (thisTime < otherTime)
			return -1;
		else if (thisTime > otherTime)
			return 1;
		else
			return 0;
	}

	bool ExecutionObject::containsEvent(FormatterEvent* event) {
		bool contains;

		lockEvents();
		contains = (events->count(event->getId()) != 0);
		unlockEvents();

		return contains;
	}

	FormatterEvent* ExecutionObject::getEvent(string id) {
		FormatterEvent* ev;
		lockEvents();
		if (events != NULL && events->count(id) != 0) {
			ev = (*events)[id];
			unlockEvents();
			return ev;
		}

		unlockEvents();
		return NULL;
	}

	vector<FormatterEvent*>* ExecutionObject::getEvents() {
		vector<FormatterEvent*>* eventsVector = NULL;
		map<string, FormatterEvent*>::iterator i;

		lockEvents();
		if (events == NULL || events->empty()) {
			unlockEvents();
			return NULL;
		}

		eventsVector = new vector<FormatterEvent*>;
		for (i = events->begin(); i != events->end(); ++i) {
			eventsVector->push_back(i->second);
		}
		unlockEvents();

		return eventsVector;
	}

	double ExecutionObject::getExpectedStartTime() {
		return startTime;
	}

	PresentationEvent* ExecutionObject::getWholeContentPresentationEvent() {
		return wholeContent;
	}

	void ExecutionObject::setStartTime(double t) {
		startTime = t;
	}

	void ExecutionObject::updateEventDurations() {
		int i, size;

		size = presentationEvents->size();
		for (i = 0; i < size; i++) {
			updateEventDuration(
				    (PresentationEvent*)((*presentationEvents)[i]));
		}
	}

	void ExecutionObject::updateEventDuration(PresentationEvent* event) {
		if (!containsEvent((FormatterEvent*)event)) {
			return;
		}

		double duration;
		duration = NaN();

		if (descriptor != NULL) {
			if (descriptor->instanceOf("CascadingDescriptor")) {
				if (!isNaN(
					    descriptor->getExplicitDuration()) &&
					    event == wholeContent) {

					duration = descriptor->getExplicitDuration();

				} else if (event->getDuration() > 0) {
					duration = event->getDuration();

				} else {
					duration = 0;
				}
			}

		} else {
			if (event->getDuration() > 0) {
				duration = event->getDuration();

			} else {
				duration = 0;
			}
		}

		if (duration < 0) {
			event->setDuration(NaN());

		} else {
			event->setDuration(duration);
		}
	}

	bool ExecutionObject::removeEvent(FormatterEvent* event) {
		if (!containsEvent(event)) {
			return false;
		}

		vector<FormatterEvent*>::iterator i;
		set<SelectionEvent*>::iterator j;

		if (event->instanceOf("PresentationEvent")) {
			for (i = presentationEvents->begin();
				    i != presentationEvents->end(); ++i) {

				if (*i == event) {
					presentationEvents->erase(i);
					break;
				}
			}
			removeEventTransition((PresentationEvent*)event);

		} else if (event->instanceOf("SelectionEvent")) {
			j = selectionEvents->find(((SelectionEvent*)event));
			if (j != selectionEvents->end()) {
				selectionEvents->erase(j);
			}

		} else {
			for (i = otherEvents->begin();
				    i != otherEvents->end(); ++i) {

				if (*i == event) {
					otherEvents->erase(i);
					break;
				}
			}
		}

		lockEvents();
		events->erase(events->find(event->getId()));
		unlockEvents();
		return true;
	}

	bool ExecutionObject::isCompiled() {
		return isItCompiled;
	}

	void ExecutionObject::setCompiled(bool status) {
		isItCompiled = status;
	}

	void ExecutionObject::removeNode(Node* node) {
		Node* parentNode;

		if (node != dataObject) {
			if (nodeParentTable->count(node) != 0) {
				parentNode = (*nodeParentTable)[node];

				if (parentTable->count(parentNode) != 0) {
					parentTable->erase(parentTable->find(parentNode));
				}

				nodeParentTable->erase(nodeParentTable->find(node));
			}
		}
	}

	vector<Node*>* ExecutionObject::getNodes() {
		if (nodeParentTable->empty()) {
			return NULL;
		}

		vector<Node*>* nodes;
		map<Node*, Node*>::iterator i;

		nodes = new vector<Node*>;
		for (i = nodeParentTable->begin(); i != nodeParentTable->end(); ++i) {
			nodes->push_back(i->first);
		}

		if (nodeParentTable->count(dataObject) == 0) {
			nodes->push_back(dataObject);
		}

		return nodes;
	}

	NodeNesting* ExecutionObject::getNodePerspective() {
		return getNodePerspective(dataObject);
	}

	NodeNesting* ExecutionObject::getNodePerspective(Node* node) {
		Node* parentNode;
		NodeNesting* perspective;
		CompositeExecutionObject* parentObject;

		if (nodeParentTable->count(node) == 0) {
			if (dataObject == node) {
				perspective = new NodeNesting();

			} else {
				return NULL;
			}

		} else {
			parentNode = (*nodeParentTable)[node];
			if (parentTable->count(parentNode) != 0) {

				parentObject = (CompositeExecutionObject*)(
					    (*parentTable)[parentNode]);

				perspective = parentObject->getNodePerspective(parentNode);

			} else {
				return NULL;
			}
		}
		perspective->insertAnchorNode(node);
		return perspective;
	}

	vector<ExecutionObject*>* ExecutionObject::getObjectPerspective() {
		return getObjectPerspective(dataObject);
	}

	vector<ExecutionObject*>* ExecutionObject::getObjectPerspective(
		    Node* node) {

		Node* parentNode;
		vector<ExecutionObject*>* perspective = NULL;
		CompositeExecutionObject* parentObject;

		if (nodeParentTable->count(node) == 0) {
			if (dataObject == node) {
				perspective = new vector<ExecutionObject*>;

			} else {
				return NULL;
			}

		} else {
			parentNode = (*nodeParentTable)[node];
			if (parentTable->count(parentNode) != 0) {

				parentObject = (CompositeExecutionObject*)(
				 	    (*parentTable)[parentNode]);

				perspective = parentObject->getObjectPerspective(parentNode);

			} else {
				return NULL;
			}
		}
		if (perspective != NULL) {
			perspective->push_back(this);
		}
		return perspective;
	}

	vector<Node*>* ExecutionObject::getParentNodes() {
		if (nodeParentTable->empty()) {
			return NULL;
		}

		vector<Node*>* parents;
		map<Node*, Node*>::iterator i;

		parents = new vector<Node*>;
		for (i = nodeParentTable->begin(); i != nodeParentTable->end(); ++i) {
			parents->push_back(i->second);
		}

		return parents;
	}

	FormatterEvent* ExecutionObject::getMainEvent() {
		return mainEvent;
	}

	bool ExecutionObject::prepare(FormatterEvent* event, double offsetTime) {
		CompositeExecutionObject* parentObject;
		int size;
		EventTransition* transition;
		map<Node*, void*>::iterator i;
		double startTime = 0;
		ContentAnchor* contentAnchor;

		lock();
		if (event == NULL || !containsEvent(event)) {
			return false;
		}

		mainEvent = event;

		if (mainEvent->getCurrentState() != EventUtil::ST_SLEEPING) {
			return false;
		}

		if (mainEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)mainEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				i = parentTable->begin();
				while (i != parentTable->end()) {
					parentObject = (CompositeExecutionObject*)(i->second);
					// register parent as a mainEvent listener
					mainEvent->addEventListener(parentObject);
					++i;
				}
				return true;
			}
		}

		if (mainEvent->instanceOf("PresentationEvent")) {
			startTime = ((PresentationEvent*)mainEvent)->
					getBegin() + offsetTime;

			if (startTime > ((PresentationEvent*)mainEvent)->getEnd()) {
				return false;
			}
		}

		i = parentTable->begin();
		while (i != parentTable->end()) {
			parentObject = (CompositeExecutionObject*)(i->second);
			// register parent as a mainEvent listener
			mainEvent->addEventListener(parentObject);
			++i;
		}

		if (mainEvent == wholeContent && startTime == 0.0) {
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
		string value;

		if (otherEvents != NULL) {
			size = otherEvents->size();
			for (j = 0; j < size; j++) {
				auxEvent = (*otherEvents)[j];
				if (auxEvent->instanceOf("AttributionEvent")) {
					attributeEvent = (AttributionEvent*)auxEvent;
					attributeAnchor = attributeEvent->getAnchor();
					value = attributeAnchor->getPropertyValue();
					if (value != "") {
						attributeEvent->setValue(value);
					}
				}
			}
		}

		this->offsetTime = startTime;
		currentTransitionIndex = startTransitionIndex;
		return true;
	}

	bool ExecutionObject::start() {
		EventTransition* transition;
		ContentAnchor* contentAnchor;

		if (mainEvent == NULL && wholeContent == NULL) {
			return false;
		}

		if (mainEvent != NULL &&
				mainEvent->getCurrentState() != EventUtil::ST_SLEEPING) {

			return false;
		}

		if (mainEvent == NULL) {
			prepare(wholeContent, 0.0);
		}

		if (mainEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)mainEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				mainEvent->start();
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

	void ExecutionObject::updateTransitionTable(double currentTime) {
		EventTransition* transition;

		while ((unsigned int)currentTransitionIndex <
			    transitionTable->size()) {

			transition = (*transitionTable)[currentTransitionIndex];

			if (transition->getTime() <= currentTime) {
				if (transition->instanceOf("BeginEventTransition")) {
					transition->getEvent()->start();

				} else {
					transition->getEvent()->stop();
				}
				currentTransitionIndex++;

			} else {
				break;
			}
		}
	}

	EventTransition* ExecutionObject::getNextTransition() {
		if (mainEvent == NULL ||
				mainEvent->getCurrentState() == EventUtil::ST_SLEEPING ||
				!mainEvent->instanceOf("PresentationEvent")) {

			return NULL;
		}

		EventTransition* transition;
		if ((unsigned int)currentTransitionIndex <
				transitionTable->size()) {

			transition = transitionTable->at(currentTransitionIndex);
			if (IntervalAnchor::isObjectDuration(
					((PresentationEvent*)mainEvent)->getEnd()) ||
					(transition->getTime() <=
							((PresentationEvent*)mainEvent)->getEnd())) {

				return transition;
			}
		}

		return NULL;
	}

	bool ExecutionObject::stop() {
		EventTransition* transition;
		ContentAnchor* contentAnchor;
		vector<EventTransition*>::iterator i;
		double endTime;

		if (mainEvent == NULL) {
			return false;
		}

		if (mainEvent->getCurrentState() == EventUtil::ST_SLEEPING) {
			return false;
		}

		if (mainEvent->instanceOf("PresentationEvent")) {
			endTime = ((PresentationEvent*)mainEvent)->getEnd();
			i = transitionTable->begin();
			while (i != transitionTable->end()) {
				transition = *i;
				if (transition->getTime() > endTime) {

					transition->getEvent()->setCurrentState(
						    EventUtil::ST_SLEEPING);

				} else if (transition->instanceOf("EndEventTransition")) {
					transition->getEvent()->stop();
				}
				++i;
			}

		} else if (mainEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)mainEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				mainEvent->stop();
			}
		}

		currentTransitionIndex = startTransitionIndex;
		pauseCount = 0;
		return true;
	}

	bool ExecutionObject::abort() {
		int i, size;
		EventTransition* transition;
		ContentAnchor* contentAnchor;
		short objectState;

		if (mainEvent == NULL) {
			return false;
		}

		objectState = mainEvent->getCurrentState();

		if (objectState == EventUtil::ST_SLEEPING) {
			return false;
		}

		size = transitionTable->size();
		if (mainEvent->instanceOf("PresentationEvent")) {
			for (i = currentTransitionIndex; i < size; i++) {
				transition = (*transitionTable)[i];
				if (transition->getTime() > ((PresentationEvent*)mainEvent)->
						getEnd()) {

					transition->getEvent()->setCurrentState(
						    EventUtil::ST_SLEEPING);

				} else if (transition->instanceOf("EndEventTransition")) {
					transition->getEvent()->abort();
				}
			}

		} else if (mainEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)mainEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				mainEvent->abort();
			}
		}

		currentTransitionIndex = startTransitionIndex;
		pauseCount = 0;
		return true;
	}

	bool ExecutionObject::pause() {
		FormatterEvent* event;
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		if (mainEvent == NULL ||
				mainEvent->getCurrentState() == EventUtil::ST_SLEEPING) {

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

	bool ExecutionObject::resume() {
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

	bool ExecutionObject::setPropertyValue(
		    AttributionEvent* event,
		    string value, Animation* anim) {

		if (!containsEvent(event) || value == "") {
			cout << "ExecutionObject::setPropertyValue event '";
			cout << event->getId() << "' not found!" << endl;
			return false;
		}

		string propName;
		FormatterRegion* region;
		LayoutRegion* ncmRegion;

		bool done = false;
		double initTime;
		bool changed;

		propName = (event->getAnchor())->getPropertyName();
		/*cout << "ExecutionObject::setPropertyValue prop '" << propName;
		cout << "' value '" << value << "' for '" << getId() << "'";
		if (anim != NULL) {
			cout << " animDur = '" << anim->getDuration() << "'";
		}
		cout << " anim = '" << anim << "'" << endl;*/
		if (anim == NULL || stof(anim->getDuration()) <= 0.0) {
			if (propName == "size") {
				value = trim(value);
				vector<string>* params;
				params = split(value, ",");

				if (params->size() == 2) {
					region = descriptor->getFormatterRegion();
					ncmRegion = region->getLayoutRegion();
					if (isPercentualValue((*params)[0])) {
						ncmRegion->setWidth(getPercentualValue(
							    (*params)[0]), true);

					} else {
						ncmRegion->setWidth(
								(double)(stof((*params)[0])), false);
					}

					if (isPercentualValue((*params)[1])) {
						ncmRegion->setHeight(
							    getPercentualValue((*params)[1]), true);

					} else {
						ncmRegion->setHeight(
								(double)(stof((*params)[1])), false);
					}

					delete params;
					params = NULL;

					region->updateRegionBounds();
					event->stop();
					return true;
				}

				delete params;
				params = NULL;

			} else if (propName == "location") {
				value = trim(value);
				vector<string>* params;
				params = split(value, ",");

				if (params->size() == 2) {
					region = descriptor->getFormatterRegion();
					ncmRegion = region->getLayoutRegion();
					if (isPercentualValue((*params)[0])) {
						ncmRegion->setLeft(
								getPercentualValue((*params)[0]), true);
					} else {
						ncmRegion->setLeft(
								(double)(stof((*params)[0])), false);
					}

					if (isPercentualValue((*params)[1])) {
						ncmRegion->setTop(
								getPercentualValue((*params)[1]), true);
					} else {
						ncmRegion->setTop((double)(stof((*params)[1])), false);
					}

					delete params;
					params = NULL;

					region->updateRegionBounds();
					event->stop();
					return true;
				}

				delete params;
				params = NULL;

			} else if (propName == "bounds") {
				value = trim(value);
				vector<string>* params;
				params = split(value, ",");

				if (params->size() == 4) {
					region = descriptor->getFormatterRegion();
					ncmRegion = region->getLayoutRegion();
					if (ncmRegion->compareWidthSize((*params)[2]) <= 0) {
						// first resize the region, then update its location
						if (isPercentualValue((*params)[2])) {
							ncmRegion->setWidth(getPercentualValue(
								    (*params)[2]), true);
						} else {
							ncmRegion->setWidth(
								    (double)(stof((*params)[2])), false);
						}

						if (isPercentualValue((*params)[0])) {
							ncmRegion->setLeft(getPercentualValue(
								    (*params)[0]), true);

						} else {
							ncmRegion->setLeft(
								    (double)(stof((*params)[0])), false);
						}

					} else {
						// first update and then resize
						if (isPercentualValue((*params)[0])) {
							ncmRegion->setLeft(getPercentualValue(
								    (*params)[0]), true);

						} else {
							ncmRegion->setLeft(
								    (double)(stof((*params)[0])), false);
						}

						if (isPercentualValue((*params)[2])) {
							ncmRegion->setWidth(getPercentualValue(
								    (*params)[2]), true);

						} else {
							ncmRegion->setWidth(
								    (double)(stof((*params)[2])), false);
						}
					}

					if (ncmRegion->compareHeightSize((*params)[3]) <= 0) {
						// first resize the region, then update its location
						if (isPercentualValue((*params)[3])) {
							ncmRegion->setHeight(getPercentualValue(
								    (*params)[3]), true);

						} else {
							ncmRegion->setHeight(
								    (double)(stof((*params)[3])), false);
						}

						if (isPercentualValue((*params)[1])) {
							ncmRegion->setTop(getPercentualValue(
								    (*params)[1]), true);

						} else {
							ncmRegion->setTop(
								    (double)(stof((*params)[1])), false);
						}

					} else {
						// first update and then resize
						if (isPercentualValue((*params)[1])) {
							ncmRegion->setTop(getPercentualValue(
								    (*params)[1]), true);

						} else {
							ncmRegion->setTop(
								    (double)(stof((*params)[1])), false);
						}

						if (isPercentualValue((*params)[3])) {
							ncmRegion->setHeight(
								    getPercentualValue((*params)[3]), true);

						} else {
							ncmRegion->setHeight(
								    (double)(stof((*params)[3])), false);
						}
					}

					/*cout << "ExecutionObject::setPropertyValue bounds ";
					cout << " to '" << getId() << "' value = '";
					cout << value << "'" << endl;*/

					delete params;
					params = NULL;

					region->updateRegionBounds();
					event->stop();
					return true;
				}

				delete params;
				params = NULL;

			} else if (propName == "top" || propName == "left" ||
					propName == "bottom" || propName == "right" ||
					propName == "width" || propName == "height") {

				region = descriptor->getFormatterRegion();
				ncmRegion = region->getLayoutRegion();

				if (propName == "top") {
					if (isPercentualValue(value)) {
						ncmRegion->setTop(getPercentualValue(value), true);
					} else {
						ncmRegion->setTop((double)(stof(value)), false);
					}

				} else if (propName == "left") {
					if (isPercentualValue(value)) {
						ncmRegion->setLeft(getPercentualValue(value), true);
					} else {
						ncmRegion->setLeft((double)(stof(value)), false);
					}

				} else if (propName == "width") {
					if (isPercentualValue(value)) {
						ncmRegion->setWidth(getPercentualValue(value), true);
					} else {
						ncmRegion->setWidth((double)(stof(value)), false);
					}

				} else if (propName == "height") {
					if (isPercentualValue(value)) {
						ncmRegion->setHeight(getPercentualValue(value), true);
					} else {
						ncmRegion->setHeight((double)(stof(value)), false);
					}

				} else if (propName == "bottom") {
					if (isPercentualValue(value)) {
						ncmRegion->setBottom(getPercentualValue(value), true);
					} else {
						ncmRegion->setBottom((double)(stof(value)), false);
					}

				} else if (propName == "right") {
					if (isPercentualValue(value)) {
						ncmRegion->setRight(getPercentualValue(value), true);
					} else {
						ncmRegion->setRight((double)(stof(value)), false);
					}
				}
				region->updateRegionBounds();
				event->stop();
				return true;
			}

		} else {
			double dur;
			dur = stof(anim->getDuration());
			initTime = getCurrentTimeMillis();

			if (propName == "size") {
				value = trim(value);
				vector<string>* params;
				params = split(value, ",");

				if (params->size() == 2) {
					region = descriptor->getFormatterRegion();

					pthread_t id1;
					pthread_t id2;
					struct animeInfo* data1;
					struct animeInfo* data2;

					data1 = new struct animeInfo;
					data1->value = (*params)[0];
					data1->region = region;
					data1->duration = dur;
					data1->propName = "width";

					data2 = new struct animeInfo;
					data2->value = (*params)[1];
					data2->region = region;
					data2->duration = dur;
					data2->propName = "height";

					pthread_create(
							&id1, 0, ExecutionObject::animeThread, data1);

					pthread_create(
							&id2, 0, ExecutionObject::animeThread, data2);

					delete params;
					params = NULL;

					pthread_join(id1, NULL);
					pthread_join(id2, NULL);

					event->stop();
					return true;
				}

				delete params;
				params = NULL;

			} else if (propName == "location") {
				value = trim(value);
				vector<string>* params;
				params = split(value, ",");

				if (params->size() == 2) {
					region = descriptor->getFormatterRegion();

					pthread_t id1;
					pthread_t id2;
					struct animeInfo* data1;
					struct animeInfo* data2;

					data1 = new struct animeInfo;
					data1->value = (*params)[0];
					data1->region = region;
					data1->duration = dur;
					data1->propName = "left";

					data2 = new struct animeInfo;
					data2->value = (*params)[1];
					data2->region = region;
					data2->duration = dur;
					data2->propName = "top";

					pthread_create(
							&id1, 0, ExecutionObject::animeThread, data1);

					pthread_create(
							&id2, 0, ExecutionObject::animeThread, data2);

					delete params;
					params = NULL;

					pthread_join(id1, NULL);
					pthread_join(id2, NULL);

					event->stop();
					return true;
				}

				delete params;
				params = NULL;

			} else if (propName == "bounds") {
				value = trim(value);
				vector<string>* params;
				params = split(value, ",");

				if (params->size() == 4) {
					region = descriptor->getFormatterRegion();

					pthread_t id1;
					pthread_t id2;
					pthread_t id3;
					pthread_t id4;
					struct animeInfo* data1;
					struct animeInfo* data2;
					struct animeInfo* data3;
					struct animeInfo* data4;

					data1 = new struct animeInfo;
					data1->value = (*params)[0];
					data1->region = region;
					data1->duration = dur;
					data1->propName = "left";

					data2 = new struct animeInfo;
					data2->value = (*params)[1];
					data2->region = region;
					data2->duration = dur;
					data2->propName = "top";

					data3 = new struct animeInfo;
					data3->value = (*params)[2];
					data3->region = region;
					data3->duration = dur;
					data3->propName = "width";

					data4 = new struct animeInfo;
					data4->value = (*params)[3];
					data4->region = region;
					data4->duration = dur;
					data4->propName = "height";

					pthread_create(
							&id1, 0, ExecutionObject::animeThread, data1);

					pthread_create(
							&id2, 0, ExecutionObject::animeThread, data2);

					pthread_create(
							&id3, 0, ExecutionObject::animeThread, data3);

					pthread_create(
							&id4, 0, ExecutionObject::animeThread, data4);

					delete params;
					params = NULL;

					pthread_join(id1, NULL);
					pthread_join(id2, NULL);
					pthread_join(id3, NULL);
					pthread_join(id4, NULL);

					event->stop();
					return true;
				}

				delete params;
				params = NULL;

			} else if (propName == "top" || propName == "left" ||
					propName == "bottom" || propName == "right" ||
					propName == "width" || propName == "height") {

				region = descriptor->getFormatterRegion();
				ncmRegion = region->getLayoutRegion();

				initTime = getCurrentTimeMillis();
				while (!done) {
					done = animeStep(
							value,
							ncmRegion,
							initTime,
							stof(anim->getDuration()), propName);

					region->updateRegionBounds();
				}

				event->stop();
				return true;
			}
		}

		return false;
	}

	void* ExecutionObject::animeThread(void* ptr) {
		struct animeInfo* data;
		string value, propName;
		FormatterRegion* fRegion;
		LayoutRegion* region;
		double initTime, dur;
		bool done;

		data = (struct animeInfo*)ptr;

		done = false;
		value = data->value;
		fRegion = data->region;
		dur = data->duration;
		propName = data->propName;

		delete data;
		data = NULL;

		initTime = getCurrentTimeMillis();
		region = fRegion->getLayoutRegion();

		while (!done) {
			done = animeStep(value, region, initTime, dur, propName);
			fRegion->updateRegionBounds();
		}

		return NULL;
	}

	bool ExecutionObject::animeStep(
			string value,
			LayoutRegion* region,
			double initTime,
			double dur,
			string param) {

		LayoutRegion* clone;
		double time;
		int factor, current, target;
		bool percent;

		clone = region->cloneRegion();
		percent = isPercentualValue(value);
		if (percent) {
			if (param == "width") {
				current = region->getWidthInPixels();
				clone->setWidth(getPercentualValue(value), true);
				target = clone->getWidthInPixels();

			} else if (param == "height") {
				current = region->getHeightInPixels();
				clone->setHeight(getPercentualValue(value), true);
				target = clone->getHeightInPixels();

			} else if (param == "left") {
				current = region->getLeftInPixels();
				clone->setLeft(getPercentualValue(value), true);
				target = clone->getLeftInPixels();

			} else if (param == "top") {
				current = region->getTopInPixels();
				clone->setTop(getPercentualValue(value), true);
				target = clone->getTopInPixels();

			} else if (param == "bottom") {
				current = region->getBottomInPixels();
				clone->setBottom(getPercentualValue(value), true);
				target = clone->getBottomInPixels();

			} else if (param == "right") {
				current = region->getRightInPixels();
				clone->setRight(getPercentualValue(value), true);
				target = clone->getRightInPixels();
			}

		} else {
			if (param == "width") {
				current = region->getWidthInPixels();
				clone->setWidth(stof(value), false);
				target = clone->getWidthInPixels();

			} else if (param == "height") {
				current = region->getHeightInPixels();
				clone->setHeight(stof(value), false);
				target = clone->getHeightInPixels();

			} else if (param == "left") {
				current = region->getLeftInPixels();
				clone->setLeft(stof(value), false);
				target = clone->getLeftInPixels();

			} else if (param == "top") {
				current = region->getTopInPixels();
				clone->setTop(stof(value), false);
				target = clone->getTopInPixels();

			} else if (param == "bottom") {
				current = region->getBottomInPixels();
				clone->setBottom(stof(value), false);
				target = clone->getBottomInPixels();

			} else if (param == "right") {
				current = region->getRightInPixels();
				clone->setRight(stof(value), false);
				target = clone->getRightInPixels();
			}
		}

		/*cout << "ExecutionObject::animeStep value '" << value << "'";
		cout << " initTime '" << initTime << "' time '" << time << "'";
		cout << " target '" << target << "' current '" << current << "'";
		cout << " param '" << param << "'" << endl;*/

		if (target > current) {
			factor = 1;

		} else if (target < current) {
			factor = -1;

		} else {
			//cout << endl << "ExecutionObject::animeStep TRUE 1" << endl;
			delete clone;
			return true;
		}

		time = getCurrentTimeMillis();
		current = getNextStepValue(
				current, target,
				factor, time, initTime, (dur * 1000), 1);

		if (current < 0) {
			if (param == "width") {
				region->setWidth(target, false);

			} else if (param == "height") {
				region->setHeight(target, false);

			} else if (param == "left") {
				region->setLeft(target, false);

			} else if (param == "top") {
				region->setTop(target, false);

			} else if (param == "bottom") {
				region->setBottom(target, false);

			} else if (param == "right") {
				region->setRight(target, false);
			}
			delete clone;
			return true;
		}

		//cout << " nextStep '" << current << "'" << endl;

		if (param == "width") {
			region->setWidth(current, false);

		} else if (param == "height") {
			region->setHeight(current, false);

		} else if (param == "left") {
			region->setLeft(current, false);

		} else if (param == "top") {
			region->setTop(current, false);

		} else if (param == "bottom") {
			region->setBottom(current, false);

		} else if (param == "right") {
			region->setRight(current, false);
		}

		//cout << "ExecutionObject::animeStep FALSE" << endl;
		delete clone;
		return false;
	}

	bool ExecutionObject::unprepare() {
		if (mainEvent == NULL ||
				mainEvent->getCurrentState() != EventUtil::ST_SLEEPING) {

			unlock();
			return false;
		}

		map<Node*, void*>::iterator i;
		CompositeExecutionObject* parentObject;

		i = parentTable->begin();
		while (i != parentTable->end()) {
			parentObject = (CompositeExecutionObject*)(i->second);
			// register parent as a mainEvent listener
			mainEvent->removeEventListener(parentObject);
			++i;
		}

		mainEvent = NULL;
		unlock();
		return true;
	}

	void ExecutionObject::select(int accessCode, double currentTime) {
		int size, selCode;
		SelectionEvent* selectionEvent;
		IntervalAnchor* intervalAnchor;
		set<SelectionEvent*>* selectedEvents;
		set<SelectionEvent*>::iterator i;

		//simulating some keys of keyboard
		if (accessCode == CodeMap::KEY_F1) {
			accessCode = CodeMap::KEY_RED;

		} else if (accessCode == CodeMap::KEY_F2) {
			accessCode = CodeMap::KEY_GREEN;

		} else if (accessCode == CodeMap::KEY_F3) {
			accessCode = CodeMap::KEY_YELLOW;

		} else if (accessCode == CodeMap::KEY_F4) {
			accessCode = CodeMap::KEY_BLUE;

		} else if (accessCode == CodeMap::KEY_F5) {
			accessCode = CodeMap::KEY_MENU;

		} else if (accessCode == CodeMap::KEY_F6) {
			accessCode = CodeMap::KEY_INFO;

		} else if (accessCode == CodeMap::KEY_F7) {
			accessCode = CodeMap::KEY_EPG;

		} else if (accessCode == CodeMap::KEY_PLUS_SIGN) {
			accessCode = CodeMap::KEY_VOLUME_UP;

		} else if (accessCode == CodeMap::KEY_MINUS_SIGN) {
			accessCode = CodeMap::KEY_VOLUME_DOWN;

		} else if (accessCode == CodeMap::KEY_PAGE_UP) {
			accessCode = CodeMap::KEY_CHANNEL_UP;

		} else if (accessCode == CodeMap::KEY_PAGE_DOWN) {
			accessCode = CodeMap::KEY_CHANNEL_DOWN;

		} else if (accessCode == CodeMap::KEY_BACKSPACE) {
			accessCode = CodeMap::KEY_BACK;

		} else if (accessCode == CodeMap::KEY_ESCAPE) {
			accessCode = CodeMap::KEY_EXIT;
		}

		selectedEvents = new set<SelectionEvent*>;
		i = selectionEvents->begin();
		while (i != selectionEvents->end()) {
			selectionEvent = (SelectionEvent*)(*i);
			selCode = selectionEvent->getSelectionCode();
			if (selCode == accessCode) {
				if (selectionEvent->getAnchor()->instanceOf(
					    "IntervalAnchor")) {

					intervalAnchor = (IntervalAnchor*)(
						    selectionEvent->getAnchor());

					if (intervalAnchor->getBegin() <= currentTime
						    && currentTime <= intervalAnchor->getEnd()) {

						selectedEvents->insert(selectionEvent);
					}

				} else {
					selectedEvents->insert(selectionEvent);
				}
			}
			++i;
		}

		i = selectedEvents->begin();
		while (i != selectedEvents->end()) {
			selectionEvent = (*i);
			if (!selectionEvent->start()) {
				cout << "ExecutionObject::select Warning cant start '";
				cout << selectionEvent->getId() << "'" << endl;
			}
			++i;
		}

		delete selectedEvents;
		selectedEvents = NULL;
	}

	set<int>* ExecutionObject::getInputEvents() {
		set<SelectionEvent*>::iterator i;
		set<int>* evs;
		SelectionEvent* ev;
		int keyCode;

		evs = new set<int>;
		i = selectionEvents->begin();
		while (i != selectionEvents->end()) {
			ev = (*i);
			keyCode = ev->getSelectionCode();
			evs->insert(keyCode);
			if (keyCode == CodeMap::KEY_RED) {
				evs->insert(CodeMap::KEY_F1);

			} else if (keyCode == CodeMap::KEY_GREEN) {
				evs->insert(CodeMap::KEY_F2);

			} else if (keyCode == CodeMap::KEY_YELLOW) {
				evs->insert(CodeMap::KEY_F3);

			} else if (keyCode == CodeMap::KEY_BLUE) {
				evs->insert(CodeMap::KEY_F4);

			} else if (keyCode == CodeMap::KEY_MENU) {
				evs->insert(CodeMap::KEY_F5);

			} else if (keyCode == CodeMap::KEY_INFO) {
				evs->insert(CodeMap::KEY_F6);

			} else if (keyCode == CodeMap::KEY_EPG) {
				evs->insert(CodeMap::KEY_F7);

			} else if (keyCode == CodeMap::KEY_VOLUME_UP) {
				evs->insert(CodeMap::KEY_PLUS_SIGN);

			} else if (keyCode == CodeMap::KEY_VOLUME_DOWN) {
				evs->insert(CodeMap::KEY_MINUS_SIGN);

			} else if (keyCode == CodeMap::KEY_CHANNEL_UP) {
				evs->insert(CodeMap::KEY_PAGE_UP);

			} else if (keyCode == CodeMap::KEY_CHANNEL_DOWN) {
				evs->insert(CodeMap::KEY_PAGE_DOWN);

			} else if (keyCode == CodeMap::KEY_BACK) {
				evs->insert(CodeMap::KEY_BACKSPACE);

			} else if (keyCode == CodeMap::KEY_EXIT) {
				evs->insert(CodeMap::KEY_ESCAPE);
			}
			++i;
		}

		return evs;
	}

	void ExecutionObject::lock() {
		pthread_mutex_lock(&mutex);
	}

	void ExecutionObject::unlock() {
		pthread_mutex_unlock(&mutex);
	}

	void ExecutionObject::lockEvents() {
		pthread_mutex_lock(&mutexEvent);
	}

	void ExecutionObject::unlockEvents() {
		pthread_mutex_unlock(&mutexEvent);
	}
}
}
}
}
}
}
}
