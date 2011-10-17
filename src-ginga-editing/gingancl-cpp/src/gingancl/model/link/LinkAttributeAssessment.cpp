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

#include "../../../include/LinkAttributeAssessment.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkAttributeAssessment::LinkAttributeAssessment(
		    FormatterEvent* ev, short attrType) : LinkAssessment() {

		event = ev;
		attributeType = attrType;
		offset = "";
		typeSet.insert("LinkAttributeAssessment");
	}

	FormatterEvent *LinkAttributeAssessment::getEvent() {
		return event;
	}

	void LinkAttributeAssessment::setOffset(string offset) {
		this->offset = offset;
	}

	string LinkAttributeAssessment::getOffset() {
		return offset;
	}

	void LinkAttributeAssessment::setEvent(FormatterEvent *ev) {
		event = ev;
	}

	short LinkAttributeAssessment::getAttributeType() {
		return attributeType;
	}

	void LinkAttributeAssessment::setAttributeType(short attrType) {
		attributeType = attrType;
	}

	string LinkAttributeAssessment::getValue() {
		wclog << endl << endl;
		wclog << "LinkAttributeAssessment::getValue() ";

		switch (attributeType) {
			case EventUtil::ATT_STATE:
				return FormatterEvent::getStateName(event->getCurrentState());

			case EventUtil::ATT_OCCURRENCES:
				return getAssessmentWithOffset(itos(
					    event->getOccurrences()));

			case EventUtil::ATT_REPETITIONS:
				if (event->instanceOf("PresentationEvent")) {
					return getAssessmentWithOffset(itos(
						    ((PresentationEvent*)event)->getRepetitions()));

				} else {
					return "";
				}

			case EventUtil::ATT_NODE_PROPERTY:
				if (event->instanceOf("AttributionEvent")) {
					return getAssessmentWithOffset(
						    ((AttributionEvent*)event)->getCurrentValue());

				} else {
					return "";
				}

			default:
				return "";
		}
	}

	string LinkAttributeAssessment::getAssessmentWithOffset(
		    string assessmentValue) {

		if (isNumeric((void*)(assessmentValue.c_str()))) {
			return itos(stof(
				    assessmentValue) + stof(offset));

		}

		return assessmentValue;
		/*
		if (!(assessmentValue->instanceof Double) || offset == null ||
			    !(offset instanceof Double)) {

			return assessmentValue;
		}
		else {
			return new Double(((Double)assessmentValue).doubleValue() +
				    ((Double)offset).doubleValue());
		}
		*/
	}
}
}
}
}
}
}
}
