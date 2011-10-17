/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
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

#include <async.h>

#include "../../../include/LuaPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

// Indices para variaveis locais ao modulo.
// Deve iniciar de 3 (REFNIL=-1/NOREF=-2)
#define LUAPLAYER_EVENT "luaplayer.Event"
enum {
	  REFLISTENERS = 3 // listeners table
	, REFNEWLISTENERS  // pending listeners table
	, REFNCLMAP        // table for ("action"->int | int->"action") mapping
};

/*******************************************************************************
 * FUNCOES INTERNAS: definicoes no fim do arquivo
 ******************************************************************************/

// Eventos TCP
static void* tcp_thread (void* data);
pthread_t tcp_thread_id;
static int tcp_started = 0;
static void cb_connect (LuaPlayer* player, const char* host, int port, int fd);
static void cb_write (LuaPlayer* player, int fd, strbuf req);
static void tcp_disconnect (LuaPlayer* player, int fd);

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
 *   ser convertido para Lua (evt_key()).
 *
 *   Destino 'out': cada caso eh tratado de forma especial (tcp, ncl, etc).
 *
 ******************************************************************************/

/*********************************************************************
 * event.dispatch
 * - PILHA: [ -1/+0 ]
 * - executa os tratadores registrados passando o evento recebido
 *********************************************************************/

static void int_dispatch (lua_State* L)
{
    // [ ... | evt ]

    lua_getfield(L, LUA_REGISTRYINDEX, LUAPLAYER_EVENT);

    // [ ... | evt | env ]

    lua_rawgeti(L, -1, -REFNEWLISTENERS);  // [ ... | evt | env | newlst ]
    if (!lua_isnil(L, -1)) {
        lua_pushvalue(L, -1);              // [ ... | evt | env | newlst | newlst ]
        lua_rawseti(L, -3, -REFLISTENERS); // [ ... | evt | env | newlst ]
        lua_pushnil(L);                    // [ ... | evt | env | newlst | nil ]
        lua_rawseti(L, -3, -REFNEWLISTENERS); // [  | evt | env | newlst ]
    }
    else {
        lua_pop(L, 1);                     // [ ... | evt | env ]
	    lua_rawgeti(L, -1, -REFLISTENERS); // [ ... | evt | env | lst ]
    }

    // [ ... | evt | env | lst ]
    // iterate over all listeners and call each of them
	int len = lua_objlen(L, -1);
	for (int i=1; i<=len; i++)
    {
		lua_rawgeti(L, -1, i);         // [ ... | evt | env | lst | t ]
        lua_getfield(L, -1, "__func"); // [ ... | evt | env | lst | t | func ]
		lua_pushvalue(L, -5);          // [ ... | evt | env | lst | t | func | evt]
		lua_call(L, 1, 1);             // [ ... | evt | env | lst | t | ret]
        if (lua_toboolean(L, -1)) {
            lua_pop(L, 2);             // [ ... | evt | env | lst ]
            break;
        }
        lua_pop(L, 2);                 // [ ... | evt | env | lst ]
	}

    // [ ... | evt | env | lst ]
    lua_pop(L, 3);                     // [ ... ]
}

LUALIB_API int ext_postRef (lua_State* L, int ref)
{
    // [ ... ]
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);  // [ ... | evt ]
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    int_dispatch(L);                         // [ ... ]
    return 0;
}

LUALIB_API int ext_postHash (lua_State* L, map<string,string>evt)
{
    // [ ... ]
    map<string,string>::iterator it;
	lua_newtable(L);                            // [ ... | evt ]
    for (it=evt.begin(); it!=evt.end(); it++) {
		lua_pushstring(L, it->second.c_str());  // [ ... | evt | value ]
		lua_setfield(L, -2, it->first.c_str()); // [ ... | evt ]
    }
    // [ ... | evt ]
    int_dispatch(L);  // [ ...]
    return 0;
}

/*
static void cb_dispatch (lua_State* L, int ref)
{
    // [ ... ]
    GETPLAYER(L)->lock();
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);  // [ ... | evt ]
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    int_dispatch(L);                         // [ ... ]
    GETPLAYER(L)->unlock();
    // [ ... ]
}
*/

static int l_post (lua_State* L)
{
    // [ [dst] | evt ]
	if (lua_gettop(L) == 1) {
		lua_pushstring(L, "out");                 // [ evt | "out" ]
		lua_insert(L, 1);                         // [ "out" | evt ]
	}
	const char* dst = luaL_checkstring(L, 1);

    // [ dst | evt ]

	// dst == "in"
	if ( !strcmp(dst, "in") )
	{
		// [ dst | evt ]
        luaL_checktype(L, 2, LUA_TTABLE);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX); // [ dst ]

        GETPLAYER(L)->im->postEvent(
	        ((UserEventCreator*)(GETPLAYER(L)->cm->getObject("UserEvent")))
                (ref));
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
            lua_getfield(L, 2, "type");      // [ dst | evt | class | type ]
		    const char* type = luaL_checkstring(L, -1);

            // connect
            if ( !strcmp(type, "connect") ) {
                lua_getfield(L, 2, "host");
                    // [ dst | evt | class | type | host ]
                lua_getfield(L, 2, "port");
                    // [ dst | evt | class | type | host | port ]
                const char* host = luaL_checkstring(L, -2);
                int port = luaL_checknumber(L, -1);
                tcpconnect(host, port,
                           wrap(cb_connect, GETPLAYER(L), host, port) );
            }

            // disconnect
            if ( !strcmp(type, "disconnect") ) {
                lua_getfield(L, 2, "connection");
                    // [ dst | evt | class | type | connection ]
                int fd = luaL_checknumber(L, -1);
                fdcb(fd, selread, 0);
                close(fd);
                tcp_disconnect(GETPLAYER(L), fd);
            }

            // data
            else if ( !strcmp(type, "data") )
            {
                lua_getfield(L, 2, "connection");
                    // [ dst | evt | class | type | connection ]
                lua_getfield(L, 2, "value");
                    // [ dst | evt | class | type | connection | value ]
                int fd = luaL_checknumber(L, -2);
                const char* value = luaL_checkstring(L, -1);
                strbuf req(value);
                fdcb(fd, selwrite,
                     wrap(cb_write, GETPLAYER(L), fd, req));
            }
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
				lua_getfield(L, 2, "action");
                    // [ dst | evt | class | type | ttrans | trans ]
				lua_gettable(L, -2);
                    // [ dst | evt | class | type | ttrans | TRANS ]
				lua_getfield(L, 2, "label");
                    // [ dst | evt | class | type | ttrans | TRANS | label ]
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1);
                    // [ dst | evt | class | type | ttrans | TRANS ]
					lua_pushstring(L, "");
                    // [ dst | evt | class | type | ttrans | TRANS | "" ]
				}

                if ((lua_tointeger(L, -2) == Player::PL_NOTIFY_STOP) &&
                        (!strcmp(luaL_checkstring(L, -1),""))) {

                	GETPLAYER(L)->im->removeProceduralInputEventListener(GETPLAYER(L));
                }

				GETPLAYER(L)->notifyListeners(lua_tointeger(L, -2),
                                              luaL_checkstring(L, -1));
			}
			// ATTRIBUTION event
			else if ( !strcmp(type, "attribution") )
			{
				lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNCLMAP);
                    // [ dst | evt | class | type | ttrans ]
				lua_getfield(L, 2, "action");
                    // [ dst | evt | class | type | ttrans | trans ]
				lua_gettable(L, -2);
                    // [ dst | evt | class | type | ttrans | TRANS ]
				lua_getfield(L, 2, "name");
                    // [ dst | evt | class | type | ttrans | TRANS | prop ]
				lua_getfield(L, 2, "value");
                    // [ dst | evt | class | type | ttrans | TRANS | prop | v ]
				GETPLAYER(L)->setPropertyValue2(luaL_checkstring(L,-2),
                                                luaL_optstring(L,-1,""));
				GETPLAYER(L)->notifyListeners(lua_tointeger(L, -3),
                                              luaL_checkstring(L, -2),
                                              Player::TYPE_ATTRIBUTION);
			}
			// NCLEDIT event
			else if ( !strcmp(type, "ncledit") )
			{
				lua_getfield(L, 2, "command");
                    // [ dst | evt | class | type | command ]
				GETPLAYER(L)->notifyListeners(Player::PL_NOTIFY_NCLEDIT,
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
static int l_timer_cancel (lua_State* L)
{
	// [ ]
    timecb_remove( (timecb_t*)
                   lua_touserdata(L,lua_upvalueindex(1)));
	return 0;
}

static void cb_timer (lua_State* L, int ref)
{
    // [ ... ]
    GETPLAYER(L)->lock();
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);  // [ ... | func ]
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    lua_call(L, 0, 0);                       // [ ... ]
    GETPLAYER(L)->unlock();
}

static int l_timer (lua_State* L)
{
    // [ msec | func ]
	int ms = luaL_checknumber(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);   // [ msec ]
    timecb_t* timer =
        delaycb( ms/1000, ms%1000*1000000, wrap(cb_timer, L, ref));

    // returns cancel function
    lua_pushlightuserdata(L, timer);            // [ msec | timer ]
    lua_pushcclosure(L, l_timer_cancel, 1);     // [ msec | fcancel ]
	return 1;
}
 *********************************************************************/

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
 *********************************************************************/

static void int_newlisteners (lua_State* L, int lst)
{
    // [ ... ]
    lua_newtable(L);               // [ ... | newlst ]
    int len = lua_objlen(L, lst);
    for (int i=1; i<=len; i++)
    {
        lua_rawgeti(L, lst, i);    // [ ... | newlst | hdlr ]
        lua_rawseti(L, -2, i);     // [ ... | newlst ]
    }
    // [ ... | newlst ]
}

// registra duas iguais
static int l_register (lua_State* L)
{
    // [ [i] | func | [filter] ]

    // [i] -> i
    if (lua_type(L, 1) == LUA_TFUNCTION) {
	    lua_rawgeti(L, LUA_ENVIRONINDEX, -REFLISTENERS);
            // [ func | [filter] | listeners ]
        lua_pushnumber(L, lua_objlen(L, -1)+1);
            // [ func | [filter] | listeners | #listeners+1 ]
        lua_insert(L, 1);
            // [ #listeners+1 | func | [filter] | listeners ]
        lua_pop(L, 1);
            // [ #listeners+1 | func | [filter] ]
    }

    // [ i | func | [filter] ]

    // [filter] -> filter
    if (lua_gettop(L) == 2)
        lua_newtable(L);    // [ i | func | filter ]

    // [ i | func | filter ]
	luaL_checktype(L, 1, LUA_TNUMBER);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	luaL_checktype(L, 3, LUA_TTABLE);

    // filter.__func = func
    lua_pushvalue(L, 2);           // [ i | func | filter | func ]
    lua_setfield(L, -2, "__func"); // [ i | func | filter ]

    // creates newlisteners
    lua_rawgeti(L, LUA_ENVIRONINDEX, -REFLISTENERS);    // [ i | func | filter | lst ]
    lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS); // [ i | func | filter | lst | ? ]
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);           // [ i | func | filter | lst ]
        int_newlisteners(L, 4);  // [ i | func | filter | lst | newlst ]
        lua_pushvalue(L, -1);    // [ i | func | filter | lst | newlst | newlst ]
        lua_rawseti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS); // [ ewlst ]
    }

    // [ i | func | filter | lst | newlst ]
    lua_getglobal(L, "table");     // [ -> | table ]
    lua_getfield(L, -1, "insert"); // [ -> | table | tinsert ]
    lua_pushvalue(L, 5);           // [ -> | table | tinsert | newlst ]
    lua_pushvalue(L, 1);           // [ -> | table | tinsert | newlst | i ]
    lua_pushvalue(L, 3);           // [ -> | table | tinsert | newlst | i | filter ]
    lua_call(L, 3, 0);             // [ -> | table ]

    // [ i | func | filter | lst | newlst | table ]
    return 0;
}

/*********************************************************************
 * event.unregister
 *********************************************************************/

static int l_unregister (lua_State* L)
{
	// [ func ]
	luaL_checktype(L, 1, LUA_TFUNCTION);

    // creates newlisteners
    lua_rawgeti(L, LUA_ENVIRONINDEX, -REFLISTENERS);    // [ func | lst ]
    lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS); // [ func | lst | ? ]
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);                        // [ func | lst ]
        int_newlisteners(L, 4);               // [ func | lst | newlst ]
        lua_pushvalue(L, -1);                 // [ func | lst | newlst | newlst ]
        lua_rawseti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS); // [ ewlst ]
    }

    // [ func | lst | newlst ]

	int len = lua_objlen(L, 3);
    for (int i=len; i>=1; i--)
    {
	    lua_rawgeti(L, -1, i);         // [ func | lst | newlst | t ]
        lua_getfield(L, -1, "__func"); // [ func | lst | newlst | t | f' ]
	    if (!lua_equal(L, -3, -1)) {
            lua_getglobal(L, "table");     // [ -> | f' | table ]
            lua_getfield(L, -1, "remove"); // [ -> | f' | table | remove ]
            lua_pushvalue(L, 3);           // [ -> | f' | table | remove | newlst ]
            lua_pushnumber(L, i);          // [ -> | f' | table | remove | newlst | i ]
            lua_call(L, 2, 0);             // [ -> | f' | table ]
            lua_pop(L, 1);                 // [ -> | f' ]
	    }
	    lua_pop(L, 2);                 // [ func | lst | newlst ]
	}

    // [ func | lst | newlst ]
    return 0;
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
	{ NULL,         NULL         }
};

/*********************************************************************
 * Funcao que carrega o modulo.
 * - cria as variaveis locais ao modulo
 ********************************************************************/

LUALIB_API int luaclose_event (lua_State* L)
{
    // TODO: find a solution to stops tcp thread
    /*tcp_started--;
    if (tcp_started == 0) {
	    pthread_cancel(tcp_thread_id);
    }*/

}

LUALIB_API int luaopen_event (lua_State* L)
{
	// env = {}
	lua_newtable(L);                                  // [ env ]
    lua_pushvalue(L, -1);                             // [ env | env ]
    lua_setfield(L, LUA_REGISTRYINDEX, LUAPLAYER_EVENT); // [ env ]
	lua_replace(L, LUA_ENVIRONINDEX);                 // [ ]

    // env[LISTENERS] = {}
	lua_newtable(L);                                  // [ listeners ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFLISTENERS);  // [ ]

    // env[NCLMAP] = {
    //     start  = PL_NOTIFY_START,
    //     stop   = PL_NOTIFY_STOP,
    //     pause  = PL_NOTIFY_PAUSE,
    //     resume = PL_NOTIFY_RESUME,
    //     abort  = PL_NOTIFY_ABORT,
	// }
	lua_newtable(L);                              // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_START);   // [ nclmap | START ]
	lua_setfield(L, -2, "start");                 // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_STOP);    // [ nclmap | STOP ]
	lua_setfield(L, -2, "stop");                  // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_PAUSE);   // [ nclmap | PAUSE ]
	lua_setfield(L, -2, "pause");                 // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_RESUME);  // [ nclmap | RESUME ]
	lua_setfield(L, -2, "resume");                // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_ABORT);   // [ nclmap | ABORT ]
	lua_setfield(L, -2, "abort");                 // [ nclmap ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFNCLMAP); // [ ]

    // trigger tcp thread
    if (tcp_started == 0) {
	    pthread_create(&tcp_thread_id, 0, tcp_thread, NULL);
	    pthread_detach(tcp_thread_id);
    }
    tcp_started++;

	luaL_register(L, "event", funcs);             // [ event ]
	return 1;
}

/*********************************************************************
 * FUNCOES INTERNAS
 ********************************************************************/

void tcp_disconnect (LuaPlayer* player, int fd)
{
    // [ ... ]
    player->lock();
    lua_State* L = player->L;

    lua_newtable(L);                   // [ ... | evt ]
    lua_pushstring(L, "tcp");          // [ ... | evt | "tcp" ]
    lua_setfield(L, -2, "class");      // [ ... | evt ]
    lua_pushstring(L, "disconnect");   // [ ... | evt | "disconnect" ]
    lua_setfield(L, -2, "type");       // [ ... | evt ]
    lua_pushnumber(L, fd);             // [ ... | evt | fd ]
    lua_setfield(L, -2, "connection"); // [ ... | evt ]

    int_dispatch(L);                   // [ ... ]
    player->unlock();
}

void cb_read (LuaPlayer* player, int fd)
{
    strbuf resp;

    int n = resp.tosuio()->input(fd);
    if (n < 0) {
        tcp_disconnect(player, fd);
        return;
    }
    if (n == 0) {
        fdcb(fd, selread, 0);
        close(fd);
        tcp_disconnect(player, fd);
        return;
    }

    // [ ... ]

    player->lock();
    lua_State* L = player->L;

    lua_newtable(L);                   // [ ... | evt ]
    lua_pushstring(L, "tcp");          // [ ... | evt | "tcp" ]
    lua_setfield(L, -2, "class");      // [ ... | evt ]
    lua_pushstring(L, "data");         // [ ... | evt | "connect" ]
    lua_setfield(L, -2, "type");       // [ ... | evt ]
    lua_pushstring(L, (str)resp);      // [ ... | evt | host ]
    lua_setfield(L, -2, "value");      // [ ... | evt ]
    lua_pushnumber(L, fd);             // [ ... | evt | fd ]
    lua_setfield(L, -2, "connection"); // [ ... | evt ]

    int_dispatch(L);                   // [ ... ]
    player->unlock();
}

void cb_write (LuaPlayer* player, int fd, strbuf req)
{
    int n = req.tosuio()->output(fd);
    if (n < 0) {
        tcp_disconnect(player, fd);
        return;
    }

    // still stuff to write: don't disable writability callback
    if(req.tosuio()->resid())
        return;

    // done writing request.
    fdcb(fd, selwrite, 0);
    fdcb(fd, selread, wrap(cb_read, player, fd));
}

void cb_connect (LuaPlayer* player, const char* host, int port, int fd)
{
    // [ ... ]
    player->lock();
    lua_State* L = player->L;

    lua_newtable(L);              // [ ... | evt ]
    lua_pushstring(L, "tcp");     // [ ... | evt | "tcp" ]
    lua_setfield(L, -2, "class"); // [ ... | evt ]
    lua_pushstring(L, "connect"); // [ ... | evt | "connect" ]
    lua_setfield(L, -2, "type");  // [ ... | evt ]
    lua_pushstring(L, host);      // [ ... | evt | host ]
    lua_setfield(L, -2, "host");  // [ ... | evt ]
    lua_pushnumber(L, port);      // [ ... | evt | port ]
    lua_setfield(L, -2, "port");  // [ ... | evt ]

    if (fd < 0) {
        lua_pushstring(L, "connection error");
            // [ ... | evt | err_msg ]
        lua_setfield(L, -2, "error");
            // [ ... | evt ]
    }
    else {
        lua_pushnumber(L, fd);
            // [ ... | evt | fd ]
        lua_setfield(L, -2, "connection");
            // [ ... | evt ]
    }

    int_dispatch(L);              // [ ... ]
    player->unlock();
}

static void* tcp_thread (void* data)
{
    async_init();
    amain();
}
