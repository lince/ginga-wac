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
* @file DescriptorSwitchGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/DescriptorSwitchGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {

    string DescriptorSwitchGenerator::generateCode() {
		string ret = "<descriptorSwitch id=\"" + this->getId() + "\" >";
		GenericDescriptor* defaultDescriptor = this->getDefaultDescriptor();
		if (defaultDescriptor != NULL) {
			ret +="\t<defaultDescriptor descriptor=\"" + defaultDescriptor->getId() + "\" />";
		}
		
		unsigned int numDescriptors = this->getNumDescriptors();
		if (numDescriptors > 0) {
			for (int index = 0; index < numDescriptors; index++) {
				GenericDescriptor* genDescriptor = this->getDescriptor(index);
				DescriptorGenerator* descriptorGenerator = static_cast<DescriptorGenerator*>((Descriptor*) genDescriptor);
				ret += descriptorGenerator->generateCode() + "\n";
			}
		}

		unsigned int numRules = this->getNumRules();
		if (numRules > 0) {
			for (int index = 0; index < numRules; index++) {
				Rule* rule = this->getRule(index);
				GenericDescriptor* descriptor = this->getDescriptor(rule);
				ret += "<bindRule rule=\"" + rule->getId() + "\" ";
				ret += "constituent=\"" + descriptor->getId() + "\" />\n"; 
			}
		}

        return ret;
    }
    
    
}
}
}
}
}
