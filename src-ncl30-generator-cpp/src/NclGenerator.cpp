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
namespace generator {

	NclGenerator::NclGenerator() {
		referDocument = NULL;
	}

	NclGenerator::~NclGenerator() {

	}

	void NclGenerator::setReferNclDocument(NclDocument* referDocument) {
		cerr<<" NclGenerator::setReferNclDocument"<<endl;
		cerr<<"Chegamos aqui!"<<endl;
		this->referDocument = referDocument;

	}

	NclDocument* NclGenerator::getReferNclDocument() {
		return this->referDocument;
	}

	string NclGenerator::generateXmlCode(Entity* entity) {
		if (entity == NULL) {
			string men = "NclGenerator::generateXmlCode(Entity* entity)\n";
			men += "Tentativa de Gerar Código para entidade Ncl nula.";
			BadArgumentException ex(men);
			throw ex;
		}

		if (referDocument == NULL) {
			string men = "NclGenerator::generateXmlCode(Entity* entity)\n";
			men += "Tentativa de Gerar Código sem setar o documento Ncl de referência.";
			InitializationException ex(men);
			throw ex;
		}

		if (entity->instanceOf("Anchor")) {
			return static_cast<AnchorGenerator*>(entity)->generateCode();

		} else if (entity->instanceOf("CausalConnector")) {
			return static_cast<CausalConnectorGenerator*>(entity)->generateCode();

		} else if (entity->instanceOf("CausalLink")) {
			return static_cast<CausalLinkGenerator*>(entity)->generateCode(
					referDocument->getConnectorBase());

		} else if (entity->instanceOf("ContentNode")) {
			return static_cast<ContentNodeGenerator*>(entity)->generateCode();

		} else if (entity->instanceOf("ContextNode")) {
			return static_cast<ContextNodeGenerator*>(entity)->generateCode(referDocument);

		} else if (entity->instanceOf("Descriptor")) {
			return static_cast<DescriptorGenerator*>(entity)->generateCode();

		} else if (entity->instanceOf("LayoutRegion")) {
			return static_cast<LayoutRegionGenerator*>(entity)->generateCode();

		} else if (entity->instanceOf("Port")) {
			return static_cast<PortGenerator*>(entity)->generateCode();

		} else if (entity->instanceOf("DescriptorSwitch")) {
			return static_cast<DescriptorSwitchGenerator*>(entity)->generateCode();

		} else if (entity->instanceOf("Transition")) {
			return static_cast<TransitionGenerator*>(entity)->generateCode();

		} else if (entity->instanceOf("SimpleRule")) {
			return static_cast<SimpleRuleGenerator*>(entity)->generateCode();

		} else if (entity->instanceOf("CompositeRule")) {
			return static_cast<CompositeRuleGenerator*>(entity)->generateCode();

		} else if (entity->instanceOf("SwitchNode")) {
			return static_cast<SwitchNodeGenerator*>(entity)->generateCode(referDocument);

		} else if (entity->instanceOf("SwitchPort")) {
			return static_cast<SwitchPortGenerator*>(entity)->generateCode();

		} else {
			string men = "NclGenerator::generateXmlCode(Entity* entity)\n";
			men += "Tipo de entidade NCl não é suportada pela versão atual de NclGenerator.";
			UnsupportedNclEntityException ex(men);
			throw ex;
		}
	}

	string NclGenerator::generateXmlCode(ConditionExpression* conditionExpression) {

		if (conditionExpression == NULL) {
			string men = "NclGenerator::generateXmlCode(ConditionExpression* conditionExpression)\n";
			men += "Tentativa de Gerar Código para conditionExpression Ncl nulo.";
			BadArgumentException ex(men);
			throw ex;
		}

		if (referDocument == NULL) {
			string men = "NclGenerator::generateXmlCode(ConditionExpression* conditionExpression)\n";
			men += "Tentativa de Gerar Código sem setar o documento Ncl de referência.";
			InitializationException ex(men);
			throw ex;
		}

		if (conditionExpression->instanceOf("CompoundStatement")) {
			return static_cast<CompoundStatementGenerator*>(conditionExpression)->generateCode();

		} else if (conditionExpression->instanceOf("CompoundCondition")) {
			return static_cast<CompoundConditionGenerator*>(conditionExpression)->generateCode();

		} else if (conditionExpression->instanceOf("SimpleCondition")) {
			return static_cast<SimpleConditionGenerator*>(conditionExpression)->generateCode();

		} else if (conditionExpression->instanceOf("AssessmentStatement")) {
			return static_cast<AssessmentStatementGenerator*>(conditionExpression)->generateCode();

		} else {
			string men = "NclGenerator::generateXmlCode(ConditionExpression* conditionExpression)\n";
			men += "Tipo de conditionExpression não é suportado pela versão atual de NclGenerator.";
			UnsupportedNclEntityException ex(men);
			throw ex;
		}
	}

	string NclGenerator::generateXmlCode(Bind* bind) {

		if (bind == NULL) {
			string men = "NclGenerator::generateXmlCode(Bind* bind)\n";
			men += "Tentativa de Gerar Código para bind Ncl nula.";
			BadArgumentException ex(men);
			throw ex;
		}

		if (referDocument == NULL) {
			string men = "NclGenerator::generateXmlCode(Bind* bind)\n";
			men += "Tentativa de Gerar Código sem setar o documento Ncl de referência.";
			InitializationException ex(men);
			throw ex;
		}
		if (bind->instanceOf("Bind")) {
			return static_cast<BindGenerator*>(bind)->generateCode();

		}
	}

	string NclGenerator::generateXmlCode(Action* action) {
		if (action == NULL) {
			string men = "NclGenerator::generateXmlCode(Action* action)\n";
			men += "Tentativa de Gerar Código para Ação Ncl nula.";
			BadArgumentException ex(men);
			throw ex;
		}

		if (referDocument == NULL) {
			string men = "NclGenerator::generateXmlCode(Action* action)\n";
			men += "Tentativa de Gerar Código sem setar o documento Ncl de referência.";
			InitializationException ex(men);
			throw ex;
		}

		if (action->instanceOf("CompoundAction")) {
			return static_cast<CompoundActionGenerator*>(action)->generateCode();

		} else if (action->instanceOf("SimpleAction")) {
			return static_cast<SimpleActionGenerator*>(action)->generateCode();

		} else {
			string men = "NclGenerator::generateXmlCode(Action* action)\n";
			men += "Tipo de parametro Action Ncl não é suportado pela versão atual de NclGenerator.";
			UnsupportedNclEntityException ex(men);
			throw ex;
		}
	}

	string NclGenerator::generateXmlCode(Base* base) {
		if (base == NULL) {
			string men = "NclGenerator::generateXmlCode(Base* base)\n";
			men += "Tentativa de Gerar Código para base Ncl nula.";
			BadArgumentException ex(men);
			throw ex;
		}

		if (referDocument == NULL) {
			string men = "NclGenerator::generateXmlCode(Base* base)\n";
			men += "Tentativa de Gerar Código sem setar o documento Ncl de referência.";
			InitializationException ex(men);
			throw ex;
		}

		if (base->instanceOf("ConnectorBase")) {
			return static_cast<ConnectorBaseGenerator*>(base)->generateCode();

		} else if (base->instanceOf("DescriptorBase")) {
			return static_cast<DescriptorBaseGenerator*>(base)->generateCode();

		} else if (base->instanceOf("RegionBase")) {
			return static_cast<RegionBaseGenerator*>(base)->generateCode();

		} else if (base->instanceOf("TransitionBase")) {
			return static_cast<TransitionBaseGenerator*>(base)->generateCode();

		} else if (base->instanceOf("RuleBase")) {
			return static_cast<RuleBaseGenerator*>(base)->generateCode();

		} else {
			string men = "NclGenerator::generateXmlCode(Base* base)\n";
			men += "Tipo de base NCL não é suportada pela versão atual de NclGenerator.";
			UnsupportedNclEntityException ex(men);
			throw ex;
		}
	}

	string NclGenerator::generateXmlCode(NclDocument* nclDocument) {
		if (nclDocument == NULL) {
			nclDocument = referDocument;
		}

		if (nclDocument == NULL) {
			string men = "NclGenerator::generateXmlCode(NclDocument* nclDocument)\n";
			men += "Tentativa de gerar documento NCL a partir de instâncias nulas de NclDocument.";
			InitializationException ex(men);
			throw ex;
		}

		return static_cast<NclDocumentGenerator*>(nclDocument)->generateCode();
	}

	string NclGenerator::generateXmlCode(Parameter* parameter, string paramType) {
		if (parameter == NULL) {
			string men = "NclGenerator::generateXmlCode(Parameter* parameter, string paramType)\n";
			men += "Tentativa de Gerar Código para Parametro Ncl nulo.";
			BadArgumentException ex(men);
			throw ex;
		}

		if (referDocument == NULL) {
			string men = "NclGenerator::generateXmlCode(Parameter* parameter, string paramType)\n";
			men += "Tentativa de Gerar Código sem setar o documento Ncl de referência.";
			InitializationException ex(men);
			throw ex;
		}

		string valueName;

		if (paramType == "descriptorParam") {
			valueName = "value";
		} else if (paramType == "bindParam") {
			valueName = "value";
		} else if (paramType == "linkParam") {
			valueName = "value";
		} else if (paramType == "connectorParam") {
			valueName = "type";
		} else {
			string men = "NclGenerator::generateXmlCode(Parameter* parameter, string paramType)\n";
			men += "Tipo de parametro '" + paramType + "' não é suportado pela versão atual de NclGenerator.";
			UnsupportedNclEntityException ex(men);
			throw ex;
		}

		return static_cast<ParameterGenerator*>(parameter)->generateCode(paramType, valueName);
	}

	bool NclGenerator::generateNclFile(string fileName, NclDocument* nclDoc) {
		if (nclDoc == NULL) {
			nclDoc = referDocument;
		}

		if (nclDoc == NULL) {
			string men = "NclGenerator::generateNclFile(string fileName, NclDocument nclDoc)\n";
			men += "Tentativa de gerar documento NCL a partir de instâncias nulas de NclDocument.";
			InitializationException ex(men);
			throw ex;
		}

		NclFileGenerator* fileGen = new NclFileGenerator(nclDoc, fileName);
		bool result = fileGen->generateFile();
		delete fileGen;
		return result;

	}

}
}
}
}
}


extern "C" ::br::ufscar::lince::ncl::generator::INclGenerator*
		createNclGenerator() {

	return new::br::ufscar::lince::ncl::generator::NclGenerator::NclGenerator();
}

extern "C" void destroyNclGenerator(
		::br::ufscar::lince::ncl::generator::INclGenerator* nclGenerator) {

	delete nclGenerator;
}
