/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware 
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob 
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free 
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM 
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

#include "../../../include/LinksPlayerAdapter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace text {
	LinksPlayerAdapter::LinksPlayerAdapter() : FormatterPlayerAdapter() {
		
	}

	void LinksPlayerAdapter::createPlayer() {
		FormatterRegion* region;
		LayoutRegion* ncmRegion;
		CascadingDescriptor* descriptor;
		int w, h, x, y;
		string value;

		if (mrl != "") {
			x = 1;
			y = 1;
			w = 1;
			h = 1;

			descriptor = object->getDescriptor();
			if (descriptor == NULL) {
				return;
			}

			region = descriptor->getFormatterRegion();
			if (region != NULL) {
				ncmRegion = region->getLayoutRegion();
				x = (int)(ncmRegion->getAbsoluteLeft());
				y = (int)(ncmRegion->getAbsoluteTop());
				w = (int)(ncmRegion->getWidthInPixels());
				h = (int)(ncmRegion->getHeightInPixels());
			}

			mrl = updatePath(mrl);
			player = new LinksPlayer(mrl, x, y, w, h);
			if (player != NULL) {
				((LinksPlayer*)player)->setBounds(x, y, w, h);
				value = descriptor->getParameterValue("transparency");
				if (value != "") {
				   	((LinksPlayer*)player)->setPropertyValue(
				   			"transparency", value);
				} else {
				   	((LinksPlayer*)player)->setPropertyValue(
				   			"transparency", "1");
				}
			}
		}

		FormatterPlayerAdapter::createPlayer();
	}

	bool LinksPlayerAdapter::setPropertyValue(
		    AttributionEvent* event, string value, Animation* animation) {

		string propName;
		propName = (event->getAnchor())->getPropertyName();
		if (propName == "size" || propName == "bounds" || propName == "top" ||
			    propName == "left" || propName == "bottom" ||
			    propName == "right" || propName == "width" ||
			    propName == "height") {

			if (player != NULL) {
				FormatterRegion* region;
				LayoutRegion* ncmRegion;
				CascadingDescriptor* descriptor;
				int x, y, w, h;

				descriptor = object->getDescriptor();
				region = descriptor->getFormatterRegion();
				ncmRegion = region->getLayoutRegion();

				x = (int)(ncmRegion->getAbsoluteLeft());
				y = (int)(ncmRegion->getAbsoluteTop());
				w = (int)(ncmRegion->getWidthInPixels());
				h = (int)(ncmRegion->getHeightInPixels());
				((LinksPlayer*)player)->updateBounds(x, y, w, h);
			}
		}
		return FormatterPlayerAdapter::setPropertyValue(
				event, value, animation);
	}
}
}
}
}
}
}
}
