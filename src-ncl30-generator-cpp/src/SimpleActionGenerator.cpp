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
* @file SimpleActionGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/SimpleActionGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {
    
    string stringActionType(short actionType);
  
    string SimpleActionGenerator::generateCode() {
		string ret ="<simpleAction role=\"";
        ret += this->getLabel() + "\" ";
        string aux = this->getDelay();
        if (aux != "0") {
            ret += "delay=\"" + aux + "\" "; 
        }
        ret += "eventType=\"" + EventUtil::getTypeName(this->getEventType()) + "\" ";
		ret += "actionType=\"" + stringActionType(this->getActionType()) + "\" ";


        aux = this->getValue();
        if (aux != "") {
             ret += "value=\"" + aux + "\" ";
        }
        
        int min = this->getMinCon();
        if (min != 1) {
            if (min == Role::UNBOUNDED) {
                ret += "min=\"unbounded\" ";
            } else {
                ret+= "min=\"" + itos(min) + "\" ";
            }
        }
        int max = this->getMaxCon();
        if (max != 1) {
            if (max ==  Role::UNBOUNDED) {
                ret += "max=\"unbounded\" ";
            } else {
                ret+= "max=\"" + itos(max) + "\" ";
            }
        }
        short qualifier = this->getQualifier();
        ret += "qualifier=\"";
            if (qualifier == CompoundAction::OP_PAR) {
                ret+="par\" ";
            } else if (qualifier == CompoundAction::OP_SEQ) {
                ret+="seq\" ";
            } else if (qualifier == CompoundAction::OP_EXCL) {
                ret+="excl\" ";
            }
        aux = this->getRepeat();
        if (aux != "0") {
            ret += "repeat=\"" + aux + "\" ";
        }
        
        aux = this->getRepeatDelay();
        if (aux != "0") {
            ret+= "repeatDelay=\"" + aux + "\" ";
        }

		//Na especificação existe os campos Duration e By que não estão presentes na implementação
		//Provavelmente esta funcionalidade ainda não está implementada, por isso a ausência dos códigos.
        
        ret +="/>";
		return ret;
         
    }
    
    string stringActionType(short actionType) {
        if (actionType == SimpleAction::ACT_START) {
             return  "start";
         } else if (actionType == SimpleAction::ACT_PAUSE) {
             return "pause";
         } else if (actionType == SimpleAction::ACT_RESUME) {
             return "resume";
         } else if (actionType == SimpleAction::ACT_STOP) {
             return "stop";
         } else if (actionType == SimpleAction::ACT_ABORT) {
             return "abort";
         } else if (actionType == SimpleAction::ACT_SET) {
             return "set";
         }
        return "";
    }
    
}
}
}
}
}
