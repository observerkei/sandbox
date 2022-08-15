#include "physics.h"
#include "space.h"


local_st::local_st() : m_x(0), m_y(0)
{
}

bool local_st::operator==(const local_st &where) const
{
    if (m_x != where.m_x)
        return false;
    if (m_y != where.m_y)
        return false;
    return true;
}

bool local_st::operator!=(const local_st &where) const
{
    if (m_x != where.m_x)
        return true;
    if (m_y != where.m_y)
        return true;
    return false;
}

local_st::local_st(const size_t x, const size_t y)
{
    m_x = x;
    m_y = y;
}

Physics::Physics() : m_quality(0), m_type(OBJECT_DEFAULT), m_show(VIEW_EMPTY), m_local(0, 0) 
{

}

Physics::Physics(const Physics &where)
{
    *this = where;
}

Physics::Physics(local_t local) : m_local(local), m_quality(0), m_type(OBJECT_DEFAULT), m_show(VIEW_EMPTY)
{
}

#include <iostream>

bool Physics::operator==(const Physics &where) const 
{
    if (m_type != where.m_type)
        return false;
    if (m_quality != where.m_quality)
        return false;
    if (m_show != where.m_show)
        return false;
    if (m_local != where.m_local)
        return false;
    return true;
}