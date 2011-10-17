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
* @file DocumentManagerAdapter.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef _DOCUMENTMANAGERADAPTER_H_
#define _DOCUMENTMANAGERADAPTER_H_

#include "converter/DocumentManager.h"
using namespace ::br::pucrio::telemidia::converter;

#include <ginga/linceutil/LoggerUtil.h>
using namespace br::ufscar::lince::util;

#include "PrivateBaseManager.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace privateBase {

	/** Funciona como camada de adaptação com o Formatter, permitindo que ele trabalhe com bases privadas.
	 *
	 *	Esta classe foi desenvolvida para facilitar a utilização de bases privadas pelo Formatter e por
	 *	outras classes que as desejam utilizar mantendo ainda a base privada padrão utilizada na implementação
	 *	de refência do ginga versão 0.10.1.
	 *	Todos os métodos publicos da base privada são mapeados neste adaptor além de novos métodos especialmente
	 *	desenvolvidos para operar com bases privadas foram criados.
	 */
	class DocumentManagerAdapter {

		private:
			///Instância do Gerenciador de Bases Privadas
			PrivateBaseManager* pbm;

			///Instância utilizada para tratar as mensagens de log
			HLoggerPtr logger;

			//Identificador da Base Privada Padrão.
			string defaultPrivateBaseId;

			///Gerenciador de documentos da Base Privada Padrão
			DocumentManager* defaultDocumentManager;

		private:
			/**
			 * Decompõe o nome composto de uma aplicação NCL.
			 * @param compositeDocumentId Nome composto da aplicação.
			 * @param privateBaseId Argumento que conterá o nome da base privada da aplicação.
			 * @param documentId Argumento que conterá o nome do documento NCl da aplicação.
			 * @return true Se a aplicação possuía nome composto; false caso contrário.
			 */
			bool resolveCompositeDocumentId(string compositeDocumentId, 
					string& privateBaseId, string& documentId);

		public:
			/**
			 * Constrói uma instância de DocumentManagerAdapter.
			 * @param defaultPrivateBase Nome da Base Privada Padrão dos documentos.
			 */
			DocumentManagerAdapter(string defaultPrivateBase);

			/**
			 * Destrói a instância de DocumentManagerAdapter.
			 */
			~DocumentManagerAdapter();

			//Métodos adicionais criados apra manipulação de bases

			/**
			 * Adiciona um documento NCL em uma base privada.
			 * @param location String contendo o localização do arquivo NCL a ser adicionado.
			 * @param baseId Id da base privada onde o documento será adicionado.
			 * @return Uma instância que representa o documento NCL adicionado; NULL se algo der errado.
			 */
			NclDocument* addDocument(string location, string baseId);

			/**
			 * Adiciona um documento NCL visivel em uma base privada.
			 * @param location String contendo o localização do arquivo NCL a ser adicionado.
			 * @param baseId Identificador da base privada onde o documento será adicionado.
			 * @return Uma instância que representa o documento NCL adicionado; NULL se algo der errado.
			 */
			NclDocument* addVisibleDocument(string location, string baseId);

			/**
			 * Retorna todos os documentos NCL existentes em uma base privada.
			 * @param baseId Identificador da base privada.
			 * @return Instânciad e vector contento todos os documentos NCL da base.
			 */
			vector<NclDocument*>* getDocuments(string baseId);

			/**
			 * Remove todos os documentos NCL de uma base privada.
			 * @param baseId Identificador da base privada.
			 */
			void clear(string baseId);

			//Métodos "herdados" para manipulação de documentos

			/**
			 * Adiciona um documento NCL à base privada padrão.
			 * @param location String contendo o localização do arquivo NCL a ser adicionado.
			 * @return Uma instância que representa o documento NCL adicionado; NULL se algo der errado.
			 */
			NclDocument* addDocument(string location);

			/**
			 * Adiciona um documento NCL visivel à base privada padrão.
			 * @param location String contendo o localização do arquivo NCL a ser adicionado.
			 * @return Uma instância que representa o documento NCL adicionado; NULL se algo der errado.
			 */
			NclDocument* addVisibleDocument(string location);

			/**
			 * Obtém a localização do arquivo-fonte de uma aplicação NCL.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @return String contendo  a localização do documento NCL.
			 */
			string getDocumentLocation(string compositeDocId);

			/**
			 * Obtém a representação a nível de objeto de uma aplicação NCL.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @return A instância que represente a aplicação, se ela existir; NULL caso contrário.
			 */
			NclDocument* getDocument(string compositeDocId);

			/**
			 * Retorna todos os documentos NCL existentes na base privada padrão.
			 * @return Instânciad e vector contento todos os documentos NCL da base.
			 */
			vector<NclDocument*>* getDocuments();

			/**
			 * Remove um documento NCL das bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @return A instância que represente a aplicação, se ela foi removida; NULL caso contrário.
			 */
			NclDocument* removeDocument(string compositeDocId);


			/**
			 * Remove todos os documentos NCL da base privada padrão.
			 */
			void clear();

			//Métodos necessárioas para edição ao vivo para edição ao vivo de documentos

			/**
			 * Adiciona uma Região a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param regionId Região onde a nova região será adicionada.
			 * @param xmlRegion Código XML da nova região adicionada.
			 * @return Instância que representa a região adiciona, se ela for adicionada; NULL caso contrário.
			 */
			LayoutRegion* addRegion(
				     string compositeDocId, string regionId, string xmlRegion);

			/**
			 * Remove uma Região de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param regionId Nome composto da Região que será removida.
			 * @return Instância que representava a região removida, se ela for removida; NULL caso contrário.
			 */
			LayoutRegion* removeRegion(string compositeDocId, string regionId);

			/**
			 * Adiciona uma Base de Regiões a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlRegionBase Código XML da nova base de regiões adicionada.
			 * @return Instância que representa a base de regiões adiciona, se ela for adicionada; NULL caso contrário.
			 */
			RegionBase* addRegionBase(string compositeDocId, string xmlRegionBase);

			/**
			 * Remove uma Base de Regiões de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param regionBaseId Nome da Base de Regiões que será removida.
			 * @return Instância que representava a Base de Regiões removida, se ela for removida; NULL caso contrário.
			 */
			RegionBase* removeRegionBase(
				    string compositeDocId, string regionBaseId);

			/**
			 * Adiciona uma Regra a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlRule Código XML da nova regra adicionada.
			 * @return Instância que representa a regra adiciona, se ela for adicionada; NULL caso contrário.
			 */
			Rule* addRule(string compositeDocId, string xmlRule);

			/**
			 * Remove uma Regra de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param ruleId Nome da Regra que será removida.
			 * @return Instância que representava a Regra removida, se ela for removida; NULL caso contrário.
			 */
			Rule* removeRule(string compositeDocId, string ruleId);

			/**
			 * Adiciona uma Base de Regras a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlRuleBase Código XML da nova base de regras adicionada.
			 * @return Instância que representa a base de regras adiciona, se ela for adicionada; NULL caso contrário.
			 */
			RuleBase* addRuleBase(string compositeDocId, string xmlRuleBase);

			/**
			 * Remove uma Base de Regras de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param ruleBaseId Nome da Base de Regras que será removida.
			 * @return Instância que representava a Base de Regras removida, se ela for removida; NULL caso contrário.
			 */
			RuleBase* removeRuleBase(string compositeDocId, string ruleBaseId);

			/**
			 * Adiciona uma Transição a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlTransition Código XML da nova transição adicionada.
			 * @return Instância que representa a transição adiciona, se ela for adicionada; NULL caso contrário.
			 */
			Transition* addTransition(string compositeDocId, string xmlTransition);

			/**
			 * Remove uma Transição de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param transitionId Nome da Transição que será removida.
			 * @return Instância que representava a Transição removida, se ela for removida; NULL caso contrário.
			 */
			Transition* removeTransition(string compositeDocId, string transitionId);

			/**
			 * Adiciona uma Base de Transições a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlTransitionBase Código XML da nova base de transições adicionada.
			 * @return Instância que representa a base de transições adiciona, se ela for adicionada; NULL caso contrário.
			 */
			TransitionBase* addTransitionBase(
				    string compositeDocId, string xmlTransitionBase);

			/**
			 * Remove uma Base de Transições de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param transitionBaseId Nome da Base de Transições  que será removida.
			 * @return Instância que representava a Base de Transições removida, se ela for removida; NULL caso contrário.
			 */
			TransitionBase* removeTransitionBase(
				    string compositeDocId, string transitionBaseId);

			/**
			 * Adiciona um Conector a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlTransitionBase Código XML do novo conector adicionado.
			 * @return Instância que representa o conector adiciono, se ele for adicionado; NULL caso contrário.
			 */
			Connector* addConnector(string compositeDocId, string xmlConnector);

			/**
			 * Remove um Conector de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param connectorId Nome do Conector que será removido.
			 * @return Instância que representava o Conector removido, se ele for removido; NULL caso contrário.
			 */
			Connector* removeConnector(string compositeDocId, string connectorId);

			/**
			 * Adiciona uma Base de Conectores a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlConnectorBase Código XML da nova Base de Conectores adicionada.
			 * @return Instância que representa a Base de Conectores adiciona, se ela for adicionada; NULL caso contrário.
			 */
			ConnectorBase* addConnectorBase(
				    string compositeDocId, string xmlConnectorBase);

			/**
			 * Remove uma Base de Conectores de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param connectorBaseId Nome da Base de Conectores que será removida.
			 * @return Instância que representava a Base de Conectores removida, se ela for removida; NULL caso contrário.
			 */
			ConnectorBase* removeConnectorBase(
				    string compositeDocId, string connectorBaseId);

			/**
			 * Adiciona um Descritor a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlDescriptor Código XML do novo descritor adicionado.
			 * @return Instância que representa o Descritor adiciono, se ele for adicionado; NULL caso contrário.
			 */
			GenericDescriptor* addDescriptor(
				    string compositeDocId, string xmlDescriptor);

			/**
			 * Remove um Descritor de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param descriptorId Nome do Descritor que será removido.
			 * @return Instância que representava o Descritor removido, se ele for removido; NULL caso contrário.
			 */
			GenericDescriptor* removeDescriptor(
				    string compositeDocId, string descriptorId);

			/**
			 * Adiciona uma Base de Descritores a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlDescriptorBase Código XML da nova Base de Descritores adicionada.
			 * @return Instância que representa a Base de Descritores adiciona, se ela for adicionada; NULL caso contrário.
			 */
			DescriptorBase* addDescriptorBase(
				    string compositeDocId, string xmlDescriptorBase);

			/**
			 * Remove uma Base de Descritores de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param descriptorBaseId Nome da Base de Descritores que será removida.
			 * @return Instância que representava a Base de Descritores removida, se ele for removida; NULL caso contrário.
			 */
			DescriptorBase* removeDescriptorBase(
				    string compositeDocId, string descriptorBaseId);

			/**
			 * Adiciona uma Base de importação a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param docBaseId Nome que será dado a esta Base dentro da Aplicação NCL.
			 * @param xmlImportBase Código XML da nova Base de importação adicionada.
			 * @return Instância que representa a Base de importação adiciona, se ela for adicionada; NULL caso contrário.
			 */
			Base* addImportBase(
				    string compositeDocId, string docBaseId, string xmlImportBase);

			/**
			 * Remove uma Base de importação de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param docBaseId Nome da base dentro do documento NCL.
			 * @param documentURI URI do documento de onde a base foi adicionada.
			 * @return Instância que representava a Base de importação removida, se ele for removida; NULL caso contrário.
			 */
			Base* removeImportBase(
				    string compositeDocId, string docBaseId, string documentURI);
			/**
			 * Adiciona uma Base importada de outro documento a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlImportedDocumentBase Código XML da nova Base importada de outro documento adicionada.
			 * @return Instância que representa a Base importada de outro documento adiciona, se ela for adicionada; NULL caso contrário.
			 */
			NclDocument* addImportedDocumentBase(
				    string compositeDocId, string xmlImportedDocumentBase);

			/**
			 * Remove uma Base importada de outro documento de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param importedDocumentBaseId Nome da Base importada de outro documento que será removida.
			 * @return Instância que representava a Base importada de outro documento removida, se ela for removida; NULL caso contrário.
			 */
			NclDocument* removeImportedDocumentBase(
				    string compositeDocId, string importedDocumentBaseId);

			/**
			 * Adiciona um Documento NCL importado a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param xmlImportNCL Código XML do novo Documento NCL importado adicionado.
			 * @return Instância que representa o documento NCL importado adiciono, se ele for adicionado; NULL caso contrário.
			 */
			NclDocument* addImportNCL(string compositeDocId, string xmlImportNCL);

			/**
			 * Remove uma Documento NCL importado de uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param documentURI URI do Documento NCL importado será removido.
			 * @return Instância que representava o Documento NCL importado, se ele for removido; NULL caso contrário.
			 */
			NclDocument* removeImportNCL(string compositeDocId, string documentURI);

			/**
			 * Adiciona um Nó a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param compositeId Nome composto do nó de contexto onde este nó será adicionado.
			 * @param xmlNode Código XML da novo nó adicionado.
			 * @return Instância que representa o nó adiciono, se ele for adicionado; NULL caso contrário.
			 */
			Node* addNode(string compositeDocId, string compositeId, string xmlNode);

			/**
			 * Adiciona uma nova Interface a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param nodeId Nome composto do nó onde esta interface será adicionada.
			 * @param xmlInterface Código XML da nova interface adicionada.
			 * @return Instância que representa a interface adicionada, se ela for adicionada; NULL caso contrário.
			 */
			InterfacePoint* addInterface(
				    string compositeDocId, string nodeId, string xmlInterface);

			/**
			 * Adiciona um Elo a uma aplicação NCL existente nas bases privadas.
			 * @param compositeDocId Nome composto da aplicação NCL.
			 * @param compositeId Nome composto do nó de contexto onde este link será adicionado.
			 * @param xmlLink Código XML da novo elo adicionado.
			 * @return Instância que representa o elo, se ele for adicionado; NULL caso contrário.
			 */
			Link* addLink(string compositeDocId, string compositeId, string xmlLink);

			//Manipulação de base privada

			/**
			 * Retorna o Contexto da Base Privada Padrão.
			 * @return Contexto da Base Privada Padrão.
			 **/
			PrivateBase* getPrivateBaseContext();

			/**
			 * Retorna o Contexto de uma Base Privada.
			 * @param baseId Identificador da Base Privada.
			 * @return Contexto da Base Privada.
			 **/
			PrivateBase* getPrivateBaseContext(string baseId);
	};

}
}
}
}
}

#endif //_DOCUMENTMANAGERADAPTER_H_
