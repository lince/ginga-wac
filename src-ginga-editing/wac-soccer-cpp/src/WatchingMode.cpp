
#include "../include/WatchingMode.h"


namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace soccer {

	WatchingMode::WatchingMode() {
		logger = LoggerUtil::getLogger("br.ufscar.lince.ginga.wac.soccer.WatchingMode");
		LoggerUtil_info(logger, "Inicializando e cadastrando novos botões\n\n\n");

		EnhancedInputManager* eim = EnhancedInputManager::getInstance();
		eim->addInputMode("soccer-watching", this);

		CodeMap* codeMap = CodeMap::getInstance();
		green = codeMap->addCode("_WAC__GREEN");
		red = codeMap->addCode("_WAC__RED");
		blue = codeMap->addCode("_WAC__BLUE");
		yellow = codeMap->addCode("_WAC__YELLOW");

		imageStarting = new ImagemDisplay("/misc/media/watchmode2.png");
		imageExiting =  new ImagemDisplay("/misc/media/nclmode2.png");
		imageStarting->initializeWindow(20, 20, 360, 108);
		imageExiting->initializeWindow(20, 20, 360, 108);
	}

	WatchingMode::~WatchingMode() {

	}

	void WatchingMode::startingInputMode() {
		LoggerUtil_info(logger, "Entrando no Watching Mode");
		imageExiting->hide();
		imageStarting->display(3000000);

	}

	void WatchingMode::exitingInputMode() {
		LoggerUtil_info(logger, "Saindo do Watching Mode");
		imageStarting->hide();
		imageExiting->display(3000000);
	}


	void WatchingMode::initializeInputMode() {

	}

	IInputEvent* WatchingMode::modifierEvent(IInputEvent* event) {
		const int code = event->getKeyCode();
		LoggerUtil_info(logger, "Evento com código: "<<code<<" chegou.");
		if (code == CodeMap::KEY_F1 || code == CodeMap::KEY_RED) {
			event->setKeyCode(red);
		} else if (code == CodeMap::KEY_F2 || code == CodeMap::KEY_GREEN) {
				event->setKeyCode(green);
		} else if (code == CodeMap::KEY_F3 || code == CodeMap::KEY_YELLOW) {
				event->setKeyCode(yellow);
		} else if (code == CodeMap::KEY_F4 || code == CodeMap::KEY_BLUE) {
				event->setKeyCode(blue);
		}
		LoggerUtil_info(logger, "Evento com código: "<<code<<" saiu.");
		return event;
	}

}
}
}
}
}
}



