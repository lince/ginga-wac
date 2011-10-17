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
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "io/CodeMap.h"
using namespace ::br::pucrio::telemidia::ginga::core::io;

#include <string>
#include <map>
#include <cstdarg>
using namespace std;

// Indices para variaveis locais ao modulo.
// Deve iniciar de 3 (REFNIL=-1/NOREF=-2)
enum {
	  REFQUEUE = 3     // event queue table
	, REFLISTENERS     // listeners table
	, REFNEWLISTENERS  // listeners table for concurrent unregister
	, REFINPUTEVT      // table for receiveing input events
	, REFINPUTMAP      // table for (KeySymbol -> string) mapping
	, REFNCLEVT        // table for receiveing ncl events
	, REFNCLMAP        // table for ("transition"->int | int->"action") mapping
	, REFTCP           // function to execute TCP connections
};

/*******************************************************************************
 * FUNCOES INTERNAS: definicoes no fim do arquivo
 ******************************************************************************/

// Converte uma HashTable C++ para uma tabela Lua.
LUALIB_API int int_map2table (lua_State* L, map<string,string>t);

// Cria eventos NCLua a partir de C++.
LUALIB_API int int_event2table (lua_State* L, const char* clazz, ...);

// Cria eventos de tecla NCLua a partir de C++.
static int int_DFBInputEvent2table (lua_State* L, DFBInputEvent* evt);

// Converte o mapa de teclas C++ para Lua.
static int int_DFBInputEventMAP (lua_State* L);

// Eventos TCP
struct t_tcp {
	LuaPlayer* player;
    lua_State* L;
	lua_State* N;
};
static void* tcp (void* data);

/*******************************************************************************
 * FUNCOES DO MODULO
 ******************************************************************************/

/*******************************************************************************
 * event.post
 * - dst: in/out, default=out
 * - evt: tabela com o  evento
 *
 *   Destino 'in': o evento eh postado na fila interna e o Player eh acordado
 *   (unlockConditionSatisfied()). No caso de um evento de teclado, este deve
 *   ser convertido para Lua (int_DFBInputEvent2table()).
 *
 *   Destino 'out': cada caso eh tratado de forma especial (tcp, ncl, etc).
 ******************************************************************************/

static int l_post (lua_State* L)
{
	// [ dst | evt ]
	if (lua_gettop(L) == 1) {
		lua_pushstring(L, "out");
		lua_insert(L, 1);
	}
	const char* dst = luaL_checkstring(L, 1);

	// dst == "in"
	if ( !strcmp(dst, "in") )
	{
		// [ dst | evt ]
		if (lua_type(L,-1) == LUA_TLIGHTUSERDATA) {
			DFBInputEvent* evt = (DFBInputEvent*)lua_touserdata(L, -1);
			if (evt->clazz == DFEC_INPUT) {
				lua_pop(L, -1);                   // [ dst ]
				int_DFBInputEvent2table(L, evt);  // [ dst | evt ]
			}
		}

        // QUEUE[#QUEUE+1] = evt
		lua_rawgeti(L, LUA_ENVIRONINDEX, -REFQUEUE); // [ dst | evt | queue ]
		lua_pushvalue(L, -2);                        // [ dst | evt | queue | evt ]
		lua_rawseti(L, -2, lua_objlen(L,-2)+1);      // [ dst | evt | queue ]

		GETPLAYER(L)->unlockConditionSatisfied();
	}

	// dst == "out"
	else if ( !strcmp(dst, "out") )
	{
		luaL_checktype(L, 2, LUA_TTABLE);
		lua_getfield(L, 2, "class");         // [ dst | evt | class ]
		const char* clazz = luaL_checkstring(L, -1);

		// TCP event
		if ( !strcmp(clazz, "tcp") )
		{
			lua_rawgeti(L, LUA_ENVIRONINDEX, -REFTCP);
                        // [ dst | evt | class | tcp ]
			lua_pushvalue(L, -3);
                        // [ dst | evt | class | tcp | evt ]
			lua_State* N = lua_newthread(L);
                        // [ dst | evt | class | tcp | evt | N ]
			lua_pushboolean(L, 1);
                        // [ dst | evt | class | tcp | evt | N | true ]
			lua_settable(L, LUA_REGISTRYINDEX);
                        // [ dst | evt | class | tcp | evt ]
			lua_xmove(L, N, 2);
                        // L: [ dst | evt | class ]
                        // N: [ tcp | evt ]

			struct t_tcp* t = (struct t_tcp*) malloc(sizeof(struct t_tcp));
			t->player = GETPLAYER(L);
            t->L      = L;
			t->N      = N;

			// trigger tcp thread
			pthread_t id;
			pthread_create(&id, 0, tcp, (void*)t);
			pthread_detach(id);
		}

		// NCL event
		else if ( !strcmp(clazz, "ncl") )
		{
			lua_getfield(L, 2, "type");
                    // [ dst | evt | class | type ]
			const char* type = luaL_checkstring(L, -1);

			// PRESENTATION event
			if ( !strcmp(type, "presentation") )
			{
				lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNCLMAP);
                    // [ dst | evt | class | type | ttrans ]
				lua_getfield(L, 2, "transition");
                    // [ dst | evt | class | type | ttrans | trans ]
				lua_gettable(L, -2);
                    // [ dst | evt | class | type | ttrans | TRANS ]
				lua_getfield(L, 2, "area");
                    // [ dst | evt | class | type | ttrans | TRANS | area ]
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1);
                    // [ dst | evt | class | type | ttrans | TRANS ]
					lua_pushstring(L, "");
                    // [ dst | evt | class | type | ttrans | TRANS | "" ]
				}
				GETPLAYER(L)->notifyListeners(lua_tointeger(L, -2),
                                              luaL_checkstring(L, -1));
			}
			// ATTRIBUTION event
			else if ( !strcmp(type, "attribution") )
			{
				lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNCLMAP);
                    // [ dst | evt | class | type | ttrans ]
				lua_getfield(L, 2, "transition");
                    // [ dst | evt | class | type | ttrans | trans ]
				lua_gettable(L, -2);
                    // [ dst | evt | class | type | ttrans | TRANS ]
				lua_getfield(L, 2, "property");
                    // [ dst | evt | class | type | ttrans | TRANS | prop ]
				lua_getfield(L, 2, "value");
                    // [ dst | evt | class | type | ttrans | TRANS | prop | v ]
				GETPLAYER(L)->setPropertyValue2(luaL_checkstring(L,-2),
                                                luaL_checkstring(L,-1));
				GETPLAYER(L)->notifyListeners(lua_tointeger(L, -3),
                                              luaL_checkstring(L, -2),
                                              PlayerListener::TYPE_ATTRIBUTION);
			}
			// NCLEDIT event
			else if ( !strcmp(type, "ncledit") )
			{
				lua_getfield(L, 2, "command");
                    // [ dst | evt | class | type | command ]
				GETPLAYER(L)->notifyListeners(PlayerListener::PL_NOTIFY_NCLEDIT,
                                              luaL_checkstring(L, -1));
			}
		}
		else
			return luaL_error(L, "invalid event class");
	}

	else
		return luaL_argerror(L, 1, "possible values are: 'in', 'out'");

	return 0;
}

/*********************************************************************
 * event.timer
 * Cria uma thread para dormir pelo tempo especificado.
 * A thread recebe uma estrutura com o tempo, Player e estado Lua.
 * Essa estrutura eh associada aa callback a ser executada na expiracao do
 * timer, assim a thread pode ter acesso a ela apos expirar.
 *********************************************************************/

struct t_timer {
	LuaPlayer* player;
    lua_State* L;
	int time;
};
static void* sleep (void* data)
{
	struct t_timer* t = (struct t_timer *)data;
	usleep(t->time*1000);

	t->player->lock();
	lua_pushlightuserdata(t->L, t);         // [ ... | t* ]
	lua_gettable(t->L, LUA_REGISTRYINDEX);  // [ ... | func ]
	if (!lua_isnil(t->L, -1))
		lua_call(t->L, 0, 0);               // [ ... ]
	else
		lua_pop(t->L, 1);                   // [ ... ]
	t->player->unlock();

	free(t);
	return NULL;
}
static int l_cancel (lua_State* L)
{
	// [ ]
	lua_pushvalue(L, lua_upvalueindex(2));  // [ t* ]
	lua_pushvalue(L, -1);                   // [ t* | t* ]
	lua_gettable(L, LUA_REGISTRYINDEX);     // [ t* | funcR ]
	lua_pushvalue(L, lua_upvalueindex(1));  // [ t* | funcR | funcU ]

	if (lua_equal(L, -1, -2)) {
		lua_pop(L, 2);                      // [ t* ]
		lua_pushnil(L);                     // [ t* | nil ]
		lua_settable(L, LUA_REGISTRYINDEX); // [ ]
	} else
		lua_pop(L, 3);                      // [ ]

	return 0;
}

static int l_timer (lua_State* L)
{
	// [ msec, func ]
	struct t_timer* t = (struct t_timer*) malloc(sizeof(struct t_timer));
	t->player = GETPLAYER(L);
    t->L      = L;
	t->time   = luaL_checkinteger(L, 1);

	lua_pushlightuserdata(L, t);            // [ msec | func | t* ]
	lua_pushvalue(L, -1);                   // [ msec | func | t* | t* ]
	lua_pushvalue(L, -3);                   // [ msec | func | t* | t* | func ]

	// REGISTRY[t] = func
	luaL_checktype(L, 2, LUA_TFUNCTION);
	lua_settable(L, LUA_REGISTRYINDEX);     // [ msec | func | t* ]

	// returns `cancel` function
	lua_pushcclosure(L, l_cancel, 2);       // [ msec | fcancel ]

	// sleep msec
	pthread_t id;
	pthread_create(&id, 0, sleep, (void*)t);
	pthread_detach(id);

	return 1;
}

/*********************************************************************
 * event.uptime
 * Retorna o tempo de execucao do Player.
 *********************************************************************/

static int l_uptime (lua_State* L)
{
	lua_pushnumber(L, (GETPLAYER(L)->getMediaTime()) * 1000);  // [ msec ]
	return 1;
}

/*********************************************************************
 * event.register
 * - o regitro eh sempre no final da lista
 *********************************************************************/

// registra duas iguais
static int l_register (lua_State* L)
{
	// [ func ]
	luaL_checktype(L, 1, LUA_TFUNCTION);

	// listeners[#listeners+1] = func
	lua_rawgeti(L, LUA_ENVIRONINDEX, -REFLISTENERS); // [ func | listeners ]
	lua_pushvalue(L, 1);                        // [ func | listeners | func ]
	lua_rawseti(L, -2, lua_objlen(L,-2)+1);     // [ func | listeners ]

	// listeners[#listeners+1] = func
	lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS);
                                                // [ func | newlisteners ]
	if (!lua_isnil(L, -1)) {
		lua_pushvalue(L, 1);                    // [ func | listeners | func ]
		lua_rawseti(L, -2, lua_objlen(L,-2)+1); // [ func | listeners ]
	}
	else
		lua_pop(L, 1);

	return 0;
}

/*********************************************************************
 * event.unregister
 * - desregistra todas as funcoes iguais a passada
 * - cria nova tabela em REFNEWLISTENERS
 * - retorna se foi desregistrado
 *
 * Como o desregistro eh efetuado durante a varredura da tabela de registrados,
 * a operacao nao pode alterar essa tabela.
 * Uma nova tabela eh gerada sem a funcao a ser desregistrada.
 *********************************************************************/

static int l_unregister (lua_State* L)
{
	// [ func ]
	luaL_checktype(L, 1, LUA_TFUNCTION);

	lua_rawgeti(L, LUA_ENVIRONINDEX, -REFLISTENERS);  // [ func | listeners ]
	lua_newtable(L);                   // [ func | listeners | newtable ]

	int ret = 0;
	int top = 0;
	int len = lua_objlen(L, -2);
	for (int i=1; i<=len; i++) {
		lua_rawgeti(L, -2, i);         // [ func | listeners | newtable | v[i] ]
		if (!lua_equal(L, 1, -1)) {
			lua_rawseti(L, -2, ++top); // [ func | listeners | newtable ]
			ret = 1;
		}
		else
			lua_pop(L, 1);             // [ func | listeners | newtable ]
	}

	lua_rawseti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS); // [ func | listeners ]
	lua_pushboolean(L, ret);           // [ func | listeners | ret ]
	return 1;
}

/*********************************************************************
 * event.dipatch
 * - varre a fila de eventos
 * - executa os tratadores registrados passando os eventos
 * - caso novos eventos sejam gerados, estes sao colocados em outra fila
 * - no fim, caso haja nova tabela em REFNEWLISTENERS, substitui os tratadores
 * - retorna se ainda ha eventos a serem processados
 *********************************************************************/

static int l_dispatch (lua_State* L)
{
	// [ ]

	// push old QUEUE and get its size
	lua_rawgeti(L, LUA_ENVIRONINDEX, -REFQUEUE); // [ queue ]
	int evts = lua_objlen(L, -1);

	// new QUEUE = {}
	lua_newtable(L);                             // [ queue | newqueue ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFQUEUE); // [ queue ]

	// handle events in old QUEUE
	for (int evt=1; evt<=evts; evt++)
	{
		// iterate over all listeners and call each of them
		lua_rawgeti(L, -1, evt);    // [ queue | evt ]
		lua_rawgeti(L, LUA_ENVIRONINDEX, -REFLISTENERS);
                                    // [ queue | evt | listeners ]
		int len = lua_objlen(L, -1);
		for (int i=1; i<=len; i++) {
			lua_rawgeti(L, -1, i);  // [ queue | evt | listeners | func ]
			lua_pushvalue(L, -3);   // [ queue | evt | listeners | func | evt ]
			lua_call(L, 1, 0);      // [ queue | evt | listeners ]
		}
		lua_pop(L, 2);              // [ queue ]

		// check if listeners has been changed
		lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS);
                                                    // [ queue | newlisteners ]
		if (!lua_isnil(L, -1)) {
			lua_rawseti(L, LUA_ENVIRONINDEX, -REFLISTENERS); // [ queue ]
			lua_pushnil(L);                         // [ queue | nil ]
			lua_rawseti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS); // [ queue ]
		} else
			lua_pop(L, 1);                          // [ queue ]
	}
	lua_pop(L, 1);                                  // [ ]

	// check for more in new QUEUE
	lua_rawgeti(L, LUA_ENVIRONINDEX, -REFQUEUE);  // [ newqueue ]
	lua_pushboolean(L, lua_objlen(L,-1));         // [ newqueue | more ]
	lua_remove(L, -2);                            // [ more ]

	return 1;
}

/*********************************************************************
 * Funcoes exportadas pelo modulo.
 ********************************************************************/

static const struct luaL_Reg funcs[] = {
	{ "post",       l_post       },
	{ "timer",      l_timer      },
	{ "uptime",     l_uptime     },
	{ "register",   l_register   },
	{ "unregister", l_unregister },
	{ "dispatch",   l_dispatch   },
	{ NULL,         NULL         }
};

/*********************************************************************
 * Funcao que carrega o modulo.
 * - cria as variaveis locais ao modulo
 ********************************************************************/

LUALIB_API int luaopen_event (lua_State* L)
{
	// env = {}
	lua_newtable(L);                                  // [ env ]
	lua_replace(L, LUA_ENVIRONINDEX);                 // [ ]

	// env[QUEUE] = {}
	lua_newtable(L);                                  // [ queue ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFQUEUE);      // [ ]

	// env[LISTENERS] = {}
	lua_newtable(L);                                  // [ listeners ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFLISTENERS);  // [ ]

	// env[INPUTEVT] = {}
	lua_newtable(L);                                  // [ inputevt ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFINPUTEVT);   // [ ]
	// env[INPUTMAP] = {}
	int_DFBInputEventMAP(L);                          // [ inputmap ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFINPUTMAP);   // [ ]

	// env[NCLEVT] = {}
	lua_newtable(L);                                  // [ nclevt ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFNCLEVT);     // [ ]

	// env[NCLMAP] = {
	//     starts  = PL_NOTIFY_START,
	//     stops   = PL_NOTIFY_STOP,
	//     pauses  = PL_NOTIFY_PAUSE,
	//     resumes = PL_NOTIFY_RESUME,
	//     aborts  = PL_NOTIFY_ABORT,
	// }
	lua_newtable(L);                                      // [ nclmap ]
	lua_pushnumber(L, PlayerListener::PL_NOTIFY_START);   // [ nclmap | START ]
	lua_setfield(L, -2, "starts");                        // [ nclmap ]
	lua_pushnumber(L, PlayerListener::PL_NOTIFY_STOP);    // [ nclmap | STOP ]
	lua_setfield(L, -2, "stops");                         // [ nclmap ]
	lua_pushnumber(L, PlayerListener::PL_NOTIFY_PAUSE);   // [ nclmap | PAUSE ]
	lua_setfield(L, -2, "pauses");                        // [ nclmap ]
	lua_pushnumber(L, PlayerListener::PL_NOTIFY_RESUME);  // [ nclmap | RESUME ]
	lua_setfield(L, -2, "resumes");                       // [ nclmap ]
	lua_pushnumber(L, PlayerListener::PL_NOTIFY_ABORT);   // [ nclmap | ABORT ]
	lua_setfield(L, -2, "aborts");                        // [ nclmap ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFNCLMAP);         // [ ]

    // TODO: criar um local padronizado para o arquivo tcp.lua
	// env[TCP] = dofile 'tcp.lua'
	luaL_loadfile(L, "/linux/src/Telemidia/ginga"
                     "/gingacc-player/files/scripts/tcp.lua");
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFTCP);

	luaL_register(L, "event", funcs);                 // [ event ]
	return 1;
}

/*********************************************************************
 * FUNCOES INTERNAS
 ********************************************************************/

LUALIB_API int int_map2table (lua_State* L, map<string,string>t)
{
    // [ ... ]
    map<string,string>::iterator it;
	lua_newtable(L);                            // [ ... | evt ]
    for (it=t.begin(); it!=t.end(); it++) {
		lua_pushstring(L, it->second.c_str());  // [ ... | evt | value ]
		lua_setfield(L, -2, it->first.c_str()); // [ ... | evt ]
    }
    return 1;
}

LUALIB_API int int_event2table (lua_State* L, const char* clazz, ...)
{
	// [ ... ]
	lua_newtable(L);                       // [ ... | evt ]
	lua_pushstring(L, clazz);              // [ ... | evt | clazz ]
	lua_setfield(L, -2, "class");          // [ ... | evt ]

	va_list ap;
	va_start(ap, clazz);

	// NCL event
	if (!strcmp(clazz, "ncl"))
	{
		char* type = va_arg(ap,char*);
		lua_pushstring(L, type);           // [ ... | evt | type ]
		lua_setfield(L, -2, "type");       // [ ... | evt ]

		// PRESENTATION event
		if (!strcmp(type, "presentation"))
		{
			lua_pushstring(L, va_arg(ap,char*)); // [ ... | evt | action]
			lua_setfield(L, -2, "action");       // [ ... | evt ]
			lua_pushstring(L, va_arg(ap,char*)); // [ ... | evt | area ]
			lua_setfield(L, -2, "area");         // [ ... | evt ]
			va_end(ap);
			return 1;
		}
		// ATTRIBUTION event
		else if (!strcmp(type, "attribution"))
		{
			lua_pushstring(L, va_arg(ap,char*)); // [ ... | evt | action]
			lua_setfield(L, -2, "action");       // [ ... | evt ]
			lua_pushstring(L, va_arg(ap,char*)); // [ ... | evt | property ]
			lua_setfield(L, -2, "property");     // [ ... | evt ]
			lua_pushstring(L, va_arg(ap,char*)); // [ ... | evt | value ]
			lua_setfield(L, -2, "value");        // [ ... | evt ]
			va_end(ap);
			return 1;
		}
	}
	return luaL_error(L, "invalid event");
}

static int int_DFBInputEvent2table (lua_State* L, DFBInputEvent* evt)
{
	// [ ... ]
	lua_rawgeti(L, LUA_ENVIRONINDEX, -REFINPUTEVT); // [ ... | evt ]

	// evt.class = 'key'
	lua_pushstring(L, "key");                    // [ ... | evt | class ]
	lua_setfield(L, -2, "class");                // [ ... | evt ]

	// evt.type = 'press' or 'release'
	// TEMP: optimize
	if (evt->type == DIET_KEYPRESS)
		lua_pushstring(L, "press");              // [ ... | evt | 'press' ]
	else
		lua_pushstring(L, "release");            // [ ... | evt | 'release' ]
	lua_setfield(L, -2, "type");                 // [ ... | evt ]

	// evt.key = 'VK_*'
	lua_rawgeti(L, LUA_ENVIRONINDEX, -REFINPUTMAP); // [ ... | evt | map ]
	lua_rawgeti(L, -1, evt->key_symbol);         // [ ... | evt | map | "key" ]
	lua_setfield(L, -3, "key");                  // [ ... | evt | map ]
	lua_pop(L, 1);                               // [ ... | evt ]

	return 1;
}

static int int_DFBInputEventMAP (lua_State* L)
{
	// [ ... ]
	lua_newtable(L);            // [ ... | map ]

	map<string, int>::iterator iter;
	map<string, int>* valueMap = CodeMap::getInstance()->cloneKeyMap();
	for (iter=valueMap->begin(); iter!=valueMap->end(); ++iter)
	{
		// t[code] = 'VK_*'
		lua_pushstring(L, iter->first.c_str()); // [ ... | map | 'VK_*' ]
		lua_rawseti(L, -2, (int)iter->second);  // [ ... | map ]
	}
	delete valueMap;

	// TODO: hardcoded
	// F1=RED, F2=GREEN, F3=YELLOW, F4=BLUE
	lua_pushstring(L, "RED");                   // [ ... | map | 'RED' ]
	lua_rawseti(L, -2, 61697);                  // [ ... | map ]
	lua_pushstring(L, "GREEN");                 // [ ... | map | 'GREEN' ]
	lua_rawseti(L, -2, 61698);                  // [ ... | map ]
	lua_pushstring(L, "YELLOW");                // [ ... | map | 'YELLOW' ]
	lua_rawseti(L, -2, 61699);                  // [ ... | map ]
	lua_pushstring(L, "BLUE");                  // [ ... | map | 'BLUE' ]
	lua_rawseti(L, -2, 61700);                  // [ ... | map ]

	// [ ... | map ]
	return 1;
}

static void* tcp (void* data)
{
	// L: [ ... ]
	// N: [ tcp | evt ]
	struct t_tcp* t = (struct t_tcp *)data;

	lua_pushvalue(t->N, -1);               // N: [ tcp | evt | evt ]
	lua_insert(t->N, 1);                   // N: [ evt | tcp | evt ]

	lua_call(t->N, 1, 2);                  // N: [ evt | ret | err ]
	lua_setfield(t->N, 1, "error");        // N: [ evt | ret ]
	lua_setfield(t->N, 1, "value");        // N: [ evt ]

	t->player->lock();
	lua_getfield(t->L, LUA_EVENTINDEX, "post"); // L: [ ... | event.post ]
	lua_pushstring(t->L, "in");    // L: [ ... | event.post | "in" ]
	lua_xmove(t->N, t->L, 1);      // L: [ ... | event.post | "in" | evt ]
	lua_call(t->L, 2, 0);          // L: [ ... ]
	lua_pushthread(t->N);                  // N: [ evt | N ]
	lua_pushnil(t->N);                     // N: [ evt | N | nil ]
	lua_settable(t->N, LUA_REGISTRYINDEX); // N: [ evt ]
	t->player->unlock();

	free(t);
	return NULL;
}
