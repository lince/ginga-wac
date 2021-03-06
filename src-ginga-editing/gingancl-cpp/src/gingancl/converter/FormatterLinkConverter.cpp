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

#include "../../include/FormatterLinkConverter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace converter {
	FormatterLinkConverter::FormatterLinkConverter(
		    FormatterConverter* compiler) {

		this->compiler = compiler;
	}

	FormatterLinkConverter::~FormatterLinkConverter() {
		wclog << "FormatterLinkConverter::~FormatterLinkConverter" << endl;
		compiler = NULL;
	}

	FormatterCausalLink* FormatterLinkConverter::createCausalLink(
		     CausalLink* ncmLink,
		     CompositeExecutionObject* parentObject,
		     int depthLevel) {

		if (ncmLink == NULL) {
			return NULL;
		}

		CausalConnector* connector;
		ConditionExpression* conditionExpression;
		Action* actionExpression;
		LinkCondition* formatterCondition;
		LinkAction* formatterAction;
		FormatterCausalLink* formatterLink;
		vector<LinkAction*>* acts;
		vector<LinkAction*>::iterator i;
		LinkAttributeAssessment* atAssessment;
		LinkAssignmentAction* action;
		string value, roleId;
		FormatterEvent* event;

		vector<Bind*>* binds;
		vector<Bind*>::iterator j;
		NodeNesting* refPerspective;
		ExecutionObject* refObject;
		FormatterEvent* refEvent;
		InterfacePoint* refInterface;

		// compile link condition and verify if it is a trigger condition
		connector = (CausalConnector*)(ncmLink->getConnector());
		conditionExpression = connector->getConditionExpression();
		formatterCondition = createCondition(
			    (TriggerExpression*)conditionExpression,
			    ncmLink,
			    parentObject, 
				depthLevel);

		if (formatterCondition == NULL ||
			    !(formatterCondition->instanceOf("LinkTriggerCondition"))) {

			return NULL;
		}

		// compile link action
		actionExpression = connector->getAction();
		formatterAction = createAction(
			    actionExpression, ncmLink, parentObject, depthLevel);

		if (formatterAction == NULL) {
			return NULL;
		}

		// create formatter causal link
		formatterLink = new FormatterCausalLink(
			    (LinkTriggerCondition*)formatterCondition,
			    formatterAction,
			    ncmLink,
			    (CompositeExecutionObject*)parentObject);

		//&got
		if (formatterCondition->instanceOf("LinkCompoundTriggerCondition")) {
			acts = formatterAction->getImplicitRefRoleActions();
			if (acts != NULL) {
				i = acts->begin();
				while (i != acts->end()) {
					action = (LinkAssignmentAction*)(*i);
					value = action->getValue();
					cout << "FormatterLinkConverter::createCausalLink value ";
					cout << " = '" << value;
					value = value.substr(1, value.length());
					cout << "', value.substr = '" << value;
					event = ((LinkRepeatAction*)(*i))->getEvent();
					if (event->instanceOf("AttributionEvent")) {
						binds = ncmLink->getBinds();
						j = binds->begin();
						while (j != binds->end()) {
							roleId = (*j)->getRole()->getLabel();
							cout << "' compare with label '" << roleId << "'";
							if (roleId == value) {
								refInterface = (*j)->getInterfacePoint();
								if (refInterface != NULL &&
										refInterface->instanceOf(
												"PropertyAnchor")) {

									cout << " MATCH when interface is '";
									cout << refInterface->getId() << "'";
									refPerspective = new NodeNesting(
											(*j)->getNode()->getPerspective());

									refObject = compiler->getExecutionObject(
											refPerspective,
											(*j)->getDescriptor(), 1);

									refEvent = compiler->getEvent(
											refObject,
											refInterface,
											EventUtil::EVT_ATTRIBUTION, "");

									((AttributionEvent*)event)->
											setImplicitRefAssessmentEvent(
													roleId,
													refEvent);

									cout << ". Thus, event '";
									cout << event->getId() << "' was setted";
									cout << " whith refEvent '";
									cout << refEvent->getId() << "'";
									break;
								}
							}
							++j;
						}
					}
					cout << endl;
					++i;
				}

				delete acts;
			}
		}

		return formatterLink;
	}

	LinkAction* FormatterLinkConverter::createAction(
		   Action* actionExpression,
		   CausalLink* ncmLink,
		   CompositeExecutionObject* parentObject,
		   int depthLevel) {

		double delay;
		SimpleAction* sae;
		CompoundAction* cae;
		vector<Bind*>* binds;
		int i, size;
		string delayObject;
		LinkSimpleAction* simpleAction;
		LinkCompoundAction* compoundAction;

		if (actionExpression->instanceOf("SimpleAction")) {
			sae = (SimpleAction*)actionExpression;
			binds = ncmLink->getRoleBinds(sae);
			if (binds != NULL) {
				size = binds->size();
				if (size == 1) {
					return createSimpleAction(
						   sae, (*binds)[0], ncmLink, parentObject, depthLevel);

				} else if (size > 1) {
					compoundAction = new LinkCompoundAction(
						    sae->getQualifier());

					for (i = 0; i < size; i++) {
						simpleAction = createSimpleAction(
							    sae,
							    (*binds)[i],
							    ncmLink,
							    parentObject,
							    depthLevel);

						compoundAction->addAction(simpleAction);
					}

					return compoundAction;

				} else {
					return NULL;
				}
			}

		} else { // CompoundAction)
			delayObject = actionExpression->getDelay();
			delay = compileDelay(ncmLink, delayObject, NULL);
			cae = (CompoundAction*)actionExpression;
			return createCompoundAction(
				    cae->getOperator(),
				    delay,
				    cae->getActions(),
				    ncmLink,
				    parentObject,
				    depthLevel);

		}

		return NULL;
	}

	LinkCondition* FormatterLinkConverter::createCondition(
		    ConditionExpression* ncmExpression,
		    CausalLink* ncmLink,
		    CompositeExecutionObject* parentObject,
		    int depthLevel) {

		if (ncmExpression->instanceOf("TriggerExpression")) {
			return createCondition(
				    (TriggerExpression*)ncmExpression,
				    ncmLink,
				    parentObject,
				    depthLevel);

		} else { // IStatement
			return createStatement(
				    (Statement*)ncmExpression,
				    ncmLink,
				    parentObject,
				    depthLevel);
		}
	}

	LinkCompoundTriggerCondition*
		FormatterLinkConverter::createCompoundTriggerCondition(
		    short op,
		    double delay,
		    vector<ConditionExpression*>* ncmChildConditions,
		    CausalLink* ncmLink,
		    CompositeExecutionObject* parentObject,
		    int depthLevel) {

		LinkCompoundTriggerCondition* condition;  
		ConditionExpression* ncmChildCondition;
		LinkCondition* childCondition;

		if (op == CompoundCondition::OP_AND) {
			condition = new LinkAndCompoundTriggerCondition();

		} else {
			condition = new LinkCompoundTriggerCondition();
		}

		if (delay > 0) {
			condition->setDelay(delay);
		}

		if (ncmChildConditions != NULL) {
			vector<ConditionExpression*>::iterator i;
			i = ncmChildConditions->begin();
			while (i != ncmChildConditions->end()) {
				ncmChildCondition = (*i);
				childCondition = createCondition(
					    ncmChildCondition, ncmLink, parentObject, depthLevel);

				condition->addCondition(childCondition);
				++i;
			}
		}

		return condition;
	}

	LinkCondition* FormatterLinkConverter::createCondition(
		    TriggerExpression* condition,
		    CausalLink* ncmLink,
		    CompositeExecutionObject* parentObject,
		    int depthLevel) {

		double delay;
		SimpleCondition* ste;
		CompoundCondition* cte;
		vector<Bind*>* binds;
		int i, size;
		string delayObject;
		LinkCompoundTriggerCondition* compoundCondition;
		LinkTriggerCondition* simpleCondition;

		if (condition->instanceOf("SimpleCondition")) {
			ste = (SimpleCondition*)condition;
			binds = ncmLink->getRoleBinds(ste);
			if (binds != NULL) {
				size = binds->size();
				if (size == 1) {
					return createSimpleCondition(
						    ste,
						    (*binds)[0], ncmLink, parentObject, depthLevel);

				} else if (size > 1) {
					if (ste->getQualifier() == 
						    CompoundCondition::OP_AND) {

						compoundCondition = 
							    new LinkAndCompoundTriggerCondition();

					} else {
						compoundCondition = new LinkCompoundTriggerCondition();	
					}

					for (i = 0; i < size; i++) {
						simpleCondition = createSimpleCondition(ste,
							    (*binds)[i], ncmLink, parentObject, depthLevel);

						compoundCondition->addCondition(simpleCondition);
					}
					return compoundCondition;

				} else {
					return NULL;
				}
			}

		} else { // CompoundCondition
			delayObject = condition->getDelay();
			delay = compileDelay(ncmLink, delayObject, NULL);
			cte = (CompoundCondition*)condition;
			return createCompoundTriggerCondition(
				    cte->getOperator(),
				    delay,
				    cte->getConditions(),
				    ncmLink, parentObject, depthLevel);
		}
		return NULL;
	}

	LinkAssessmentStatement* FormatterLinkConverter::createAssessmentStatement(
		    AssessmentStatement* assessmentStatement,
		    Bind* bind,
		    Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		LinkAttributeAssessment* mainAssessment;
		LinkAssessment* otherAssessment;
		AttributeAssessment* aa;
		LinkAssessmentStatement* statement;
		ValueAssessment* valueAssessment;
		string paramValue;
		Parameter* connParam,* param;
		vector<Bind*>* otherBinds;

		mainAssessment = createAttributeAssessment(
			    assessmentStatement->getMainAssessment(),
			    bind, ncmLink, parentObject, depthLevel);

		if (assessmentStatement->getOtherAssessment()->
			    instanceOf("ValueAssessment")) {

			valueAssessment = (ValueAssessment*)(
				    assessmentStatement->getOtherAssessment());

			paramValue = valueAssessment->getValue();
			if (paramValue[0] == '$') { //instanceOf("Parameter")
				connParam = new Parameter(
					    paramValue.substr(1, paramValue.length()), "");

				param = bind->getParameter(connParam->getName());
				if (param == NULL) {
					param = ncmLink->getParameter(connParam->getName());
				}
				if (param == NULL) {
					return NULL;
				}
				paramValue = param->getValue();
			}

			if (paramValue == "") {
				return NULL;
			}

			otherAssessment = new LinkValueAssessment(paramValue);

		} else {
			aa = (AttributeAssessment*)(assessmentStatement->
				    getOtherAssessment());

			otherBinds = ncmLink->getRoleBinds(aa);
			if (otherBinds != NULL && !otherBinds->empty()) {
				otherAssessment = createAttributeAssessment(
					    aa,
					    (*otherBinds)[0],
					    ncmLink,
					    parentObject,
					    depthLevel);

			} else {
				otherAssessment = createAttributeAssessment(
					    aa, NULL, ncmLink, parentObject, depthLevel);
			}
		}
		statement = new LinkAssessmentStatement(
			    assessmentStatement->getComparator(),
			    mainAssessment, otherAssessment);

		return statement;
	}
  
	LinkStatement* FormatterLinkConverter::createStatement(
		    Statement* statementExpression,
		    Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		AssessmentStatement* as;
		CompoundStatement* cs;
		vector<Bind*>* binds;
		int size;
		LinkStatement* statement;
		LinkStatement* childStatement;
		vector<Statement*>* statements;
		vector<Statement*>::iterator i;
		Statement* ncmChildStatement;

		if (statementExpression->instanceOf("AssessmentStatement")) {
			as = (AssessmentStatement*)statementExpression;
			binds = ncmLink->getRoleBinds(as->getMainAssessment());
			if (binds != NULL) {
				size = binds->size();
				if (size == 1) {
					statement = createAssessmentStatement(
						    as,
						    (*binds)[0], ncmLink, parentObject, depthLevel);

				} else {
					return NULL;
				}

			} else {
				return NULL;
			}

		} else { // CompoundStatement
			cs = (CompoundStatement*)statementExpression;
			statement = new LinkCompoundStatement(cs->getOperator());
			((LinkCompoundStatement*)statement)->setNegated(cs->isNegated());
			statements = cs->getStatements();
			if (statements != NULL) {
				i = statements->begin();
				while (i != statements->end()) {
					ncmChildStatement = (*i);
					childStatement = createStatement(
						    ncmChildStatement,
						    ncmLink, parentObject, depthLevel);

					((LinkCompoundStatement*)statement)->addStatement(
						    childStatement);

					++i;
				}
			}
		}

		return statement;
	}

	LinkAttributeAssessment* FormatterLinkConverter::createAttributeAssessment(
		    AttributeAssessment* attributeAssessment,
		    Bind* bind, Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		FormatterEvent* event;

		event = createEvent(bind, ncmLink, parentObject, depthLevel);
		return new LinkAttributeAssessment(
			    event, attributeAssessment->getAttributeType());
	}

	LinkSimpleAction* FormatterLinkConverter::createSimpleAction(
		    SimpleAction* sae,
		    Bind* bind, Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		FormatterEvent* event;
		short actionType;
		LinkSimpleAction* action;
		Parameter* connParam;
		Parameter* param;
		string paramValue;
		Animation* animation;
		long repeat;
		double delay;

		action = NULL;
		event = createEvent(bind, ncmLink, parentObject, depthLevel);
		actionType = sae->getActionType();
		switch (actionType) {
			case SimpleAction::ACT_START:
				action = new LinkRepeatAction(event, actionType);

				//repeat
				paramValue = sae->getRepeat();
				if (paramValue == "") {
					repeat = 0;

				} else if (paramValue[0] == '$') { //instanceOf("Parameter")
					connParam = new Parameter(
						    paramValue.substr(1, paramValue.length()), "");

					param = bind->getParameter(connParam->getName());
					if (param == NULL) {
						param = ncmLink->getParameter(connParam->getName());
					}

					if (param == NULL) {
						repeat = 0;

					} else {
						repeat = (long)stof(param->getValue());
					}

				} else {
					repeat = (long)stof(paramValue);
				}

				((LinkRepeatAction*)action)->setRepetitions(repeat);

				//repeatDelay
				paramValue = sae->getRepeatDelay();
				delay = compileDelay(ncmLink, paramValue, bind);
				((LinkRepeatAction*)action)->setRepetitionInterval(delay);
				break;

			case SimpleAction::ACT_STOP:
			case SimpleAction::ACT_PAUSE:
			case SimpleAction::ACT_RESUME:
			case SimpleAction::ACT_ABORT:
				action = new LinkSimpleAction(event, actionType);
				break;

			case SimpleAction::ACT_SET:
				paramValue = sae->getValue();
				if (paramValue != "" &&
					    paramValue[0] == '$') { //instanceOf("Parameter")

					connParam = new Parameter(
						    paramValue.substr(1, paramValue.length()), "");

					param = bind->getParameter(connParam->getName());
					if (param == NULL) {
						param = ncmLink->getParameter(connParam->getName());
					}

					delete connParam;
					connParam = NULL;

					if (param != NULL) {
						paramValue = param->getValue();

					} else {
						paramValue = "";
					}
				}

				action = new LinkAssignmentAction(
					    event, actionType, paramValue);

				//animation
				animation = sae->getAnimation();
				if (animation != NULL) {
					paramValue = animation->getDuration();
					if (paramValue[0] == '$') {
						connParam = new Parameter(
								paramValue.substr(1, paramValue.length()), "");

						param = bind->getParameter(connParam->getName());
						if (param == NULL) {
							param = ncmLink->getParameter(connParam->getName());
						}

						delete connParam;
						connParam = NULL;

						if (param != NULL) {
							animation->setDuration(param->getValue());

						} else {
							animation->setDuration("0");
						}
					}

					paramValue = animation->getBy();
					if (paramValue[0] == '$') {
						connParam = new Parameter(
								paramValue.substr(1, paramValue.length()), "");

						param = bind->getParameter(connParam->getName());
						if (param == NULL) {
							param = ncmLink->getParameter(connParam->getName());
						}

						delete connParam;
						connParam = NULL;

						if (param != NULL) {
							animation->setBy(param->getValue());

						} else {
							animation->setBy("0");
						}
					}

					((LinkAssignmentAction*)action)->setAnimation(animation);
				}
				break;

			default:
				action = NULL;
				break;
		}

		if (action != NULL) {
			paramValue = sae->getDelay();
			delay = compileDelay(ncmLink, paramValue, bind);
			action->setWaitDelay(delay);
		}

		return action;
	}

	LinkCompoundAction* FormatterLinkConverter::createCompoundAction(
		    short op,
		    double delay,
		    vector<Action*>* ncmChildActions,
		    CausalLink* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		LinkCompoundAction* action;  
		Action* ncmChildAction;
		LinkAction* childAction;

		action = new LinkCompoundAction(op);
		if (delay > 0) {
			action->setWaitDelay(delay);
		}

		if (ncmChildActions != NULL) {
			vector<Action*>::iterator i;
			i = ncmChildActions->begin();
			while (i != ncmChildActions->end()) {
				ncmChildAction = (*i);
				childAction = createAction(
					    ncmChildAction, ncmLink, parentObject, depthLevel);

				if (childAction != NULL) {
					action->addAction(childAction);
				}
				++i;
			}
		}

		return action;
	}

	LinkTriggerCondition* FormatterLinkConverter::createSimpleCondition(
		    SimpleCondition* simpleCondition,
		    Bind* bind,
		    Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		FormatterEvent* event;
		double delay;
		string delayObject;
		LinkTriggerCondition* condition;

		event = createEvent(bind, ncmLink, parentObject, depthLevel);
		condition = new LinkTransitionTriggerCondition(
			    event, simpleCondition->getTransition());

		delayObject = simpleCondition->getDelay();
		delay = compileDelay(ncmLink, delayObject, bind);
		if (delay > 0) {
			condition->setDelay(delay);
		}
		return condition;
	}

	FormatterEvent* FormatterLinkConverter::createEvent(
		    Bind* bind,
		    Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		NodeNesting* endPointNodeSequence;
		NodeNesting* endPointPerspective;
		Node* parentNode;
		ExecutionObject* executionObject;
		InterfacePoint* interfacePoint;
		string key;
		FormatterEvent* event = NULL;

		endPointPerspective = parentObject->getNodePerspective();

		// parent object may be a refer
		parentNode = endPointPerspective->getAnchorNode();

		// teste para verificar se ponto terminal eh o proprio contexto ou
		// refer para o proprio contexto
		endPointNodeSequence = new NodeNesting(bind->getNodeNesting());
		if (endPointNodeSequence->getAnchorNode() != 
			    endPointPerspective->getAnchorNode() &&
			    endPointNodeSequence->getAnchorNode() !=
			    parentNode->getDataEntity()) {

			endPointPerspective->append(endPointNodeSequence);
		}

		try {
			executionObject = compiler->getExecutionObject(
				    endPointPerspective, bind->getDescriptor(), depthLevel);

		} catch (ObjectCreationForbiddenException* exc) {
			wclog << endl;
			wclog << "FormatterLinkConverter::createEvent catching an ";
			wclog << "ObjectCreationForbiddenException returning NULL";
			wclog << endl << endl;
			return NULL;
		}

		interfacePoint = bind->getEndPointInterface();
		if (interfacePoint == NULL) {
			//TODO: This is an error, the formatter then return the main event
			wclog << "FormatterLinkConverter::createEvent Warning! returning ";
			wclog << "wholePresentationEvent for '";
			wclog << executionObject->getId().c_str() << "'" << endl;
			return executionObject->getWholeContentPresentationEvent();
		}

		key = getBindKey(ncmLink, bind);
		event = compiler->getEvent(
			    executionObject,
			    interfacePoint, bind->getRole()->getEventType(), key);

		return event;
	}

	double FormatterLinkConverter::getDelayParameter(
		    Link* ncmLink, Parameter* connParam, Bind* ncmBind) {

		Parameter* parameter;

		parameter = NULL;
		if (ncmBind != NULL) {
			parameter = ncmBind->getParameter(connParam->getName());
		}

		if (parameter == NULL) {
			parameter = ncmLink->getParameter(connParam->getName());
		}

		if (parameter == NULL) {
			return 0.0;

		} else {
			try {
				return stof(
					    parameter->getValue().substr(
					    	    0, parameter->getValue().length())) * 1000;

			} catch (exception* exc) {
				return 0.0;
			}
		}
	}

	string FormatterLinkConverter::getBindKey(Link* ncmLink, Bind* ncmBind) {
		Role* role;
		string keyValue;
		Parameter* param,* auxParam;
		string key;

		role = ncmBind->getRole();
		if (role == NULL) {
			wclog << "FormatterLinkConverter::getBindKey Warning! role == NULL";
			wclog << endl;
			return "";
		}

		if (role->instanceOf("SimpleCondition")) {
			keyValue = ((SimpleCondition*)role)->getKey();

		} else if (role->instanceOf("AttributeAssessment")) {
			keyValue = ((AttributeAssessment*)role)->getKey();

		} else {
			return "";
		}

		if (keyValue == "") {
			key = "";

		} else if (keyValue[0] == '$') { // instanceof Parameter
			param = new Parameter(
				    keyValue.substr(1, keyValue.length()), "");

			auxParam = ncmBind->getParameter(param->getName());
			if (auxParam == NULL) {
				auxParam = ncmLink->getParameter(param->getName());
			}

			if (auxParam != NULL) {
				key = auxParam->getValue();

			} else {
				key = "";
			}

		} else {
			key = keyValue;
		}

		return key;
	}

	double FormatterLinkConverter::compileDelay(
		    Link* ncmLink,
		    string delayObject, Bind* bind) {

		double delay;
		Parameter* param;

		if (delayObject == "") {
			delay = 0;

		} else if (delayObject[0] == '$') { // instanceof Parameter
			param = new Parameter(
				    delayObject.substr(1, delayObject.length()), "");

			delay = getDelayParameter(ncmLink, param, bind);

		} else {
			delay = (double)(stof(delayObject));
		}

		return delay;
	}
}
}
}
}
}
}
