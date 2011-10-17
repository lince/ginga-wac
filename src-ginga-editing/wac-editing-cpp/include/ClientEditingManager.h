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
* @file ClientEditingManager.h
* @author Caio Viel
* @date 29-01-10
*/

#ifndef CLIENTEDITINGMANAGER_H
#define CLIENTEDITINGMANAGER_H

#include <list>
#include <string>
#include <cstdlib>
#include <map>

#include "ncl/generator/NclGenerator.h"
#include "ncl/generator/NclDocumentGenerator.h"
#include "ncl/generator/NclFileGenerator.h"
using namespace ::br::ufscar::lince::ncl::generate;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "IClientEditing.h"
#include "EditingCommand.h"
#include "IFormatterAdapter.h"
#include "ISchedulerAdapter.h"

#include <ginga/linceutil/LoggerUtil.h>
using namespace br::ufscar::lince::util;

using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace editing {

/**
 * Permite a aplicações desenvolvidas pelo usuário realizar anotações e edições ao vivo em um documento NCL
 * Esta classe basicamente facilita a criação de aplicações de anotação e edição ao vivo do lado cliente, pois
 * faz parte das tarefas necessárias para aplicações deste tipo, além de permitir retroceder de alterações erroneas
 * e fazer um log das alterações.
 */

class ClientEditingManager : public IClientEditing {
	protected:
		/// Instância única de ClientEditingManager.
		static ClientEditingManager* _instance;

		/// Interface utilizada para comunicação com o Formatter do módulo Formatter.
		IFormatterAdapter* formatter;

		/// Lista que contém todas os comandos de edição enviados por aplicações cliente.
		list<EditingCommand*>* editingCommands;

		/// Mapa que relaciona os comandos de edições com as Ids das entidades NCL adicionadas ou removidas.
		map<string, EditingCommand*>* mapCommands;

		/// Instância do documento NCL atualmente em execução.
		NclDocument* currentDocument;

		/// Nome da Base Privada dos documentos do Cliente.
		string clientPrivateBaseId;

		/// Nome da Base Privada dos documentos da emissora.
		string broadcasterPrivateBaseId;

		/// Caminho onde esta gravado o documento NCL atualmente em execução.
		string documentPath;

	private:

		/// Instância responsável por exibir e tratar logs e mensagens de erro.
		HLoggerPtr logger;

		/**
		 * Constrói a instância única de ClientEditingManager.
		 */
		ClientEditingManager();
		
	public:
		/**
		 * Permite acessar a instância única de ClientEditingManager.
		 * @return Instância única de ClientEditingManager.
		 */
		static ClientEditingManager* getInstance();
		
		/**
		 * Destrói a instância única de ClientEditingManager
		 */
		~ClientEditingManager();		
		
		/**
		 * Adiciona uma Região na Versão Cliente do Documento NCL em execução.
		 * @param regionId Identificador da Região onde a nova Região será adiconada.
		 * @param layoutRegion Instância que representa a Região que será adiconada.
		 * @return True se a região foi adicionada; False caso contrário.
		 */
		bool addRegion(string regionId, 
			LayoutRegion* layoutRegion);

		/**
		 * Adiciona uma Região na Versão Cliente do Documento NCL em execução.
		 * @param regionId Identificador da Região onde a nova Região será adiconada.
		 * @param xml Código xml da Região que será adiconada.
		 * @return True se a região foi adicionada; False caso contrário.
		 */
		bool addRegion(string regionId, 
			string xml);

		/**
		 * Adiciona uma Regra na Versão Cliente do Documento NCL em execução.
		 * @param rule Instância que representa a Regra que será adiconada.
		 * @return True se a regra foi adicionada; False caso contrário.
		 */
		bool addRule(Rule* rule);

		/**
		 * Adiciona uma Regra na Versão Cliente do Documento NCL em execução.
		 * @param xml Código xml da Regra que será adiconada.
		 * @return True se a regra foi adicionada; False caso contrário.
		 */
		bool addRule(string xml);

		/**
		 * Remove uma regra da Versão Cliente do Documento NCL em execução.
		 * @param ruleId Identificador da Regra que será removida.
		 * @return True se a regra for removida; False caso contrário.
		 */
		bool removeRule(string ruleId);

		/**
		 * Adiciona uma transição na Versão Cliente do Documento NCL em execução.
		 * @param transition Instância que representa a transição que será adiconada.
		 * @return True se a transição foi adicionada; False caso contrário.
		 */
		bool addTransition(Transition* transition);

		/**
		 * Adiciona uma transição na Versão Cliente do Documento NCL em execução.
		 * @param xml Código xml da transição que será adiconada.
		 * @return True se a transição foi adicionada; False caso contrário.
		 */
		bool addTransition(string xml);

		/**
		 * Remove uma transição da Versão Cliente do Documento NCL em execução.
		 * @param transitionId Identificador da transição que será removida.
		 * @return True se a transição for removida; False caso contrário.
		 */
		bool removeTransition(string transitionId);

		/**
		 * Adiciona um Conector na Versão Cliente do Documento NCL em execução.
		 * @param connector Instância que representa o conector que será adiconado.
		 * @return True se o conector foi adicionado; False caso contrário.
		 */
		bool addConnector(Connector* connector);

		/**
		 * Adiciona um Conector na Versão Cliente do Documento NCL em execução.
		 * @param xml Código xml do conector que será adiconado.
		 * @return True se o conector foi adicionado; False caso contrário.
		 */
		bool addConnector(string xml);

		/**
		 * Remove um conector da Versão Cliente do Documento NCL em execução.
		 * @param connectorId Identificador do conector que será removido.
		 * @return True se o conector for removido; False caso contrário.
		 */
		bool removeConnector(string connectorId);

		/**
		 * Adiciona um Descritor na Versão Cliente do Documento NCL em execução.
		 * @param descriptor Instância que representa o Descritor que será adiconado.
		 * @return True se o Descritor foi adicionado; False caso contrário.
		 */
		bool addDescriptor(GenericDescriptor* descriptor);

		/**
		 * Adiciona um Descritor na Versão Cliente do Documento NCL em execução.
		 * @param xml Código xml do Descritor que será adiconado.
		 * @return True se o Descritor foi adicionado; False caso contrário.
		 */
		bool addDescriptor(string xml);

		/**
		 * Remove um descritor da Versão Cliente do Documento NCL em execução.
		 * @param descriptorId Identificador do descritor que será removido.
		 * @return True se o descritor for removido; False caso contrário.
		 */
		bool removeDescriptor(string descriptorId);

		/**
		 * Adiciona um Nó à Versão Cliente do Documento NCL em execução.
		 * @param compositeId Idenitificador do nó onde este novo nó será adicionado.
		 * @param node Instância que representa o nó que será adiconado.
		 * @return True se o nó foi adicionado; False caso contrário.
		 */
		bool addNode(string compositeId, Node* node);

		/**
		 * Adiciona um Nó à Versão Cliente do Documento NCL em execução.
		 * @param compositeId Idenitificador do nó onde este novo nó será adicionado.
		 * @param xml Código xml do nó que será adiconado.
		 * @return True se o nó foi adicionado; False caso contrário.
		 */
		bool addNode(string compositeId, string xml);

		/**
		 * Adiciona uma Interface à Versão Cliente do Documento NCL em execução.
		 * @param nodeId Idenitificador do nó onde esta nova interface será adicionada.
		 * @param interface Instância que representa a interface que será adiconada.
		 * @return True se a interface foi adicionada; False caso contrário.
		 */
		bool addInterface(string nodeId, 
			InterfacePoint* interface);

		/**
		 * Adiciona uma Interface à Versão Cliente do Documento NCL em execução.
		 * @param nodeId Idenitificador do nó onde esta nova interface será adicionada.
		 * @param xml Código xml da interface que será adiconada.
		 * @return True se a interface foi adicionada; False caso contrário.
		 */
		bool addInterface(string nodeId, 
			string xml);

		/**
		 * Remove uma interface da Versão Cliente do Documento NCL em execução.
		 * @param nodeId Idenitificador do nó de onde esta interface será removida.
		 * @param descriptorId Identificador do descritor que será removido.
		 * @return True se o descritor for removido; False caso contrário.
		 */
		bool removeInterface(string nodeId, 
			string interfaceId);

		/**
		 * Adiciona um elo à Versão Cliente do Documento NCL em execução.
		 * @param compositeId Idenitificador do nó de contexto onde este novo elo será adicionado.
		 * @param link Instancia que representa o elo que será adiconado.
		 * @return True se o elo foi adicionado; False caso contrário.
		 */
		bool addLink(string compositeId, Link* link);

		/**
		 * Adiciona um elo à Versão Cliente do Documento NCL em execução.
		 * @param compositeId Idenitificador do nó de contexto onde este novo elo será adicionado.
		 * @param xml Código xml do elo que será adiconado.
		 * @return True se o elo foi adicionado; False caso contrário.
		 */
		bool addLink(string compositeId, string xml);

		/**
		 * Remove um elo da Versão Cliente do Documento NCL em execução.
		 * @param nodeId Idenitificador do nó de onde este elo será removido.
		 * @param descriptorId Identificador do elo que será removido.
		 * @return True se o elo for removido; False caso contrário.
		 */
		bool removeLink(string compositeId, string linkId);

		/**
		 * Retorno a instância que representa o documento NCl que esta sendo apresentado.
		 * @return Documento NCL que está sendo apresentado.
		 */
		NclDocument* getCurrentDocument();

		/**
		 * Permite desfazer as ultimas edições realizadas por aplicações do cliente.
		 * @param n Número de edições que serão desfeitas. Se omitido será desfeita uma edição.
		 * @return True se as alterações foram desfeitas com sucesso; False Caso contrário.
		 */
		bool undoLastEditing(int n=1);

		/**
		 * Método utilizado para setar o documento ncl atualmente em execução.
		 * @param nclDocument O documento NCL atualmente em execução.
		 */
		void setCurrentDocument(NclDocument* nclDocument);

		/**
		 * Seta o nome da base privada do cliente.
		 * @param id Nome da base privada do cliente.
		 */
		void setClientPrivateBaseId(string id);

		/**
		 * Seta o nome da base privada da emissora.
		 * @param id Nome da base privada da emissora.
		 */
		void setBroadcasterPrivateBaseId(string id);

		/**
		 * Seta a instância do formatter que irá receber os comandos de edição ao vivo.
		 * @param formatter Instância de Formatter que receberá os comandos de edição ao vivo.
		 */
		void setIFormatterAdpater(IFormatterAdapter* formatter);

		/**
		 * Salva o Versão Cliente do documento NCL atualmente em execução em um arquivo.
		 * @param fileName Nome do arquivo que será gerado.
		 * @return True se o arquivo foi criado com sucesso; false caso contrário.
		 */
		bool saveCurrentFile(string fileName);

		/**
		 * Retorna o caminho do documento NCL atual.
		 * @return Uma String contendo o caminho do documento NCL atual.
		 */
		string getCurrentDocumentPath();

		/// Prefixo anexado aos identificadores das entidades NCL adicionadas pelo cliente.
		static const string CLIENT_LINK_PREFIX;
		
};

}
}
}
}
}
}

#endif //CLIENTEDITINGMANAGER_H
