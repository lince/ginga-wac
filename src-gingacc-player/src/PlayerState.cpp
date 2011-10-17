
#include "../include/PlayerState.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace core {
namespace player {

   const short PlayerState::NONE = 0;
   const short PlayerState::PLAY = 1;
   const short PlayerState::STOP = 2;
   const short PlayerState::PAUSE = 3;


    PlayerState::PlayerState() {
        this->status = NONE;
        this->presented = false;
        this->visible = true;
        this->width = 0;
        this->hight = 0;
	this->properties = NULL;
    }

    PlayerState::~PlayerState() {
	if (properties != NULL) {
		delete properties;
	}

    }
    
    void PlayerState::setPresented(bool b) {
        this->presented = b;
    }
    
    void PlayerState::setVisible(bool b) {
        this->visible = b;
    }
    
    void PlayerState::setStatus(short state) {
        this->status = state;
    }
    void PlayerState::setHight(int h) {
        this->hight = h;
    }
    
    void PlayerState::setWidth(int w) {
        this->width = w;
    }
    void PlayerState::setMediaTime(double time) {
        this->mediaTime = time;
    }
  
    short PlayerState::getStatus() {
        return this->status;
    }
    
    double PlayerState::getMediaTime() {
        return this->mediaTime;
    }
    
    int PlayerState::getHight() {
        return this->hight;
    }
    
    int PlayerState::getWidh() {
        return this->width;
    }
    
    bool PlayerState::isPresented() {
        return this->presented;
    }
    
    bool PlayerState::isVisible() {
        return this->visible;
    }

   void PlayerState::setMrl(string mrl) {
	this->mrl = mrl;
   }

   string PlayerState::getMrl() {
	return mrl;
   }

	void PlayerState::setProperties(map<string, string>* prop) {
		this->properties = prop;
	}

	string PlayerState::getPropertyValue(string name) {
		if (properties->count(name) != 0) {
			return (*properties)[name];
		}
		return "";
	}

	vector<string>* PlayerState::getPropertiesNames() {
		vector<string>* names = new vector<string>();
		map<string,string>::iterator it = properties->begin();
		while (it != properties->end()) {
			names->push_back(it->first);
			it++;
		}
		return names;
	}
    
}
}
}
}
}
}


