#ifndef EDITOR_H
#define EDITOR_H

#include "stb_tilemap_editor.h"
#include <SDL.h>

static unsigned int prop_type(int n, short *tiles);
static const char* prop_name(int n, short* tiles);
static bool allow_link(short *src, short *dest);

class Editor
{
public:
    Editor(int width, int height);
    void editor_key(enum stbte_action action);
    void process_event(SDL_Event* event);

    stbte_tilemap *m_edit_map;
    float m_scale = 1.0;
};

#endif // EDITOR_H
