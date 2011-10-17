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

#include "../include/LuaPlayer.h"

void l_dump (lua_State* L, char* point)
{
	int i;
	int top = lua_gettop(L);
	printf("Ponto %s: ", point);
	for (i=1; i<=top; i++)
	{
		int t = lua_type(L, i);
		switch (t)
		{
			case LUA_TSTRING:
				printf("'%s'", lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				printf("%g", lua_tonumber(L, i));
				break;
			default:
				printf("%s (%p)", lua_typename(L, t), lua_topointer(L, i));
				break;
		}
		printf(" ");
	}
	printf("\n");
}

void t_dump (lua_State* L, int idx)
{
	lua_pushnil(L);
	while (lua_next(L, idx) != 0)
	{
		printf("%s - %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

// MACROS ASSERTIVAS

#define CHECK(method)                                               \
	bool ok = (this->L != NULL);                                    \
	if (! ok) {                                                     \
		wclog << "[LUA] error in LuaPlayer at " << method << endl;  \
		return;                                                     \
	}
#define CHECKRET(method, ret)                                       \
	bool ok = (this->L != NULL);                                    \
	if (! ok) {                                                     \
		wclog << "[LUA] error in LuaPlayer at " << method << endl;  \
		return ret;                                                 \
	}

/*
 * Construtor:
 * - cria o estado Lua
 * - cria superficie grafica
 * - inicializa estruturas internas de apoio
 * - inicializa bibliotecas disponiveis ao NCLua
 * - associa Player ao seu estado Lua
 */
LuaPlayer::LuaPlayer (string mrl) : Player(mrl), Thread()
{
    chdir(getPath(mrl).c_str());    // execucao a partir do diretorio fonte
    this->L = luaL_newstate();      // estado Lua
    CHECK("CONSTRUCTOR()")          // assertiva
    this->surface = new Surface();  // superficie grafica do NCLua

    // ESCOPO
    this->currentScope = "";        // escopo global
	this->presentationScopes = new map<string, struct scopeInfo*>;
	this->attributionScopes = new map<string, struct scopeInfo*>;

    // a partir de um estado Lua eh possivel chegar ao seu Player
	lua_pushlightuserdata(this->L, this);         // [ LuaPlayer* ]
	lua_setfield(this->L, LUA_REGISTRYINDEX, LUAPLAYER_PLAYER);    // [ ]

    // INNICIALIZACAO DE BIBLIOTECAS DISPONIVEIS AO NCLUA

    // biblioteca padrao
	luaL_openlibs(this->L);

    // modulo 'event'
	lua_pushcfunction(this->L, luaopen_event);    // [ l_event ]
	lua_call(this->L, 0, 1);                      // [ event ]
	// LUA_EVENTINDEX = 1

    // modulo 'canvas'
	lua_pushcfunction(this->L, luaopen_canvas);   // [ event | l_canvas ]
	lua_call(this->L, 0, 0);                      // [ event ]

    // estado inicial
	this->running = true;
	this->played = false;
}

/*
 * DESTRUTOR:
 * - para de receber eventos de teclado
 * - fecha o estado Lua
 * - sinaliza fim da execucao
 */
LuaPlayer::~LuaPlayer ()
{
	CHECK("DESTRUCTOR()")
	InputManager::getInstance()->removeProceduralInputEventListener(this);
	this->running = false;
	this->lock();
	this->unlockConditionSatisfied();
	lua_close(this->L);
	this->L = NULL;
	this->unlock();
}

// Retorna Player associado ao estado Lua.
LuaPlayer* LuaPlayer::getPlayer (lua_State* L)
{
	// [ ... ]
	lua_getfield(L, LUA_REGISTRYINDEX, LUAPLAYER_PLAYER);
                                                      // [ ... | LuaPlayer* ]
	LuaPlayer* player = (LuaPlayer*) lua_touserdata(L, -1);
	lua_pop(L, 1);                                    // [ ... ]
	return player;
}

void LuaPlayer::run ()
{
	CHECK("run()")

    // carrega o NCLua e checa se houve erros
	this->lock();
	if( luaL_loadfile(this->L, this->mrl.c_str()) ) { // [ func ]
		this->notifyListeners(PlayerListener::PL_NOTIFY_ABORT, "");
		lua_error(this->L);
		return;
	}

    /*
     * Entra em modo orientado a eventos:
     * - carrega a funcao de dispatch
     * - executa-a enquanto ouver eventos
     * - aguarda novo evento
     * - repete o processo
     */

    lua_call(this->L, 0, 0);                          // [ ]
    lua_getfield(this->L, LUA_EVENTINDEX, "dispatch");// [ dispatch ]
	this->unlock();

    while (this->running)
	{
		usleep(0);
		this->lock();
		lua_pushvalue(this->L, -1);                   // [ dispatch | dispatch ]
		lua_call(this->L, 0, 1);                      // [ dispatch | more ]
		int ret = lua_toboolean(this->L, -1);         // more events?
		lua_pop(this->L, 1);                          // [ dispatch ]
		this->unlock();
        if (this->forcedNaturalEnd)
            break;
		if (!ret && this->running) {
			// TODO: se eu descomento a linha abaixo dá problema de concorrencia
			// cout << "[LUA] waiting " << ret << endl;
			this->waitForUnlockCondition();
		}
	}
	// [ event.dispatch | ret ]

    if (this->running)
		this->notifyListeners(PlayerListener::PL_NOTIFY_STOP);
}

// METODOS DE COMUNICACAO COM O FORMATADOR: play, stop, abort, pause e resume

void LuaPlayer::play()
{
	CHECK("play()")

	IDirectFBSurface* s;
	this->played = true;

    // execucao com escopo global:
    // disponibiliza a superficie do NCLua atraves do modulo canvas
	if (this->currentScope == "")
	{
		this->lock();
		s = (IDirectFBSurface*)(this->getSurface()->getContent());
		if (s) {
			lua_createcanvas(this->L, s, 0);
                                               // [ canvas ]
			lua_setglobal(this->L, "canvas");  // [ ]
		}
		this->unlock();
	}

    // sinaliza o inicio da ancora ao NCLua
	if (presentationScopes->count(currentScope) != 0 || currentScope == "") {
		// event.post({ class='ncl', type='presentation',
        //              action='start', area=currentScope })
		this->lock();
		lua_getfield(this->L, LUA_EVENTINDEX, "post");
                                                 // [ event.post ]
		lua_pushstring(this->L, "in");           // [ event.post | "in" ]
		int_event2table(this->L, "ncl", "presentation", "start",
                        currentScope.c_str());   // [ event.post | "in" | evt ]
		lua_call(this->L, 2, 0);                 // [ ]
		this->unlock();
		this->unlockConditionSatisfied();

	} else if (this->attributionScopes->count(this->currentScope) != 0) {
		//TODO: attribution
	}

	Player::play();  // must be between getSurface() and Thread::start()
    if (currentScope == "")
		Thread::start();
}

void LuaPlayer::stop ()
{
    CHECK("stop()")

    // escopo global: sinaliza o fim natural da midia
    if (this->currentScope == "") {
		forcedNaturalEnd = true;
	}
    
    // sinaliza o fim da ancora ao NCLua
    else
    {
		this->lock();
		// event.post({ class='ncl', type='presentation',
        //              action='stop', area=currentScope })
		lua_getfield(this->L, LUA_EVENTINDEX, "post");    // [ event.post ]
		lua_pushstring(this->L, "in");           // [ event.post | "in" ]
		int_event2table(this->L, "ncl", "presentation", "stop",
                        currentScope.c_str());   // [ event.post | "in" | evt ]
		lua_call(this->L, 2, 0);                 // [ ]
		this->unlock();
		this->unlockConditionSatisfied();
	}
	Player::stop();
	// TODO: terminar thread, ajeitar corotina, etc
}

void LuaPlayer::pause()
{
    // sinaliza o pause da ancora ao NCLua
	CHECK("pause()")
	// event.post('in', { class='ncl', type='presentation',
    //                    action='pause', area=currentScope })
    this->lock();
	lua_getfield(this->L, LUA_EVENTINDEX, "post"); // [ event.post ]
	lua_pushstring(this->L, "in");               // [ event.post | "in" ]
	int_event2table(this->L, "ncl", "presentation", "pause",
                    currentScope.c_str());       // [ event.post | "in" | evt ]
	lua_call(this->L, 2, 0);                     // [ ]
	this->unlock();
	this->unlockConditionSatisfied();
}

void LuaPlayer::resume ()
{
    // sinaliza o resume da ancora ao NCLua
	CHECK("resume()")
	// event.post("in", { class='ncl', type='presentation',
    //                    action='resume', area=currentScope })
    this->lock();
	lua_getfield(this->L, LUA_EVENTINDEX, "post");// [ event.post ]
	lua_pushstring(this->L, "in");                // [ event.post | "in" ]
	int_event2table(this->L, "ncl", "presentation", "resume",
                    currentScope.c_str());        // [ event.post | "in" | evt ]
	lua_call(this->L, 2, 0);                      // [ ]
	this->unlock();
	this->unlockConditionSatisfied();
}

void LuaPlayer::abort()
{
	CHECK("abort()")
    luaL_error(this->L, "player aborted");
}

void LuaPlayer::setPropertyValue2 (string name, string value)
{
	CHECK("setPropertyValue2()")
	Player::setPropertyValue(name, value, 0, 0);
}

void LuaPlayer::setPropertyValue (string name, string value,
                                  double duration, double by)
{
	CHECK("setPropertyValue()")
	Player::setPropertyValue(name, value, duration, by);

	this->lock();
	lua_getfield(this->L, LUA_EVENTINDEX, "post");// [ event.post ]
	lua_pushstring(this->L, "in");               // [ event.post | "in" ]
	int_event2table(this->L, "ncl", "attribution",
	                "start", name.c_str(), value.c_str());
                                                 // [ event.post | "in" | evt ]
	lua_call(this->L, 2, 0);                     // [ ]
	this->unlock();
	this->unlockConditionSatisfied();
}

// TECLADO

bool LuaPlayer::userEventReceived (InputEvent* evt)
{
	//CHECK("userEventReceived()")
	this->lock();
	lua_getfield(this->L, LUA_EVENTINDEX, "post"); // [ event.post ]
	lua_pushstring(this->L, "in");              // [ event.post | "in" ]
	lua_pushlightuserdata(this->L, evt->getContent());
                                                // [ event.post | "in" | evt* ]
	lua_call(this->L, 2, 0);                    // [ ]
	this->unlock();
	this->unlockConditionSatisfied();
	return true;
}

/*string LuaPlayer::getPropertyValue (string name)
{
	CHECKRET("getPropertyValue()", "")
	cout << "[LUA] getPropertyValue('" << name.c_str() << "')";

	this->lock();
	lua_getglobal(this->L, name.c_str());         // [ var ]
	string ret = luaL_optstring(L, -1, "");
	lua_pop(this->L, 1);                          // [ ]
	this->unlock();

	cout << " -> '" << ret.c_str() << "'" << endl;
	return ret;
}*/

// MÉTODOS AUXILIARES

bool LuaPlayer::hasPresented() {
	return !played;
}

void LuaPlayer::setFocusHandler(bool isHandler) {
	if (isHandler)
		InputManager::getInstance()->
                           addProceduralInputEventListener(this);
	else
		InputManager::getInstance()->
                           removeProceduralInputEventListener(this);
}

void LuaPlayer::setScope(string scopeId, short type,
                         double begin, double end)
{
	addScope(scopeId, type, begin, end);
}

void LuaPlayer::addScope(string scopeId, short type, double begin, double end)
{
	struct scopeInfo* newScope;

	if (type == PlayerListener::TYPE_PRESENTATION) {
		if (presentationScopes->count(scopeId) == 0) {
			newScope = new struct scopeInfo;
			newScope->scopeId = scopeId;
			newScope->initTime = begin;
			newScope->endTime = end;
			(*presentationScopes)[scopeId] = newScope;
		}

	} else if (type == PlayerListener::TYPE_ATTRIBUTION) {
		if (attributionScopes->count(scopeId) == 0) {
			newScope = new struct scopeInfo;
			newScope->scopeId = scopeId;
			newScope->initTime = begin;
			newScope->endTime = end;
			(*attributionScopes)[scopeId] = newScope;
		}
	}
}

void LuaPlayer::setCurrentScope(string scopeId) {
	this->currentScope = scopeId;
}

} } } } } }
