#include "../include/EventHandle.h"


namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace soccer {

	EventHandle::EventHandle() : Thread() {
		eventId = 0;
		listener = NULL;
	}

	EventHandle::~EventHandle() {

	}

	void EventHandle::setHandleListener(IHandleListener* listener) {
		this->listener = listener;
	}

	void EventHandle::setEvent(int eventId) {
		this->eventId = eventId;
	}	

	void EventHandle::run() {
		listener->handleEvent(eventId);
	}


}
}
}
}
}
}


