#include "sandbox.h"
#include <iostream>
#include <pthread.h>
#include "black_hole.h"
#include <unistd.h>
#include <sys/time.h>
#include "timer.h"

typedef struct sandbox_st {
    void *data;
} sandbox_t;

enum {
    PTHREAD_VIEW = 0,
    PTHREAD_MAX
};

static pthread_t s_tid[PTHREAD_MAX] = {0};

static bool s_running_enable = true;
static sandbox_t s_sandbox = {0};
static Space s_space; 

bool still_running(void)
{
    return s_running_enable;
}

inline Space *get_space(void)
{
    return &s_space;
}

static void *view(void *arg)
{
    while (still_running())
        s_space.draw_view();
    return NULL;
}

int sandbox_init(void)
{
    Space *space = get_space();
    space->set_object(new BlackHole(local_t(5, 2)));
    space->set_object(new BlackHole(local_t(5, 5)));
    space->set_object(new BlackHole(local_t(5, 15)));
    space->set_object(new BlackHole(local_t(10, 25)));
    space->set_object(new BlackHole(local_t(15, 35)));

    timer_init();

    pthread_create(&s_tid[PTHREAD_VIEW], NULL, view, &s_sandbox);
    add_unit_timer(s_space.fflash_fps);

    return 0;
}

void sandbox_exit(void)
{
    pthread_join(s_tid[PTHREAD_VIEW], NULL);
    timer_exit();
    return;
}

struct time_st {
    time_st();
    enum limit_et {
        DEEP = 3,
    };
    size_t inc(size_t val[], size_t deep);
    size_t m_trick[DEEP];
    size_t m_min, m_mid, m_max;
    time_st &operator++();
};

size_t time_st::inc(size_t val[], size_t deep)
{
    if (val[deep] == -1ULL){
        val[deep] = 0;
        if (deep < DEEP)
            return inc(val, deep + 1);
        else 
            return val[deep] = 0;
    } else {
        ++val[deep];
        return 0;
    }
}

time_st::time_st() : m_trick(), m_min(0), m_mid(0), m_max(0)
{
}

time_st &time_st::operator++()
{
#if 1
    inc(m_trick, 0);
    return *this;
#else
    if (m_min == -1ULL) {
        m_min = 0;
        if (m_mid == -1ULL) {
            m_mid = 0;
            if (m_max == -1ULL) {
                m_max = 0;
            } else {
                ++m_max;
            }
        } else {
            ++m_mid;
        } 
    } else {
        ++m_min;
    } 

    return *this;
#endif
}

int sandbox_exec(void)
{
    while (still_running()) {
        for (auto iter = s_space.begin(); iter != s_space.end(); ++iter) {
            Object &obj = *iter;
            if (!obj.m_quality)
                continue;

            for (auto other = s_space.begin(); other != s_space.end(); ++other) {
                Object &other_obj = *other;

                if (!other_obj.m_quality)
                    continue;
                if (other_obj == obj)
                    continue;

                if (obj.interest(other_obj)) {
                    obj.action(other_obj);
                }
            }
        }

        usleep(1000);
    }

    return 0;
}


int main(int argc, char const* argv[])
{
    sandbox_init();
    sandbox_exec();
    sandbox_exit();

    return 0;
}
