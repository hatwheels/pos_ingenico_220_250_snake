/*
 * snake.c
 *
 *  Created on: Mar 10, 2017
 *      Author: kmarkostamos
 */

#include "inc/snake.h"

static const long difficulty[6] = {100, 90, 80, 70, 60, 50};
static size_t user_diff = 2;

static long snake_get_difficulty (void)
{
  return difficulty[user_diff];
}

static void snake_set_difficulty (size_t new_diff)
{
  user_diff = new_diff;
}

static void snake_rng_pixel (blob *pixel, blob *player, unsigned int* size)
{
  int x = rand () % Xmax;
  int y = rand () % Ymax;
  int idx;

  for (idx = 0; idx < *size; idx++)
  {
    if (x == Xmin || y == Xmin || (x == player[idx].xs && y == player[idx].ys))
    {
      idx = -1;
      x = rand () % Xmax;
      y = rand () % Ymax;
    }
  }
  pixel->xs = x;
  pixel->ys = y;
}

static blob* snake_pixel_hit (blob *player, unsigned int *size, blob* pixel, blob* buffer)
{
  if (player[*size - 1].xs == pixel->xs && player[*size - 1].ys == pixel->ys)
  {
    int idx;
    blob *temp = (blob *) realloc (player, ++(*size) * sizeof (blob));

    if (temp == NULL)
    {
      free (player);
      return NULL;
    }
    player = temp;
    player[*size - 1].xs = player[*size - 2].xs;
    player[*size - 1].ys = player[*size - 2].ys;
    player[*size - 1].xe =  player[*size - 1].ye = 0;
    for (idx = *size - 2; idx > 0; idx--)
    {
      player[idx].xs = player[idx - 1].xs;
      player[idx].ys = player[idx - 1].ys;
    }
    player[0].xs = buffer->xs;
    player[0].ys = buffer->ys;
    _SetPixel (pixel->xs, pixel->ys, _OFF_);
    snake_rng_pixel (pixel, player, size);
    SetPixel (pixel->xs, pixel->ys, _ON_);
  }

  return player;
}

static unsigned char snake_collision (blob* player, unsigned int * size)
{
  unsigned int idx;

  if (player[*size - 1].xs == Xmin || player[*size - 1].xs == Xmax ||
        player[*size - 1].ys == Ymin || player[*size - 1].ys == Ymax)
    return TRUE;

  for (idx = 0; idx < *size - 1; idx++)
  {
    if (player[*size - 1].xs == player[idx].xs && player[*size - 1].ys == player[idx].ys)
      return TRUE;
  }
  return FALSE;
}

static void snake_step (blob* player, unsigned int* size, blob* direction)
{
  int idx;

  for (idx = 0; idx < *size; idx++)
    _SetPixel (player[idx].xs, player[idx].ys, _OFF_);
  RefreshUserArea (Xmin, Xmax, Ymin, Ymax);
  for (idx = 0; idx < *size - 1; idx++) {
    player[idx].xs = player[idx + 1].xs;
    player[idx].ys = player[idx + 1].ys;
    _SetPixel (player[idx].xs, player[idx].ys, _ON_);
  }
  player[*size - 1].xs += direction->xs;
  player[*size - 1].ys += direction->ys;
  _SetPixel (player[*size - 1].xs, player[*size - 1].ys, _ON_);
  RefreshUserArea (Xmin, Xmax, Ymin, Ymax);
}

static int snake_main (timer* tm, const blob* wall, blob* player, unsigned int* size, blob* pixel) {
  blob direction = {1, 0, 0, 0};
  blob buffer = {0, 0, 0, 0};

  for (;;) {
    /// next frame
    if (timer_haselapsed (tm)) {

      /// update step
      buffer.xs = player[0].xs;
      buffer.ys = player[0].ys;
      snake_step (player, size, &direction);

      /// collision detection
      if (snake_collision (player, size))
      {
        char menu[2][DISPLAY_COL + 1] = {"RESTART", "QUIT"};

        display_publish (0, "GAME OVER!", 0, 0, DP_INFO);
        ttestall (0, 100);

        return (execute_menu ("GAME OVER!", (char *) menu, 2, 1, MF_NUMBERED | MF_MAIN |  MF_NO_TIMEOUT) % 2);
      }

      /// pixel hit detection
      if ((player = snake_pixel_hit (player, size, pixel, &buffer)) == NULL)
        return 1;

      timer_start (tm, snake_get_difficulty ());
    }

    /// Key pressed
    if (keyboard_hit ()) {
      switch (keyboard_getchar ()) {
        case CH_CANCEL: /// Cancel
          display_clear (0);
          return 0;

        ///Pause
        case CH_BACKSP: {
          char menu[4][DISPLAY_COL + 1] = {"CONTINUE", "DIFFICULTY", "RESTART", "QUIT"};
          unsigned int idx;

          display_publish (0, "PAUSE", 0, 0, DP_INFO);
          ttestall (0, 100);

          switch (execute_menu ("PAUSE", (char *) menu, 4, 1, MF_NUMBERED | MF_MAIN |  MF_NO_TIMEOUT)) {
            case 1: ///Continue
              break;
            case 2: ///Difficulty
              {
                char menu[6][DISPLAY_COL + 1] = {"VERY EASY", "EASY", "NORMAL", "HARD", "VERY HARD", "INSANE"};
                int ret = execute_menu ("DIFFICULTY", (char *) menu, 6, 1, MF_NUMBERED | MF_MAIN |  MF_NO_TIMEOUT);

                if (ret > 0 && ret < 7)
                  snake_set_difficulty (ret - 1);
              }
              break;

            case 3: ///Restart
              return 1;

            case 4: ///Exit
              return 0;
          }
          display_clear (0);
          _SetPixel (pixel->xs, pixel->ys, _ON_);
          for (idx = 0; idx < *size; idx++)
            _SetPixel (player[idx].xs, player[idx].ys, _ON_);
          _DrawLine (wall[0].xs, wall[0].ys, wall[0].xe, wall[0].ye, _ON_);
          _DrawLine (wall[1].xs, wall[1].ys, wall[1].xe, wall[1].ye, _ON_);
          _DrawLine (wall[2].xs, wall[2].ys, wall[2].xe, wall[2].ye, _ON_);
          DrawLine (wall[3].xs, wall[3].ys, wall[3].xe, wall[3].ye, _ON_);

          break;
        }

        case CH_2: //UP
          if (!(direction.xs == 0 && direction.ys == 1)) {
            direction.xs = 0;
            direction.ys = -1;
          }
          break;

        case CH_8: //DOWN
          if (!(direction.xs == 0 && direction.ys == -1)) {
            direction.xs = 0;
            direction.ys = 1;
          }
          break;

        case CH_4: //LEFT
          if (!(direction.xs == 1 && direction.ys == 0)) {
            direction.xs = -1;
            direction.ys = 0;
          }
          break;

        case CH_6: //RIGHT
          if (!(direction.xs == -1 && direction.ys == 0)) {
            direction.xs = 1;
            direction.ys = 0;
          }
          break;

        default:
          break;
      }
    }
  }
}

void snake_init (void) {
  for (;;) {
    const blob wall[4] = {
        { Xmin, Ymin, Xmax, Ymin }, // Wall, Up
        { Xmin, Ymax, Xmax, Ymax }, // Wall, Down
        { Xmin, Ymin, Xmin, Ymax }, // Left Wall
        { Xmax, Ymin, Xmax, Ymax }  // Right Wall
    };
    timer tm;
    unsigned int size = 10;
    blob *player = (blob *) malloc (size * sizeof (blob));
    blob buf[10] = {
        {57, 32, 0, 0}, {58, 32, 0, 0}, {59, 32, 0, 0}, {60, 32, 0, 0}, {61, 32, 0, 0},
        {62, 32, 0, 0}, {63, 32, 0, 0}, {64, 32, 0, 0}, {65, 32, 0, 0}, {66, 32, 0, 0}
    };
    blob pixel = {0, 0, 0, 0};
    unsigned int idx;

    memcpy (player, buf, size * sizeof (blob));
    srand (OS_ClkGetMilliseconds ());
    rand ();
    snake_rng_pixel (&pixel, player, &size);
    display_clear (0);
    _SetPixel (pixel.xs, pixel.ys, _ON_);
    for (idx = 0; idx < size; idx++)
      _SetPixel (player[idx].xs, player[idx].ys, _ON_);
    _DrawLine (wall[0].xs, wall[0].ys, wall[0].xe, wall[0].ye, _ON_);
    _DrawLine (wall[1].xs, wall[1].ys, wall[1].xe, wall[1].ye, _ON_);
    _DrawLine (wall[2].xs, wall[2].ys, wall[2].xe, wall[2].ye, _ON_);
    DrawLine (wall[3].xs, wall[3].ys, wall[3].xe, wall[3].ye, _ON_);

    timer_start (&tm, snake_get_difficulty ());

    if (snake_main (&tm, wall, player, &size, &pixel) == 0)
      break;
  }
  snake_set_difficulty (2);
}
