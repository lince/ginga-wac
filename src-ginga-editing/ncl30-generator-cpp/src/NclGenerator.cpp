/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Copyright (C) 2009 UFSCar/Lince, Todos os Direitos Reservados.

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
lince@dc.ufscar.br
http://www.ncl.org.br
http://www.ginga.org.br
http://lince.dc.ufscar.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright (C) 2009 UFSCar/Lince, Todos os Direitos Reservados.

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
lince@dc.ufscar.br
http://www.ncl.org.br
http://www.ginga.org.br
http://lince.dc.ufscar.br
*******************************************************************************/

/**
* @file NclGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/NclGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generate {

	AnchorGenerator* NclGenerator::getGenerableInstance(Anchor* anchor) {
		return static_cast<AnchorGenerator*>(anchor);
	}

	AssessmentStatementGenerator* NclGenerator::getGenerableInstance(AssessmentStatement* assessmentStatement) {
		return static_cast<AssessmentStatementGenerator*>(assessmentStatement);
	}

	AtributeAssessmentGenerator* NclGenerator::getGenerableInstance(AttributeAssessment* attributeAssessment) {
		return static_cast<AtributeAssessmentGenerator*>(attributeAssessment);
	}

	BindGenerator* NclGenerator::getGenerableInstance(Bind* bind) {
		return static_cast<BindGenerator*>(bind);
	}

	CausalConnectorGenerator* NclGenerator::getGenerableInstance(CausalConnector* causalConnector) {
		return static_cast<CausalConnectorGenerator*>(causalConnector);
	}

	CausalLinkGenerator*  NclGenerator::getGenerableInstance(CausalLink* causalLink) {
		return static_cast<CausalLinkGenerator*>(causalLink);
	}

	CircleSpatialAnchorGenerator* NclGenerator::getGenerableInstance(CircleSpatialAnchor* circleSpatialAnchor) {
		return static_cast<CircleSpatialAnchorGenerator*>(circleSpatialAnchor);
	}
	
	CompoundActionGenerator* NclGenerator::getGenerableInstance(CompoundAction* compoundAction) {
		return static_cast<CompoundActionGenerator*>(compoundAction);
	}

	CompoundConditionGenerator* NclGenerator::getGenerableInstance(CompoundCondition* compoundContition) {
		return static_cast<CompoundConditionGenerator*>(compoundContition);
	}

	CompoundStatementGenerator* NclGenerator::getGenerableInstance(CompoundStatement* compoundStatement) {
		return static_cast<CompoundStatementGenerator*>(compoundStatement);
	}

	ConnectorBaseGenerator* NclGenerator::getGenerableInstance(ConnectorBase* connectorBase) {
		return static_cast<ConnectorBaseGenerator*>(connectorBase);
	}

	ContentNodeGenerator* NclGenerator::getGenerableInstance(ContentNode* contentNode) {
		return static_cast<ContentNodeGenerator*>(contentNode);
	}

	ContextNodeGenerator* NclGenerator::getGenerableInstance(ContextNode* contextNode) {
		return static_cast<ContextNodeGenerator*>(contextNode);
	}

	DescriptorBaseGenerator* NclGenerator::getGenerableInstance(DescriptorBase* descriptorBase) {
		return static_cast<DescriptorBaseGenerator*>(descriptorBase);
	}

	DescriptorGenerator* NclGenerator::getGenerableInstance(Descriptor* descriptor) {
		return static_cast<DescriptorGenerator*>(descriptor);
	}

	IntervalAnchorGenerator* NclGenerator::getGenerableInstance(IntervalAnchor* intervalAnchor) {
		return static_cast<IntervalAnchorGenerator*>(intervalAnchor);
	}

	LayoutRegionGenerator* NclGenerator::getGenerableInstance(LayoutRegion* layoutRegion) {
		return static_cast<LayoutRegionGenerator*>(layoutRegion);
	}

	NclDocumentGenerator* NclGenerator::getGenerableInstance(NclDocument* nclDocument) {
		return static_cast<NclDocumentGenerator*>(nclDocument);
	}

	ParameterGenerator* NclGenerator::getGenerableInstance(Parameter* parameter) {
		return static_cast<ParameterGenerator*>(parameter);
	}

	PortGenerator* NclGenerator::getGenerableInstance(Port* port) {
		return static_cast<PortGenerator*>(port);
	}

	PropertyAnchorGenerator* NclGenerator::getGenerableInstance(PropertyAnchor* propertyAnchor) {
		return static_cast<PropertyAnchorGenerator*>(propertyAnchor);
	}

	RectangleSpatialAnchorGenerator* NclGenerator::getGenerableInstance(RectangleSpatialAnchor* rectangleSpatialAnchor) {
		return static_cast<RectangleSpatialAnchorGenerator*>(rectangleSpatialAnchor);
	}

	RegionBaseGenerator* NclGenerator::getGenerableInstance(RegionBase* regionBase) {
		return static_cast<RegionBaseGenerator*>(regionBase);
	}

	SimpleActionGenerator* NclGenerator::getGenerableInstance(SimpleAction* simpleAction) {
		return static_cast<SimpleActionGenerator*>(simpleAction);
	}

	SimpleConditionGenerator* NclGenerator::getGenerableInstance(SimpleCondition* simpleCondition) {
		return static_cast<SimpleConditionGenerator*>(simpleCondition);
	}

	TextAnchorGenerator* NclGenerator::getGenerableInstance(TextAnchor* textAnchor) {
		return static_cast<TextAnchorGenerator*>(textAnchor);
	}

	ValueAssessmentGenerator* NclGenerator::getGenerableInstance(ValueAssessment* valueAssessment) {
		return static_cast<ValueAssessmentGenerator*>(valueAssessment);
	}

	DescriptorSwitchGenerator* NclGenerator::getGenerableInstance(DescriptorSwitch* descriptorSwitch)  {
		return static_cast<DescriptorSwitchGenerator*>(descriptorSwitch);
	}

	TransitionBaseGenerator* NclGenerator::getGenerableInstance(TransitionBase* transitionBase) {
		return static_cast<TransitionBaseGenerator*>(transitionBase);
	}

	TransitionGenerator* NclGenerator::getGenerableInstance(Transition* transition) {
		return static_cast<TransitionGenerator*>(transition);
	}

	RuleBaseGenerator* NclGenerator::getGenerableInstance(RuleBase* ruleBase) {
		return static_cast<RuleBaseGenerator*>(ruleBase);
	}

	SimpleRuleGenerator* NclGenerator::getGenerableInstance(SimpleRule* simpleRule) {
		return static_cast<SimpleRuleGenerator*>(simpleRule);
	}

	CompositeRuleGenerator* NclGenerator::getGenerableInstance(CompositeRule* compositeRule) {
		return static_cast<CompositeRuleGenerator*>(compositeRule);
	}

	SwitchNodeGenerator* NclGenerator::getGenerableInstance(SwitchNode* switchNode) {
		return static_cast<SwitchNodeGenerator*>(switchNode);
	}

	SwitchPortGenerator* NclGenerator::getGenerableInstance(SwitchPort* switchPort) {
		return static_cast<SwitchPortGenerator*>(switchPort);
	}

	LabeledAnchorGenerator* NclGenerator::getGenerableInstance(LabeledAnchor* labeledAnchor) {
		return static_cast<LabeledAnchorGenerator*>(labeledAnchor);
	}


}
}
}
}
}
