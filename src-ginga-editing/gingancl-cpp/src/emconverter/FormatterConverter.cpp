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

#include "../../include/emconverter/FormatterConverter.h"
#include "../../include/emconverter/FormatterLinkConverter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace emconverter {
	FormatterConverter::FormatterConverter(RuleAdapter* ruleAdapter) {
		executionObjects = new map<string, ExecutionObject*>;
		settingObjects = new set<ExecutionObject*>;
		linkCompiler = (void*)(new FormatterLinkConverter(this));

		this->scheduler = NULL;
		this->ruleAdapter = ruleAdapter;

		depthLevel = 1;
		//depthLevel = Formatter::DEEPEST_LEVEL;
	}

	FormatterConverter::~FormatterConverter() {
		ruleAdapter = NULL;
		scheduler = NULL;
		if (executionObjects != NULL) {
			delete executionObjects;
			executionObjects = NULL;
		}

		linkCompiler = NULL;
	}

	void FormatterConverter::setScheduler(FormatterScheduler* scheduler) {
		this->scheduler = scheduler;
	}

	void FormatterConverter::setDepthLevel(int level) {
		depthLevel = level;
	}

	int FormatterConverter::getDepthLevel() {
		return depthLevel;
	}

	CompositeExecutionObject* FormatterConverter::addSameInstance(
		    ExecutionObject* executionObject, ReferNode* referNode) {

		NodeNesting* referPerspective;
		CompositeExecutionObject* referParentObject;
		CascadingDescriptor* desc;
		string objectId;

		referPerspective = new NodeNesting(referNode->getPerspective());

		/*cout << "FormatterConverter::addSameInstance referNode entity '";
		cout << referNode->getReferredEntity()->getId();
		cout << "' execution object '" << executionObject->getId();
		cout << "'" << endl;*/

		try {
			referParentObject = getParentExecutionObject(
				    referPerspective, depthLevel);

			if (referParentObject != NULL) {
				executionObject->addParentObject(
					    referNode,
					    referParentObject,
					    referPerspective->getNode(
					    	    referPerspective->getNumNodes() - 2));

				// A new entry for the execution object is inserted using
				// the refer node id. As a consequence, links referring to the
				// refer node will generate events in the execution object.
				desc = executionObject->getDescriptor();
				if (desc != NULL) {
					objectId = (referPerspective->getId() + "/" +
							executionObject->getDescriptor()->getId());

				} else {
					objectId = referPerspective->getId();
				}

				(*executionObjects)[objectId] = executionObject;
			}

			return referParentObject;

		} catch (ObjectCreationForbiddenException* exc) {
			// nothing to be done
			return NULL;
		}
	}

	void FormatterConverter::checkGradSameInstance(
			set<ReferNode*>* gradSame, ExecutionObject* object) {

		cout << "FormatterConverter::checkGradSameInstance obj '";
		cout << object->getId() << "'";

		NodeNesting* entityPerspective;
		NodeNesting* perspective;
		NodeNesting* referPerspective;
		set<ReferNode*>::iterator i;
		NodeEntity* dataObject;

		dataObject = (NodeEntity*)(object->getDataObject());
		perspective = object->getNodePerspective();
		cout << " objectPerspective = '" << perspective->getId() << "'";
		cout << " with anchorNode = '";
		cout << perspective->getAnchorNode()->getId();
		cout << "' with entity = '" << dataObject->getId() << "' with";
		entityPerspective = new NodeNesting(dataObject->getPerspective());
		cout << " perspective '" << entityPerspective->getId();
		i = gradSame->begin();
		while (i != gradSame->end()) {
			referPerspective = new NodeNesting((*i)->getPerspective());
			cout << "', gradNode = '";
			cout << (*i)->getId() << "' has perspective '";
			cout << referPerspective->getId() << "' with anchorNode '";
			cout << referPerspective->getAnchorNode()->getId();
			++i;
		}
		cout << "'" << endl;
	}

	void FormatterConverter::addExecutionObject(
		    ExecutionObject* executionObject,
		    CompositeExecutionObject* parentObject,
		    int depthLevel) {

		NodeEntity* dataObject;
		set<ReferNode*>* sameInstances;
		set<ReferNode*>::iterator i;
		ReferNode* referNode;
		NodeNesting* referPerspective;
		Node* headNode;
		CascadingDescriptor* descriptor;
		Entity* entity;

		vector<Node*>* nodes;
		vector<Node*>::iterator j;

		(*executionObjects)[executionObject->getId()] = executionObject;
		dataObject = (NodeEntity*)(executionObject->getDataObject());

		/*cout << endl;
		cout << "FormatterConverter::addExecutionObject '";
		cout << executionObject->getId() << "' in parent '";
		if (parentObject != NULL) {
			cout << parentObject->getId() << "'";
		} else {
			cout << "NULL'";
		}
		cout << " and dataObject '" << dataObject->getId() << "':";
		dataObject->printHierarchy();
		cout << endl;*/

		if (dataObject->instanceOf("ContentNode") &&
			    ((ContentNode*)dataObject)->getNodeType() ==
			    ContentNode::SETTING_NODE) {

			settingObjects->insert(executionObject);
		}

		if (dataObject->instanceOf("ReferNode")) {
			if (((ReferNode*)dataObject)->getInstanceType() == "instSame") {
				entity = ((ReferNode*)dataObject)->getDataEntity();
				if (entity->instanceOf("ContentNode") &&
						((ContentNode*)entity)->getNodeType() ==
							ContentNode::SETTING_NODE) {

					settingObjects->insert(executionObject);
				}
			}
		}

		if (parentObject != NULL) {
			parentObject->addExecutionObject(executionObject);
		}

		if (dataObject->instanceOf("NodeEntity")) {
			sameInstances = dataObject->getInstSameInstances();
			if (sameInstances != NULL) {
				i = sameInstances->begin();

				while (i != sameInstances->end()) {
					referNode = (ReferNode*)(*i);
					headNode = executionObject->getNodePerspective()->
							getHeadNode();

					if (headNode->instanceOf("CompositeNode")) {
						if (((CompositeNode*)headNode)->
								recursivelyContainsNode(referNode)) {

							addSameInstance(executionObject, referNode);
						}
					}
					++i;
				}
			}
		}

		descriptor = executionObject->getDescriptor();
		if (descriptor != NULL) {
			descriptor->setFormatterRegion(scheduler->getLayoutManager());
		}

		if (descriptor != NULL && descriptor->getPlayerName() != "" &&
			 ((descriptor->getPlayerName() == "VideoChannelPlayerAdapter") ||
    		 (descriptor->getPlayerName() == "AudioChannelPlayerAdapter") ||
    		 (descriptor->getPlayerName() == "JmfVideoChannelPlayerAdapter") ||
    		 (descriptor->getPlayerName() == "JmfAudioChannelPlayerAdapter") ||
    		 (descriptor->getPlayerName() == "QtVideoChannelPlayerAdapter") ||
    		 (descriptor->getPlayerName() == "QtAudioChannelPlayerAdapter"))) {

			createMultichannelObject(
				    (CompositeExecutionObject*)executionObject, depthLevel);
		}

		if (depthLevel != 0) {
			if (depthLevel > 0) {
				depthLevel--;
			}
			compileExecutionObjectLinks(executionObject, depthLevel);

		} else if (executionObject->instanceOf("CompositeExecutionObject")) {
			compileExecutionObjectLinks(executionObject, depthLevel);
		}
	}

	void FormatterConverter::compileExecutionObjectLinks(
		    ExecutionObject* executionObject, int depthLevel) {

		vector<Node*>* nodes;
		vector<Node*>::iterator i;
		Node* node;

		/*cout << "FormatterConverter::compileExecutionObjectLinks";
		cout << " EO " << executionObject->getId().c_str();
		cout << " depth " << depthLevel << endl;*/

		nodes = executionObject->getNodes();
		if (nodes != NULL) {
			i = nodes->begin();
			while (i != nodes->end()) {
				node = (Node*)(*i);
				compileExecutionObjectLinks(
					    executionObject,
					    node,
						(CompositeExecutionObject*)(
								executionObject->getParentObject(node)),
						depthLevel);

				++i;
			}
			delete nodes;
			nodes = NULL;
		}
	}

	ExecutionObject* FormatterConverter::getExecutionObject(
		    NodeNesting* perspective,
		    GenericDescriptor* descriptor,
		    int depthLevel) throw(ObjectCreationForbiddenException*) {

		CascadingDescriptor* cascadingDescriptor = NULL;
		string id;
		CompositeExecutionObject* parentObject;
		ExecutionObjectSwitch* parentSwitch;
		ExecutionObject* executionObject;
		Node* selectedNode;

		id = perspective->getId() + "/";
		cascadingDescriptor = getCascadingDescriptor(perspective, descriptor);
		if (cascadingDescriptor != NULL) {
			id = id + cascadingDescriptor->getId();
		}

		/*cout << "FormatterConverter::getExecutionObject '";
		cout << id << "'" << endl;*/

		if (executionObjects->count(id) != 0) {
			if (cascadingDescriptor != NULL) {
				delete cascadingDescriptor;
				cascadingDescriptor = NULL;
			}
			return (*executionObjects)[id];
		}

		parentSwitch = NULL;

		parentObject = getParentExecutionObject(perspective, depthLevel);
		if (parentObject != NULL) {
			if (parentObject->instanceOf("ExecutionObjectSwitch")) {

				parentSwitch = (ExecutionObjectSwitch*)parentObject;
				if (parentSwitch->getSelectedObject() == NULL) {
					selectedNode = ruleAdapter->adaptSwitch(
						    (SwitchNode*)(parentSwitch->getDataObject()->
						    	    getDataEntity()));

					if (selectedNode != perspective->getAnchorNode()) {
						// the excObj has been created when creating the switch
						throw (new ObjectCreationForbiddenException());
					}

				} else if (parentSwitch->getSelectedObject()->getDataObject()
					    != perspective->getAnchorNode()) {

					// the exec obj has been created when creating the switch
					throw (new ObjectCreationForbiddenException());
				}
			}
		}

		executionObject = createExecutionObject(
			    id, perspective, cascadingDescriptor, depthLevel);

		if (executionObject == NULL) {
			if (cascadingDescriptor != NULL) {
				delete cascadingDescriptor;
				cascadingDescriptor = NULL;
			}
			return NULL;
		}

		addExecutionObject(executionObject, parentObject, depthLevel);

		if (parentSwitch != NULL) {
			parentSwitch->select(executionObject);
			resolveSwitchEvents(parentSwitch, depthLevel);
		}

		return executionObject;
	}

	set<ExecutionObject*>* FormatterConverter::getSettingNodeObjects() {
		return new set<ExecutionObject*>(*settingObjects);
	}

	CompositeExecutionObject* FormatterConverter::getParentExecutionObject(
		    NodeNesting* perspective,
		    int depthLevel) throw(ObjectCreationForbiddenException*) {

		NodeNesting* parentPerspective;
		CompositeExecutionObject* cObj;

		if (perspective->getNumNodes() > 1) {
			parentPerspective = perspective->copy();
			parentPerspective->removeAnchorNode();
			cObj = (CompositeExecutionObject*)(this->getExecutionObject(
				    parentPerspective, NULL, depthLevel));

			delete parentPerspective;
			return cObj;

		} else {
			return NULL;
		}
	}

	FormatterEvent* FormatterConverter::getEvent(
		    ExecutionObject* executionObject,
		    InterfacePoint* interfacePoint,
		    int ncmEventType, string key) {

		string id;
		FormatterEvent* event;
		CascadingDescriptor* descriptor;

		if (key == "") {
			id = interfacePoint->getId() + "_" + itos(ncmEventType);

		} else {
			id = interfacePoint->getId() + "_" + itos(ncmEventType) + "_" + key;
		}

		event = executionObject->getEvent(id);
		if (event != NULL) {
			return event;
		}

		if (executionObject->instanceOf("ExecutionObjectSwitch")) {
			event = new SwitchEvent(id,
				    (ExecutionObjectSwitch*)executionObject,
				    interfacePoint, ncmEventType, key);

		} else {
			if (ncmEventType == EventUtil::EVT_PRESENTATION) {
				event = new PresentationEvent(
					    id, executionObject, (ContentAnchor*)interfacePoint);

				if (interfacePoint->instanceOf("LambdaAnchor")) {
					descriptor = executionObject->getDescriptor();
					if (descriptor != NULL &&
						    !isNaN(descriptor->getExplicitDuration())) {

						((PresentationEvent*)event)->setEnd(
							    descriptor->getExplicitDuration());
					}
				}

			} else {
				if (executionObject->instanceOf("CompositeExecutionObject")) {
					// TODO: eventos internos da composicao.
					// Estao sendo tratados nos elos.
					if (ncmEventType == EventUtil::EVT_ATTRIBUTION) {
						event = new AttributionEvent(id, executionObject,
								(PropertyAnchor*)interfacePoint);
					}

				} else {
					switch (ncmEventType) {
						case EventUtil::EVT_ATTRIBUTION:
							event = new AttributionEvent(id, executionObject,
								    (PropertyAnchor*)interfacePoint);
							break;

						case EventUtil::EVT_SELECTION:
							event = new SelectionEvent(id, executionObject,
								    (ContentAnchor*)interfacePoint);

							if (key != "") {
								((SelectionEvent*)event)->
									    setSelectionCode(key);
							}

							break;
					}
				}
			}
		}

		if (event != NULL) {
			executionObject->addEvent(event);
		}

		return event;
	}

	void FormatterConverter::createMultichannelObject(
		    CompositeExecutionObject* compositeObject, int depthLevel) {

		CompositeNode* compositeNode;
		vector<Node*>* nodes;
		Node* node;
		NodeNesting* perspective;
		string id;
		CascadingDescriptor* cascadingDescriptor;
		ExecutionObject* childObject;

		compositeNode = (CompositeNode*)compositeObject->getDataObject();
		nodes = compositeNode->getNodes();
		if (nodes != NULL) {
			vector<Node*>::iterator i;
			i = nodes->begin();
			while (i != nodes->end()) {
				node = *i;
				perspective = new NodeNesting(compositeObject->
					   getNodePerspective());

				perspective->insertAnchorNode(node);

				id = perspective->getId() + "/";
				cascadingDescriptor = getCascadingDescriptor(
					    perspective, NULL);

				if (cascadingDescriptor != NULL) {
					id += cascadingDescriptor->getId();
				}

				childObject = createExecutionObject(
					    id, perspective, cascadingDescriptor, depthLevel);

				if (childObject != NULL) {
					getEvent(childObject,
						    ((NodeEntity*)node->getDataEntity())->
						    	    getLambdaAnchor(),
						    EventUtil::EVT_PRESENTATION, "");

					addExecutionObject(
						    childObject, compositeObject, depthLevel);
				}

				++i;
			}
		}
	}

	ExecutionObject* FormatterConverter::createExecutionObject(
		    string id,
		    NodeNesting* perspective,
		    CascadingDescriptor* descriptor,
		    int depthLevel) {

		NodeEntity* nodeEntity;
		Node* node;
		NodeNesting* nodePerspective;
		ExecutionObject* executionObject;
		PresentationEvent* compositeEvent;

		nodePerspective = perspective;
		nodeEntity = (NodeEntity*)(perspective->
			    getAnchorNode()->getDataEntity());

		// solve execution object cross reference coming from refer nodes with
		// new instance = false
		if (nodeEntity->instanceOf("ContentNode") &&
			    ((ContentNode*)nodeEntity)->getNodeType() != "" &&
			    !(((ContentNode*)nodeEntity)->
			    	    getNodeType() == ContentNode::SETTING_NODE)) {

			node = perspective->getAnchorNode();
			if (node->instanceOf("ReferNode") &&
				    ((ReferNode*)node)->getInstanceType() != "new") {

				nodePerspective = new NodeNesting(
					    nodeEntity->getPerspective());

				// verify if both nodes are in the same base.
				if (nodePerspective->getHeadNode() ==
					    perspective->getHeadNode()) {

					try {
						executionObject = getExecutionObject(
							    nodePerspective, NULL, depthLevel);

					} catch (ObjectCreationForbiddenException* exc1) {
						if (PlayerAdapterManager::isProcedural(nodeEntity)) {
							executionObject = new ProceduralExecutionObject(
									id, nodeEntity, descriptor);

						} else {
							executionObject = new ExecutionObject(
									id, nodeEntity, descriptor);
						}

						//TODO informa a substituicao
					}

				} else {
					// not in the same base => create a new version
					if (PlayerAdapterManager::isProcedural(nodeEntity)) {
						executionObject = new ProceduralExecutionObject(
								id, nodeEntity, descriptor);

					} else {
						executionObject = new ExecutionObject(
								id, nodeEntity, descriptor);
					}

					//TODO informa a substituicao
				}

				if (executionObject != NULL) {
					return executionObject;
				}
			}
		}

		if (nodeEntity->instanceOf("SwitchNode")) {
			executionObject = new
				    ExecutionObjectSwitch(id, perspective->getAnchorNode());

			compositeEvent = new PresentationEvent(
				    nodeEntity->getLambdaAnchor()->getId() + "_" +
				    	   itos(EventUtil::EVT_PRESENTATION),
				    executionObject,
				    (ContentAnchor*)(nodeEntity->getLambdaAnchor()));

			executionObject->addEvent(compositeEvent);
			// to monitor the switch presentation and clear the selection after
			// each execution
			compositeEvent->addEventListener(this);

		} else if (nodeEntity->instanceOf("CompositeNode")) {
			executionObject = new CompositeExecutionObject(
				    id, perspective->getAnchorNode(), descriptor);

			compositeEvent = new PresentationEvent(
				    nodeEntity->getLambdaAnchor()->getId() + "_" +
				    	   itos(EventUtil::EVT_PRESENTATION),
				    executionObject,
				    (ContentAnchor*)(nodeEntity->getLambdaAnchor()));

			executionObject->addEvent(compositeEvent);

			// to monitor the presentation and remove object at stops
			//compositeEvent->addEventListener(this);

		} else if (PlayerAdapterManager::isProcedural(nodeEntity)) {
			executionObject = new ProceduralExecutionObject(
				    id, perspective->getAnchorNode(), descriptor);

		} else {
			executionObject = new ExecutionObject(
				    id, perspective->getAnchorNode(), descriptor);
		}

		return executionObject;
	}

	CascadingDescriptor* FormatterConverter::getCascadingDescriptor(
		    NodeNesting* nodePerspective, GenericDescriptor* descriptor) {

		CascadingDescriptor* cascadingDescriptor = NULL;
		NodeEntity* node;
		ContextNode* context;
		int size;

		node = (NodeEntity*)(nodePerspective->getAnchorNode()->
			    getDataEntity());

		//	there is a node descriptor?
		if (node->getDescriptor() != NULL) {
			cascadingDescriptor = new CascadingDescriptor(
				    node->getDescriptor());
		}

		// there is a node descriptor defined in the node context?
		size = nodePerspective->getNumNodes();
		if (size > 1 && nodePerspective->getNode(size - 2) != NULL &&
			    nodePerspective->getNode(size - 2)->
			    	    instanceOf("ContextNode")) {

			context = (ContextNode*)(nodePerspective->getNode(size - 2)->
				    getDataEntity());

			if (context->getNodeDescriptor(node) != NULL) {
				if (cascadingDescriptor == NULL) {
					cascadingDescriptor = new CascadingDescriptor(
						    context->getNodeDescriptor(node));

				} else {
					cascadingDescriptor->cascade(
						    context->getNodeDescriptor(node));
				}
			}
		}

		// there is an explicit descriptor (user descriptor)?
		if (descriptor != NULL) {
			if (cascadingDescriptor == NULL) {
				cascadingDescriptor = new CascadingDescriptor(descriptor);
			} else {
				cascadingDescriptor->cascade(descriptor);
			}
		}

		return cascadingDescriptor;
	}

	void FormatterConverter::compileExecutionObjectLinks(
		   ExecutionObject* executionObject,
		   Node* dataObject,
		   CompositeExecutionObject* parentObject,
		   int depthLevel) {

		executionObject->setCompiled(true);
		if (parentObject == NULL) {
			return;
		}

		set<Link*>* dataLinks;
		set<Link*>* uncompiledLinks;
		set<Link*>::iterator i;
		Link* ncmLink;
		vector<GenericDescriptor*>* descriptors;
		GenericDescriptor* descriptor;
		FormatterLink* formatterLink;

		uncompiledLinks = parentObject->getUncompiledLinks();
		if (uncompiledLinks != NULL) {
			dataLinks = new set<Link*>(*uncompiledLinks);

			i = dataLinks->begin();
			while (i != dataLinks->end()) {
				ncmLink = *i;

				/*cout << "FormatterConverter::compileExecutionObjectLinks ";
				cout << "processing ncmLink '" << ncmLink->getId() << "'";
				cout << " inside '" << parentObject->getId() << "'" << endl;*/

				// since the link may be removed in a deepest compilation its
				// necessary to certify that the link was not compiled
				if (parentObject->containsUncompiledLink(ncmLink)) {
					descriptor = NULL;
					if (executionObject->getDescriptor() != NULL) {
						descriptors = executionObject->getDescriptor()->
							    getNcmDescriptors();

						if (descriptors != NULL && !(descriptors->empty())) {
							descriptor = (*descriptors)[descriptors->size()
								    - 1];
						}
					}

					if (ncmLink->instanceOf("CausalLink")) {
						// verify if execution object is part of
						// link conditions
						if (((CausalLink*)ncmLink)->containsSourceNode(
							    dataObject, descriptor)) {

							// compile causal link
							parentObject->removeLinkUncompiled(ncmLink);
							formatterLink = ((FormatterLinkConverter*)
								    linkCompiler)->createCausalLink(
								    (CausalLink*)ncmLink,
								    parentObject,
								    depthLevel);

							if (formatterLink != NULL) {
								setActionListener(((FormatterCausalLink*)
									    formatterLink)->getAction());

								parentObject->setLinkCompiled(formatterLink);
							}

						} else {
							/*cout << "FormatterConverter::";
							cout << "compileExecutionObjectLinks ";
							cout << "can't process ncmLink '";
							cout << ncmLink->getId() << "'";
							cout << " inside '" << parentObject->getId() << "'";
							cout << "  because ncmLink does not contains '";
							cout << dataObject->getId() << "' src";
							cout << endl;*/
						}

					} else {
						/*cout << "FormatterConverter::";
						cout << "compileExecutionObjectLinks ";
						cout << "can't process ncmLink '" << ncmLink->getId() << "'";
						cout << " inside '" << parentObject->getId() << "'";
						cout << "  because isn't a causal link";
						cout << endl;*/
					}

				} else {
					/*cout << "FormatterConverter::compileExecutionObjectLinks ";
					cout << "can't process ncmLink '" << ncmLink->getId() << "'";
					cout << " inside '" << parentObject->getId() << "'";
					cout << " link may be removed in a deepest compilation";
					cout << endl;*/
				}
				++i;
			}

			delete dataLinks;
			dataLinks = NULL;

			compileExecutionObjectLinks(
				    executionObject,
				    dataObject,
				    (CompositeExecutionObject*)(parentObject->
				    	    getParentObject()),
				    depthLevel);
		}
	}

	void FormatterConverter::setActionListener(LinkAction* action) {
		if (action->instanceOf("LinkSimpleAction")) {
			((LinkSimpleAction*)action)->setActionListener(
				    (FormatterScheduler*)scheduler);

		} else {
			vector<LinkAction*>* actions;
			vector<LinkAction*>::iterator i;
			actions = ((LinkCompoundAction*)action)->getActions();
			if (actions != NULL) {
				i = actions->begin();
				while (i != actions->end()) {
					setActionListener((LinkAction*)(*i));
					++i;
				}
				delete actions;
			}
		}
	}

	ExecutionObject* FormatterConverter::processExecutionObjectSwitch(
		    ExecutionObjectSwitch* switchObject) {

		SwitchNode* switchNode;
		Node* selectedNode;
		NodeNesting* selectedPerspective;
		string id;
		CascadingDescriptor* descriptor;
		ExecutionObject* selectedObject;

		switchNode = (SwitchNode*)(
			    switchObject->getDataObject()->getDataEntity());

		selectedNode = ruleAdapter->adaptSwitch(switchNode);
		if (selectedNode == NULL) {
			return NULL;
		}

		selectedPerspective = switchObject->getNodePerspective();
		selectedPerspective->insertAnchorNode(selectedNode);

		id = selectedPerspective->getId() + "/";

		descriptor = FormatterConverter::getCascadingDescriptor(
			    selectedPerspective, NULL);

		if (descriptor != NULL) {
			id += descriptor->getId();
		}

		if (executionObjects->count(id) != 0) {
			selectedObject = (*executionObjects)[id];
			switchObject->select(selectedObject);
		  	resolveSwitchEvents(switchObject, depthLevel);
		  	if (descriptor != NULL) {
		  		delete descriptor;
		  		descriptor = NULL;
		  	}
			return selectedObject;
		}

		selectedObject = createExecutionObject(
			    id, selectedPerspective, descriptor, depthLevel);

		if (selectedObject == NULL) {
			if (descriptor != NULL) {
				delete descriptor;
				descriptor = NULL;
			}
			return NULL;
		}

		addExecutionObject(selectedObject, switchObject, depthLevel);
		switchObject->select(selectedObject);
		resolveSwitchEvents(switchObject, depthLevel);
		return selectedObject;
	}

	void FormatterConverter::resolveSwitchEvents(
		    ExecutionObjectSwitch* switchObject, int depthLevel) {

		ExecutionObject* selectedObject;
		ExecutionObject* endPointObject;
		Node* selectedNode;
		NodeEntity* selectedNodeEntity;
		vector<FormatterEvent*>* events;
		vector<FormatterEvent*>::iterator i;
		SwitchEvent* switchEvent;
		InterfacePoint* interfacePoint;
		SwitchPort* switchPort;
		vector<Node*>* nestedSeq;
		vector<Port*>* mappings;
		vector<Port*>::iterator j;
		Port* mapping;
		NodeNesting* nodePerspective;
		FormatterEvent* mappedEvent;

		selectedObject = switchObject->getSelectedObject();
		if (selectedObject == NULL) {
			return;
		}

		selectedNode = selectedObject->getDataObject();
		selectedNodeEntity = (NodeEntity*)(selectedNode->getDataEntity());
		events = switchObject->getEvents();
		if (events != NULL) {
			i = events->begin();
			while (i != events->end()) {
				mappedEvent = NULL;
				switchEvent = (SwitchEvent*)(*i);
				interfacePoint = switchEvent->getInterfacePoint();
				if (interfacePoint->instanceOf("LambdaAnchor")) {
					mappedEvent = getEvent(
						    selectedObject,
						    selectedNodeEntity->getLambdaAnchor(),
							switchEvent->getEventType(),
							switchEvent->getKey());

				} else {
					switchPort = (SwitchPort*)interfacePoint;
					mappings = ((SwitchPort*)switchPort)->getPorts();
					if (mappings != NULL) {
						j = mappings->begin();
						while (j != mappings->end()) {
							mapping = (*j);
							if (mapping->getNode() == selectedNode) {
								nodePerspective = switchObject->
									    getNodePerspective();

								nestedSeq = mapping->getMapNodeNesting();
								nodePerspective->append(nestedSeq);
								delete nestedSeq;

								try {
									endPointObject = getExecutionObject(
										    nodePerspective, NULL, depthLevel);

									if (endPointObject != NULL) {
										mappedEvent = getEvent(
											    endPointObject,
											    mapping->
											    	    getEndInterfacePoint(),
											    switchEvent->getEventType(),
											    switchEvent->getKey());
									}

								} catch (ObjectCreationForbiddenException*
									    exc) {
									// continue
								}

								break;
							}
							++j;
						}
					}
				}

				if (mappedEvent != NULL) {
					switchEvent->setMappedEvent(mappedEvent);
				}

				++i;
			}
			delete events;
			events = NULL;
		}
	}

	FormatterEvent* FormatterConverter::insertNode(
		    NodeNesting* perspective,
		    InterfacePoint* interfacePoint,
		    GenericDescriptor* descriptor) {

		ExecutionObject* executionObject;
		FormatterEvent* event;

		event = NULL;
		try {
			executionObject = getExecutionObject(
				    perspective, descriptor, depthLevel);

			if (executionObject != NULL) {
				// get the event corresponding to the node anchor
				event = getEvent(
					    executionObject,
					    interfacePoint,
					    EventUtil::EVT_PRESENTATION,
					    "");
			}

			return event;

		} catch (ObjectCreationForbiddenException* exc) {
			return NULL;
		}
	}

	FormatterEvent* FormatterConverter::insertContext(
		    NodeNesting* contextPerspective,
		    Port* port) {

		vector<Node*>* nestedSeq;
		NodeNesting* perspective;
		FormatterEvent* newEvent;

		if (contextPerspective == NULL || port == NULL ||
				!(port->getEndInterfacePoint()->instanceOf("ContentAnchor") ||
				port->getEndInterfacePoint()->instanceOf("SwitchPort")) ||
				!(contextPerspective->getAnchorNode()->getDataEntity()->
					    instanceOf("ContextNode"))) {

			return NULL;
		}

		nestedSeq = port->getMapNodeNesting();
		perspective = new NodeNesting(contextPerspective);
		perspective->append(nestedSeq);
		delete nestedSeq;

		newEvent = insertNode(perspective, port->getEndInterfacePoint(), NULL);
		delete perspective;

		return newEvent;
	}

	bool FormatterConverter::removeExecutionObject(
		    ExecutionObject* executionObject,
		    ReferNode* referNode) {

		NodeNesting* referPerspective;
		map<string, ExecutionObject*>::iterator i;
		set<ExecutionObject*>::iterator j;

		if (executionObject == NULL || referNode == NULL) {
			return false;
		}

		executionObject->removeNode(referNode);
		referPerspective = new NodeNesting(referNode->getPerspective());
		string objectId;
		objectId = referPerspective->getId() + "/" +
			    executionObject->getDescriptor()->getId();

		i = executionObjects->find(objectId);
		if (i != executionObjects->end()) {
			executionObjects->erase(i);
		}

		j = settingObjects->find(executionObject);
		if (j != settingObjects->end()) {
			settingObjects->erase(j);
		}
		//TODO: problema se esse era a base para outros objetos
		return true;
	}

	bool FormatterConverter::removeExecutionObject(
		    ExecutionObject* executionObject) {

		CompositeExecutionObject* compositeObject;
		ExecutionObject* childObject;

		map<string, ExecutionObject*>* objects;
		map<string, ExecutionObject*>::iterator i;

		if (executionObject == NULL) {
			return false;
		}

		if (executionObject->instanceOf("CompositeExecutionObject")) {
			compositeObject = (CompositeExecutionObject*)executionObject;
			objects = compositeObject->getExecutionObjects();
			if (objects != NULL) {
				i = objects->begin();
				while (i != objects->end()) {
					childObject = i->second;
					compositeObject->removeExecutionObject(childObject);

					delete objects;
					objects = NULL;

					//TODO make remove / destroy better
					removeExecutionObject(childObject);

					objects = compositeObject->getExecutionObjects();
					if (objects == NULL) {
						break;
					} else {
						i = objects->begin();
					}
				}

				if (objects != NULL) {
					delete objects;
					objects = NULL;
				}
			}
		}

		if (executionObjects->count(executionObject->getId()) != 0) {
			executionObjects->erase(executionObjects->find(
				    executionObject->getId()));
		}

		if (settingObjects->count(executionObject) != 0) {
			settingObjects->erase(settingObjects->find(executionObject));
		}

		delete executionObject;
		executionObject = NULL;
		return true;
	}

	ExecutionObject* FormatterConverter::hasExecutionObject(
		    Node* node,
		    GenericDescriptor* descriptor) {

		NodeNesting* perspective;
		string id;
		CascadingDescriptor* cascadingDescriptor;

		//TODO procurar por potenciais substitutos no caso de REFER

		perspective = new NodeNesting(node->getPerspective());

		id = perspective->getId() + "/";
		cascadingDescriptor = getCascadingDescriptor(perspective, descriptor);
		if (cascadingDescriptor != NULL) {
			id += cascadingDescriptor->getId();
			delete cascadingDescriptor;
			cascadingDescriptor = NULL;
		}

		if (executionObjects->count(id)) {
			return (*executionObjects)[id];

		} else {
			return NULL;
		}
	}

	FormatterCausalLink* FormatterConverter::addCausalLink(
		    ContextNode* context,
		    CausalLink* link,
		    int editingType) {

		ExecutionObject* object;

		object = hasExecutionObject(context, NULL);
		if (object == NULL) {
			return NULL;
		}

		CompositeExecutionObject* contextObject;
		vector<Bind*>* binds;

		contextObject = (CompositeExecutionObject*)object;
		contextObject->addNcmLink(link);

		binds = link->getConditionBinds();
		if (binds != NULL) {
			ExecutionObject* childObject;
			vector<Bind*>::iterator i;
			Bind* bind;
			FormatterCausalLink* formatterLink;

			i = binds->begin();
			while (i != binds->end()) {
				bind = (*i);
				childObject = hasExecutionObject(
					    bind->getNode(), bind->getDescriptor());

				if (childObject != NULL) {
					// compile causal link
					contextObject->removeLinkUncompiled(link);
					formatterLink = ((FormatterLinkConverter*)linkCompiler)->
						    createCausalLink(link, contextObject, depthLevel,
						    editingType);

					if (formatterLink != NULL) {
						setActionListener(formatterLink->getAction());
						contextObject->setLinkCompiled(formatterLink);
					}

					delete binds;
					return formatterLink;
				}
				++i;
			}

			delete binds;
		}
		return NULL;
	}

	void FormatterConverter::eventStateChanged(
		    void* someEvent, short transition, short previousState) {

		ExecutionObject* executionObject;
		FormatterEvent* event;

		event = (FormatterEvent*)someEvent;
		executionObject = (ExecutionObject*)(event->getExecutionObject());

		/*cout << "FormatterConverter::eventStateChanged EO = '";
		cout << executionObject->getId() << "'" << endl;*/

		if (executionObject->instanceOf("ExecutionObjectSwitch")) {
			if (transition == EventUtil::TR_STOPS ||
				    transition == EventUtil::TR_ABORTS) {

				/*
				removeExecutionObject(
						((ExecutionObjectSwitch*)executionObject)->
						getSelectedObject());
				*/
				((ExecutionObjectSwitch*)executionObject)->select(NULL);
				// removeExecutionObject(executionObject);
			}

		} else if (executionObject->instanceOf("CompositeExecutionObject")) {
			/*cout << "FormatterConverter::eventStateChanged CEO = '";
			cout << executionObject->getId() << "' transition '";
			cout << transition << "' starts=>'" << EventUtil::TR_STARTS;
			cout << "'" << endl;*/

			if (transition == EventUtil::TR_STOPS ||
				    transition == EventUtil::TR_ABORTS) {

				removeExecutionObject(executionObject);
			}
		}
	}

	short FormatterConverter::getPriorityType() {
		return IEventListener::PT_CORE;
	}

	void FormatterConverter::reset() {
		executionObjects->clear();
	}

}
}
}
}
}
}
