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

#ifndef _FORMATTER_H_
#define _FORMATTER_H_

/****************************/
//#ifdef linux
	#define SOFLAG 1
//#endif

#ifdef WIN32
	#define SOFLAG 2
#endif

/****************************/

#include "converter/DocumentManager.h"
using namespace ::br::pucrio::telemidia::converter;

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

#include "model/CompositeExecutionObject.h"
#include "model/ExecutionObject.h"
#include "model/NodeNesting.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "model/FormatterEvent.h"
#include "model/AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "model/LinkActionListener.h"
#include "model/LinkAssignmentAction.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "adaptation/context/RuleAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "emconverter/ObjectCreationForbiddenException.h"
#include "emconverter/FormatterConverter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::emconverter;

#include "../include/prefetch/IPrefetchManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::prefetch;

//#if HAVE_WAC
#include "wac/editing/ClientEditingManager.h"
#include "wac/editing/IFormatterAdapter.h"
#include "wac/editing/ISchedulerAdapter.h"
using namespace ::br::ufscar::lince::ginga::wac::editing;
//#endif

//#if HAVE_PRIVATEBASE
#include "ncl/privatebase/PrivateBaseManager.h"
#include "ncl/privatebase/DocumentManagerAdapter.h"
using namespace ::br::ufscar::lince::ncl::privateBase;
//#endif

#include "FormatterScheduler.h"

#include "IFormatter.h"

#include <map>
#include <set>
#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
	class Formatter : public IFormatter 
//#if HAVE_WAC
		, public IFormatterAdapter
//#endif
											{


		private:
//#if HAVE_PRIVATEBASE
			PrivateBaseManager* privateBaseManager;
			DocumentManagerAdapter* documentManager;
			string clientPrivateBaseId;
			string broadcasterPrivateBaseId;
//#else 
//			DocumentManager* documentManager;
//#endif
			map<string, FormatterEvent*>* documentEvents;
			map<string, vector<FormatterEvent*>*>* documentEntryEvents;
			RuleAdapter* ruleAdapter;
			FormatterScheduler* scheduler;
			PlayerAdapterManager* playerManager;
			FormatterConverter* compiler;
			vector<string>* compileErrors;
			set<IFormatterListener*>* formatterListeners;
			IPrefetchManager* pm;

		public:
			Formatter(string id);
			virtual ~Formatter();

		private:
			void initializeContextProxy();

		public:
//			Container* compileNclDocument(string docLocation);
			vector<string> *getCompileErrors();
//			bool compile(Container* container);
			//NclDocument* addDocument(string docLocation);
			void* addDocument(string docLocation);
			bool removeDocument(string documentId);

//#if HAVE_PRIVATEBASE
			void* addDocument(string docLocation, string privateBaseId);
//#endif //HAVE_PRIVATEBASE

		private:
			ContextNode* getDocumentContext(string documentId);

		public:
			void setDepthLevel(int level);
			int getDepthLevel();

		private:
			vector<FormatterEvent*>* processDocument(
				    string documentId, string interfaceId);

			vector<FormatterEvent*>* getDocumentEntryEvent(string documentId);
			bool compileDocument(string documentId, string interfaceId);
			bool prepareDocument(string documentId);

			void solveRemoteDescriptorsUris(string docLocation,
						vector<GenericDescriptor*>* descs, bool isRemoteDoc);

			void solveRemoteNodesUris(
					string docLocation, vector<Node*>* nodes, bool isRemoteDoc);

			void solveRemoteNclDeps(string docLocation, bool isRemoteDoc);

			void solveRemoteLuaDeps(
					string docLocation, string src, bool isRemoteDoc);

			string solveRemoteSourceUri(string docLocation, string src);

		public:
			bool startDocument(string documentId, string interfaceId);
			bool stopDocument(string documentId);
			bool pauseDocument(string documentId);
			bool resumeDocument(string documentId);
			void addFormatterListener(IFormatterListener* listener);
			void removeFormatterListener(IFormatterListener* listener);
			void presentationCompleted(IFormatterEvent* documentEvent);

			bool nclEdit(string editingCommand);

//#IF HAVE_WAC
			string adaptDocumentId(string documentId);

		public:
			LayoutRegion* addRegion(
				    string compositeDocumentId, string regionId, string xmlRegion,
						int=BROADCASTER_EDITING);

			LayoutRegion* removeRegion(string compositeDocumentId, string regionId,
						int=BROADCASTER_EDITING);

			RegionBase* addRegionBase(string compositeDocumentId, string xmlRegionBase,
						int=BROADCASTER_EDITING);

			RegionBase* removeRegionBase(string compositeDocumentId, string regionBaseId,
					int=BROADCASTER_EDITING);

			Rule* addRule(string compositeDocumentId, string xmlRule,
					int=BROADCASTER_EDITING);

			Rule* removeRule(string compositeDocumentId, string ruleId,
					int=BROADCASTER_EDITING);

			RuleBase* addRuleBase(string compositeDocumentId, string xmlRuleBase,
					int=BROADCASTER_EDITING);

			RuleBase* removeRuleBase(string compositeDocumentId, string ruleBaseId,
					int=BROADCASTER_EDITING);

			Transition* addTransition(string compositeDocumentId, string xmlTransition,
					int=BROADCASTER_EDITING);

			Transition* removeTransition(string compositeDocumentId, string transitionId,
					int=BROADCASTER_EDITING);

			TransitionBase* addTransitionBase(
					string compositeDocumentId, string xmlTransitionBase,
					int=BROADCASTER_EDITING);

			TransitionBase* removeTransitionBase(string documentId, 
					string transitionBaseId,
					int=BROADCASTER_EDITING);

			Connector* addConnector(string compositeDocumentId, string xmlConnector,
					int=BROADCASTER_EDITING);

			Connector* removeConnector(string compositeDocumentId, string connectorId,
					int=BROADCASTER_EDITING);

			ConnectorBase* addConnectorBase(
					string compositeDocumentId, string xmlConnectorBase,
					int=BROADCASTER_EDITING);

			ConnectorBase* removeConnectorBase(
					string compositeDocumentId, string connectorBaseId,
					int=BROADCASTER_EDITING);

			GenericDescriptor* addDescriptor(
				    	string compositeDocumentId, string xmlDescriptor,
				    	int=BROADCASTER_EDITING);

			GenericDescriptor* removeDescriptor(
				   	string compositeDocumentId, string descriptorId,
				    	int=BROADCASTER_EDITING);

			DescriptorBase* addDescriptorBase(
				    	string compositeDocumentId, string xmlDescriptorBase,
					int=BROADCASTER_EDITING);

			DescriptorBase* removeDescriptorBase(
				    	string compositeDocumentId, string descriptorBaseId,
					int=BROADCASTER_EDITING);

			Base* addImportBase(string compositeDocumentId, 
					string docBaseId, string xmlImportBase,
					int=BROADCASTER_EDITING);

			Base* removeImportBase(string compositeDocumentId, 
					string docBaseId, string documentURI,
					int=BROADCASTER_EDITING);

			NclDocument* addImportedDocumentBase(string compositeDocumentId, 
					string xmlImportedDocumentBase,
					int=BROADCASTER_EDITING);

			NclDocument* removeImportedDocumentBase(
				    	string compositeDocumentId, string importedDocumentBaseId,
					int=BROADCASTER_EDITING);

			NclDocument* addImportNCL(string compositeDocumentId, string xmlImportNCL,
					int=BROADCASTER_EDITING);

			NclDocument* removeImportNCL(string compositeDocumentId, string documentURI,
					int=BROADCASTER_EDITING);

//#endif //HAVE_WAC

		private:
			void processInsertedReferNode(ReferNode* referNode);
			void processInsertedComposition(CompositeNode* composition);

		public:
//#if HAVE_WAC
			Node* addNode(
				string compositeDocumentId, string compositeId, string xmlNode,
				int=BROADCASTER_EDITING);

			Node* removeNode(
				string compositeDocumentId, string compositeId, string nodeIdm,
				int=BROADCASTER_EDITING);

			InterfacePoint* addInterface(
				string compositeDocumentId, string nodeId, string xmlInterface,
				int=BROADCASTER_EDITING);
//#endif

		private:
			void removeInterfaceMappings(
				    Node* node,
				    InterfacePoint* interfacePoint,
				    CompositeNode* composition);

			void removeInterfaceLinks(
				    Node* node,
				    InterfacePoint* interfacePoint,
				    LinkComposition* composition);

			void removeInterface(
				    Node* node, InterfacePoint* interfacePoint);

		public:
//#if HAVE_WAC
			InterfacePoint* removeInterface(
				string compositeDocumentId, string nodeId, string interfaceId,
				int=BROADCASTER_EDITING);

			Link* addLink(
				string compositeDocumentId, string compositeId, string xmlLink,
				int=BROADCASTER_EDITING);
//#endif

		private:
			void removeLink(LinkComposition* composition, Link* link);

		public:
//#if HAVE_WAC
			Link* removeLink(
				string compositeDocumentId, string compositeId, string linkId,
				int=BROADCASTER_EDITING);
//#endif

			bool setPropertyValue(
				    string documentId,
				    string nodeId,
				    string propertyId,
				    string value);

			void pushEPGEventToEPGFactory(map<string, string> t);
	};
}
}
}
}
}

#endif //_FORMATTER_H_
