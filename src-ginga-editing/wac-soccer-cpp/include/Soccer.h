#ifndef SOCCER_H_
#define SOCCER_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <ginga/linceutil/LoggerUtil.h>
#include "capture/ImageCapture.h"
using namespace br::ufscar::lince::util;

#include "inputmode/InputModeRedirecter.h"
#include "inputmode/EnhancedInputManager.h"
using namespace ::br::ufscar::lince::ginga::core::inputmode;

#include "ncl/generator/NclGenerator.h"
using namespace ::br::ufscar::lince::ncl::generate;

#include "wac/state/StateManager.h"
#include "wac/state/PlayerStateWac.h"
using namespace ::br::ufscar::lince::ginga::wac::state;

#include "wac/editing/ClientEditingManager.h"
using namespace br::ufscar::lince::ginga::wac::editing;

#include "player/ImagemDisplay.h"
using namespace ::br::ufscar::lince::ginga::core::player;

#include "EventHandle.h"
#include "IHandleListener.h"

#include <iostream>
#include <cstdlib>
using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace soccer {

class Soccer : public InputModeRedirecter, public IHandleListener {

private:
	ImageCapture* imgCap;
	ImagemDisplay* bookmark; 
	ImagemDisplay* skip; 
	ImagemDisplay* loop;
	ImagemDisplay* inLoop;
	ImagemDisplay* inSkip;
	ImagemDisplay* stopSkip;
	ImagemDisplay* stopLoop;
	ImagemDisplay* imageStarting;
	ImagemDisplay* imageExiting;
	ImagemDisplay* bookemarked;

	PresentationState* beforeMark;
	PresentationState* afterMark;

	int bookmarkNumber;
	int skipNumber;
	int loopNumber;
	

	ClientEditingManager* cem;
	StateManager* stateManager;
	HLoggerPtr logger;
	NclDocument* nclDocument;
	bool first;
	int wacBlue, wacRed, wacGreen, wacYellow;

	int state;
	static const int WAITING_COMAND;
	static const int SKIP;
	static const int LOOP;
	
public:
	Soccer();
	~Soccer();

	bool userEventReceived(IInputEvent* ev);
	void startingInputMode();
	void exitingInputMode();
	void initializeInputMode();
		
	void startingMode();
	void exitingMode();

private:

	void addNclMenuWac();
	virtual void handleEvent(int code);

	void doAddBookmark();
	void doBeginLoop();
	void doEndLoop();
	void doBeginSkip();
	void doEndSkip();
	
	void screenShot(string fileName);
	void cleanupState();
	void addIntervalAnchor(string nodeId, string anchorId, 
			double begin, double end=0);
	
};

}
}
}
}
}
}


#endif //SOCCER_H_

