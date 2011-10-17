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

/*******************************************************************************
 * CANVAS
 * A estrutura abaixo mantem um ponteiro para a superficie do DFB e atributos
 * configurados atraves de chamadas a `attr*()`.
 *
 * Os metodos com prefixo `attr` sao usado para ler e alterar atributos,
 * portanto sao sempre divididos em duas partes (GET e SET).
 * A parte GET acessa a estrutura e retorna os valores correntes.
 * A parte SET altera a estrutura e chama a funcao DFB correspondente.
 *
 * As funcoes de `draw*()` (primitivas graficas) e miscelanea apenas chamam as
 * funcoes DFB correspondentes.
 ******************************************************************************/

#include "../include/LuaPlayer.h"
#include "../include/ImagePlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

/* macro for a safe call to DirectFB functions */
#ifndef DFBCHECK
#define DFBCHECK(x...)                                            \
{                                                                 \
	DFBResult err;                                                \
	err = x;                                                      \
	if (err != DFB_OK) {                                          \
		fprintf( stdout, "%s <%d>:\n\t", __FILE__, __LINE__ );    \
		DirectFBErrorFatal( #x, err );                            \
	}                                                             \
}
#endif /*DFBCHECK*/

#define LUAPLAYER_CANVAS  "luaplayer.Canvas"
#define REFFILL           (-3)
#define REFFRAME          (-4)
#define FONTDIR   "/linux/src/Telemidia/ginga/gingacc-player/files/fonts/"
#define CHECKCANVAS(L) ((Canvas*) luaL_checkudata(L, 1, LUAPLAYER_CANVAS))

typedef struct Canvas {
	IDirectFBSurface* sfc;
	int    collect;
	Color* color;
	struct { int x; int y; int w; int h; }
           clip;
	struct { char face[20]; int size; char style[20]; }
           font;
} Canvas;

/*******************************************************************************
 * canvas:new()
 * - Imagem: retorna um novo canvas com a imagem renderizada.
 * - Dimensoes: retorna um novo canvas com as dimensoes passadas.
 ******************************************************************************/

LUALIB_API int lua_createcanvas (lua_State* L, IDirectFBSurface* sfc,
                                 int collect);
static int l_new (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);
	IDirectFBSurface* sfc = NULL;
	int type = lua_type(L, 2);

	switch (type)
	{
		// IMAGE
		// [ canvas | img_path ]
		case LUA_TSTRING: {
			Surface* _sfc =
                     ImagePlayer::renderImage((char*)luaL_checkstring(L, 2));
			sfc = (IDirectFBSurface*)(_sfc->getContent());
			_sfc->setContent(NULL);
			delete _sfc;
			break;
		}

		// NEW { w, h }
		// [ canvas | w | h ]
		case LUA_TNUMBER: {
			DFBSurfaceDescription dsc;
			dsc.width  = luaL_checkint(L, 2);
			dsc.height = luaL_checkint(L, 3);
			dsc.flags = (DFBSurfaceDescriptionFlags)
                            (DSDESC_WIDTH | DSDESC_HEIGHT);
			sfc = (IDirectFBSurface*)(
					OutputManager::getInstance()->createSurface(&dsc));
			DFBCHECK( sfc->Clear(sfc, canvas->color->getR(),
                                 canvas->color->getG(), canvas->color->getB(),
                                 canvas->color->getAlpha()) );
			break;
		}

		default:
			return luaL_argerror(L, 2, NULL);
	}

	return lua_createcanvas(L, sfc, 1);  // [ ... | canvas ] -> canvas
}

/*******************************************************************************
 * ATRIBUTOS:
 * - canvas:attrSize()
 * - canvas:attrColor()
 * - canvas:attrFont()
 * - canvas:attrClip()
 ******************************************************************************/

static int l_attrSize (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);

	// GET
	// [ canvas ]
	if (lua_gettop(L) == 1) {
		int width, height;
		DFBCHECK( canvas->sfc->GetSize(canvas->sfc, &width, &height) );
		lua_pushnumber(L, width);   // [ canvas | width ]
		lua_pushnumber(L, height);  // [ canvas | width | height ]
		return 2;                   // -> width, height
	}

	// SET
	return luaL_error(L, "not supported");
}

static int l_attrColor (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);

	// GET
	// [ canvas ]
	if (lua_gettop(L) == 1) {
		lua_pushnumber(L, canvas->color->getR());     // [ canvas|R ]
		lua_pushnumber(L, canvas->color->getG());     // [ canvas|R|G ]
		lua_pushnumber(L, canvas->color->getB());     // [ canvas|R|G|B ]
		lua_pushnumber(L, canvas->color->getAlpha()); // [ canvas|R|G|B|A ]
		return 4;                                     // -> R, G, B, A
	}

	// SET
	// [ canvas | R | G | B | A ]
	if (lua_type(L, -1) == LUA_TNUMBER)
	{
		// TODO: teste limites de r,g,b,a
		canvas->color = new Color(luaL_checkint(L, 2),
		                          luaL_checkint(L, 3),
								  luaL_checkint(L, 4),
								  luaL_checkint(L, 5));
	}
	// [ canvas | 'color' ]
	else
	{
		canvas->color = new Color(luaL_checkstring(L, -1));
	}
	DFBCHECK( canvas->sfc->SetColor(canvas->sfc,
                                    canvas->color->getR(),
                                    canvas->color->getG(),
                                    canvas->color->getB(),
                                    canvas->color->getAlpha()) );
	return 0;
}

static int l_attrFont (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);

	// GET
	// [ canvas ]
	if (lua_gettop(L) == 1) {
		lua_pushstring(L, canvas->font.face);   // [ canvas|face ]
		lua_pushnumber(L, canvas->font.size);   // [ canvas|face|size ]
		lua_pushstring(L, canvas->font.style);  // [ canvas|face|size|style ]
		return 3;                               // -> face, size, style
	}

	// SET
	// [ canvas | face | size | style ]
	strncpy(canvas->font.face, luaL_checkstring(L,2), 20);
	canvas->font.size = luaL_checkint(L, 3);
	strncpy(canvas->font.style, luaL_optstring(L,4,"normal"), 20);

	char path[255]; path[0] = '\0';
	strncat(path, FONTDIR, 100);
	strncat(path, canvas->font.face, 20);
	strncat(path, "-", 2);
	strncat(path, canvas->font.style, 20);
	strncat(path, ".ttf", 5);

	FontProvider* font = new FontProvider("vera.ttf", canvas->font.size);
	if (font == NULL) luaL_error(L, "invalid font: %s", path);
	DFBCHECK( canvas->sfc->SetFont(canvas->sfc, (IDirectFBFont*)(font->getContent())) );
	delete font;
	return 0;
}

static int l_attrClip (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);

	// GET
	// [ canvas ]
	if (lua_gettop(L) == 1) {
		lua_pushnumber(L, canvas->clip.x);  // [ canvas | x ]
		lua_pushnumber(L, canvas->clip.y);  // [ canvas | x | y ]
		lua_pushnumber(L, canvas->clip.w);  // [ canvas | x | y | w ]
		lua_pushnumber(L, canvas->clip.h);  // [ canvas | x | y | w | h ]
		return 4;                           // -> x, y, w, h
	}

	// SET
	// [ canvas | x | y | w | h ]
	canvas->clip.x = luaL_checkint(L, 2);
	canvas->clip.y = luaL_checkint(L, 3);
	canvas->clip.w = luaL_checkint(L, 4);
	canvas->clip.h = luaL_checkint(L, 5);
	DFBRegion rg; rg.x1 = canvas->clip.x;
                  rg.y1 = canvas->clip.y;
                  rg.x2 = rg.x1+canvas->clip.w;
                  rg.y2 = rg.y1+canvas->clip.h;
	DFBCHECK( canvas->sfc->SetClip(canvas->sfc, &rg) );
	return 0;
}

/******************************************************************************
 * PRIMITIVAS GRAFICAS:
 * - canvas:drawLine()
 * - canvas:drawRect()
 * - canvas:drawText()
 ******************************************************************************/

static int l_drawLine (lua_State* L)
{
	// [ canvas | x1 | y1 | x2 | y2 ]
	Canvas* canvas = CHECKCANVAS(L);
	DFBCHECK( canvas->sfc->DrawLine(canvas->sfc,
                                    luaL_checkint(L, 2),
                                    luaL_checkint(L, 3),
                                    luaL_checkint(L, 4),
                                    luaL_checkint(L, 5)) );
	return 0;
}

static int l_drawRect (lua_State* L)
{
	// [ canvas | mode | x | y | w | h ]
	Canvas* canvas = CHECKCANVAS(L);
	lua_settop(L, 6);

	// FILL
    // [ canvas | mode | x | y | w | h | "fill" ]
	lua_rawgeti(L, LUA_ENVIRONINDEX, REFFILL);
	if (lua_equal(L, 2, -1)) {
		DFBCHECK( canvas->sfc->FillRectangle(canvas->sfc,
                                             luaL_checkint(L, 3),
                                             luaL_checkint(L, 4),
                                             luaL_checkint(L, 5),
                                             luaL_checkint(L, 6)) );
		return 0;
	}

	// FRAME
    // [ canvas | mode | x | y | w | h | "fill" | "frame" ]
	lua_rawgeti(L, LUA_ENVIRONINDEX, REFFRAME);
	if (lua_equal(L, 2, -1)) {
		DFBCHECK( canvas->sfc->DrawRectangle(canvas->sfc,
                                             luaL_checkint(L, 3),
                                             luaL_checkint(L, 4),
                                             luaL_checkint(L, 5),
                                             luaL_checkint(L, 6)) );
		return 0;
	}

	return luaL_argerror(L, 2, "invalid mode");
}

static int l_drawText (lua_State* L)
{
	// [ canvas | text | x | y ]
	Canvas* canvas = CHECKCANVAS(L);
	DFBCHECK( canvas->sfc->DrawString(canvas->sfc,
                                      luaL_checkstring(L, 2),
                                      -1,
                                      luaL_checkint(L,3),
                                      luaL_checkint(L,4),
                                      (DFBSurfaceTextFlags)(DSTF_LEFT|DSTF_TOP)) );
	return 0;
}

/*******************************************************************************
 * MISCELANEA:
 * - canvas:flush()
 * - canvas:compose()
 * - canvas:measureText()
 ******************************************************************************/

static int l_flush (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);
	DFBCHECK( canvas->sfc->Flip (canvas->sfc, NULL, DSFLIP_NONE) );
	return 0;
}

static int l_compose (lua_State* L)
{
	// [ canvas | x | y | src | x | y | w | h ]
	Canvas* canvas = CHECKCANVAS(L);
	Canvas* src    = (Canvas*) luaL_checkudata(L, 4, LUAPLAYER_CANVAS);
	int x = luaL_checkint(L, 2);
	int y = luaL_checkint(L, 3);

	// clipping rect
	DFBRectangle rect, *r = NULL;
	if (lua_isnumber(L, 5)) {
		rect.x = luaL_checkint(L, 5);
		rect.y = luaL_checkint(L, 6);
		rect.w = luaL_checkint(L, 7);
		rect.h = luaL_checkint(L, 8);
		r = &rect;
	}

	DFBCHECK(canvas->sfc->SetBlittingFlags(
			canvas->sfc, (DFBSurfaceBlittingFlags)(DSBLIT_SRC_COLORKEY)));

	DFBCHECK( canvas->sfc->Blit(canvas->sfc, src->sfc, r, x, y) );
	return 0;
}

static int l_measureText (lua_State* L)
{
	// [ canvas | text ]
	Canvas* canvas = CHECKCANVAS(L);
	DFBRectangle rect;
	IDirectFBFont* font;
	DFBCHECK( canvas->sfc->GetFont(canvas->sfc, &font) );
	DFBCHECK( font->GetStringExtents(font,
                                     luaL_checkstring(L,2),
                                     -1,
                                     &rect,
                                     NULL) );
	lua_pushnumber(L, rect.w);  // [ canvas | text | w ]
	lua_pushnumber(L, rect.h);  // [ canvas | text | w | h ]
	return 2;
}

static int l_gc (lua_State* L)
{
	// [ canvas ]
	Canvas* canvas = CHECKCANVAS(L);
	delete canvas->color;
	if (canvas->collect)
		OutputManager::getInstance()->releaseSurface(canvas->sfc);
	return 0;
}

/*********************************************************************
 * Funcoes exportadas pelo modulo.
 ********************************************************************/

static const struct luaL_Reg meths[] = {
	{ "new",         l_new         },
	{ "attrSize",    l_attrSize    },
	{ "attrColor",   l_attrColor   },
	{ "attrFont",    l_attrFont    },
	{ "attrClip",    l_attrClip    },
	{ "drawLine",    l_drawLine    },
	{ "drawRect",    l_drawRect    },
	{ "drawText",    l_drawText    },
	{ "flush",       l_flush       },
	{ "compose",     l_compose     },
	{ "measureText", l_measureText },
	{ "__gc",        l_gc          },
	{ NULL,          NULL          }
};

/*********************************************************************
 * Funcao que carrega o modulo.
 * - cria as variaveis locais ao modulo
 ********************************************************************/

LUALIB_API int luaopen_canvas (lua_State* L)
{
	// [ ... ]
	lua_pushstring(L, "fill");                       // [ ... | 1 ]
	lua_pushstring(L, "frame");                      // [ ... | 2 ]
	lua_rawseti(L, LUA_ENVIRONINDEX, REFFRAME);      // [ ... | 1 ]
	lua_rawseti(L, LUA_ENVIRONINDEX, REFFILL);       // [ ... ]

	// meta = { __index=meta }
	luaL_newmetatable(L, LUAPLAYER_CANVAS);          // [ ... | meta ]
	lua_pushvalue(L, -1);                            // [ ... | meta | meta ]
	lua_setfield(L, -2, "__index");                  // [ ... | meta ]

	luaL_register(L, NULL, meths);                   // [ ... | meta ]
	lua_pop(L, 1);                                   // [ ... ]
	lua_pushnil(L);                                  // [ ... | nil ]
	return 1;
}

/*********************************************************************
 * FUNCOES INTERNAS
 ********************************************************************/

LUALIB_API int lua_createcanvas (lua_State* L, IDirectFBSurface* sfc, int collect)
{
	// [ ... ]
	Canvas* canvas = (Canvas*) lua_newuserdata(L, sizeof(Canvas));
                                               // [ ... | canvas ]
	canvas->sfc = sfc;
	canvas->collect = collect;
	luaL_getmetatable(L, LUAPLAYER_CANVAS);    // [ ... | canvas | meta ]
	lua_setmetatable(L, -2);                   // [ ... | canvas ]

	// default color: black
	canvas->color = new Color("black");
	DFBCHECK( canvas->sfc->SetColor(canvas->sfc,
                                    canvas->color->getR(),
                                    canvas->color->getG(),
                                    canvas->color->getB(),
                                    canvas->color->getAlpha()) );

	return 1;
}
