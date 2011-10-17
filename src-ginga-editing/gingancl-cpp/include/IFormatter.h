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

#ifndef _IFORMATTER_H_
#define _IFORMATTER_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "adapters/INclEditListener.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "IFormatterListener.h"
#include "IFormatterSchedulerListener.h"

#include <map>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
	class IFormatter : public IFormatterSchedulerListener,
			public INclEditListener {

		public:
			static const int DEEPEST_LEVEL = -1;

			virtual ~IFormatter(){};
			virtual void* addDocument(string docLocation)=0;
			//virtual INclDocument* addDocument(string docLocation)=0;
			virtual bool removeDocument(string documentId)=0;
			virtual void setDepthLevel(int level)=0;
			virtual int getDepthLevel()=0;
			virtual bool startDocument(string documentId, string interfaceId)=0;
			virtual bool stopDocument(string documentId)=0;
			virtual bool pauseDocument(string documentId)=0;
			virtual bool resumeDocument(string documentId)=0;
			virtual void addFormatterListener(IFormatterListener* listener)=0;
			virtual void removeFormatterListener(
					IFormatterListener* listener)=0;

			virtual void presentationCompleted(
					IFormatterEvent* documentEvent)=0;

			virtual bool nclEdit(string editingCommand)=0;

/*			LayoutRegion* addRegion(
				    string documentId, string regionId, string xmlRegion);

			LayoutRegion* removeRegion(string documentId, string regionId);
			RegionBase* addRegionBase(string documentId, string xmlRegionBase);
			RegionBase* removeRegionBase(
				    string documentId, string regionBaseId);

			Rule* addRule(string documentId, string xmlRule);
			Rule* removeRule(string documentId, string ruleId);
			RuleBase* addRuleBase(string documentId, string xmlRuleBase);
			RuleBase* removeRuleBase(string documentId, string ruleBaseId);
			Transition* addTransition(string documentId, string xmlTransition);
			Transition* removeTransition(
				    string documentId, string transitionId);

			TransitionBase* addTransitionBase(
				    string documentId, string xmlTransitionBase);

			TransitionBase* removeTransitionBase(
				    string documentId, string transitionBaseId);

			Connector* addConnector(string documentId, string xmlConnector);
			Connector* removeConnector(string documentId, string connectorId);
			ConnectorBase* addConnectorBase(
				    string documentId, string xmlConnectorBase);

			ConnectorBase* removeConnectorBase(
				    string documentId, string connectorBaseId);

			GenericDescriptor* addDescriptor(
				    string documentId, string xmlDescriptor);

			GenericDescriptor* removeDescriptor(
				    string documentId, string descriptorId);

			DescriptorBase* addDescriptorBase(
				    string documentId, string xmlDescriptorBase);

			DescriptorBase* removeDescriptorBase(
				    string documentId, string descriptorBaseId);

			Base* addImportBase(
				    string documentId, string docBaseId, string xmlImportBase);

			Base* removeImportBase(
				    string documentId, string docBaseId, string documentURI);

			NclDocument* addImportedDocumentBase(
				    string documentId, string xmlImportedDocumentBase);

			NclDocument* removeImportedDocumentBase(
				    string documentId, string importedDocumentBaseId);

			NclDocument* addImportNCL(string documentId, string xmlImportNCL);
			NclDocument* removeImportNCL(string documentId, string documentURI);

			Node* addNode(
				    string documentId, string compositeId, string xmlNode);

			Node* removeNode(
				    string documentId, string compositeId, string nodeId);

			InterfacePoint* addInterface(
				    string documentId, string nodeId, string xmlInterface);

			InterfacePoint* removeInterface(
				    string documentId, string nodeId, string interfaceId);

			Link* addLink(
				    string documentId, string compositeId, string xmlLink);

			Link* removeLink(
				    string documentId, string compositeId, string linkId);*/

			virtual bool setPropertyValue(
				    string documentId,
				    string nodeId,
				    string propertyId,
				    string value)=0;

			virtual void pushEPGEventToEPGFactory(map<string, string> t)=0;

	};
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::ncl::IFormatter* FormatterCreator(
		string id);

typedef void FormatterDescroyer(
		::br::pucrio::telemidia::ginga::ncl::IFormatter* f);

#endif //_IFORMATTER_H_
