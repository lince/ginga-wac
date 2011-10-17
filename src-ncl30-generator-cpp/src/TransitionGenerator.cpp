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
* @file TransitionGenerator.cpp
* @author Caio Viel
* @date 29-01-10
*/

#include "../include/generables/TransitionGenerator.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ncl {
namespace generator {

    string TransitionGenerator::generateCode() {
        string ret = "<transition it=\"" + 	this->getId() + "\" ";
		ret += "type=\"" + TransitionUtil::getTypeName(this->getType()) + "\" ";
		ret += "subtype=\"" + TransitionUtil::getSubtypeName(this->getType(), this->getSubtype()) + "\" ";

		double dur = this->getDur();
		if (dur != 1000) {
			ret += "dur=\"" + itos(dur/1000) + "s\" ";
		}
		double startProgress = this->getStartProgress();
		if (startProgress != 0.0) {
			ret += "startProgress=\"" + itos(startProgress) + "s\" ";
		}
		double endProgress = this->getEndProgress();
		if (endProgress != 1.0) {
			ret += "endProgress=\"" + itos(endProgress) + "s\" ";
		}
		ret += "direction=\"" + TransitionUtil::getDirectionName(this->getDirection()) + "\" ";

		int horzRepeat = this->getHorzRepeat();
		if (horzRepeat != 1) {
			ret += "horzRepeat=\"" + itos(horzRepeat) + "\" ";
		}
		int vertRepeat = this->getVertRepeat();
		if (vertRepeat != 1) {
			ret += "vertRepeat=\"" + itos(vertRepeat) + "\" ";
		}

		int borderWidth = this->getBorderWidth();
		if (borderWidth != 0) {
			ret += "borderWidth=\"" + itos(borderWidth) + "\" ";
		}

		Color* fadeColor = this->getFadeColor();
		if (fadeColor != NULL) {
			ret += "fadeColor=\"" + GeneratorUtil::getColorString(fadeColor) + "\" ";
		}
		Color* borderColor = this->getBorderColor();
		if (borderColor != NULL) {
			ret += "borderColor=\"" + GeneratorUtil::getColorString(borderColor) + "\" ";
		}

		ret += "/>";
        return ret;
    }
    
}
}
}
}
}
