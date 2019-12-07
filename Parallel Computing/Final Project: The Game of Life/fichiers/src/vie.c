
#include "compute.h"
#include "debug.h"
#include "global.h"
#include "graphics.h"
#include "ocl.h"
#include "scheduler.h"
#include "monitoring.h"

#include <stdbool.h>

char * need_to_change_before = NULL;
char * need_to_change_after = NULL;

void vie_init() // Will be executed before playing. Usefull for malloc, ...
{
  need_to_change_before = malloc(GRAIN * GRAIN * sizeof(char));
  need_to_change_after = malloc(GRAIN * GRAIN * sizeof(char));
  memset(need_to_change_before, 1, GRAIN * GRAIN * sizeof(char));
  memset(need_to_change_after, 1, GRAIN * GRAIN * sizeof(char));
}

void vie_finalize() // Will be executed after playing. Usefull for free data.
{
  free(need_to_change_before);
  free(need_to_change_after);
}

void vie_refresh_img() // Will be executed at each image refresh.
{
  char * tmp = need_to_change_before;
  need_to_change_before = need_to_change_after;
  need_to_change_after = tmp;
  bzero(need_to_change_after, GRAIN * GRAIN * sizeof(char));
/**
  * Code that did not work because of initialitation issue (-36) of the function 'clEnqueueWriteBuffer'
  *
  *
  cl_int err = 0;
  err |= clEnqueueWriteBuffer(queue, need_to_change_before_buffer, CL_TRUE, 0, sizeof(char) * GRAIN * GRAIN, need_to_change_before, 0, NULL, NULL);
  err |= clEnqueueWriteBuffer(queue, need_to_change_after_buffer, CL_TRUE, 0, sizeof(char) * GRAIN * GRAIN, need_to_change_after, 0, NULL, NULL);
  check (err, "Could not write buffer");
  */
}

// ========================================================================== //
// =============================VERSION BASIQUE============================== //
// ========================================================================== //

static int compute_new_state_basic(int y, int x)
{
  unsigned n      = 0;
  unsigned change = 0;
  int is_alive_before = cur_img(y, x) != 0;

  if (x > 0 && x < DIM - 1 && y > 0 && y < DIM - 1) {
    for (int i = y - 1; i <= y + 1; i++)
      for (int j = x - 1; j <= x + 1; j++)
        n += (cur_img(i, j) != 0);
    n -= is_alive_before;

    if (cur_img (y, x) != 0) {
      if (n == 2 || n == 3)
        n = 0xFFFF00FF;
      else {
        n      = 0;
        change = 1;
      }
    } else {
      if (n == 3) {
        n      = 0xFFFF00FF;
        change = 1;
      } else
        n = 0;
    }

    next_img(y, x) = n;
  }

  return change;
}

static int traiter_tuile_basic(int i_d, int j_d, int i_f, int j_f)
{
  unsigned change = 0;

  PRINT_DEBUG('c', "tuile [%d-%d][%d-%d] traitée\n", i_d, i_f, j_d, j_f);

  for (int i = i_d; i <= i_f; i++)
    for (int j = j_d; j <= j_f; j++)
      change |= compute_new_state_basic(i, j);

  return change;
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_seq_basic(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    unsigned change = traiter_tuile_basic(0, 0, DIM - 1, DIM - 1);

    swap_images();

    if (!change)
      return it;
  }

  return 0;
}

// ========================================================================== //
// =============================VERSION NO JUMP============================== //
// ========================================================================== //

static int compute_new_state_no_jump(int y, int x)
{
  unsigned n      = 0;
  char change = 0;
  int is_alive_before = cur_img(y, x) != 0;

  if (x > 0 && x < DIM - 1 && y > 0 && y < DIM - 1) {
    for (int i = y - 1; i <= y + 1; i++)
      for (int j = x - 1; j <= x + 1; j++)
        n += (cur_img(i, j) != 0);
    n -= is_alive_before;

    int is_alive_after = (n == 3) || (is_alive_before && (n == 2));
    change = (is_alive_after != is_alive_before);
    n = 0xFFFF00FF * is_alive_after;

    next_img(y, x) = n;
  }

  return change;
}

static int traiter_tuile_no_jump(int i_d, int j_d, int i_f, int j_f)
{

  char change = 0;

  PRINT_DEBUG('c', "tuile [%d-%d][%d-%d] traitée\n", i_d, i_f, j_d, j_f);

  for (int i = i_d; i <= i_f; i++)
    for (int j = j_d; j <= j_f; j++)
      change |= compute_new_state_no_jump(i, j);

  return change;
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_seq_no_jump(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    unsigned change = traiter_tuile_no_jump(0, 0, DIM - 1, DIM - 1);

    swap_images();

    if (!change)
      return it;
  }

  return 0;
}

// ========================================================================== //
// ==============================VERSION TUILEE============================== //
// ========================================================================== //

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_seq_tuilee(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    unsigned change = 0;
    unsigned size = DIM / GRAIN;

    for (int i = 0; i < GRAIN; i++) {
      for (int j = 0; j < GRAIN; j++) {
        change |= traiter_tuile_no_jump(i * size, j * size, (i + 1) * size, (j + 1) * size);
      }
    }

    swap_images();

    if (!change)
      return it;
  }

  return 0;
}

// ========================================================================== //
// =========================VERSION TUILEE OPTI============================== //
// ========================================================================== //

unsigned has_changed_tuilee_opti(int i, int j)
{
  char changed = 0;

  for (int a = -1; a <= 1; a++)
    for (int b = -1; b <= 1; b++)
      if (0 <= i + a && i + a < GRAIN && 0 <= j + b && j + b < GRAIN) {
        changed = changed || need_to_change_before[(i + a) * GRAIN + (j + b)];
      }
  return changed;
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_seq_tuilee_opti(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    char change = 0;
    unsigned size = DIM / GRAIN;

    for (int i = 0; i < GRAIN; i++) {
      for (int j = 0; j < GRAIN; j++) {
        if (has_changed_tuilee_opti(i, j)) {
          #ifdef ENABLE_MONITORING
              monitoring_add_tile(0, i * size, DIM, size + 1, i * GRAIN + j);
          #endif
          need_to_change_after[i * GRAIN + j] = traiter_tuile_no_jump(i * size, j * size, (i + 1) * size, (j + 1) * size);
          change |= need_to_change_after[i * GRAIN + j];
        }
      }
    }

    swap_images();

    if (!change)
      return it;
  }

  return 0;
}

// ========================================================================== //
// =====================VERSION BASIQUE OMP FOR============================== //
// ========================================================================== //


static int traiter_tuile_omp_for_basic(int i_d, int j_d, int i_f, int j_f)
{
  unsigned change = 0;

  PRINT_DEBUG('c', "tuile [%d-%d][%d-%d] traitée\n", i_d, i_f, j_d, j_f);
  //  #pragma omp parallel for schedule(dynamic, 1) collapse(2)
  for (int i = i_d; i <= i_f; i++)
    for (int j = j_d; j <= j_f; j++){
      change |= compute_new_state_basic(i, j);
    }

  return change;
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_omp_for_basic(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    unsigned change = traiter_tuile_basic(0, 0, DIM - 1, DIM - 1);

    swap_images();

    if (!change)
      return it;
  }

  return 0;
}

// ========================================================================== //
// =============================VERSION NO JUMP OMP FOR====================== //
// ========================================================================== //


static int traiter_tuile_omp_for_no_jump(int i_d, int j_d, int i_f, int j_f)
{

  unsigned change = 0;

  PRINT_DEBUG('c', "tuile [%d-%d][%d-%d] traitée\n", i_d, i_f, j_d, j_f);
  //#pragma omp parallel for schedule(dynamic, 1) collapse(2)
  for (int i = i_d; i <= i_f; i++)
    for (int j = j_d; j <= j_f; j++){
      change |= compute_new_state_no_jump(i, j);
    }

  return change;
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_omp_for_no_jump(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    unsigned change = traiter_tuile_no_jump(0, 0, DIM - 1, DIM - 1);

    swap_images();

    if (!change)
      return it;
  }

  return 0;
}

// ========================================================================== //
// ==============================VERSION TUILEE OMP FOR====================== //
// ========================================================================== //

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_omp_for_tuilee(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    unsigned change = 0;
    unsigned size = DIM / GRAIN;
    #pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (int i = 0; i < GRAIN; i++) {
      for (int j = 0; j < GRAIN; j++) {
        #ifdef ENABLE_MONITORING
                monitoring_add_tile(j * size, i * size, size, size, omp_get_thread_num());
        #endif
        change |= traiter_tuile_no_jump(i * size, j * size, (i + 1) * size, (j + 1) * size);
      }
    }

    swap_images();

    if (!change)
      return it;
  }

  return 0;
}
//

// ========================================================================== //
// ==============================VERSION TUILEE OMP FOR====================== //
// ========================================================================== //

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_omp_for_tuilee_static(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    unsigned change = 0;
    unsigned size = DIM / GRAIN;
      #pragma omp parallel for schedule(static, 1) collapse(2)
    for (int i = 0; i < GRAIN; i++) {
      for (int j = 0; j < GRAIN; j++) {
        #ifdef ENABLE_MONITORING
                monitoring_add_tile(j * size, i * size, size, size, omp_get_thread_num());
        #endif
        change |= traiter_tuile_no_jump(i * size, j * size, (i + 1) * size, (j + 1) * size);
      }
    }

    swap_images();

    if (!change)
      return it;
  }

  return 0;
}
//
// ========================================================================== //
// =========================VERSION TUILEE OPTI OMP FOR STATIC=============== //
// ========================================================================== //


// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_omp_for_tuilee_opti_static(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    char change = 0;
    unsigned size = DIM / GRAIN;

    #pragma omp parallel for schedule(static, 1) collapse(2)
    for (int i = 0; i < GRAIN; i++) {
      for (int j = 0; j < GRAIN; j++) {
        if (has_changed_tuilee_opti(i, j)) {
          #ifdef ENABLE_MONITORING
                  monitoring_add_tile(j * size, i * size, size, size, omp_get_thread_num());
          #endif
          need_to_change_after[i * GRAIN + j] = traiter_tuile_no_jump(i * size, j * size, (i + 1) * size, (j + 1) * size);
          change |= need_to_change_after[i * GRAIN + j];
        }
      }
    }

    swap_images();

    if (!change)
      return it;
  }

  return 0;
}

//
// ========================================================================== //
// =========================VERSION TUILEE OPTI OMP FOR====================== //
// ========================================================================== //


// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_omp_for_tuilee_opti(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    char change = 0;
    unsigned size = DIM / GRAIN;

    #pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (int i = 0; i < GRAIN; i++) {
      for (int j = 0; j < GRAIN; j++) {
        if (has_changed_tuilee_opti(i, j)) {
          #ifdef ENABLE_MONITORING
                  monitoring_add_tile(j * size, i * size, size, size, omp_get_thread_num());
          #endif
          need_to_change_after[i * GRAIN + j] = traiter_tuile_no_jump(i * size, j * size, (i + 1) * size, (j + 1) * size);
          change |= need_to_change_after[i * GRAIN + j];
        }
      }
    }

    swap_images();

    if (!change)
      return it;
  }

  return 0;
}

// ========================================================================== //
// =========================VERSION TUILEE OMP TASK========================== //
// ========================================================================== //

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned vie_compute_omp_task_tuilee(unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it++) {
    char change = 0;
    #pragma omp parallel
    {
      unsigned size = DIM / GRAIN;

      #pragma omp single
      {
        for (int i = 0; i < GRAIN; i++) {
          for (int j = 0; j < GRAIN; j++) {
            #pragma omp task
            {
              #ifdef ENABLE_MONITORING
                monitoring_add_tile(j * size, i * size, size, size, omp_get_thread_num());
              #endif

              change |= traiter_tuile_no_jump(i * size, j * size, (i + 1) * size, (j + 1) * size);
            }
          }
        }
      }
      #pragma omp taskwait

      #pragma omp single
      {
        swap_images();
      }
    }

    if (!change)
      return it;
  }

  return 0;
}

// ========================================================================== //
// =========================VERSION TUILEE OPTI OMP TASK===================== //
// ========================================================================== //

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0

unsigned vie_compute_omp_task_tuilee_opti(unsigned nb_iter)
{
  //for (unsigned it = 1; it <= nb_iter; it++) {
    //char change = 0;
    #pragma omp parallel
    {
      unsigned size = DIM / GRAIN;

      #pragma omp single
      {
        for (int i = 0; i < GRAIN; i++) {
          for (int j = 0; j < GRAIN; j++) {
#pragma omp task //firstprivate(i,j) depend(out: need_to_change_after[i][j]) depend(in: need_to_change_before[i-1][j-1], need_to_change_before[i-1][j], need_to_change_before[i-1][j+1], need_to_change_before[i][j-1], need_to_change_before[i][j], need_to_change_before[i][j+1], need_to_change_before[i+1][j-1], need_to_change_before[i+1][j], need_to_change_before[i+1][j+1])
            {
              #ifdef ENABLE_MONITORING
                      monitoring_add_tile(j * size, i * size, size, size, omp_get_thread_num());
              #endif

              need_to_change_after[i * GRAIN + j] = traiter_tuile_no_jump(i * size, j * size, (i + 1) * size, (j + 1) * size);
              //change |= need_to_change_after[i * GRAIN + j];
            }
          }
        }
      }
      #pragma omp taskwait

      #pragma omp single
      {
        swap_images();
      }
    //}

    //if (!change)
      //return it;
  }

  return 0;
}

//////////////////////////////////////////////// OpenCL
/*
unsigned call_kernel_tile(int lowerx, int lowery, int upperx, int uppery){
  size_t global[2] = {DIM, DIM};   // global domain size for our calculation
  size_t local[2]  = {DIM / GRAIN, DIM / GRAIN}; // local domain size for our calculation
  cl_int err;
  err = 0;
  err |= clSetKernelArg (compute_kernel, 0, sizeof (cl_mem), &cur_buffer);
  err |= clSetKernelArg (compute_kernel, 1, sizeof (cl_mem), &next_buffer);
  err |= clSetKernelArg (compute_kernel, 2, sizeof (unsigned), &lowerx);
  err |= clSetKernelArg (compute_kernel, 2, sizeof (unsigned), &lowery);
  err |= clSetKernelArg (compute_kernel, 2, sizeof (unsigned), &upperx);
  err |= clSetKernelArg (compute_kernel, 2, sizeof (unsigned), &uppery);
  check (err, "Failed to set kernel arguments");
  err = clEnqueueNDRangeKernel (queue, compute_kernel, 2, NULL, global, local, 0, NULL, NULL);
  check (err, "Failed to execute kernel");
}

unsigned vie_compute_ocl_tuilee (unsigned nb_iter)
{

  for (unsigned it = 1; it <= nb_iter; it++) {

    // Set kernel arguments
    //
    call_kernel_tile(0, 1, 1, 1);
    for (int j = 1; j < GRAIN-1; j++) {
      call_kernel_tile(0, 1, 1, 1);
      call_kernel_tile(1, 0, 1, 1);
      call_kernel_tile(1, 1, 0, 1);
      call_kernel_tile(1, 1, 1, 0);
    }
    for (int i = 1; i < GRAIN-1; i++) {
      for (int j = 1; j < GRAIN-1; j++) {
        int lowerx = 1;
        int lowery = 1;
        int upperx = 1;
        int uppery = 1;
        if (i > 0 && j < GRAIN - 1 && i > 0 && j < GRAIN - 1) {

        }
        if(i > 0)
        err = 0;
        err |= clSetKernelArg (compute_kernel, 0, sizeof (cl_mem), &cur_buffer);
        err |= clSetKernelArg (compute_kernel, 1, sizeof (cl_mem), &next_buffer);
        check (err, "Failed to set kernel arguments");
        err = clEnqueueNDRangeKernel (queue, compute_kernel, 2, NULL, global, local, 0, NULL, NULL);
        check (err, "Failed to execute kernel");
      }
    }
    // Swap buffers
    {
      cl_mem tmp  = cur_buffer;
      cur_buffer  = next_buffer;
      next_buffer = tmp;
    }
    swap_images();
  }

  return 0;
}
*/
unsigned vie_compute_ocl_tuilee (unsigned nb_iter)
{
  size_t global[2] = {DIM, DIM};   // global domain size for our calculation
  size_t local[2]  = {DIM / GRAIN, DIM / GRAIN}; // local domain size for our calculation
  cl_int err = 0;
/**
  * Code that did not work because of initialitation issue (-36) of the function 'clEnqueueWriteBuffer'
  *
  err |= clEnqueueWriteBuffer(queue, need_to_change_before_buffer, CL_TRUE, 0, sizeof(char) * GRAIN * GRAIN, need_to_change_before, 0, NULL, NULL);
  err |= clEnqueueWriteBuffer(queue, need_to_change_after_buffer, CL_TRUE, 0, sizeof(char) * GRAIN * GRAIN, need_to_change_after, 0, NULL, NULL);
  check (err, "Could not write buffer1");
*/
  for (unsigned it = 1; it <= nb_iter; it++) {


    err = 0;
    err |= clSetKernelArg (compute_kernel, 0, sizeof (cl_mem), &cur_buffer);
    err |= clSetKernelArg (compute_kernel, 1, sizeof (cl_mem), &next_buffer);
/**
  * Code that did not work because of initialitation issue (-36) of the function 'clEnqueueWriteBuffer'
  *
    err |= clSetKernelArg (compute_kernel, 2, sizeof (cl_mem), &need_to_change_before_buffer);
    err |= clSetKernelArg (compute_kernel, 3, sizeof (cl_mem), &need_to_change_after_buffer);
  */
    check (err, "Failed to set kernel arguments");
    err = clEnqueueNDRangeKernel (queue, compute_kernel, 2, NULL, global, local, 0, NULL, NULL);
    check (err, "Failed to execute kernel");
  }
    // Swap buffers
  {
    cl_mem tmp  = cur_buffer;
    cur_buffer  = next_buffer;
    next_buffer = tmp;
  }
  swap_images();

  return 0;
}

///////////////////////////// Configuration initiale

void draw_stable (void);
void draw_guns (void);
void draw_random (void);
void draw_clown (void);
void draw_diehard (void);

void vie_draw (char *param)
{
  char func_name[1024];
  void (*f) (void) = NULL;

  if (param == NULL)
    f = draw_guns;
  else {
    sprintf (func_name, "draw_%s", param);
    f = dlsym (DLSYM_FLAG, func_name);

    if (f == NULL) {
      PRINT_DEBUG ('g', "Cannot resolve draw function: %s\n", func_name);
      f = draw_guns;
    }
  }

  f ();
}

static unsigned couleur = 0xFFFF00FF; // Yellow

static void gun (int x, int y, int version)
{
  bool glider_gun[11][38] = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
       0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
       0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
      {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
       0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1,
       0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
       0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  if (version == 0)
    for (int i = 0; i < 11; i++)
      for (int j = 0; j < 38; j++)
        if (glider_gun[i][j])
          cur_img (i + x, j + y) = couleur;

  if (version == 1)
    for (int i = 0; i < 11; i++)
      for (int j = 0; j < 38; j++)
        if (glider_gun[i][j])
          cur_img (x - i, j + y) = couleur;

  if (version == 2)
    for (int i = 0; i < 11; i++)
      for (int j = 0; j < 38; j++)
        if (glider_gun[i][j])
          cur_img (x - i, y - j) = couleur;

  if (version == 3)
    for (int i = 0; i < 11; i++)
      for (int j = 0; j < 38; j++)
        if (glider_gun[i][j])
          cur_img (i + x, y - j) = couleur;
}

void draw_stable (void)
{
  for (int i = 1; i < DIM - 2; i += 4)
    for (int j = 1; j < DIM - 2; j += 4)
      cur_img (i, j) = cur_img (i, (j + 1)) = cur_img ((i + 1), j) =
          cur_img ((i + 1), (j + 1))        = couleur;
}

void draw_guns (void)
{
  memset (&cur_img (0, 0), 0, DIM * DIM * sizeof (cur_img (0, 0)));

  gun (0, 0, 0);
  gun (0, DIM - 1, 3);
  gun (DIM - 1, DIM - 1, 2);
  gun (DIM - 1, 0, 1);
}

void draw_random (void)
{
  for (int i = 1; i < DIM - 1; i++)
    for (int j = 1; j < DIM - 1; j++)
      cur_img (i, j) = random () & 01;
}

void draw_clown (void)
{
  memset (&cur_img (0, 0), 0, DIM * DIM * sizeof (cur_img (0, 0)));

  int mid                = DIM / 2;
  cur_img (mid, mid - 1) = cur_img (mid, mid) = cur_img (mid, mid + 1) =
      couleur;
  cur_img (mid + 1, mid - 1) = cur_img (mid + 1, mid + 1) = couleur;
  cur_img (mid + 2, mid - 1) = cur_img (mid + 2, mid + 1) = couleur;
}

void draw_diehard (void)
{
  memset (&cur_img (0, 0), 0, DIM * DIM * sizeof (cur_img (0, 0)));

  int mid = DIM / 2;

  cur_img (mid, mid - 3) = cur_img (mid, mid - 2) = couleur;
  cur_img (mid + 1, mid - 2)                      = couleur;

  cur_img (mid - 1, mid + 3)     = couleur;
  cur_img (mid + 1, mid + 2)     = cur_img (mid + 1, mid + 3) =
      cur_img (mid + 1, mid + 4) = couleur;
}
