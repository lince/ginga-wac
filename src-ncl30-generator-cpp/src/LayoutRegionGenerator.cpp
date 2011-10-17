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
* @file LayoutRegionGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/LayoutRegionGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {

    string LayoutRegionGenerator::generateCode() {
        string ret = "<region id=\"" + this->getId() + "\" ";
        double aux = this->getLeft();
        if (! isNaN(aux)) {
            ret += "left =\"" + itos(aux);
            if (this->isLeftPercentual()) {
                ret += "%";
            }
             ret += "\" ";
        }
        aux = this->getTop();
        if (! isNaN(aux)) {
            ret += "top =\"" + itos(aux);
            if (this->isTopPercentual()) {
                ret += "%";
            }
             ret += "\" ";
        }
        aux = this->getRight();
        if (! isNaN(aux)) {
            ret += "right=\"" + itos(aux);
            if (this->isRightPercentual()) {
                ret += "%";
            }
             ret += "\" ";
        }
        
        aux = this->getBottom();
        if (! isNaN(aux)) {
            ret += "bottom=\"" + itos(aux);
            if (this->isBottomPercentual()) {
                ret += "%";
            }
             ret += "\" ";
        }
        
        aux = this->getHeight();
        if (! isNaN(aux)) {
            ret += "height=\"" + itos(aux);
            if (this->isHeightPercentual()) {
                ret += "%";
            }
             ret += "\" ";
        }
        
        aux = this->getWidth();
        if (! isNaN(aux)) {
            ret += "width=\"" + itos(aux);
            if (this->isWidthPercentual()) {
                ret += "%";
            }
             ret += "\" ";
        }
        
        if (this->getTitle() != "") {
            ret += "title=\"" + this->getTitle() + "\" ";
        }
        int zIndex = getZIndex();
        if (zIndex != 0) {
            ret +="zIndex= \"" + itos(zIndex) + "\" ";
		}
        
        //Não existe nada na norma sobre movable, resized... porém existem
        //atributos que tratam disto na classe LayoutRegion.
        
        vector<LayoutRegion*>* childRegions = this->getRegions();
        if (childRegions->size() != 0) {
                ret+= ">\n";
				vector<LayoutRegion*>::iterator it;
				it = childRegions->begin();
				while (it != childRegions->end()) {
					ret += (static_cast<LayoutRegionGenerator*>(*it))->generateCode();
					it++;
				}
                ret += "</region>\n";
        }
        else {
            ret+= "/>\n";
        }
        return ret;
    }
    
}
}
}
}
}
