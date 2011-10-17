
#include "../include/Soccer.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace soccer {

	const int Soccer::WAITING_COMAND = 0;
	const int Soccer::SKIP = 1;
	const int Soccer::LOOP = 2;

	Soccer::Soccer() {
		logger = LoggerUtil::getLogger("br.ufscar.lince.ginga.wac.soccer.Soccer");
		LoggerUtil_info(logger, "Veridicando Chamada do logger"<<endl<<endl<<endl<<endl);
		cem = ClientEditingManager::getInstance();
		nclDocument = NULL;
		stateManager = StateManager::getInstance();
		imgCap->ImageCaptureFactory();
    		imgCap->setWidth(400);
    		imgCap->setHeight(225);
    		imgCap->setImageFormat("gif");
		LoggerUtil_info(logger, "Depois de instanciar o ImageCapture"<<endl<<endl<<endl<<endl);

		EnhancedInputManager* eim = EnhancedInputManager::getInstance();
		eim->addInputMode("soccer-anotation", this);

		first = true;
		bookmark = new ImagemDisplay("/misc/media/bookmark2.png");
		skip = new ImagemDisplay("/misc/media/skip2.png");
		stopSkip = new ImagemDisplay("/misc/media/stopskip2.png");

		loop = new ImagemDisplay("/misc/media/loop2.png");		
		stopLoop = new ImagemDisplay("/misc/media/stoploop2.png");

		inLoop = new ImagemDisplay("/misc/media/waitstoploop2.png");
		inSkip = new ImagemDisplay("/misc/media/waitstopskip2.png");;
		bookemarked = new ImagemDisplay("/misc/media/bookmarked2.png");
		
		imageStarting = new ImagemDisplay("/misc/media/wacmode2.png");
		imageExiting =  new ImagemDisplay("/misc/media/nclmode2.png");
		

		imageStarting->initializeWindow(20, 20, 360, 108);
		imageExiting->initializeWindow(20, 20, 360, 108);

		bookmark->initializeWindow(50, 620, 270, 81);
		bookemarked->initializeWindow(20, 20, 360, 108);
		
		skip->initializeWindow(320, 620, 270, 81);
		inSkip->initializeWindow(320, 620, 270, 81);
		stopSkip->initializeWindow(20, 20, 360, 108);

		loop->initializeWindow(590, 620, 270, 81);
		inLoop->initializeWindow(590, 620, 270, 81);
		stopLoop->initializeWindow(20, 20, 360, 108);

		bookmarkNumber = 0;
		skipNumber = 0;
		loopNumber = 0;

		PresentationState* beforeMark = NULL;
		PresentationState* afterMark = NULL;

		state = WAITING_COMAND;
	}

	Soccer::~Soccer() {
		
	}	

	void Soccer::handleEvent(int code) {
		if (first) {
			first = false;
			addNclMenuWac();
		}

		if (state == WAITING_COMAND) {
			if (code == CodeMap::KEY_F3 || code == CodeMap::KEY_YELLOW) {
				//Começa o Skip
				doBeginSkip();

			} else if (code == CodeMap::KEY_F2 || code == CodeMap::KEY_GREEN) {
				//Realiza Bookmark
				doAddBookmark();

			} else if (code == CodeMap::KEY_F4 || code == CodeMap::KEY_BLUE) {
				//Começa o Loop
				doBeginLoop();

			} else {
				LoggerUtil_info(logger, "Tecla pressionada sem função: "<<code);
			}
		} else if (state == SKIP) {
			if (code == CodeMap::KEY_F3 || code == CodeMap::KEY_YELLOW) {
				doEndSkip();

			} else {
				LoggerUtil_info(logger, "Tecla pressionada sem função... Resetando o estado. "<<code);
				state = WAITING_COMAND;
				cleanupState();
				inSkip->hide();
				stopSkip->hide();
				skip->display();
			}
		} else if (state == LOOP) {
			if (code == CodeMap::KEY_F4 || code == CodeMap::KEY_BLUE) {
				doEndLoop();

			} else {
				LoggerUtil_info(logger, "Tecla pressionada sem função... Resetando o estado. "<<code);
				state = WAITING_COMAND;
				cleanupState();
				inLoop->hide();
				stopLoop->hide();
				loop->display();
			}
		}
		cem->saveCurrentFile("/misc/ncl30/soccer/soccer_anotaded.ncl");
	}	

	bool Soccer::userEventReceived(IInputEvent* ev){
		LoggerUtil_info(logger, "Evento recebido em Soccer");
		EventHandle* eventHandle = new EventHandle();
		eventHandle->setEvent(ev->getKeyCode());
		eventHandle->setHandleListener(this);
		eventHandle->start();
		return true;
	}

	void Soccer::initializeInputMode() {
	
	}

	void Soccer::startingInputMode(){
		LoggerUtil_info(logger, "Entrando no modo de Anotação Soccer");
		imageExiting->hide();
		imageStarting->display(3000000);
		bookmark->display();
		skip->display();
		loop->display();
	}

	void Soccer::exitingInputMode() {
		LoggerUtil_info(logger, "Saindo do modo de Anotação Soccer");
		bookmark->hide();
		skip->hide();
		loop->hide();
		inLoop->hide();
		inSkip->hide();
		stopLoop->hide();
		stopSkip->hide();
		imageStarting->hide();
		imageExiting->display();
	}

	void Soccer::addNclMenuWac() {

		nclDocument = cem->getCurrentDocument();

		//Adiciona as regiões.
		string aux = "<region id=\"rgMenu\" left=\"78%\" top=\"4.7%\" width=\"21%\" height=\"95%\" zIndex=\"4\"/>";
		cem->addRegion("rgTV", aux);

		aux = "<region id=\"rgItem1\" left =\"81.15%\" top=\"7%\" width=\"18.75%\" height=\"10%\" zIndex=\"5\" />";
		cem->addRegion("rgTV", aux);

		aux = "<region id=\"rgItem2\" left =\"81.15%\" top=\"22%\" width=\"18.75%\" height=\"10%\" zIndex=\"5\" />";
		cem->addRegion("rgTV", aux);

		aux = "<region id=\"rgItem3\" left =\"81.15%\" top=\"37%\" width=\"18.75%\" height=\"10%\" zIndex=\"5\" />";
		cem->addRegion("rgTV", aux);

		aux = "<region id=\"rgItem4\" left =\"81.15%\" top=\"52%\" width=\"10.75%\" height=\"10%\" zIndex=\"5\" />";
		cem->addRegion("rgTV", aux);

		aux = "<region id=\"rgAmostra1\" left =\"81.15%\" top=\"7%\" width=\"14%\" height=\"14%\" zIndex=\"5\" />";
		cem->addRegion("rgTV", aux);

		aux = "<region id=\"rgAmostra2\" left =\"81.15%\" top=\"22%\" width=\"14%\" height=\"14%\" zIndex=\"5\" />";
		cem->addRegion("rgTV", aux);

		aux = "<region id=\"rgAmostra3\" left =\"81.15%\" top=\"37%\" width=\"14%\" height=\"14%\" zIndex=\"5\" />";
		cem->addRegion("rgTV", aux);

		aux = "<region id=\"rgAmostra4\" left =\"81.15%\" top=\"52%\" width=\"14%\" height=\"14%\" zIndex=\"5\" />";
		cem->addRegion("rgTV", aux);


		aux = "<region id=\"rgIcone\" left=\"20\" top=\"20\" width=\"360\" height=\"108\" zIndex=\"5\" />";
		cem->addRegion("", aux);

		//Adiciona os Descritores.
		aux = "<descriptor id=\"dMenu\" region=\"rgMenu\"/>";
		cem->addDescriptor(aux);;

		aux = "<descriptor id=\"dAmostra1\" region=\"rgAmostra1\" focusIndex=\"1\" moveDown=\"2\" focusBorderWidth=\"-2\" focusBorderColor=\"red\" />";
		cem->addDescriptor(aux);

		aux = "<descriptor id=\"dAmostra2\" region=\"rgAmostra2\" focusIndex=\"2\" moveDown=\"3\" moveUp=\"1\" focusBorderWidth=\"-2\" focusBorderColor=\"red\" />";
		cem->addDescriptor(aux);

		aux = " <descriptor id=\"dAmostra3\" region=\"rgAmostra3\" focusIndex=\"3\" moveDown=\"4\" moveUp=\"2\" focusBorderWidth=\"-2\" focusBorderColor=\"red\" />";
		cem->addDescriptor(aux);

		aux = "<descriptor id=\"dAmostra4\" region=\"rgAmostra4\" focusIndex=\"4\" moveUp=\"3\" focusBorderWidth=\"-2\" focusBorderColor=\"red\" />";
		cem->addDescriptor(aux);

				//Adiciona os Descritores.
		aux = "<descriptor id=\"dMenu\" region=\"rgMenu\"/>";
		cem->addDescriptor(aux);;

		aux = "<descriptor id=\"dItem1\" region=\"rgItem1\" focusIndex=\"1\" moveDown=\"2\" focusBorderWidth=\"-2\" focusBorderColor=\"red\" />";
		cem->addDescriptor(aux);

		aux = "<descriptor id=\"dItem2\" region=\"rgItem2\" focusIndex=\"2\" moveDown=\"3\" moveUp=\"1\" focusBorderWidth=\"-2\" focusBorderColor=\"red\" />";
		cem->addDescriptor(aux);

		aux = " <descriptor id=\"dItem3\" region=\"rgItem3\" focusIndex=\"3\" moveDown=\"4\" moveUp=\"2\" focusBorderWidth=\"-2\" focusBorderColor=\"red\" />";
		cem->addDescriptor(aux);

		aux = "<descriptor id=\"dItem4\" region=\"rgItem4\" focusIndex=\"4\" moveUp=\"3\" focusBorderWidth=\"-2\" focusBorderColor=\"red\" />";
		cem->addDescriptor(aux);

		aux = "<descriptor id=\"dIcone\" region=\"rgIcone\" />";
		cem->addDescriptor(aux);

		//Adiciona as midias
		string docId = nclDocument->getId();

		aux = "<media id=\"_WAC__menu\" src=\"/misc/ncl30/soccer/media/menu.png\" descriptor=\"dMenu\"/>";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__cenas\" src=\"/misc/ncl30/soccer/media/bookmark2.png\" descriptor=\"dItem1\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__skips\" src=\"/misc/ncl30/soccer/media/skip2.png\" descriptor=\"dItem2\"/>";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__loops\" src=\"/misc/ncl30/soccer/media/loop2.png\" descriptor=\"dItem3\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__voltar\" src=\"/misc/ncl30/soccer/media/back.png\" descriptor=\"dItem4\" />";
		cem->addNode(docId, aux);

		/*Experimento com as mídias já adicionadas*/
		aux = "<media id=\"_WAC__cena1\" src=\"/misc/ncl30/soccer/media/bookmark1.gif\" descriptor=\"dAmostra1\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__cena2\" src=\"/misc/ncl30/soccer/media/bookmark2.gif\" descriptor=\"dAmostra2\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__cena3\" src=\"/misc/ncl30/soccer/media/bookmark3.gif\" descriptor=\"dAmostra3\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__cena4\" src=\"/misc/ncl30/soccer/media/bookmark4.gif\" descriptor=\"dAmostra4\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__skip1\" src=\"/misc/ncl30/soccer/media/skip1.gif\" descriptor=\"dAmostra1\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__skip2\" src=\"/misc/ncl30/soccer/media/skip2.gif\" descriptor=\"dAmostra2\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__skip3\" src=\"/misc/ncl30/soccer/media/skip2.gif\" descriptor=\"dAmostra3\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__skip4\" src=\"/misc/ncl30/soccer/media/skip4.gif\" descriptor=\"dAmostra4\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__loop1\" src=\"/misc/ncl30/soccer/media/loop1.gif\" descriptor=\"dAmostra1\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__loop2\" src=\"/misc/ncl30/soccer/media/loop2.gif\" descriptor=\"dAmostra2\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__loop3\" src=\"/misc/ncl30/soccer/media/loop3.gif\" descriptor=\"dAmostra3\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__loop4\" src=\"/misc/ncl30/soccer/media/loop4.gif\" descriptor=\"dAmostra4\" />";
		cem->addNode(docId, aux);
		/*Experimento com as mídias já adicionadas*/;

		aux = "<media id=\"_WAC__iconeSkip\" src=\"/misc/ncl30/soccer/media/skip2.png\" descriptor=\"dIcone\" />";
		cem->addNode(docId, aux);

		aux = "<media id=\"_WAC__iconeLoop\" src=\"/misc/ncl30/soccer/media/loop2.png\" descriptor=\"dIcone\" />";
		cem->addNode(docId, aux);

		aux =  "<link id=\"lAbreMenu\" xconnector=\"onKeySelectionStartN\">";
		aux += "	<bind component=\"video\" role=\"onSelection\">";
		aux += "		<bindParam name=\"keyCode\" value=\"_WAC__RED\"/>";
		aux += "	</bind>";
		aux += "	<bind component=\"_WAC__menu\" role=\"start\"/>";
		aux += "	<bind component=\"_WAC__skips\" role=\"start\"/>";
		aux += "	<bind component=\"_WAC__loops\" role=\"start\"/>";
		aux += "	<bind component=\"_WAC__cenas\" role=\"start\"/>";
		aux += "	<bind component=\"_WAC__voltar\" role=\"start\"/>";
		aux += "</link>";
		cem->addLink(docId, aux);

		aux =  "<link id=\"lMenuVoltar\" xconnector=\"onSelectionStopN\">";
		aux += "	<bind component=\"_WAC__voltar\" role=\"onSelection\"/>";
		aux += "	<bind component=\"_WAC__menu\" role=\"stop\"/>";
		aux += "	<bind component=\"_WAC__skips\" role=\"stop\"/>";
		aux += "	<bind component=\"_WAC__loops\" role=\"stop\"/>";
		aux += "	<bind component=\"_WAC__cenas\" role=\"stop\"/>";
		aux += "	<bind component=\"_WAC__voltar\" role=\"stop\"/>";
		aux += "</link>";
		cem->addLink(docId, aux);
	}

	void Soccer::doAddBookmark() {
		LoggerUtil_info(logger, "Realizando anotação de Bookmark.");
		imageStarting->hide();
		bookemarked->display(1000000);

		bookmarkNumber++;
		string docId = nclDocument->getId();
		string aux;
		string id = itos(bookmarkNumber);

		//Adiciona a média que representa a cena
		screenShot("bookmark" + id);

		//Remove o Link anterior que abria para o menu de cenas
		cem->removeLink(docId, "lMenuCena");

		//Adiciona o novo link para o menu de cenas
		aux =  "<link id=\"lMenuCena\" xconnector=\"onSelectionStopNStartN\">";
		aux += "<bind component=\"_WAC__cenas\" role=\"onSelection\"/>";
		aux += "<bind component=\"_WAC__skips\" role=\"stop\"/>";
		aux += "<bind component=\"_WAC__loops\" role=\"stop\"/>";
		aux += "<bind component=\"_WAC__cenas\" role=\"stop\"/>";
		aux += "<bind component=\"_WAC__voltar\" role=\"stop\"/>";
		for (int i = 1; i <= bookmarkNumber; i++) {
			aux += "<bind component=\"_WAC__cena" + itos(i) + "\" role=\"start\"/>";
		}
		aux += "</link>";
		cem->addLink(docId, aux);

		//Adiciona ancora na media "video"
		PresentationState* preState = stateManager->getPresentationState();
		PlayerStateWac* psw = NULL;
		psw = preState->getPlayerState("video");
		double seconds;
		if (psw != NULL) {
			seconds = psw->getMediaTime();
		}
		aux = "<area id=\"cena" + id + "\"  begin=\"" + itos(seconds) + "s\" />";
		cem->addInterface("video", aux);

		//Adiciona o link que dispara a cena
		aux = "<link id=\"lMenuCena" + id + "\" xconnector=\"onSelectionAbortNStopNStartN\">\n";
		aux += "\t<bind component=\"_WAC__cena" + id + "\" role=\"onSelection\" />\n";
		ContextNode* body = nclDocument->getBody();
		vector<Node*>* nodes = body->getNodes();
		vector<Node*>::iterator it;

		it = nodes->begin();
		//Adiciona os bindings que irão abortar todas as midias em execução originais
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}

			string nodeId = node->getId();

			if (nodeId.find("_WAC__") != string::npos) {
				it++;
				continue;
			}

			aux += "\t<bind component=\"" + node->getId() + "\" role=\"abort\" />\n";
			it++;
		}
		it = nodes->begin();
		//Adiciona os bindings que irão parar todas as midias em execução adicionas pela aplicação
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}

			string nodeId = node->getId();

			if (nodeId.find("_WAC__") == string::npos) {
				it++;
				continue;
			}

			aux += "\t<bind component=\"" + node->getId() + "\" role=\"stop\" />\n";
			it++;
		}

		it = nodes->begin();
		//Adiciona os bindings que irão reiniciar as midias que estiverem tocando no momento.
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId == "video") {
				//Midias tratadas de forma especial
				it++;
				continue;
			}
			PlayerStateWac* stateWac = preState->getPlayerState(nodeId);
			if (stateWac != NULL  &&
				stateWac->getStatus() == PlayerStateWac::PLAY) {
				aux += "\t<bind component=\"" + nodeId + "\" role=\"start\" />\n";
			}
			it++;
		}
		aux += "\t<bind component=\"video\" interface=\"cena" + id + 
					"\" role=\"start\" />\n";
		aux += "</link>\n";
		cem->addLink(docId, aux);

		delete preState;
	}

	void Soccer::doBeginSkip() {
		LoggerUtil_info(logger, "Iniciando anotação de Skip.");	
		skip->hide();
		imageStarting->hide();
		inSkip->display();
		stopSkip->display();
		state = SKIP;	

		beforeMark = stateManager->getPresentationState();
	}

	void Soccer::doEndSkip() {
		LoggerUtil_info(logger, "Finalizando anotação de Skip.");
		inSkip->hide();
		stopSkip->hide();
		skip->display();
		state = WAITING_COMAND;
		skipNumber++;
		string aux;
		string id = itos(skipNumber);

		string docId = nclDocument->getId();
		afterMark = stateManager->getPresentationState();
		
		//Adicionar nova imagem da cena de skip
		screenShot("skip" + id);

		//Remove o link que anteriormente iniciava o menu de skips
		cem->removeLink(docId, "lMenuSkip");

		//Adiciona novo link que iniciara o menu de skips
		aux =  "<link id=\"lMenuSkip\" xconnector=\"onSelectionStopNStartN\">";
		aux += "<bind component=\"_WAC__skips\" role=\"onSelection\"/>";
		aux += "<bind component=\"_WAC__skips\" role=\"stop\"/>";
		aux += "<bind component=\"_WAC__loops\" role=\"stop\"/>";
		aux += "<bind component=\"_WAC__cenas\" role=\"stop\"/>";
		aux += "<bind component=\"_WAC__voltar\" role=\"stop\"/>";
		for (int i = 1; i <= skipNumber; i++) {
			aux += "<bind component=\"_WAC__skip" + itos(i) + "\" role=\"start\"/>";
		}
		aux += "</link>";
		cem->addLink(docId, aux);

		//Adiciona a ancora do inicio do trecho de skip
		PlayerStateWac* psw = NULL;
		psw = beforeMark->getPlayerState("video");
		double beginSec;
		if (psw != NULL) {
			beginSec = psw->getMediaTime();
		}
		aux = "<area id=\"skipBegin" + id + "\"  begin=\"" + itos(beginSec) + "s\" />";
		cem->addInterface("video", aux);

		//Adiciona o link que mostra o icone de skip
		aux =  "<link id=\"lSkipBegin" + id + "\" xconnector=\"onBegin1StartN\" >\n";
		aux += "\t<bind component=\"video\" interface=\"skipBegin" + id + "\" role=\"onBegin\" />\n";
		aux += "\t<bind component=\"_WAC__iconeSkip\" role=\"start\" />\n";
		aux += "</link>\n";
		cem->addLink(docId, aux);
	
		//Adiciona a ancora do fim do trecho de skip
		psw = afterMark->getPlayerState("video");
		double endSec;
		if (psw != NULL) {
			endSec = psw->getMediaTime();
		}
		aux = "<area id=\"skipEnd" + id + "\"  begin=\"" + itos(endSec) + "s\" />";
		cem->addInterface("video", aux);

		//Adiciona o link que some com o icone de skip
		aux =  "<link id=\"lSkipEnd" + id + "\" xconnector=\"onBegin1StopN\" >\n";
		aux += "\t<bind component=\"video\" interface=\"skipEnd" + id + "\" role=\"onBegin\" />\n";
		aux += "\t<bind component=\"_WAC__iconeSkip\" role=\"stop\" />\n";
		aux += "</link>\n";
		cem->addLink(docId, aux);


		//Adiciona o link no menu de skip que via para o inicio da cena
		aux = "<link id=\"lMenuSkip" + id + "\" xconnector=\"onSelectionAbortNStopNStartN\">\n";
		aux += "\t<bind component=\"_WAC__skip" + id + "\" role=\"onSelection\" />\n";
		ContextNode* body = nclDocument->getBody();
		vector<Node*>* nodes = body->getNodes();
		vector<Node*>::iterator it;

		it = nodes->begin();
		//Adiciona os bindings que irão paralizar todas as midias em execução
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId.find("_WAC__") != string::npos) {
				it++;
				continue;
			}

			aux += "\t<bind component=\"" + node->getId() + "\" role=\"abort\" />\n";
			it++;
		}
		it = nodes->begin();
		//Adiciona os bindings que irão paralizar todas as midias em execução
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId.find("_WAC__") == string::npos) {
				it++;
				continue;
			}

			aux += "\t<bind component=\"" + node->getId() + "\" role=\"stop\" />\n";
			it++;
		}
		it = nodes->begin();
		//Adiciona os bindings que irão reiniciar as midias que estiverem tocando no momento.
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId == "video") {
				//Midias tratadas de forma especial
				it++;
				continue;
			}
			PlayerStateWac* stateWac = beforeMark->getPlayerState(nodeId);
			if (stateWac != NULL  &&
				stateWac->getStatus() == PlayerStateWac::PLAY) {
				aux += "\t<bind component=\"" + nodeId + "\" role=\"start\" />\n";
			}
			it++;
		}
		aux += "\t<bind component=\"video\" interface=\"skipBegin" + id + 
					"\" role=\"start\" />\n";
		aux += "</link>\n";
		cem->addLink(docId, aux);


		//Adiciona o link que é acionado pelo icone de skip
		aux = "<link id=\"lDoSkip" + id + "\" xconnector=\"onKeySelectionAbortNStopNStartN\">\n";
		aux += "\t<bind component=\"_WAC__iconeSkip\" role=\"onSelection\" >\n";
		aux += "\t\t<bindparam name=\"keyCode\" value=\"_WAC__YELLOW\" />\n";
		aux += "\t</bind>\n";

		it = nodes->begin();
		//Adiciona os bindings que irão paralizar todas as midias em execução
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId.find("_WAC__") != string::npos) {
				it++;
				continue;
			}
			aux += "\t<bind component=\"" + nodeId + "\" role=\"abort\" />\n";
			it++;
		}
				it = nodes->begin();
		//Adiciona os bindings que irão paralizar todas as midias em execução
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId.find("_WAC__") == string::npos) {
				it++;
				continue;
			}
			aux += "\t<bind component=\"" + nodeId + "\" role=\"stop\" />\n";
			it++;
		}

		it = nodes->begin();
		//Adiciona os bindings que irão reiniciar as midias que estiverem tocando no momento.
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId == "video") {
				//Midias tratadas de forma especial
				it++;
				continue;
			}
			PlayerStateWac* stateWac = afterMark->getPlayerState(nodeId);
			if (stateWac != NULL  &&
				stateWac->getStatus() == PlayerStateWac::PLAY) {
				aux += "\t<bind component=\"" + nodeId + "\" role=\"start\" />\n";
			}
			it++;
		}
		aux += "\t<bind component=\"video\" interface=\"skipEnd" + id + 
					"\" role=\"start\" />\n";
		aux += "</link>\n";
		cem->addLink(docId, aux);


		cleanupState();
	}

	void Soccer::doBeginLoop() {
		LoggerUtil_info(logger, "Iniciando anotação de Loop.");
		imageStarting->hide();
		loop->hide();
		inLoop->display();
		stopLoop->display();
		state = LOOP;

		beforeMark = stateManager->getPresentationState();
		screenShot("loop" + itos(loopNumber + 1));
	}


	void Soccer::doEndLoop() {
		LoggerUtil_info(logger, "Finalizando anotação de Loop.");
		inLoop->hide();
		stopLoop->hide();
		loop->display();
		state = WAITING_COMAND;
		loopNumber++;
		string aux;
		string id = itos(loopNumber);

		string docId = nclDocument->getId();
		afterMark = stateManager->getPresentationState();
		
		//Adicionar nova imagem da cena de loop
		//!TODO Utilizar o printscreen do franco.

		//Remove o link que anteriormente iniciava o menu de loops
		cem->removeLink(docId, "lMenuLoop");

		//Adiciona novo link que iniciara o menu de loops
		aux =  "<link id=\"lMenuLoop\" xconnector=\"onSelectionStopNStartN\">";
		aux += "<bind component=\"_WAC__loops\" role=\"onSelection\"/>";
		aux += "<bind component=\"_WAC__skips\" role=\"stop\"/>";
		aux += "<bind component=\"_WAC__loops\" role=\"stop\"/>";
		aux += "<bind component=\"_WAC__cenas\" role=\"stop\"/>";
		aux += "<bind component=\"_WAC__voltar\" role=\"stop\"/>";
		for (int i = 1; i <= loopNumber; i++) {
			aux += "<bind component=\"_WAC__loop" + itos(i) + "\" role=\"start\"/>";
		}
		aux += "</link>";
		cem->addLink(docId, aux);

		//Adiciona a ancora do inicio do trecho de skip
		PlayerStateWac* psw = NULL;
		psw = beforeMark->getPlayerState("video");
		double beginSec;
		if (psw != NULL) {
			beginSec = psw->getMediaTime();
		}
		addIntervalAnchor("video", "loopBegin" + id, beginSec);

		//Adiciona o link que mostra o icone de skip
		aux =  "<link id=\"lLoopBegin" + id + "\" xconnector=\"onBegin1StartN\" >\n";
		aux += "\t<bind component=\"video\" interface=\"loopBegin" + id + "\" role=\"onBegin\" />\n";
		aux += "\t<bind component=\"_WAC__iconeLoop\" role=\"start\" />\n";
		aux += "</link>\n";
		cem->addLink(docId, aux);
	
		//Adiciona a ancora do fim do trecho de skip
		psw = afterMark->getPlayerState("video");
		double endSec;
		if (psw != NULL) {
			endSec = psw->getMediaTime();
		}
		addIntervalAnchor("video", "loopEnd" + id, endSec);

		//Adiciona o link que some com o icone de skip
		aux =  "<link id=\"lLoopEnd" + id + "\" xconnector=\"onBegin1StopN\" >\n";
		aux += "\t<bind component=\"video\" interface=\"loopEnd" + id + "\" role=\"onBegin\" />\n";
		aux += "\t<bind component=\"_WAC__iconeLoop\" role=\"stop\" />\n";
		aux += "</link>\n";
		cem->addLink(docId, aux);


		//Adiciona o link no menu de skip que via para o inicio da cena
		aux = "<link id=\"lMenuLoop" + id + "\" xconnector=\"onSelectionAbortNStopNStartN\">\n";
		aux += "\t<bind component=\"_WAC__loop" + id + "\" role=\"onSelection\" />\n";
		ContextNode* body = nclDocument->getBody();
		vector<Node*>* nodes = body->getNodes();
		vector<Node*>::iterator it;

		it = nodes->begin();
		//Adiciona os bindings que irão paralizar todas as midias em execução
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId.find("_WAC__") != string::npos) {
				it++;
				continue;
			}

			aux += "\t<bind component=\"" + node->getId() + "\" role=\"abort\" />\n";
			it++;
		}
		it = nodes->begin();
		//Adiciona os bindings que irão paralizar todas as midias em execução
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId.find("_WAC__") == string::npos) {
				it++;
				continue;
			}

			aux += "\t<bind component=\"" + node->getId() + "\" role=\"stop\" />\n";
			it++;
		}
		it = nodes->begin();
		//Adiciona os bindings que irão reiniciar as midias que estiverem tocando no momento.
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId == "video") {
				//Midias tratadas de forma especial
				it++;
				continue;
			}
			PlayerStateWac* stateWac = beforeMark->getPlayerState(nodeId);
			if (stateWac != NULL  &&
				stateWac->getStatus() == PlayerStateWac::PLAY) {
				aux += "\t<bind component=\"" + nodeId + "\" role=\"start\" />\n";
			}
			it++;
		}
		aux += "\t<bind component=\"video\" interface=\"loopBegin" + id + 
					"\" role=\"start\" />\n";
		aux += "</link>\n";
		cem->addLink(docId, aux);

		//Adiciona o link que é acionado pelo icone de loop
		aux = "<link id=\"lDoLoop" + id + "\" xconnector=\"onKeySelectionAbortNStopNStartN\">\n";
		aux += "\t<bind component=\"_WAC__iconeLoop\" role=\"onSelection\" >\n";
		aux += "\t\t<bindparam name=\"keyCode\" value=\"_WAC__BLUE\" />\n";
		aux += "\t</bind>\n";

		it = nodes->begin();
		//Adiciona os bindings que irão paralizar todas as midias em execução
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId.find("_WAC__") != string::npos) {
				it++;
				continue;
			}
			aux += "\t<bind component=\"" + nodeId + "\" role=\"abort\" />\n";
			it++;
		}
				it = nodes->begin();
		//Adiciona os bindings que irão paralizar todas as midias em execução
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId.find("_WAC__") == string::npos) {
				it++;
				continue;
			}
			aux += "\t<bind component=\"" + nodeId + "\" role=\"stop\" />\n";
			it++;
		}

		it = nodes->begin();
		//Adiciona os bindings que irão reiniciar as midias que estiverem tocando no momento.
		while (it != nodes->end()) {
			Node* node = *it;
			if (!node->instanceOf("ContentNode")) {
				it++;
				continue;
			}
			string nodeId = node->getId();
			if (nodeId == "video") {
				//Midias tratadas de forma especial
				it++;
				continue;
			}
			PlayerStateWac* stateWac = beforeMark->getPlayerState(nodeId);
			if (stateWac != NULL  &&
				stateWac->getStatus() == PlayerStateWac::PLAY) {
				aux += "\t<bind component=\"" + nodeId + "\" role=\"start\" />\n";
			}
			it++;
		}
		aux += "\t<bind component=\"video\" interface=\"loopBegin" + id + 
					"\" role=\"start\" />\n";
		aux += "</link>\n";
		cem->addLink(docId, aux);


		cleanupState();
	}

	void Soccer::screenShot(string fileName) {
		string aux = "rm -f /misc/ncl30/soccer/media/" + fileName + ".gif";
    		system(aux.c_str());
    		try{
			aux = "/misc/ncl30/soccer/media/" + fileName + ".gif";
        		imgCap->setDestination(aux.c_str());
        		imgCap->capture();
    		} catch(...){
			aux = "rm -f /misc/ncl30/soccer/media/" + fileName + ".gif";
        		system(aux.c_str());
			aux = "/misc/ncl30/soccer/media/" + fileName + ".gif";
        		imgCap->setDestination(aux.c_str());
        		imgCap->capture();
    		}
		/*string prompt = "/misc/bin/ffmpeg -f fbgrab -r 1 -y -i : -f rawvideo -vcodec gif -vframes 1 -an /misc/ncl30/soccer/media/" + 
				fileName + ".gif";
	     system(prompt.c_str()); */
	}

	void Soccer::cleanupState() {
		LoggerUtil_info(logger, "cleanupState: "<<(int)beforeMark<< ", "<<(int)afterMark<<"\n\n\n\n");
		if (beforeMark > 0) {
			LoggerUtil_info(logger, "Deletando o beforeMark\n\n");
			delete beforeMark;
			beforeMark = NULL;
		}
		
		if (afterMark > 0) {
			LoggerUtil_info(logger, "Deletando o afterMark\n\n");
			delete afterMark;
			afterMark = NULL;
		}
		LoggerUtil_info(logger, "Saindo de Cleanup: "<<(int)beforeMark<< ", "<<(int)afterMark<<"\n\n\n\n");
	}

	void Soccer::addIntervalAnchor(string nodeId, string anchorId, 
			double begin, double end) {

		string aux = "<area id=\"" + anchorId + "\" ";
		if (begin != 0) {
			aux += "begin=\"" + itos(begin) + "s\" ";
		} 
		if (end != 0) {
			aux += "end=\"" + itos(end) + "s\" ";
		}
		aux += "/>";
		cem->addInterface(nodeId, aux);

	}

}
}
}
}
}
}
