#include "kernel/common.cl"


////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// vie
////////////////////////////////////////////////////////////////////////////////

__kernel void vie (__global unsigned * img,
                   __global unsigned * img_next,
                   __global char * need_to_change_before,
                   __global char * need_to_change_after)
{
  int lx = get_local_id (0);
  int ly = get_local_id (1);
  int gx = get_global_id (0);
  int gy = get_global_id (1);
  int index = gx * DIM + gy;
  int grain = DIM / get_local_size(0);
  int X_tile = gx / get_local_size(0);
  int Y_tile = gy / get_local_size(1);
  int index_tile = X_tile * grain + Y_tile;

  if (!(gx > 0 && gx < DIM - 1 && gy > 0 && gy < DIM - 1)) return; // pixel du bord
///*
  char changed = 0;
  for (int X = X_tile - 1; X <= X_tile + 1; X++)
    for (int Y = Y_tile - 1; Y <= Y_tile + 1; Y++)
      if (0 <= X && X < grain && 0 <= Y && Y < grain) {
        changed = changed || need_to_change_before[X * grain + Y];
      }
  if (!changed) return; // pas besoin de changer
//*/
	unsigned n = 0;
  int is_alive_before = img[index] != 0;

  for (int i = gy - 1; i <= gy + 1; i++)
    for (int j = gx - 1; j <= gx + 1; j++)
      n += (img[j * DIM + i] != 0);
  n -= is_alive_before;

  int is_alive_after = (n == 3) || (is_alive_before && (n == 2));
  need_to_change_after[index_tile] = (is_alive_after != is_alive_before);

  img_next[index] = 0xFFFF00FF * is_alive_after;
}
