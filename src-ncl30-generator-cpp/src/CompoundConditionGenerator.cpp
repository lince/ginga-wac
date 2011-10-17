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
* @file CompoundConditionGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/CompoundConditionGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {

    string CompoundConditionGenerator::generateCode() {
        string ret = "<compoundCondition operator=\"";
        short op = this->getOperator();
        if (op == OP_AND) {
            ret+= "and\" ";
        }
        if (op == OP_OR) {
            ret+= "or\" ";
        }
        string aux = this->getDelay();
        if (aux != "0") {
            ret += "delay=\"" + aux + "\" ";
        }
        ret+= ">\n";

		vector<ConditionExpression*>* conditions = this->getConditions();
		vector<ConditionExpression*>::iterator it;
		it = conditions->begin();
		while (it != conditions->end()) {
			ConditionExpression* condition = *it;
            if (condition->instanceOf("SimpleCondition")) {
                ret+= static_cast<SimpleConditionGenerator*>(condition)->generateCode() + "\n";
            } else  if (condition->instanceOf("CompoundCondition")) {
                ret+= static_cast<CompoundConditionGenerator*>(condition)->generateCode() + "\n";
            } else if (condition->instanceOf("AssessmentStatement")) {
                ret+= static_cast<AssessmentStatementGenerator*>(condition)->generateCode() + "\n";
            } else if (condition->instanceOf("CompoundStatement")) {
                ret+= static_cast<CompoundStatementGenerator*>(condition)->generateCode() + "\n";
            }
			it++;
		}

        ret+= "</compoundCondition>";
        return ret;
    }

}
}
}
}
}

