#include <stc/cstr.h>
#include <stc/forward.h>

forward_csmap(pnt, struct Point, int);

struct MyStruct {
    csmap_pnt pntmap;
    cstr name;
} typedef MyStruct;

// int => int map
#define i_key int
#define i_val int
#include <stc/csmap.h>

// Point => int map
struct Point { int x, y; } typedef Point;
int point_compare(const Point* a, const Point* b) {
    int c = c_default_compare(&a->x, &b->x);
    return c ? c : c_default_compare(&a->y, &b->y);
}
#define f_tag pnt  // f=forward declared
#define i_key Point
#define i_val int
#define i_cmp point_compare
#include <stc/csmap.h>

// int => int map
#define i_key_str
#define i_val_str
#include <stc/csmap.h>

// string set
#define i_key_str
#include <stc/csset.h>


int main()
{
    csmap_int map = csmap_int_init();
    csmap_int_insert(&map, 123, 321);
    csmap_int_del(&map);

    csmap_pnt pmap = csmap_pnt_init();
    csmap_pnt_insert(&pmap, (Point){42, 14}, 1);
    csmap_pnt_insert(&pmap, (Point){32, 94}, 2);
    csmap_pnt_insert(&pmap, (Point){62, 81}, 3);
    c_foreach (i, csmap_pnt, pmap)
        printf(" (%d,%d: %d)", i.ref->first.x, i.ref->first.y, i.ref->second);
    puts("");
    csmap_pnt_del(&pmap);

    csmap_str smap = csmap_str_init();
    csmap_str_emplace(&smap, "Hello, friend", "this is the mapped value");
    csmap_str_del(&smap);

    csset_str sset = csset_str_init();
    csset_str_emplace(&sset, "Hello, friend");
    csset_str_del(&sset);
}