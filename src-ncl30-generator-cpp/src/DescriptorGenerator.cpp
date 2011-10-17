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
* @file DescriptorGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/DescriptorGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {

    string generateKeyNavigation(KeyNavigation* keys);
    string generateFocusDecoration(FocusDecoration* focus);

    string DescriptorGenerator::generateCode() {
        string ret = "<descriptor id=\"" + this->getId() + "\" ";
        LayoutRegion* region = this->getRegion();

        if (region != NULL) {
             ret += "region=\"" + region->getId() + "\" ";
        }
        string player = this->getPlayerName();
        if (player != "") {
            ret += "player=\"" + player + "\" ";
        }
        double explicitDuration = this->getExplicitDuration();
        if (!isNaN(explicitDuration)) {
            ret +="explicitDur=\"" + itos(explicitDuration/1000.0) + "s\" ";
        }

		if (this->isFreeze()) {
			ret+= "freeze=\"true\" ";
		}

		long repetitions = this->getRepetitions();
		if (repetitions > 0) {
			ret +="explicitDur=\"" + itos(repetitions) + "\" ";
		}

        KeyNavigation* keys = this->getKeyNavigation() ;
        if (keys != NULL) {
            ret += generateKeyNavigation(keys);
		}

        FocusDecoration* focus = this->getFocusDecoration(); {
        if (focus != NULL)
            ret += generateFocusDecoration(focus);
		}

		vector<Transition*>* inputTransitions = this->getInputTransitions();
		vector<Transition*>::iterator transIt;
		transIt = inputTransitions->begin();
		if (transIt != (inputTransitions->end())) {
			ret +="transIn=\"";
			while (true) {
				Transition* transition = *transIt;
				ret += transition->getId();
				transIt++;
				if (transIt != inputTransitions->end()) {
					ret +=",";
				} else {
					ret +="\" ";
					break;
				}
			}
		}

		vector<Transition*>* outputTransitions = this->getOutputTransitions();
		transIt = outputTransitions->begin();
		if (transIt != (outputTransitions->end())) {
			ret +="transOut=\"";
			while (true) {
				Transition* transition = *transIt;
				ret += transition->getId();
				transIt++;
				if (transIt != outputTransitions->end()) {
					ret +=",";
				} else {
					ret +="\" ";
					break;
				}
			}
		}

        vector<Parameter*>* parameters = this->getParameters();
        if (parameters->size() == 0) {
            ret += "/>";
        } else {
	    	ret += ">";
			vector<Parameter*>::iterator it;
			it = parameters->begin();
			while (it != parameters->end()) {
				ParameterGenerator* paramGen = static_cast<ParameterGenerator*>(*it);
				ret += paramGen->generateCode("descriptorParam", "value") + "\n";
				it++;
			}
            ret += "</descriptor>\n";
        }
        return ret;
    }
    

    string generateKeyNavigation(KeyNavigation* keys) {
        string ret = "";
        if (keys != NULL) {
            if (keys->getMoveLeft()  != "")
                ret += "moveLeft=\"" + keys->getMoveLeft() + "\" ";
             if (keys->getMoveRight()  != "")
                ret += "moveRight=\"" + keys->getMoveRight() + "\" ";
             if (keys->getMoveUp()  != "")
                ret += "moveUp=\"" + keys->getMoveUp() + "\" ";
             if (keys->getMoveDown()  != "")
                ret += "moveDown=\"" + keys->getMoveDown() + "\" ";
             if (keys->getFocusIndex()  != "")
                ret += "focusIndex=\"" + keys->getFocusIndex() + "\" ";
        }
	return ret;
    }
    
    string generateFocusDecoration(FocusDecoration* focus) {
        string ret = "";
        int borderWitdh = focus->getFocusBorderWidth();
        if (borderWitdh != -3) {
            ret += "focusBorderWidth=\"" + itos(borderWitdh) + "\" ";
        }
        
        double focusBorderTransparency = focus->getFocusBorderTransparency();
        if (!isNaN(focusBorderTransparency)) {
             ret += "focusBorderTransparency=\"" + itos(focusBorderTransparency) + "\" ";
        }
        string focusSrc = focus->getFocusSrc();
        if (focusSrc  != "") {
            ret += "focusSrc=\"" + focusSrc + "\" ";
        }
        
        string focusSelSrc = focus->getFocusSelSrc();
        if (focusSelSrc != "") {
            ret +="focusSelSrc=\"" + focusSelSrc + "\" ";
        }

		Color* fbColor = focus->getFocusBorderColor();
		if (fbColor != NULL) {
			ret += "focusBorderColor=\"" + GeneratorUtil::getColorString(fbColor) +"\" ";
		}

		Color* fsColor = focus->getSelBorderColor();
		if (fsColor != NULL) {
			ret += "selBorderColor=\"" + GeneratorUtil::getColorString(fsColor) + "\" ";
		}
		
        return ret;
    }
    
}
}
}
}
}
