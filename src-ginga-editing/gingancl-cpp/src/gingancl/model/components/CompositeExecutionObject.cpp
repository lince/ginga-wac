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

#include "../../../include/CompositeExecutionObject.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
	CompositeExecutionObject::CompositeExecutionObject(
		    string id,
		    Node* dataObject) : ExecutionObject(id, dataObject) {

		initializeCompositeExecutionObject(id, dataObject, NULL);
	}

	CompositeExecutionObject::CompositeExecutionObject(
		    string id,
		    Node* dataObject,
		    CascadingDescriptor* descriptor) :
		    	ExecutionObject(id, dataObject, descriptor) {

		initializeCompositeExecutionObject(id, dataObject, descriptor);
	}

	CompositeExecutionObject::~CompositeExecutionObject() {
		wclog << "CompositeExecutionObject::~CompositeExecutionObject for '";
		wclog << id.c_str() << "'" << endl;
		FormatterLink* link;
		ExecutionObject* object;

		lockComposite();

		if (runningEvents != NULL) {
			runningEvents->clear();
			delete runningEvents;
			runningEvents = NULL;
		}

		if (pausedEvents != NULL) {
			pausedEvents->clear();
			delete pausedEvents;
			pausedEvents = NULL;
		}

		if (pendingLinks != NULL) {
			map<FormatterLink*, int>::iterator i;
			i = pendingLinks->begin();
			while (i != pendingLinks->end()) {
				link = i->first;
				if (link != NULL) {
					delete link;
					link = NULL;
				}
				++i;
			}
			delete pendingLinks;
			pendingLinks = NULL;
		}

		if (links != NULL) {
			vector<FormatterLink*>::iterator j;
			j = links->begin();
			while (j != links->end()) {
				link = *j;
				if (link != NULL) {
					delete link;
					link = NULL;
				}
			}
			links->clear();
			delete links;
			links = NULL;
		}

		if (uncompiledLinks != NULL) {
			uncompiledLinks->clear();
			delete uncompiledLinks;
			uncompiledLinks = NULL;
		}

		if (execObjList != NULL) {
			vector<ExecutionObject*>::iterator k;

			k = execObjList->begin();
			while (k != execObjList->end()) {
				object = *k;
				if (object != this && object != NULL) {
					delete object;
					object = NULL;
				}
				++k;
			}

			execObjList->clear();
			delete execObjList;
			execObjList = NULL;
		}

		unlockComposite();
		pthread_mutex_destroy(&mutexComposite);
	}

	void CompositeExecutionObject::initializeCompositeExecutionObject(
		    string id,
		    Node* dataObject,
		    CascadingDescriptor* descriptor) {

		typeSet.insert("CompositeExecutionObject");
		pthread_mutex_init(&mutexComposite, NULL);

		vector<Link*>* compositionLinks = NULL;

		execObjList = new vector<ExecutionObject*>;
		links = new vector<FormatterLink*>;
		uncompiledLinks = new vector<Link*>;

		Entity* entity;
		entity = dataObject->getDataEntity();

		if (entity != NULL && entity->instanceOf("LinkComposition")) {
			if (entity->instanceOf("ContextNode")) {
				ContextNode* compositeNode;

				compositeNode = (ContextNode*)entity;
				compositionLinks = compositeNode->getLinks();
				if (compositionLinks != NULL) {
					vector<Link*>::iterator i;
					i = compositionLinks->begin();
					while (i != compositionLinks->end()) {
						uncompiledLinks->push_back(*i);
						++i;
					}
				}
			}
		}

		runningEvents = new set<FormatterEvent*>;
		pausedEvents = new set<FormatterEvent*>;
		pendingLinks = new map<FormatterLink*, int>;
	}

	bool CompositeExecutionObject::addExecutionObject(ExecutionObject* obj) {
		if (obj == NULL) {
			return false;
		}

		lockComposite();
		execObjList->push_back(obj);
		unlockComposite();
		obj->addParentObject((void*)this, getDataObject());
		return true;
	}

	bool CompositeExecutionObject::containsExecutionObject(string execObjId) {
		if (getExecutionObject(execObjId) != NULL)
			return true;
		else
			return false;
	}

	ExecutionObject* CompositeExecutionObject::getExecutionObject(string id) {
		lockComposite();
		if (execObjList->empty()) {
			unlockComposite();
			return NULL;
		}

		vector<ExecutionObject*>::iterator iterator;
		ExecutionObject* execObj;

		iterator = execObjList->begin();
		while (iterator != execObjList->end()) {
			execObj = (ExecutionObject*)(*iterator);
			if ((execObj->getId()).compare(id) == 0) {
				unlockComposite();
				return execObj;
			}
			++iterator;
		}

		unlockComposite();
		return NULL;
	}

	vector<ExecutionObject*>* CompositeExecutionObject::getExecutionObjects() {
		vector<ExecutionObject*>* objs;
		lockComposite();
		if (execObjList == NULL || execObjList->empty()) {
			unlockComposite();
			return NULL;
		}
		objs = new vector<ExecutionObject*>(*execObjList);
		unlockComposite();
		return objs;
	}

	int CompositeExecutionObject::getNumExecutionObjects() {
		int size = 0;
		lockComposite();
		if (execObjList != NULL) {
			size = execObjList->size();
		}
		unlockComposite();
		return size;
	}

	vector<ExecutionObject*>*
		    CompositeExecutionObject::recursivellyGetExecutionObjects() {

		vector<ExecutionObject*>* objects;
		int i, size;
		ExecutionObject* childObject;
		vector<ExecutionObject*>* grandChildrenObjects;
		vector<ExecutionObject*>::iterator j;

		objects = new vector<ExecutionObject*>;
		lockComposite();
		size = execObjList->size();
		for (i = 0; i < size; i++) {
			childObject = (ExecutionObject*)((*execObjList)[i]);
			objects->push_back(childObject);
			if (childObject->instanceOf("CompositeExecutionObject")) {
				grandChildrenObjects = ((CompositeExecutionObject*)
					    childObject)->recursivellyGetExecutionObjects();

				j = grandChildrenObjects->begin();
				while (j != grandChildrenObjects->end()) {
					objects->push_back(*j);
					++j;
				}
				delete grandChildrenObjects;
				grandChildrenObjects = NULL;
			}
		}
		unlockComposite();
		return objects;
	}

	bool CompositeExecutionObject::removeExecutionObject(ExecutionObject* obj) {
		vector<ExecutionObject*>::iterator i;

		lockComposite();
		i = execObjList->begin();
		while (i != execObjList->end()) {
			if ((*i) == obj) {
				execObjList->erase(i);
				unlockComposite();
				return true;
			}
			++i;
		}
		unlockComposite();
		return false;
	}

	vector<Link*>* CompositeExecutionObject::getUncompiledLinks() {
		return uncompiledLinks;
	}

	bool CompositeExecutionObject::containsUncompiledLink(Link* dataLink) {
		lockComposite();
		vector<Link*>::iterator i;
		for (i = uncompiledLinks->begin(); i != uncompiledLinks->end(); ++i) {
			if (*i == dataLink) {
				unlockComposite();
				return true;
			}
		}

		unlockComposite();
		return false;
	}

	void CompositeExecutionObject::removeLinkUncompiled(Link* ncmLink) {
		lockComposite();
		vector<Link*>::iterator i;
		for (i = uncompiledLinks->begin(); i != uncompiledLinks->end(); ++i) {
			if (*i == ncmLink) {
				uncompiledLinks->erase(i);
				unlockComposite();
				return;
			}
		}
		unlockComposite();
	}

	void CompositeExecutionObject::setLinkCompiled(FormatterLink* link) {
		if (link == NULL) {
			wclog << "CompositeExecutionObject::setLinkCompiled Warning! ";
			wclog << "trying to compile a NULL link" << endl;
			return;
		}

		Link* compiledLink;
		compiledLink = link->getNcmLink();

		if (compiledLink == NULL) {
			wclog << "CompositeExecutionObject::setLinkCompiled Warning! ";
			wclog << "formatterLink has returned a NULL ncmLink" << endl;
			return;
		}

		lockComposite();
		if (links != NULL) {
			links->push_back(link);
		}
		unlockComposite();
	}

	void CompositeExecutionObject::setLinkUncompiled(FormatterLink* link) {
		lockComposite();
		uncompiledLinks->push_back(link->getNcmLink());

		vector<FormatterLink*>::iterator i;
		for (i = links->begin(); i != links->end(); ++i) {
			if (*i == link) {
				links->erase(i);
				break;
			}
		}

		delete link;
		link = NULL;

		unlockComposite();
	}

	void CompositeExecutionObject::addNcmLink(Link* ncmLink) {
		lockComposite();
		uncompiledLinks->push_back(ncmLink);
		unlockComposite();
	}

	void CompositeExecutionObject::removeNcmLink(Link* ncmLink) {
		bool contains;

	  	contains = false;
	  	vector<Link*>::iterator i;
	  	lockComposite();
		for (i = uncompiledLinks->begin(); i != uncompiledLinks->end(); ++i) {
			if (*i == ncmLink) {
				contains = true;
				uncompiledLinks->erase(i);
				break;
			}
		}

	  	if (!contains) {
	  		FormatterLink* link;

	  		lockComposite();
	  		vector<FormatterLink*>::iterator j;
			for (j = links->begin(); j != links->end(); ++j) {
	  			link = *j;
	  			if (ncmLink == link->getNcmLink()) {
	  				links->erase(j);
	  				delete link;
	  				link = NULL;
	  				unlockComposite();
	  				return;
	  			}
	  		}
	  	}
	  	unlockComposite();
	}

	void CompositeExecutionObject::setAllLinksAsUncompiled(bool isRecursive) {
		FormatterLink* link;
		Link* ncmLink;
		int i, size;
		vector<FormatterLink*>::iterator j;
		ExecutionObject* childObject;

		lockComposite();
		while (!(links->empty())) {
			link = (*links)[links->size() - 1];
			ncmLink = link->getNcmLink();
			uncompiledLinks->push_back(ncmLink);
			delete link;
			link = NULL;
			j = links->begin() + (links->size() - 1);
			links->erase(j);
		}

		if (isRecursive) {
			size = execObjList->size();
			for (i = 0; i < size; i++) {
				childObject = (*execObjList)[i];
				if (childObject->instanceOf("CompositeExecutionObject")) {
					((CompositeExecutionObject*)childObject)->
						    setAllLinksAsUncompiled(isRecursive);
				}
			}
		}
		unlockComposite();
	}

	vector<FormatterLink*>* CompositeExecutionObject::getLinks() {
		return links;
	}

	void CompositeExecutionObject::setParentsAsListeners() {
		map<Node*, void*>::iterator parentObjects;
		CompositeExecutionObject* parentObject;

		lockComposite();
		parentObjects = parentTable->begin();
		while (parentObjects != parentTable->end()) {
			parentObject = (CompositeExecutionObject*)(parentObjects->second);

			// register parent as a composite presentation listener
			wholeContent->addEventListener(parentObject);
			++parentObjects;
		}
		unlockComposite();
	}

	void CompositeExecutionObject::unsetParentsAsListeners() {
		map<Node*, void*>::iterator parentObjects;
		CompositeExecutionObject* parentObject;

		lockComposite();
		parentObjects = parentTable->begin();
		while (parentObjects != parentTable->end()) {
			parentObject = (CompositeExecutionObject*)(parentObjects->second);

			// register parent as a composite presentation listener
			wholeContent->removeEventListener(parentObject);
			++parentObjects;
		}
		unlockComposite();
	}

	void CompositeExecutionObject::eventStateChanged(
		    void* changedEvent, short transition, short previousState) {

		FormatterEvent* event;
		event = (FormatterEvent*)changedEvent;
		if (!(event->instanceOf("PresentationEvent"))) {
			return;
		}

		event = (FormatterEvent*)changedEvent;
		switch (transition) {
			case EventUtil::TR_STARTS:
				if (runningEvents->empty() && pausedEvents->empty()) {
					setParentsAsListeners();
					wholeContent->start();
				}

				runningEvents->insert(event);
				break;

			case EventUtil::TR_ABORTS:
				lastTransition = transition;
				if (previousState == EventUtil::ST_OCCURRING) {
					runningEvents->erase(runningEvents->find(event));

				} else { // previousState == Event::ST_PAUSED
					pausedEvents->erase(pausedEvents->find(event));
				}

				if (runningEvents->empty() && pausedEvents->empty() &&
					    pendingLinks->empty()) {

					wholeContent->abort();
					unsetParentsAsListeners();
				}
				break;

			case EventUtil::TR_STOPS:
				if (((PresentationEvent*)event)->getRepetitions() == 0) {
					lastTransition = transition;
					if (previousState == EventUtil::ST_OCCURRING) {
						runningEvents->erase(runningEvents->find(event));

					} else { // previousState == Event::ST_PAUSED
						pausedEvents->erase(pausedEvents->find(event));
					}

					if (runningEvents->empty() && pausedEvents->empty() &&
						    pendingLinks->empty()) {

						wholeContent->stop();
						unsetParentsAsListeners();
					}
				}
				break;

			case EventUtil::TR_PAUSES:
				runningEvents->erase(runningEvents->find(event));
				pausedEvents->insert(event);
				if (runningEvents->empty()) {
					wholeContent->pause();
				}
				break;

			case EventUtil::TR_RESUMES:
				pausedEvents->erase(pausedEvents->find(event));
				runningEvents->insert(event);
				if (runningEvents->size() == 1) {
					wholeContent->resume();
				}
				break;

			default:
				break;
		}
	}

	void CompositeExecutionObject::linkEvaluationStarted(
		    FormatterCausalLink* link) {

		int linkNumber;

		wclog << "CompositeExecutionObject::linkEvaluationStarted link '"; 
		wclog << link->getNcmLink()->getId().c_str() << "'" << endl;

		lockComposite();
		if (pendingLinks->count(link) != 0) {
			linkNumber = (*pendingLinks)[link];
			(*pendingLinks)[link] = linkNumber + 1;

		} else {
			(*pendingLinks)[link] = 1;
		}
		unlockComposite();
	}

	void CompositeExecutionObject::linkEvaluationFinished(
		    FormatterCausalLink* link,
		    bool start) {

		int linkNumber;
		map<FormatterLink*, int>::iterator i;

		lockComposite();
		i = pendingLinks->find(link);
		if (i != pendingLinks->end()) {
			linkNumber = i->second;
			if (linkNumber == 1) {
				pendingLinks->erase(i);
				if (runningEvents->empty() && pausedEvents->empty() &&
					    pendingLinks->empty()) {

					unlockComposite();
					if (start) {
						// if nothing starts the composition may
						// stay locked as occurring

					} else if (lastTransition == EventUtil::TR_STOPS) {
						wholeContent->stop();
						unsetParentsAsListeners();

					} else {
						wholeContent->abort();
						unsetParentsAsListeners();
					}
					lockComposite();

				} else {
					/*if (runningEvents->size() == 1 && pausedEvents->empty()) {
						listRunningObjects();
					}*/
				}

			} else {
				(*pendingLinks)[link] = linkNumber - 1;
			}
		}
		unlockComposite();
	}

	void CompositeExecutionObject::listRunningObjects() {
		vector<ExecutionObject*>::iterator i;
		vector<FormatterEvent*>::iterator j;
		vector<FormatterEvent*>* events;
		ExecutionObject* object;
		FormatterEvent* event;

		cout << "CompositeExecutionObject::listRunningObjects for '";
		cout << id << "': ";
		i = execObjList->begin();
		while (i != execObjList->end()) {
			object = *i;
			events = object->getEvents();
			if (events != NULL) {
				j = events->begin();
				while (j != events->end()) {
					event = *j;
					if (event->getCurrentState() == EventUtil::ST_OCCURRING) {
						cout << "'" << object->getId().c_str() << "', ";
					}
					++j;
				}
				delete events;
				events = NULL;
			}
			++i;
		}
		cout << " runingEvents->size = '" << runningEvents->size() << endl;
	}

	void CompositeExecutionObject::lockComposite() {
		pthread_mutex_lock(&mutexComposite);
	}

	void CompositeExecutionObject::unlockComposite() {
		pthread_mutex_unlock(&mutexComposite);
	}
}
}
}
}
}
}
}
