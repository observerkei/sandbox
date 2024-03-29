#include "space.h"
#include "sandbox.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>

size_t Space::m_fps = 0;
size_t Space::m_draw_view_cnt = 0;

Space::~Space()
{
    for (size_t y = 0; y < SPACE_MAP_Y; ++y) {
        for (size_t x = 0; x < SPACE_MAP_X; ++x) {
            if (m_space[y][x]) {
                delete m_space[y][x];
                m_space[y][x] = nullptr;
            }
        }
    }
}

Space::Space() : m_space() 
{

    for (size_t count = 0, y = 0; y < SPACE_MAP_Y; ++y) {
        for (size_t x = 0; x < SPACE_MAP_X; ++x) {
            m_space[y][x] = new Object(local_t(x, y));
            ++count;
        }
    }

}

Space::iterator::iterator(space_t &space, local_t idx, local_t size) : 
    m_base(&space), m_idx(idx), m_size(size)
{
    assert(m_size.m_y <= SPACE_MAP_Y);
    assert(m_size.m_x <= SPACE_MAP_X);
}

Space::iterator Space::iterator::operator++()
{
    if (m_size.m_y-2 == m_idx.m_y &&
            m_idx.m_x == m_size.m_x-2) {
        m_idx.m_x = m_size.m_x;
        m_idx.m_y = m_size.m_y;
        return *this;
    }
    if (m_idx.m_y >= m_size.m_y-1)
        return *this;
    if (m_idx.m_x >= m_size.m_x-1) {
        m_idx.m_x = 0;
        ++m_idx.m_y;
        return *this;
    }
    ++m_idx.m_x;
    assert(m_idx.m_x < m_size.m_x);

    return *this;
}

bool Space::iterator::operator!=(const iterator &where) const
{
    if (m_idx != where.m_idx)
        return true;
    if (m_size != where.m_size)
        return true;
    if (m_base != where.m_base)
        return true;
    return false;
}

Object &Space::iterator::operator*()
{
    return *(*m_base)[m_idx.m_y][m_idx.m_x];
}

Space::iterator Space::begin()
{
    return iterator(m_space, local_st(0, 0), local_st(SPACE_MAP_X, SPACE_MAP_Y));
}

Space::iterator Space::end()
{
    return iterator(m_space, local_st(SPACE_MAP_X, SPACE_MAP_Y), 
            local_st(SPACE_MAP_X, SPACE_MAP_Y));
}

int Space::set_object(Object *new_obj)
{
    if (!new_obj || new_obj->m_local.m_x >= SPACE_MAP_X || new_obj->m_local.m_y >= SPACE_MAP_Y)
        return -1;

	local_t new_local = new_obj->m_local;
	Object *old = nullptr;
	int ret = this->get_object(new_local, &old);
	if (ret < 0)
		return -1;

	m_space[new_local.m_y][new_local.m_x] = new_obj;

	delete old;

    return 0;
}

int Space::del_object(const local_t &local)
{
    if (local.m_x >= SPACE_MAP_X || local.m_y >= SPACE_MAP_Y)
        return -1;

    return 0;
}

int Space::get_object(const local_t &local, Object **object)
{
    if (!object || local.m_x >= SPACE_MAP_X || local.m_y >= SPACE_MAP_Y)
        return -1;

    *object = m_space[local.m_y][local.m_x];

    return 0;
}

int Space::mov_object(Object *obj, const local_t &old_local)
{
    return 0;
}

int Space::update_object(Object *obj)
{
	if (!obj->m_quality)
		return 0;
	
	return 0;
}

static std::string add_view_line(const char *name, size_t val)
{
    assert(name);
    std::string view_info = std::string("[") + name + std::string(":%") + 
        std::to_string((SPACE_MAP_X) > strlen(name) ? 
                (SPACE_MAP_X - strlen(name) - 1) : 1) + "zu]\n";
    char buf[4096] = {0};

    snprintf(buf, sizeof(buf), view_info.c_str(), val);
    return std::string(buf);
}

void Space::draw_view(void)
{
    //backgroud
    for (size_t y = 0; y < SPACE_VIEW_Y; ++y) {
        for (size_t x = 0; x < SPACE_VIEW_X; ++x)  {
            m_view[y][x] = VIEW_EMPTY;
            if (x == 0 || x == SPACE_VIEW_X-1) 
                m_view[y][x] = VIEW_EDGE_X;
            if (y == 0 || y == SPACE_VIEW_Y-1) 
                m_view[y][x] = VIEW_EDGE_Y;
            if ((x == SPACE_VIEW_X-1 && y == SPACE_VIEW_Y-1) ||
                    (x == SPACE_VIEW_X-1 && y == 0) ||
                    (x == 0 && y == SPACE_VIEW_Y-1) || 
                    (x == 0 && y == 0)) 
                m_view[y][x] = VIEW_EDGE_POINT;
        }
        m_view[y][SPACE_VIEW_X] = '\n';
    }

    for (auto iter = this->begin(); iter != this->end(); ++iter) {
        Object &obj = *iter;

        if (obj.m_show != VIEW_EMPTY)
            m_view[obj.m_local.m_y + SPACE_VIEW_EDGE]
                [obj.m_local.m_x + SPACE_VIEW_EDGE] = obj.m_show;
    }

    std::string score_view = add_view_line("score", m_score);
    std::string fps_view = add_view_line("fps", m_fps);

    std::string view_info = score_view + fps_view;

    size_t endl_cnt = 0;
    const char *ch = view_info.c_str();
    for (size_t i = 0; i < view_info.length(); ++i) {
        if (ch[i] == '\n')
            ++endl_cnt;
    }

    std::string view_format = std::string("%s") + view_info + 
        std::string("\033[") + std::to_string(SPACE_MAP_Y + endl_cnt + 2) + "A";

    std::string view_str((const char *)m_view, sizeof(m_view));

    const char *std_p = view_str.c_str();

    SPACE_VIEW(view_format.c_str(), view_str.c_str());
    usleep(SPACE_VIEW_LOOP_TIME_MS * 1000);
    ++m_draw_view_cnt;
}

void Space::fflash_fps()
{
    m_fps = m_draw_view_cnt;
    m_draw_view_cnt = 0;
}

void Space::clear_trash()
{
}
