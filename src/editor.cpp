#include "editor.h"
#include "level.h"
#include "stb_tilemap_editor.h"

// TODO
#define STBTE_MAX_PROPERTIES 8

#define STBTE_PROP_TYPE(n, tiledata, p) prop_type(n, tiledata)
#define STBTE_PROP_NAME(n, tiledata, p) prop_name(n, tiledata)
// #define STBTE_PROP_MIN(n, tiledata, p)  prop_range(n,tiledata,0)
// #define STBTE_PROP_MAX(n, tiledata, p)  prop_range(n,tiledata,1)
#define STBTE_PROP_FLOAT_SCALE(n,td,p)  (0.1)

#define STBTE_ALLOW_LINK(srctile, srcprop, desttile, destprop) \
           allow_link(srctile, desttile)

#define STBTE_LINK_COLOR(srctile, srcprop, desttile, destprop) \
          (0xff80ff)

#define STBTE_DRAW_RECT(x0,y0,x1,y1,c)           \
          // TODO

#define STBTE_DRAW_TILE(x,y,id,highlight,props)  \
          // TODO


#define STB_TILEMAP_EDITOR_IMPLEMENTATION
#include "stb_tilemap_editor.h"

static unsigned int prop_type(int n, short *tiles)
{
    // TODO
    return STBTE_PROP_none;
}

static const char* prop_name(int n, short* tiles)
{
    const char* tile_name = "todo";
    return tile_name;
}

static bool allow_link(short *src, short *dest)
{
    return false;
}

Editor::Editor(int width, int height)
{
    m_edit_map = stbte_create_map(width, height, 1, 16,16, 100);

    stbte_set_background_tile(m_edit_map, (short)((TileType)AIR));

    stbte_define_tile(m_edit_map, (TileType)SOLID, 1, "Blocks");
    stbte_define_tile(m_edit_map, (TileType)HAZARD, 1, "Spikes");

    stbte_set_layername(m_edit_map, 1, "Level");
}

void Editor::editor_key(enum stbte_action action)
{
    stbte_action(m_edit_map, action);
}

void Editor::process_event(SDL_Event* event)
{
    switch (event->type) {
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEWHEEL:
        stbte_mouse_sdl(m_edit_map, event, 1.0 / m_scale, 1.0f / m_scale, 0, 0);
        break;
    case SDL_KEYDOWN:
        switch (event->key.keysym.sym) {
            case SDLK_RIGHT: editor_key(STBTE_scroll_right); break;
            case SDLK_LEFT : editor_key(STBTE_scroll_left ); break;
            case SDLK_UP   : editor_key(STBTE_scroll_up   ); break;
            case SDLK_DOWN : editor_key(STBTE_scroll_down ); break;
            default:                                         break;
        }
        switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_S: editor_key(STBTE_tool_select    ); break;
            case SDL_SCANCODE_B: editor_key(STBTE_tool_brush     ); break;
            case SDL_SCANCODE_E: editor_key(STBTE_tool_erase     ); break;
            case SDL_SCANCODE_R: editor_key(STBTE_tool_rectangle ); break;
            case SDL_SCANCODE_I: editor_key(STBTE_tool_eyedropper); break;
            case SDL_SCANCODE_L: editor_key(STBTE_tool_link      ); break;
            case SDL_SCANCODE_G: editor_key(STBTE_act_toggle_grid); break;
            default:                                                break;
        }
        if ((event->key.keysym.mod & KMOD_CTRL) && !(event->key.keysym.mod & ~KMOD_CTRL)) {
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_X: editor_key(STBTE_act_cut  ); break;
                case SDL_SCANCODE_C: editor_key(STBTE_act_copy ); break;
                case SDL_SCANCODE_V: editor_key(STBTE_act_paste); break;
                case SDL_SCANCODE_Z: editor_key(STBTE_act_undo ); break;
                case SDL_SCANCODE_Y: editor_key(STBTE_act_redo ); break;
                default:                                          break;
            }
        }
        break;
    }
}
