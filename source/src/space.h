#ifndef __SPACE_H__
#define __SPACE_H__

#include <stdio.h>
#include "object.h"
#include <queue>

#define SPACE_MAP_Y (35)
#define SPACE_MAP_X (2*(SPACE_MAP_Y))
#define SPACE_VIEW_EDGE (1)
#define SPACE_VIEW_INFO (1)
#define SPACE_VIEW_Y ((SPACE_MAP_Y)+2*(SPACE_VIEW_EDGE))
#define SPACE_VIEW_X ((SPACE_MAP_X)+2*(SPACE_VIEW_EDGE))
#define SPACE_VIEW_LOOP_TIME_MS 100
#define SPACE_VIEW(fmt, ...) do { \
    fprintf(stdout, fmt, ##__VA_ARGS__); \
    fflush(stdout); \
} while (0)
#define STR(x) #x
#define TO_STR(x) STR(x)

enum view_et {
    VIEW_EMPTY = ' ',
    VIEW_EDGE_X = '|',
    VIEW_EDGE_Y = '-',
    VIEW_EDGE_POINT = '+',
    VIEW_ONE = '-',
    VIEW_TWO = '+',
    VIEW_THR = '*',
    VIEW_FOR = '#',
    VIEW_ZERO = '0',
    VIEW_BLACK_HOLE = '@'
};

typedef Object *space_t[SPACE_MAP_Y][SPACE_MAP_X+1];
typedef char view_t[SPACE_VIEW_Y][SPACE_VIEW_X+1];

struct Space {
    public:
        struct iterator {
            public:
                iterator(space_t &space, local_t idx, local_t size);
                iterator operator++();
                bool operator!=(const iterator &where) const;
                Object &operator*();

            private:
                space_t *m_base;
                local_t m_idx;
                local_t m_size;
        };

    public:
        Space();
        ~Space();
        int set_object(Object *object);
        int get_object(const local_t &local, Object **object);
        int del_object(const local_t &local);
        int mov_object(Object *object, const local_t &old_local);
		int update_object(Object *object);
        void draw_view();
        iterator begin();
        iterator end();
        static void fflash_fps();
        static void clear_trash();

    public:
        space_t m_space;
        view_t m_view;
        size_t m_score;
        static size_t m_fps;
        static size_t m_draw_view_cnt;
        static std::queue<Object *> m_trash;
};

#endif//__SPACE_H__
