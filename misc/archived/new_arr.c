#include <stc/cstr.h>

#define i_val int
#include "carr2.h"

#define i_val int
#include "carr3.h"

#define i_val_str
#include "carr2.h"

int main()
{
    int w = 7, h = 5, d = 3;

    c_WITH (carr2_int volume = carr2_int_new_uninit(w, h), carr2_int_drop(&volume))
    {
        int *dat = carr2_int_data(&volume);
        for (size_t i = 0; i < carr2_int_size(&volume); ++i)
            dat[i] = i;

        for (size_t x = 0; x < volume.xdim; ++x)
            for (size_t y = 0; y < volume.ydim; ++y)
                printf(" %d", volume.data[x][y]);
        puts("");

        c_FOREACH (i, carr2_int, volume)
            printf(" %d", *i.ref);
        puts("\n");
    }

    c_WITH (carr3_int volume = carr3_int_new_uninit(w, h, d), carr3_int_drop(&volume))
    {
        int *dat = carr3_int_data(&volume);
        for (size_t i = 0; i < carr3_int_size(&volume); ++i)
            dat[i] = i;

        for (size_t x = 0; x < volume.xdim; ++x)
            for (size_t y = 0; y < volume.ydim; ++y)
                for (size_t z = 0; z < volume.zdim; ++z)
                    printf(" %d", volume.data[x][y][z]);
        puts("");

        c_FOREACH (i, carr3_int, volume)
            printf(" %d", *i.ref);
        puts("");
    }

    c_WITH (carr2_str text2d = carr2_str_with_size(h, d, cstr_NULL), carr2_str_drop(&text2d))
    {
        cstr_assign(&text2d.data[2][1], "hello");
        cstr_assign(&text2d.data[4][0], "world");

        c_FOREACH (i, carr2_str, text2d)
            printf("line: %s\n", cstr_str(i.ref));
    }
}