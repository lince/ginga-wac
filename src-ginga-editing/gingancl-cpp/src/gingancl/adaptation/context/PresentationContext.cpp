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

#include "../../../include/PresentationContext.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {
	PresentationContext::PresentationContext():ContextBase() {
		createObserversVector();
		contextFile = "/misc/maestroFormatterConfig/context/context.ini";
		open();
	}

	PresentationContext* PresentationContext::_instance = 0;

	PresentationContext* PresentationContext::getInstance() {
		if (PresentationContext::_instance == NULL) {
			PresentationContext::_instance = new PresentationContext();
		}
		return PresentationContext::_instance;
	}

	void PresentationContext::setPropertyValue(
		    string attributeId, string value) {

		string oldValue = "";

		map<string, string>::iterator i;
		i = contextTable->find(attributeId);

		if (i != contextTable->end()) {
			oldValue = i->second;
		}

		(*contextTable)[attributeId] = value;
		if ((value != "") && (value != oldValue)) {
			notifyObservers(&attributeId);
		}
	}

	vector<string>* PresentationContext::getPropertyNames() {
		vector<string>* propertyNames = new vector<string>;
		map<string, string>::iterator i;
		for (i = contextTable->begin(); i != contextTable->end(); ++i) {
			propertyNames->push_back(i->first);
		}

		return propertyNames;
	}

	string PresentationContext::getPropertyValue(string attributeId) {
		if (contextTable->count(attributeId) == 0) {
			return "";
		}

		return (*contextTable)[attributeId];
	}

	void PresentationContext::initializeDefaultValues() {
		if (contextTable->count(SYSTEM_LANGUAGE) == 0) {
			(*contextTable)[SYSTEM_LANGUAGE] = "pt";
		}

		if (contextTable->count(SYSTEM_CAPTION) == 0) {
			(*contextTable)[SYSTEM_CAPTION] = "pt";
		}

		if (contextTable->count(SYSTEM_SUBTITLE) == 0) {
			(*contextTable)[SYSTEM_SUBTITLE] = "pt";
		}

		if (contextTable->count(SYSTEM_RETURN_BIT_RATE) == 0) {
			(*contextTable)[SYSTEM_RETURN_BIT_RATE] = "0";
		}

		if (contextTable->count(SYSTEM_SCREEN_SIZE) == 0) {
			(*contextTable)[SYSTEM_SCREEN_SIZE] = "(800,600)";
		}

		if (contextTable->count(SYSTEM_SCREEN_GRAPHIC_SIZE) == 0) {
			(*contextTable)[SYSTEM_SCREEN_GRAPHIC_SIZE] = "(800,600)";
		}

		if (contextTable->count(SYSTEM_AUDIO_TYPE) == 0) {
			(*contextTable)[SYSTEM_AUDIO_TYPE] = "stereo";
		}

		if (contextTable->count(SYSTEM_CPU) == 0) {
			(*contextTable)[SYSTEM_CPU] = "266";
		}

		if (contextTable->count(SYSTEM_MEMORY) == 0) {
			(*contextTable)[SYSTEM_MEMORY] = "32";
		}

		if (contextTable->count(SYSTEM_OPERATING_SYSTEM) == 0) {
			(*contextTable)[SYSTEM_OPERATING_SYSTEM] = "linux";
		}

		if (contextTable->count(USER_AGE) == 0) {
			(*contextTable)[USER_AGE] = "5";
		}

		if (contextTable->count(USER_LOCATION) == 0) {
			(*contextTable)[USER_LOCATION] = "00000-000";
		}

		if (contextTable->count(USER_GENRE) == 0) {
			(*contextTable)[USER_GENRE] = "m";
		}

		/*
		if (!contextTable.containsKey(DEFAULT_FOCUS_BORDER_COLOR)) {
			contextTable.setProperty(DEFAULT_FOCUS_BORDER_COLOR, "blue");
		}

		if (!contextTable.containsKey(DEFAULT_SEL_BORDER_COLOR)) {
			contextTable.setProperty(DEFAULT_SEL_BORDER_COLOR, "green");
		}

		if (!contextTable.containsKey(DEFAULT_FOCUS_BORDER_WIDTH)) {
			contextTable.setProperty(DEFAULT_FOCUS_BORDER_WIDTH, "3");
		}

		if (!contextTable.containsKey(DEFAULT_FOCUS_BORDER_TRANSPARENCY)) {
			contextTable.setProperty(DEFAULT_FOCUS_BORDER_TRANSPARENCY, "1.0");
		}
		*/
	}

	void PresentationContext::open() {
		ifstream fis;
		fis.open("/misc/context.ini", ifstream::in);
		if (!fis.is_open()) {
			wclog << "PresentationContext: can't open input file:";
			wclog << " /misc/context.ini" << endl;
			return;
		}

		try {
			contextTable = new map<string, string>;
			string line, key, value;

			while (fis.good()) {
				fis >> line;
				key = line.substr(0, line.find_last_of("="));
				value = line.substr((line.find_first_of("=") + 1),
					    line.length());

				(*contextTable)[key] = value;
			}

		} catch (...) {
			//TODO: Error Class
			wclog << "PresentationContext: input file error!" << endl;
		}
		fis.close();
		initializeDefaultValues();
	}

	void PresentationContext::save() {
		ofstream fos;
		fos.open("/misc/context.ini", ofstream::out);
		if (!fos.is_open()) {
			wclog << "PresentationContext: can't open output file: ";
			wclog << contextFile.c_str() << endl;
			return;
		}

		try {
			map<string, string>::iterator i;
			for (i = contextTable->begin(); i != contextTable->end(); ++i) {
				fos << i->first << '=' << i->second << '\n';
			}

		} catch (...) {
			//TODO: Error Class
			wclog << "PresentationContext: output file error!" << endl;
		}
		fos.close();
	}
}
}
}
}
}
}
}
