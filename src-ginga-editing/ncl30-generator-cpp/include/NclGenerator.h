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
* @file NclGenerator.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef _NCLGENERATOR_H
#define _NCLGENERATOR_H

#include "AnchorGenerator.h"
#include "AssessmentStatementGenerator.h"
#include "AttributeAssessmentGenerator.h"
#include "BindGenerator.h"
#include "CausalConnectorGenerator.h"
#include "CausalLinkGenerator.h"
#include "CircleSpatialAnchorGenerator.h"
#include "CompoundActionGenerator.h"
#include "CompoundConditionGenerator.h" 
#include "CompoundStatementGenerator.h" 
#include "ConnectorBaseGenerator.h"
#include "ContentNodeGenerator.h" 
#include "ContextNodeGenerator.h"
#include "DescriptorBaseGenerator.h" 
#include "DescriptorGenerator.h"  
#include "IntervalAnchorGenerator.h" 
#include "LayoutRegionGenerator.h"  
#include "NclDocumentGenerator.h"  
#include "ParameterGenerator.h"  
#include "PortGenerator.h" 
#include "PropertyAnchorGenerator.h" 
#include "RectangleSpatialAnchorGenerator.h" 
#include "RegionBaseGenerator.h" 
#include "SimpleActionGenerator.h" 
#include "SimpleConditionGenerator.h" 
#include "TextAnchorGenerator.h"
#include "ValueAssessmentGenerator.h"
#include "SwitchPortGenerator.h"
#include "SwitchNodeGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generate {

class NclGenerator {
	public:
		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param anchor Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static AnchorGenerator*  getGenerableInstance(Anchor* anchor);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param assessmentStatement Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static AssessmentStatementGenerator*  getGenerableInstance(AssessmentStatement* assessmentStatement);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param attributeAssessment Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static AtributeAssessmentGenerator* getGenerableInstance(AttributeAssessment* attributeAssessment);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param bind Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static BindGenerator* getGenerableInstance(Bind* bind);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param causalConnector Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static CausalConnectorGenerator*  getGenerableInstance(CausalConnector* causalConnector);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param causalLink Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static CausalLinkGenerator*  getGenerableInstance(CausalLink* causalLink);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param circleSpatialAnchor Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static CircleSpatialAnchorGenerator*  getGenerableInstance(CircleSpatialAnchor* circleSpatialAnchor);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param compoundAction Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static CompoundActionGenerator* getGenerableInstance(CompoundAction* compoundAction);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param compoundContition Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static CompoundConditionGenerator*  getGenerableInstance(CompoundCondition* compoundContition);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param compoundStatement Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static CompoundStatementGenerator*  getGenerableInstance(CompoundStatement* compoundStatement);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param connectorBase Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static ConnectorBaseGenerator* getGenerableInstance(ConnectorBase* connectorBase);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param contentNode Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static ContentNodeGenerator*  getGenerableInstance(ContentNode* contentNode);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param contextNode Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static ContextNodeGenerator* getGenerableInstance(ContextNode* contextNode);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param descriptorBase Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static DescriptorBaseGenerator*  getGenerableInstance(DescriptorBase* descriptorBase);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param descriptor Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static DescriptorGenerator*  getGenerableInstance(Descriptor* descriptor);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param intervalAnchor Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static IntervalAnchorGenerator*  getGenerableInstance(IntervalAnchor* intervalAnchor);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param layoutRegion Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static LayoutRegionGenerator*  getGenerableInstance(LayoutRegion* layoutRegion);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param nclDocument Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static NclDocumentGenerator*  getGenerableInstance(NclDocument* nclDocument);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param parameter Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static ParameterGenerator*  getGenerableInstance(Parameter* parameter);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param assessmentStatement Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static PortGenerator* getGenerableInstance(Port* port);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param propertyAnchor Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static PropertyAnchorGenerator* getGenerableInstance(PropertyAnchor* propertyAnchor);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param rectangleSpatialAnchor Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static RectangleSpatialAnchorGenerator*  getGenerableInstance(RectangleSpatialAnchor* rectangleSpatialAnchor);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param reginBase Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static RegionBaseGenerator* getGenerableInstance(RegionBase* reginBase);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param simpleAction Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static SimpleActionGenerator* getGenerableInstance(SimpleAction* simpleAction);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param simpleCondition Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static SimpleConditionGenerator*  getGenerableInstance(SimpleCondition* simpleCondition);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param textAnchor Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static TextAnchorGenerator*  getGenerableInstance(TextAnchor* textAnchor);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param valueAssessment Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static ValueAssessmentGenerator*  getGenerableInstance(ValueAssessment* valueAssessment);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param assessmentStatement Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static DescriptorSwitchGenerator* getGenerableInstance(DescriptorSwitch* descriptorSwitch);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param transitionBase Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static TransitionBaseGenerator* getGenerableInstance(TransitionBase* transitionBase);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param transition Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static TransitionGenerator* getGenerableInstance(Transition* transition);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param ruleBase Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static RuleBaseGenerator* getGenerableInstance(RuleBase* ruleBase);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param simpleRule Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static SimpleRuleGenerator* getGenerableInstance(SimpleRule* simpleRule);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param compositeRule Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static CompositeRuleGenerator* getGenerableInstance(CompositeRule* compositeRule);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param switchNode Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static SwitchNodeGenerator* getGenerableInstance(SwitchNode* switchNode);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param switchPort Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static SwitchPortGenerator* getGenerableInstance(SwitchPort* switchPort);

		/**
		 * Obtem uma instância que representa uma entidade NCL capaz de gerar o código XML equivalente.
		 * @param labeledAnchor Instância que representa uma entidade NCL.
		 * @return Instância capaz de gerar código XML para a entidade NCL.
		 */
		static LabeledAnchorGenerator* getGenerableInstance(LabeledAnchor* labeledAnchor);
};

}
}
}
}
}

#endif //_NCLGENERATOR_H

