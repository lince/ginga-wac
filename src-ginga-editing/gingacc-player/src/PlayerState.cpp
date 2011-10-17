
#include "../include/PlayerState.h"

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace core {
namespace player {
    PlayerState::PlayerState() {
        this->status = NONE;
        this->presented = false;
        this->visible = true;
        this->width = 0;
        this->hight = 0;
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
    
}
}
}
}
}
}


