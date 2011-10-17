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
#ifndef _INCLGENERATOR_H
#define _INCLGENERATOR_H

#include <string>
using namespace std;

#include <ginga/ncl/NclDocument.h>
#include <ginga/ncl/Entity.h>
#include <ginga/ncl/Base.h>
#include <ginga/ncl/Parameter.h>
using namespace ::br::pucrio::telemidia::ncl;


#include <ginga/ncl/connectors/ConditionExpression.h>
#include <ginga/ncl/connectors/Action.h>
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include <ginga/ncl/link/Bind.h>
using namespace ::br::pucrio::telemidia::ncl::link;


namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {
/**
 * Interface utilizada para solicitar serviços do componente NclGenerator.
 */
class INclGenerator {

	public:
		virtual ~INclGenerator() {

		};

		/**
		 * Seta o NclDocument que servirá de referência para a geração de código.
		 * @param referDocument Instância de NclDocument que servirá como referência.
		 */
		virtual void setReferNclDocument(NclDocument* referDocument)=0;

		/**
		 * Obtém o NclDocument que está sendo usado como refência para a geração de código.
		 * @return NclDocument que esta sendo usado como refẽncia.
		 */
		virtual NclDocument* getReferNclDocument()=0;

	public:
		/**
		 * Gerá o código Xml equivalente a uma entidade Ncl representado por uma instância do componente ncl30.
		 * @param entity Entidade Ncl cujo código Xml será gerado.
		 * @return Código Xml gerado.
		 * @throw BadArgumentException Se entity == NULL.
		 * @throw InitializationException Se Documento de referência não estiver setado.
		 * @throw UnsupportedNclEntityException se a entidade não for suportada componente Ncl30Generator.
		 */
		virtual string generateXmlCode(Entity* entity)=0;

		/**
		 * Gerá o código Xml equivalente a uma conditionExpression Ncl representado por uma instância do componente ncl30.
		 * @param conditionExpression A conditionExpression Ncl cujo código Xml será gerado.
		 * @return Código Xml gerado.
		 * @throw BadArgumentException Se conditionExpression == NULL.
		 * @throw InitializationException Se Documento de referência não estiver setado.
		 * @throw UnsupportedNclEntityException se a conditionExpression não for suportada componente Ncl30Generator.
		 */
		virtual string generateXmlCode(ConditionExpression* conditionExpression)=0;

		/**
		 * Gerá o código Xml equivalente a uma bind Ncl representado por uma instância do componente ncl30.
		 * @param bind A bind Ncl cujo código Xml será gerado.
		 * @return Código Xml gerado.
		 * @throw BadArgumentException Se bind == NULL.
		 * @throw InitializationException Se Documento de referência não estiver setado.
		 */
		virtual string generateXmlCode(Bind* bind)=0;

		/**
		 * Gerá o código Xml equivalente a um Action Ncl representado por uma instância do componente ncl30.
		 * @param action A Action Ncl cujo código Xml será gerado.
		 * @return Código Xml gerado.
		 * @throw BadArgumentException Se action == NULL.
		 * @throw InitializationException Se Documento de referência não estiver setado.
		 * @throw UnsupportedNclEntityException se a action não for suportada componente Ncl30Generator.
		 */
		virtual string generateXmlCode(Action* action)=0;

		/**
		 * Gerá o código Xml equivalente a uma base Ncl representado por uma instância do componente ncl30.
		 * @param base A base Ncl cujo código Xml será gerado.
		 * @return Código Xml gerado.
		 * @throw BadArgumentException Se base == NULL.
		 * @throw InitializationException Se Documento de referência não estiver setado.
		 * @throw UnsupportedNclEntityException se a base não for suportada componente Ncl30Generator.
		 */
		virtual string generateXmlCode(Base* base)=0;

		/**
		 * Gerá o código Xml equivalente a um Documento Ncl representado por uma instância do componente ncl30.
		 * @param nclDocument O document Ncl cujo código Xml será gerado. Se omitido, será gerado o documento de referrencia.
		 * @return Código Xml gerado.
		 * @throw InitializationException Se Documento de referência não estiver setado e nclDocument == NULL; 
		 */
		virtual string generateXmlCode(NclDocument* nclDocument=NULL)=0;

		/**
		 * Gerá o código Xml equivalente a um parametro Ncl representado por uma instância do componente ncl30.
		 * @param parameter O parametro Ncl cujo código Xml será gerado.
		 * @param paramType Tipo de parametro Ncl que será gerado.
		 * @return Código Xml gerado.
		 * @throw BadArgumentException Se entity == NULL.
		 * @throw InitializationException Se Documento de referência não estiver setado.
		 * @throw UnsupportedNclEntityException se o paramType não for suportada componente Ncl30Generator.
		 */
		virtual string generateXmlCode(Parameter* parameter, string paramType)=0;

		/**
		 * Gerá um arquivo xml contendo o código equivalente a um entidade Ncl representado por uma instância do componente ncl30.
		 * @param fileName Nome do arquivo que será gerado.
		 * @param nclDoc Documento NCL que terá seu código salvo em um arquivo. Se omitido, será utilizado o documento de referência.
		 * @return true se arquivo foi gerado com sucesso; false caso contrário.
		 * @throw InitializationException Se Documento de referência não estiver setado e nclDoc == NULL.
		 */
		virtual bool generateNclFile(string fileName, NclDocument* nclDoc=NULL)=0;

};

}
}
}
}
}

typedef ::br::ufscar::lince::ncl::generator::INclGenerator* NclGeneratorCreator();

typedef void NclGeneratorDescroyer(
		::br::ufscar::lince::ncl::generator::INclGenerator* nclGenerator);

#endif //_INCLGENERATOR_H

