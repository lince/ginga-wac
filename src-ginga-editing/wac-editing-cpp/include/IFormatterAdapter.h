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
* @file IFormatterAdapter.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef IFORMATTERADAPTER_H
#define IFORMATTERADAPTER_H

#include "ncl/components/ContextNode.h"
#include "ncl/components/CompositeNode.h"
#include "ncl/components/ContentNode.h"
#include "ncl/components/Node.h"
#include "ncl/components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/interfaces/Anchor.h"
#include "ncl/interfaces/PropertyAnchor.h"
#include "ncl/interfaces/Port.h"
#include "ncl/interfaces/SwitchPort.h"
#include "ncl/interfaces/InterfacePoint.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/switches/Rule.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/link/Bind.h"
#include "ncl/link/CausalLink.h"
#include "ncl/link/Link.h"
#include "ncl/link/LinkComposition.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl/connectors/EventUtil.h"
#include "ncl/connectors/SimpleAction.h"
#include "ncl/connectors/Connector.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/reuse/ReferNode.h"
using namespace ::br::pucrio::telemidia::ncl::reuse;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "ncl/Base.h"
#include "ncl/connectors/ConnectorBase.h"
#include "ncl/descriptor/DescriptorBase.h"
#include "ncl/layout/RegionBase.h"
#include "ncl/switches/RuleBase.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/transition/Transition.h"
#include "ncl/transition/TransitionBase.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include <cstdlib>
#include <string>
using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace editing {

	/**
	 * Interface que permite que as classes do módulo Wac-Editing enviem mensagem a classe Formatter do módulo Formatter.
	 * Esta interface basicamente possuí todos os métodos de edição ao vivo (que pertencem ao formatter) e outros métodos
	 * que são necessários da classe Formatter para a execução do Ginga-Wac.
	 */
	class IFormatterAdapter {
		public:
			/**
			 * Destrói IFormatterAdapter.
			 */
			virtual ~IFormatterAdapter() {};

			/// Constante que indica edições do cliente.
			static const int CLIENT_EDITING = 1;

			/// Constante que indica edições da emissora.
			static const int BROADCASTER_EDITING =0;

			//Métodos de edição ao vivo do formatter.
			virtual LayoutRegion* addRegion(
				    string compositeDocumentId, string regionId, string xmlRegion,
				 int=BROADCASTER_EDITING)=0;

			virtual LayoutRegion* removeRegion(string compositeDocumentId, 
					string regionId,int=BROADCASTER_EDITING)=0;

			virtual RegionBase* addRegionBase(string compositeDocumentId, 
					string xmlRegionBase, int=BROADCASTER_EDITING)=0;

			virtual RegionBase* removeRegionBase(string compositeDocumentId, 
					string regionBaseId, int=BROADCASTER_EDITING)=0;

			virtual Rule* addRule(string compositeDocumentId, string xmlRule,
					int=BROADCASTER_EDITING)=0;

			virtual Rule* removeRule(string compositeDocumentId, string ruleId,
					int=BROADCASTER_EDITING)=0;

			virtual RuleBase* addRuleBase(string compositeDocumentId, 
					string xmlRuleBase, int=BROADCASTER_EDITING)=0;

			virtual RuleBase* removeRuleBase(string compositeDocumentId, 
					string ruleBaseId, int=BROADCASTER_EDITING)=0;

			virtual Transition* addTransition(string compositeDocumentId, 
					string xmlTransition, int=BROADCASTER_EDITING)=0;

			virtual Transition* removeTransition(string compositeDocumentId, 
					string transitionId, int=BROADCASTER_EDITING)=0;

			virtual TransitionBase* addTransitionBase(
					string compositeDocumentId, string xmlTransitionBase,
					int=BROADCASTER_EDITING)=0;

			virtual TransitionBase* removeTransitionBase(string documentId, 
					string transitionBaseId,
					int=BROADCASTER_EDITING)=0;

			virtual Connector* addConnector(string compositeDocumentId, 
					string xmlConnector,
					int=BROADCASTER_EDITING)=0;

			virtual Connector* removeConnector(string compositeDocumentId, 
					string connectorId,
					int=BROADCASTER_EDITING)=0;

			virtual ConnectorBase* addConnectorBase(
					string compositeDocumentId, string xmlConnectorBasem,
					int=BROADCASTER_EDITING)=0;

			virtual ConnectorBase* removeConnectorBase(
					string compositeDocumentId, string connectorBaseId,
					int=BROADCASTER_EDITING)=0;

			virtual GenericDescriptor* addDescriptor(
				    	string compositeDocumentId, string xmlDescriptor,
					int=BROADCASTER_EDITING)=0;

			virtual GenericDescriptor* removeDescriptor(
				   	string compositeDocumentId, string descriptorId,
					int=BROADCASTER_EDITING)=0;

			virtual DescriptorBase* addDescriptorBase(
				    	string compositeDocumentId, string xmlDescriptorBase,
					int=BROADCASTER_EDITING)=0;

			virtual DescriptorBase* removeDescriptorBase(
				    	string compositeDocumentId, string descriptorBaseId,
					int=BROADCASTER_EDITING)=0;

			virtual Base* addImportBase(string compositeDocumentId, 
					string docBaseId, string xmlImportBase,
					int=BROADCASTER_EDITING)=0;

			virtual Base* removeImportBase(string compositeDocumentId, 
					string docBaseId, string documentURI,
					int=BROADCASTER_EDITING)=0;

			virtual NclDocument* addImportedDocumentBase(string compositeDocumentId, 
					string xmlImportedDocumentBase,
					int=BROADCASTER_EDITING)=0;

			virtual NclDocument* removeImportedDocumentBase(
					string compositeDocumentId, string importedDocumentBaseId,
					int=BROADCASTER_EDITING)=0;

			virtual NclDocument* addImportNCL(string compositeDocumentId, 
					string xmlImportNCL, int=BROADCASTER_EDITING)=0;

			virtual NclDocument* removeImportNCL(string compositeDocumentId, 
					string documentURI, int=BROADCASTER_EDITING)=0;

			virtual Node* addNode(
				string compositeDocumentId, string compositeId, string xmlNode,
				int=BROADCASTER_EDITING)=0;

			virtual Node* removeNode(
				string compositeDocumentId, string compositeId, string nodeId,
				int=BROADCASTER_EDITING)=0;

			virtual InterfacePoint* addInterface(
				string compositeDocumentId, string nodeId, string xmlInterface,
				int=BROADCASTER_EDITING)=0;

			virtual InterfacePoint* removeInterface(
				string compositeDocumentId, string nodeId, string interfaceId,
				int=BROADCASTER_EDITING)=0;

			virtual Link* addLink(
				string compositeDocumentId, string compositeId, string xmlLink,
				int=BROADCASTER_EDITING)=0;

			virtual Link* removeLink(
				string compositeDocumentId, string compositeId, string linkId,
				int=BROADCASTER_EDITING)=0;

			virtual bool setPropertyValue(
				string compositeDocumentId,
				string nodeId,
				string propertyId,
				string value)=0;

			/*virtual bool startDocument(string documentId, string interfaceId)=0;
			virtual bool stopDocument(string documentId)=0;
			virtual bool pauseDocument(string documentId)=0;
			virtual bool resumeDocument(string documentId)=0;
			virtual void addFormatterListener(IFormatterListener* listener)=0;
			virtual void removeFormatterListener(IFormatterListener* listener)=0;
			virtual void presentationCompleted(IFormatterEvent* documentEvent)=0;*/
	};

}
}
}
}
}
}

#endif //IFORMATTERADAPTER_H
