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

#include "../../../include/NodeNesting.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
	NodeNesting::NodeNesting() {
		this->nodes = new vector<Node*>;
		id = "";
		typeSet.insert("NodeNesting");
	}

	NodeNesting::NodeNesting(Node *node) {
		this->nodes = new vector<Node*>;
		id = "";
		insertAnchorNode(node);
		typeSet.insert("NodeNesting");
	}

	NodeNesting::NodeNesting(NodeNesting *seq) {
		this->nodes = new vector<Node*>;
		id = "";
		append(seq);
		typeSet.insert("NodeNesting");
	}

	NodeNesting::NodeNesting(vector<Node*> *seq) {
		this->nodes = new vector<Node*>;
		id = "";
		append(seq);
		typeSet.insert("NodeNesting");
	}

	NodeNesting::~NodeNesting() {
		wclog << "NodeNesting::~NodeNesting" << endl;
		if (nodes != NULL) {
			delete nodes;
			nodes = NULL;
		}
	}

	bool NodeNesting::instanceOf(string s) {
		if (!typeSet.empty()) {
			return ( typeSet.find(s) != typeSet.end() );
		} else {
			return false;
		}
	}

	void NodeNesting::append(NodeNesting *otherSeq) {
		int i, size;

		size = otherSeq->getNumNodes();
		for (i = 0; i < size; i++) {
			insertAnchorNode(otherSeq->getNode(i));
		}
	}

	void NodeNesting::append(vector<Node*> *otherSeq) {
		vector<Node*>::iterator i;

		for (i = otherSeq->begin(); i != otherSeq->end(); ++i) {
			insertAnchorNode(*i);
		}
	}

	vector<Node*>* NodeNesting::toList() {
		vector<Node*>* ret = new vector<Node*>(*nodes);
		return ret;
	}

	Node* NodeNesting::getAnchorNode() {
		if (nodes->empty()) {
			return NULL;

		} else if (nodes->size() == 1) {
			return *(nodes->begin());

		} else {
			return *(nodes->end()-1);
		}
	}

	Node* NodeNesting::getHeadNode() {
		if (nodes->empty()) {
			return NULL;
		} else {
			return *(nodes->begin());
		}
	}

	Node* NodeNesting::getNode(int index) {
		if (nodes == NULL ||
			    nodes->empty() ||
			    index < 0 ||
			    index >= (int)(nodes->size())) {

			return NULL;
		}

		vector<Node*>::iterator i;
		i = nodes->begin() + index;
		return *i;
	}

	int NodeNesting::getNumNodes() {
		return nodes->size();
	}

	void NodeNesting::insertAnchorNode(Node* node) {
		if (nodes->size() > 0) {
			id = id + "/" + node->getId();
		} else {
			id = node->getId();
		}

		nodes->push_back(node);
	}

	void NodeNesting::insertHeadNode(Node* node) {
		if (nodes->size() > 0) {
			id = node->getId() + "/" + id;
		} else {
			id = node->getId();
		}
		nodes->insert(nodes->begin(), node);
	}

	bool NodeNesting::removeAnchorNode() {
		if (nodes->empty()) {
			return false;
		}

		nodes->erase(nodes->end() - 1);

		int j;
		for(j=id.length()-1; j>=0; j--) {
			if(id[j]=='/') break;
		}
		if(j<=0) id = "";
		else id = id.substr(0, j);

		return true;
	}

	bool NodeNesting::removeHeadNode() {
		if (nodes->empty()) {
			return false;
		}

		nodes->erase(nodes->begin());
		
		int j;
		for(j=id.length()-1; j>=0; j--) {
			if(id[j]=='/') break;
		}
		if(j<=0) id = "";
		else id = id.substr(0, j);

		return true;
	}

	bool NodeNesting::removeNode(Node *node) {
		unsigned int i;
		for(i=0; i<nodes->size(); i++) {
			if( (*nodes)[i] == node ) break;
		}

		if (i >= nodes->size()) return false;

		while (nodes->size() > i) {
			removeAnchorNode();
		}
		return true;
	}

	string NodeNesting::getId() {
		return id;
	}

	NodeNesting *NodeNesting::getSubsequence(int begin, int end) {
		NodeNesting *new_sequence;
		int i;

		if (begin < 0
				|| begin >= (int)nodes->size()
				|| end < begin
				|| end >= (int)nodes->size())
			return NULL;

		new_sequence = new NodeNesting((Node*)(*nodes)[begin]);
		for (i = begin + 1; i <= end; i++) {
			new_sequence->insertAnchorNode((Node*)(*nodes)[i]);
		}

		return new_sequence;
	}

	NodeNesting *NodeNesting::copy() {
		return new NodeNesting(this);
	}

	string NodeNesting::toString() {
		return id;
	}
}
}
}
}
}
}
}
