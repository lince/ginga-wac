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

#include "../include/FormatterConverter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::converter;

#include "../include/FormatterScheduler.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
	FormatterScheduler::FormatterScheduler(
		    PlayerAdapterManager* playerManager,
		    RuleAdapter* ruleAdapter,
		    void* compiler) {

		this->playerManager = playerManager;
		this->ruleAdapter = ruleAdapter;
		this->layoutManager = new FormatterLayout();
		this->focusManager = new FormatterFocusManager(
				this->playerManager, (FormatterConverter*)compiler);

		this->schedulerListeners = new vector<FormatterSchedulerListener*>;
		this->compiler = compiler;
		this->documentEvents = new vector<FormatterEvent*>;
		this->documentStatus = new map<FormatterEvent*, bool>;
	}

	FormatterScheduler::~FormatterScheduler() {
		wclog << "FormatterScheduler::~FormatterScheduler" << endl;
		playerManager = NULL;
		ruleAdapter = NULL;
		if (layoutManager != NULL) {
			delete layoutManager;
			layoutManager = NULL;
		}

		if (focusManager != NULL) {
			delete focusManager;
			focusManager = NULL;
		}

		if (schedulerListeners != NULL) {
			delete schedulerListeners;
			schedulerListeners = NULL;
		}

		compiler = NULL;

		if (documentEvents != NULL) {
			delete documentEvents;
			documentEvents = NULL;
		}

		if (documentStatus != NULL) {
			delete documentStatus;
			documentStatus = NULL;
		}
	}

	bool FormatterScheduler::isPriorityListener() {
		return true;
	}

	FormatterLayout* FormatterScheduler::getLayoutManager() {
		return layoutManager;
	}

	bool FormatterScheduler::isDocumentRunning(FormatterEvent* event) {
		ExecutionObject* executionObject;
		CompositeExecutionObject* parentObject;
		FormatterEvent* documentEvent;

		/*NodeEntity* dataObject;
		NodeNesting* referPerspective;
		set<ReferNode*>* sameInstances = NULL;
		set<ReferNode*>::iterator i;*/

		executionObject = (ExecutionObject*)(event->getExecutionObject());
		/*dataObject = (NodeEntity*)(executionObject->getDataObject());

		if (dataObject != NULL && dataObject->instanceOf("NodeEntity")) {
			sameInstances = dataObject->getSameInstances();
			if (sameInstances != NULL && !sameInstances->empty()) {
				i = sameInstances->begin();
				while (i != sameInstances->end()) {
					referPerspective = new NodeNesting((*i)->getPerspective());
					++i;
				}
			}
		}*/

		parentObject = (CompositeExecutionObject*)(executionObject->
			    getParentObject());

		if (parentObject != NULL) {
			while (parentObject->getParentObject() != NULL) {
				executionObject = (ExecutionObject*)(parentObject);
				parentObject = (CompositeExecutionObject*)(parentObject->
					   getParentObject());
			}

			documentEvent = executionObject->getWholeContentPresentationEvent();

		} else {
			documentEvent = event;
		}

		if (documentStatus->count(documentEvent) != 0) {
			return (*documentStatus)[documentEvent];
		}

		return false;
	}

	void FormatterScheduler::setTimeBaseObject(
		    ExecutionObject* object,
		    FormatterPlayerAdapter* objectPlayer, string nodeId) {

		ExecutionObject* documentObject;
		ExecutionObject* parentObject;
		ExecutionObject* timeBaseObject;

		Node* documentNode;
		Node* compositeNode;
		Node* timeBaseNode;
		NodeNesting* perspective;
		NodeNesting* compositePerspective;
		FormatterPlayerAdapter* timeBasePlayer;

		wclog << "FormatterScheduler::setTimeBaseObject 1" << endl;

		if (nodeId.find_last_of('#') != std::string::npos) {
		  	return;
		}

		wclog << "FormatterScheduler::setTimeBaseObject 2" << endl;
		documentObject = object;
		parentObject = (ExecutionObject*)(documentObject->getParentObject());
		if (parentObject != NULL) {
			while (parentObject->getParentObject() != NULL) {
				documentObject = parentObject;
				if (documentObject->getDataObject()->instanceOf("ReferNode")) {
					break;
				}
				parentObject = (ExecutionObject*)(documentObject->
					    getParentObject());
			}
		}

		if (documentObject == NULL || documentObject->getDataObject() == NULL){
			return;
		}

		wclog << "FormatterScheduler::setTimeBaseObject 3" << endl;

		documentNode = documentObject->getDataObject();
		if (documentNode->instanceOf("ReferNode")) {
			compositeNode = (NodeEntity*)((ReferNode*)documentNode)->
				    getReferredEntity();
		} else {
			compositeNode = documentNode;
		}

		if (compositeNode == NULL ||
			    !(compositeNode->instanceOf("CompositeNode"))) {

			return;
		}

		wclog << "FormatterScheduler::setTimeBaseObject 4" << endl;

		timeBaseNode = ((CompositeNode*)compositeNode)->
			    recursivelyGetNode(nodeId);

		if (timeBaseNode == NULL ||
			    !(timeBaseNode->instanceOf("ContentNode"))) {

			return;
		}

		wclog << "FormatterScheduler::setTimeBaseObject 5" << endl;

		perspective = new NodeNesting(timeBaseNode->getPerspective());
		if (documentNode->instanceOf("ReferNode")) {
			perspective->removeHeadNode();
			compositePerspective = new NodeNesting(
				    documentNode->getPerspective());

			compositePerspective->append(perspective);
			perspective = compositePerspective;
		}

		wclog << "FormatterScheduler::setTimeBaseObject perspective Id = '";
		wclog << perspective->getId().c_str() << "'" << endl;

		try {
			timeBaseObject = ((FormatterConverter*)compiler)->
				    getExecutionObject(
				    	    perspective,
				    	    NULL,
				    	    ((FormatterConverter*)compiler)->getDepthLevel());

			if (timeBaseObject != NULL) {
				timeBasePlayer = playerManager->getPlayer(timeBaseObject);
				if (timeBasePlayer != NULL) {
					wclog << "FormatterScheduler::setTimeBaseObject setting";
					wclog << " time base to '";
					wclog << timeBaseObject->getId().c_str() << "'" << endl;
					objectPlayer->setTimeBasePlayer(timeBasePlayer);
				}
			}

		} catch (ObjectCreationForbiddenException* exc) {
			return;
		}
	}

	void FormatterScheduler::runAction(void* someAction) {
		runAction(((LinkSimpleAction*)someAction)->getEvent(), someAction);
	}

	void FormatterScheduler::runAction(
			FormatterEvent* event, void* someAction) {

		LinkSimpleAction* action;
		action = (LinkSimpleAction*)someAction;

		ExecutionObject* executionObject;
		CascadingDescriptor* descriptor;
		FormatterPlayerAdapter* player;
		NodeEntity* dataObject;
		ContentAnchor* anchor;
		short actionType;
		string attName;
		string attValue;
		/*double time;

		time = getCurrentTimeMillis();*/

		if (event == NULL) {
			cout << "FormatterScheduler::runAction Warning! Trying to ";
			cout << "run a NULL event" << endl;
			return;
		}

		executionObject = (ExecutionObject*)(event->getExecutionObject());

		/*cout << "FormatterScheduler::runAction event '";
		cout << event->getId() << "' for '";
		cout << executionObject->getId() << "'" << endl;*/

		if (isDocumentRunning(event) && !executionObject->isCompiled()) {
			((FormatterConverter*)compiler)->compileExecutionObjectLinks(
				    executionObject,
				    ((FormatterConverter*)compiler)->getDepthLevel());
		}

		dataObject = (NodeEntity*)(executionObject->getDataObject()->
			    getDataEntity());

		if (dataObject->instanceOf("NodeEntity")) {
			set<ReferNode*>* gradSame;
			set<ReferNode*>::iterator i;
			gradSame = ((NodeEntity*)dataObject)->getGradSameInstances();
			if (gradSame != NULL) {
				((FormatterConverter*)compiler)->checkGradSameInstance(
						gradSame, executionObject);

				cout << "FormatterScheduler::runAction refer = '";
				cout << dataObject->getId() << "' perspective = '";
				cout << executionObject->getNodePerspective()->
						getHeadNode()->getId() << "'";

				cout << endl;
			}
		}

		if (dataObject->instanceOf("ContentNode") &&
			    ((ContentNode*)dataObject)->getNodeType() ==
			    ContentNode::SETTING_NODE &&
			    action->instanceOf("LinkAssignmentAction") &&
			    event->instanceOf("AttributionEvent")) {

			attName = ((AttributionEvent*)event)->getAnchor()->
				    getPropertyName();

			attValue = ((LinkAssignmentAction*)action)->getValue();
			if (attValue.substr(0, 1) == "$") {
				attValue = solveImplicitRefAssessment(
						attValue,
						(LinkAssignmentAction*)action,
						(AttributionEvent*)event);
			}

			event->start();
		    ((AttributionEvent*)event)->setValue(attValue);

			/*cout << "FormatterScheduler::runAction over settingnode";
			cout << " evId '" << event->getId() << "' for '";
			cout << executionObject->getId() << "' propName '";
			cout << attName << "', propValue '" << attValue << "'" << endl;*/

			if (attName == "currentFocus" ||
					attName == "service.currentFocus") {

				focusManager->setFocus(attValue);

			} else if (attName == "currentKeyMaster" ||
					attName == "service.currentKeyMaster") {

				focusManager->setKeyMaster(attValue);

			} else {
				PresentationContext::getInstance()->setPropertyValue(
					    attName, attValue);
			}
			event->stop();

		} else if (executionObject->instanceOf("ExecutionObjectSwitch") &&
			    event->instanceOf("SwitchEvent")) {

			runActionOverSwitch(
				    (ExecutionObjectSwitch*)executionObject,
				    (SwitchEvent*)event, action);

		} else if (executionObject->instanceOf("CompositeExecutionObject") &&
				(executionObject->getDescriptor() == NULL ||
				executionObject->getDescriptor()->getPlayerName() == "")) {

			runActionOverComposition(
				    (CompositeExecutionObject*)executionObject, action);

		} else if (event->instanceOf("AttributionEvent")) {
			runActionOverProperty(event, action);

		} else {
			player = playerManager->getPlayer(executionObject);
			if (player == NULL) {
				cout << "Player = NULL for ";
				cout << executionObject->getId().c_str() << endl;
				return;
			}

			if (executionObject->instanceOf("ProceduralExecutionObject") &&
					!event->instanceOf("AttributionEvent")) {

				runActionOverProceduralObject(
						(ProceduralExecutionObject*)executionObject,
						event, player, action);

				return;
			}

			actionType = action->getType();
			switch (actionType) {
				case SimpleAction::ACT_START:
					//TODO: if (isDocumentRunning(event)) {
					if (!player->hasPrepared()) {
						if (ruleAdapter->adaptDescriptor(
								executionObject)) {

							descriptor = executionObject->getDescriptor();
							if (descriptor != NULL) {
								descriptor->setFormatterRegion(
										layoutManager);
							}
						}

	        			player->prepare(
	        				    executionObject,
	        				    (PresentationEvent*)event);

	        			if (executionObject != NULL && executionObject->
	        				    getDescriptor() != NULL) {

		        			// look for a reference time base player
							attValue = executionObject->getDescriptor()->
								    getParameterValue("x-timeBaseObject");

							if (attValue != "") {
								setTimeBaseObject(
									    executionObject,
									    player,
									    attValue);
							}
						}

						Surface* renderedSurface;
						renderedSurface = player->getObjectDisplay();
	        			if (renderedSurface != NULL) {
							layoutManager->prepareFormatterRegion(
									executionObject, renderedSurface);
						}
	        		}

	        		event->addEventListener(this);

	        		//time = getCurrentTimeMillis() - time;
	        		player->start();

		        	/*} else {
		        		cout << "FormatterScheduler::runAction event '";
		        		cout << event->getId() << "' for '";
		        		cout << executionObject->getId() << "'";
		        		cout << "is !running" << endl;
		        	}*/
					break;

				case SimpleAction::ACT_PAUSE:
					player->pause();
					break;

				case SimpleAction::ACT_RESUME:
					player->resume();
					break;

				case SimpleAction::ACT_ABORT:
					player->abort();
					break;

				case SimpleAction::ACT_STOP:
					player->stop();
					break;
			}
		}
	}

	void FormatterScheduler::runActionOverProperty(
			FormatterEvent* event, LinkSimpleAction* action) {

		short actionType;
		string propValue;

		ExecutionObject* executionObject;
		FormatterPlayerAdapter* player;
		Animation* anim;

		anim = ((LinkAssignmentAction*)action)->getAnimation();
		executionObject = (ExecutionObject*)(event->getExecutionObject());
		player = playerManager->getPlayer(executionObject);
		actionType = action->getType();

		switch (actionType) {
			case SimpleAction::ACT_START:
				cout << "FormatterScheduler::runActionOverProperty";
				cout << " start" << endl;
				break;

			case SimpleAction::ACT_PAUSE:
				cout << "FormatterScheduler::runActionOverProperty";
				cout << " pause" << endl;
				break;

			case SimpleAction::ACT_RESUME:
				cout << "FormatterScheduler::runActionOverProperty";
				cout << " resume" << endl;
				break;

			case SimpleAction::ACT_ABORT:
				cout << "FormatterScheduler::runActionOverProperty";
				cout << " abort" << endl;
				break;

			case SimpleAction::ACT_STOP:
				cout << "FormatterScheduler::runActionOverProperty";
				cout << " stop" << endl;
				break;

			case SimpleAction::ACT_SET:
				/*cout << "FormatterScheduler::runActionOverProperty";
				cout << " over '" << event->getId() << "' for '";
				cout << executionObject->getId() << "'" << endl;*/
				if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
					cout << "FormatterScheduler::runActionOverProperty";
					cout << " trying to set an event that is not ";
					cout << "sleeping: '" << event->getId() << "'" << endl;
					return;
				}

				propValue = ((LinkAssignmentAction*)action)->getValue();
				if (propValue.substr(0, 1) == "$") {
					propValue = solveImplicitRefAssessment(
							propValue,
							(LinkAssignmentAction*)action,
							(AttributionEvent*)event);
				}

				event->start();
				((AttributionEvent*)event)->setValue(propValue);
				if (player != NULL && player->hasPrepared()) {
					player->setPropertyValue(
						    (AttributionEvent*)event, propValue, anim);

				} else if (!executionObject->setPropertyValue(
						    (AttributionEvent*)event,
						    propValue, anim)) {

					event->stop();
				}
				break;
		}
	}

	void FormatterScheduler::runActionOverProceduralObject(
			ProceduralExecutionObject* executionObject,
			FormatterEvent* event,
			FormatterPlayerAdapter* player,
			LinkSimpleAction* action) {

		CascadingDescriptor* descriptor;

		string attValue, attName;
		int actionType;
		actionType = action->getType();
		switch (actionType) {
			case SimpleAction::ACT_START:
				if (isDocumentRunning(event)) {
	        		if (!player->hasPrepared()) {
						if (ruleAdapter->adaptDescriptor(
								executionObject)) {

							descriptor = executionObject->getDescriptor();
							if (descriptor != NULL) {
								descriptor->setFormatterRegion(
										layoutManager);
							}
						}
	        		}

	        		player->prepare(executionObject, event);

	        		if (!player->hasPrepared()) {
	        			if (executionObject->getDescriptor() != NULL) {
		        			// look for a reference time base player
							attValue = executionObject->getDescriptor()->
								    getParameterValue("x-timeBaseObject");

							if (attValue != "") {
								setTimeBaseObject(
									    executionObject,
									    player,
									    attValue);
							}
						}

						Surface* renderedSurface;
						renderedSurface = player->getObjectDisplay();
	        			if (renderedSurface != NULL) {
							layoutManager->prepareFormatterRegion(
									executionObject, renderedSurface);
						} else {
							cout << "FormatterScheduler::runProceduralAction";
							cout << " Warning! renderedSurface is NULL!";
							cout << endl;
						}
	        		}

	        		event->addEventListener(this);
	        		((ProceduralPlayerAdapter*)player)->setCurrentEvent(event);
	        		player->start();
	        	}
				break;

			case SimpleAction::ACT_PAUSE:
				((ProceduralPlayerAdapter*)player)->setCurrentEvent(event);
				player->pause();
				break;

			case SimpleAction::ACT_RESUME:
				((ProceduralPlayerAdapter*)player)->setCurrentEvent(event);
				player->resume();
				break;

			case SimpleAction::ACT_ABORT:
				((ProceduralPlayerAdapter*)player)->setCurrentEvent(event);
				player->abort();
				break;

			case SimpleAction::ACT_STOP:
				((ProceduralPlayerAdapter*)player)->setCurrentEvent(event);
				player->stop();
				break;
		}
	}

	void FormatterScheduler::runActionOverComposition(
		    CompositeExecutionObject* compositeObject,
		    LinkSimpleAction* action) {

		CompositeNode* compositeNode;
		Port* port;
		NodeNesting* compositionPerspective;
		NodeNesting* perspective;

		vector<ExecutionObject*>* objects;
		ExecutionObject* childObject;
		FormatterEvent* childEvent;
		int i, size;
		vector<FormatterEvent*>* events;
		vector<ExecutionObject*>::iterator j;

		/*cout << "Run action " << action->getType() << " over COMPOSITION ";
		cout << compositeObject->getId().c_str() << endl;*/

		if (action->getType() == SimpleAction::ACT_SET) {
			// nothing to be done

		} else if (action->getType() == SimpleAction::ACT_START) {
			compositeNode = (CompositeNode*)(compositeObject->getDataObject()->
				    getDataEntity());

			size = compositeNode->getNumPorts();
			compositionPerspective = compositeObject->getNodePerspective();
			events = new vector<FormatterEvent*>;
			for (i = 0; i < size; i++) {
				port = compositeNode->getPort(i);
				perspective = compositionPerspective->copy();
				perspective->append(port->getMapNodeNesting());
				try {
					childObject = ((FormatterConverter*)compiler)->
						    getExecutionObject(
						    	    perspective,
						    	    NULL,
						    	    ((FormatterConverter*)compiler)->
						    	    	    getDepthLevel());

					if (childObject != NULL && port->getEndInterfacePoint() !=
						    NULL && port->getEndInterfacePoint()->instanceOf(
						    	    "ContentAnchor")) {

						childEvent = (PresentationEvent*)(
								((FormatterConverter*)compiler)->getEvent(
							    	    childObject,
							    	    port->getEndInterfacePoint(),
							    	    EventUtil::EVT_PRESENTATION, ""));

						if (childEvent != NULL) {
							events->push_back(childEvent);
						}
					}
				}
				catch (ObjectCreationForbiddenException* exc) {
					// keep on starting child objects
				}
			}
			size = events->size();
			for (i = 0; i < size; i++) {
				runAction((*events)[i], action);
			}
			delete events;
			events = NULL;

		} else {
			events = new vector<FormatterEvent*>;

			objects = compositeObject->getExecutionObjects();
			if (objects != NULL) {
				j = objects->begin();
				while (j != objects->end()) {
					childObject = (*j);
					childEvent = childObject->getMainEvent();
					if (childEvent == NULL) {
						childEvent = childObject->
							    getWholeContentPresentationEvent();
					}

					if (childEvent != NULL) {
						events->push_back(childEvent);
					}
					++j;
				}
				delete objects;
				objects = NULL;
			}

			size = events->size();
			for (i = 0; i < size; i++) {
				runAction((*events)[i], action);
			}

			delete events;
			events = NULL;
		}
	}

	void FormatterScheduler::runActionOverSwitch(
		    ExecutionObjectSwitch* switchObject,
		    SwitchEvent* event, LinkSimpleAction* action) {

		ExecutionObject* selectedObject;
		FormatterEvent* selectedEvent;

		wclog << "Run action " << action->getType() << " over SWITCH ";
		wclog << switchObject->getId().c_str() << endl;

		selectedObject = switchObject->getSelectedObject();
		if (selectedObject == NULL) {
			selectedObject = ((FormatterConverter*)compiler)->
				    processExecutionObjectSwitch(switchObject);

			if (selectedObject == NULL) {
				return;
			}
		}

		selectedEvent = event->getMappedEvent();
		if (selectedEvent != NULL) {
			runAction(selectedEvent, action);

		} else {
			runSwitchEvent(switchObject, event, selectedObject, action);
		}

		if (action->getType() == SimpleAction::ACT_STOP ||
			    action->getType() == SimpleAction::ACT_ABORT) {

			switchObject->select(NULL);
		}
	}

	void FormatterScheduler::runSwitchEvent(
		    ExecutionObjectSwitch* switchObject,
		    SwitchEvent* switchEvent,
		    ExecutionObject* selectedObject,
		    LinkSimpleAction* action) {

		FormatterEvent* selectedEvent;
		SwitchPort* switchPort;
		vector<Port*>* mappings;
		vector<Port*>::iterator i;
		Port* mapping;
		NodeNesting* nodePerspective;
		ExecutionObject* endPointObject;

		selectedEvent = NULL;
		switchPort = (SwitchPort*)(switchEvent->getInterfacePoint());
		mappings = switchPort->getPorts();
		if (mappings != NULL) {
			i = mappings->begin();
			while (i != mappings->end()) {
				mapping = *i;
				if (mapping->getNode() == selectedObject->getDataObject()) {
					nodePerspective = switchObject->getNodePerspective();
					nodePerspective->append(mapping->getMapNodeNesting());
					try {
						endPointObject = ((FormatterConverter*)compiler)->
							    getExecutionObject(
							    nodePerspective,
							    NULL,
							    ((FormatterConverter*)compiler)->
							    	    getDepthLevel());

						if (endPointObject != NULL) {
							selectedEvent = ((FormatterConverter*)compiler)->
								    getEvent(
								    endPointObject,
								    mapping->getEndInterfacePoint(),
								    switchEvent->getEventType(),
								    switchEvent->getKey());
						}

					} catch (ObjectCreationForbiddenException* exc) {
						// continue
					}

					break;
				}
				++i;
			}
		}

		if (selectedEvent != NULL) {
			switchEvent->setMappedEvent(selectedEvent);
			runAction(selectedEvent, action);
		}
	}

	string FormatterScheduler::solveImplicitRefAssessment(
			string propValue,
			LinkAssignmentAction* action, AttributionEvent* event) {

		FormatterEvent* refEvent;
		ExecutionObject* refObject;
		string roleId;

		roleId = action->getValue();
		roleId = roleId.substr(1, roleId.length());

		refEvent = ((AttributionEvent*)event)->
				getImplicitRefAssessmentEvent(roleId);

		if (refEvent != NULL) {
			propValue = ((AttributionEvent*)
					refEvent)->getCurrentValue();

			refObject = ((ExecutionObject*)(refEvent->getExecutionObject()));
			/*cout << "FormatterScheduler::solveImplicitRefAssessment refEvent";
			cout << " for '" << refObject->getId() << "' is '";
			cout << refEvent->getId() << "', got '" << propValue << "'";
			cout << endl;*/
			return propValue;

		} else {
			cout << "FormatterScheduler::solveImplicitRefAssessment warning!";
			cout << " refEvent not found for '" << event->getId() << "', ";
			cout << " LinkAssignmentAction value is '" << action->getValue();
			cout << "'" << endl;
			cout << endl;
		}

		return "";
	}

	void FormatterScheduler::startEvent(FormatterEvent* event) {
		LinkSimpleAction* fakeAction;

		fakeAction = new LinkSimpleAction(event, SimpleAction::ACT_START);
		runAction(event, fakeAction);
	}

	void FormatterScheduler::stopEvent(FormatterEvent* event) {
		LinkSimpleAction* fakeAction;

		fakeAction = new LinkSimpleAction(event, SimpleAction::ACT_STOP);
		runAction(event, fakeAction);
	}

	void FormatterScheduler::pauseEvent(FormatterEvent* event) {
		LinkSimpleAction* fakeAction;

		fakeAction = new LinkSimpleAction(event, SimpleAction::ACT_PAUSE);
		runAction(event, fakeAction);
	}

	void FormatterScheduler::resumeEvent(FormatterEvent* event) {
		LinkSimpleAction* fakeAction;

		fakeAction = new LinkSimpleAction(event, SimpleAction::ACT_RESUME);
		runAction(event, fakeAction);
	}

	void FormatterScheduler::initializeDefaultSettings() {
		string value;
		double alfa;

		value = PresentationContext::getInstance()->getPropertyValue(
			    PresentationContext::DEFAULT_FOCUS_BORDER_TRANSPARENCY);

		if (value != "") {
			alfa = stof(value);

		} else {
			alfa = 1;
		}
		int alpha;
		alpha = (int)(255 * alfa);

		value = PresentationContext::getInstance()->getPropertyValue(
			    PresentationContext::DEFAULT_FOCUS_BORDER_COLOR);

		if (value != "") {
			focusManager->setDefaultFocusBorderColor(new Color(value, alpha));
		}

		value = PresentationContext::getInstance()->getPropertyValue(
			    PresentationContext::DEFAULT_FOCUS_BORDER_WIDTH);

		if (value != "") {
			focusManager->setDefaultFocusBorderWidth((int)
				    stof(value));
		}

		value = PresentationContext::getInstance()->getPropertyValue(
			    PresentationContext::DEFAULT_SEL_BORDER_COLOR);

		if (value != "") {
			focusManager->setDefaultSelBorderColor(new Color(value, alpha));
		}
	}

	void FormatterScheduler::initializeDocumentSettings(Node* node) {
		string nodeType, value;
		vector<Anchor*>* anchors;
		vector<Anchor*>::iterator i;
		vector<Node*>* nodes;
		vector<Node*>::iterator j;
		Anchor* anchor;
		PropertyAnchor* attributeAnchor;

		if (node->instanceOf("ContentNode")) {
			nodeType = ((ContentNode*)node)->getNodeType();
			if (upperCase(nodeType) ==
				    upperCase(ContentNode::SETTING_NODE)) {

				anchors = ((ContentNode*)node)->getAnchors();
				if (anchors != NULL) {
					i = anchors->begin();
					while (i != anchors->end()) {
						anchor = (*i);
						if (anchor->instanceOf("PropertyAnchor")) {
							attributeAnchor = (PropertyAnchor*)anchor;
							value = attributeAnchor->getPropertyValue();
							if (value != "") {
								PresentationContext::getInstance()->
									setPropertyValue(
									    attributeAnchor->getPropertyName(),
									    value);
							}
						}
						++i;
					}
				}
			}

		} else if (node->instanceOf("CompositeNode")) {
			nodes = ((CompositeNode*)node)->getNodes();
			if (nodes != NULL) {
				j = nodes->begin();
				while (j != nodes->end()) {
					initializeDocumentSettings(*j);
					++j;
				}
			}

		} else if (node->instanceOf("ReferNode")) {
			initializeDocumentSettings(
				    (NodeEntity*)((ReferNode*)node)->getDataEntity());
		}
	}

	void FormatterScheduler::startDocument(
		    FormatterEvent* documentEvent,
		    vector<FormatterEvent*>* entryEvents) {

		if (documentEvent == NULL || entryEvents == NULL) {
			wclog << "FormatterScheduler::startDocument Warning! ";
			wclog << "documentEvent == NULL || entryEvents == NULL" << endl;
			return;
		}

		if (entryEvents->empty()) {
			wclog << "FormatterScheduler::startDocument Warning! ";
			wclog << "entryEvents is empty" << endl;
			return;
		}

		vector<FormatterEvent*>::iterator it;
		for (it = documentEvents->begin(); it != documentEvents->end(); ++it) {
			if (*it == documentEvent) {
				return;
			}
		}

		documentEvent->addEventListener(this);
		documentEvents->push_back(documentEvent);
		(*documentStatus)[documentEvent] = true;

		initializeDocumentSettings(((ExecutionObject*)(documentEvent->
			    getExecutionObject()))->getDataObject());

		initializeDefaultSettings();

		int i, size;
		FormatterEvent* event;

		size = entryEvents->size();
		for (i = 0; i < size; i++) {
			event = (*entryEvents)[i];
			startEvent(event);
		}
	}

	void FormatterScheduler::removeDocument(FormatterEvent* documentEvent) {
		ExecutionObject* obj;
		//TODO: do a better way to remove documents (see lockComposite)
		obj = (ExecutionObject*)(documentEvent->getExecutionObject());
		if (compiler != NULL) {
			((FormatterConverter*)compiler)->removeExecutionObject(obj);
		}

		if (documentEvents != NULL) {
			vector<FormatterEvent*>::iterator i;
			for (i = documentEvents->begin();
					i != documentEvents->end(); ++i) {

				if (*i == documentEvent) {
					documentEvents->erase(i);
					break;
				}
			}
		}

		if (documentStatus != NULL) {
			documentStatus->erase(documentStatus->find(documentEvent));
		}
	}

	void FormatterScheduler::stopDocument(FormatterEvent* documentEvent) {
		cout << "FormatterScheduler::stopDocument event received" << endl;
		if (documentStatus->count(documentEvent) != 0) {
			ExecutionObject* executionObject;

			documentEvent->removeEventListener(this);
			(*documentStatus)[documentEvent] = false;

			executionObject = (ExecutionObject*)(documentEvent->
				    getExecutionObject());

			if (executionObject->instanceOf("CompositeExecutionObject")) {
				((CompositeExecutionObject*)executionObject)->
					    setAllLinksAsUncompiled(true);
			}

			stopEvent(documentEvent);

			int i, size;
			FormatterSchedulerListener* listener;

			size = schedulerListeners->size();
			for (i = 0; i < size; i++) {
				listener = (*schedulerListeners)[i];
				listener->presentationCompleted(documentEvent);
			}

			removeDocument(documentEvent);
		}
	}

	void FormatterScheduler::pauseDocument(FormatterEvent* documentEvent) {
		vector<FormatterEvent*>::iterator i;
		for (i = documentEvents->begin(); i != documentEvents->end(); ++i) {
			if (*i == documentEvent) {
				(*documentStatus)[documentEvent] = false;
				pauseEvent(documentEvent);
				break;
			}
		}
	}

	void FormatterScheduler::resumeDocument(FormatterEvent* documentEvent) {
		bool contains;
		contains = false;
		vector<FormatterEvent*>::iterator i;
		for (i = documentEvents->begin(); i != documentEvents->end(); ++i) {
			if (*i == documentEvent) {
				contains = true;
				break;
			}
		}

		if (contains) {
			resumeEvent(documentEvent);
			(*documentStatus)[documentEvent] = true;
		}
	}

	void FormatterScheduler::stopAllDocuments() {
		int i, size;
		vector<FormatterEvent*>* auxDocEventList;
		FormatterEvent* documentEvent;

		if (!documentEvents->empty()) {
			auxDocEventList = new vector<FormatterEvent*>(*documentEvents);

			size = auxDocEventList->size();
			for (i = 0; i < size; i++) {
				documentEvent = (*auxDocEventList)[i];
				stopDocument(documentEvent);
			}

			auxDocEventList->clear();
			delete auxDocEventList;
			auxDocEventList = NULL;
		}
	}

	void FormatterScheduler::pauseAllDocuments() {
		int i, size;
		FormatterEvent* documentEvent;

		if (!documentEvents->empty()) {
			size = documentEvents->size();
			for (i = 0; i < size; i++) {
				documentEvent = (*documentEvents)[i];
				pauseDocument(documentEvent);
			}
		}
	}

	void FormatterScheduler::resumeAllDocuments() {
		int i, size;
		FormatterEvent* documentEvent;

		if (!documentEvents->empty()) {
			size = documentEvents->size();
			for (i = 0; i < size; i++) {
				documentEvent = (*documentEvents)[i];
				resumeDocument(documentEvent);
			}
		}
	}

	void FormatterScheduler::eventStateChanged(
		    void* someEvent, short transition, short previousState) {

		ExecutionObject* object;
		FormatterPlayerAdapter* player;
		int i, size;
		FormatterSchedulerListener* listener;
		FormatterEvent* event;
		bool contains;

		event = (FormatterEvent*)someEvent;

		contains = false;
		vector<FormatterEvent*>::iterator it;
		for (it = documentEvents->begin(); it != documentEvents->end(); ++it) {
			if (*it == event) {
				contains = true;
				break;
			}
		}

		if (contains) {
			switch (transition) {
				case EventUtil::TR_STOPS:
				case EventUtil::TR_ABORTS:
					size = schedulerListeners->size();
					for (i = 0; i < size; i++) {
						listener = (*schedulerListeners)[i];
						listener->presentationCompleted(event);
					}

					removeDocument(event);
					break;
			}

		} else {
			switch (transition) {
				case EventUtil::TR_STARTS:
					if (isDocumentRunning(event)) {
						object = (ExecutionObject*)(event->
								getExecutionObject());

						player = playerManager->getPlayer(object);
						if (player != NULL) {
							layoutManager->showObject(object);
							focusManager->showObject(object);
						}
					}
					break;

				case EventUtil::TR_STOPS:
					if (((PresentationEvent*)event)->getRepetitions() == 0) {
						event->removeEventListener(this);
						object = (ExecutionObject*)(event->
								getExecutionObject());

						player = playerManager->getPlayer(object);
						Surface* renderedSurface;
						renderedSurface = player->getObjectDisplay();
						if (renderedSurface != NULL) {
							focusManager->hideObject(object);
							layoutManager->hideObject(object);
						}
					}
					break;

				case EventUtil::TR_ABORTS:
					event->removeEventListener(this);
					object = (ExecutionObject*)(event->
							getExecutionObject());

					player = playerManager->getPlayer(object);
					Surface* renderedSurface;
					renderedSurface = player->getObjectDisplay();
					if (renderedSurface != NULL) {
						focusManager->hideObject(object);
						layoutManager->hideObject(object);
					}
					break;
			}
		}
	}

	void FormatterScheduler::addSchedulerListener(
		    FormatterSchedulerListener* listener) {

		bool contains;
		contains = false;
		vector<FormatterSchedulerListener*>::iterator i;
		for (i = schedulerListeners->begin();
			    i != schedulerListeners->end(); ++i) {

			if (*i == listener) {
				contains = true;
				break;
			}
		}

		if (!contains) {
			schedulerListeners->push_back(listener);
		}
	}

	void FormatterScheduler::removeSchedulerListener(
		    FormatterSchedulerListener* listener) {

		vector<FormatterSchedulerListener*>::iterator i;
		for (i = schedulerListeners->begin();
			    i != schedulerListeners->end(); ++i) {

			if (*i == listener) {
				schedulerListeners->erase(i);
				return;
			}
		}
	}
}
}
}
}
}
