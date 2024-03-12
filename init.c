#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
int main() {

  xcb_connection_t *connection;
  xcb_screen_t *screen = NULL;
  int screen_nbr;
  xcb_screen_iterator_t iterator;

  connection = xcb_connect(NULL, &screen_nbr);
  iterator = xcb_setup_roots_iterator(xcb_get_setup(connection));
  for (; iterator.rem; --screen_nbr, xcb_screen_next(&iterator)) {
    if (screen_nbr == 0) {
      screen = iterator.data;
      break;
    }
  }

  printf("\n");
  printf("SCREEN : %ud\n", screen->root);
  printf("WIDTH : %d\n", screen->width_in_pixels);
  printf("HEIGHT : %d\n", screen->height_in_pixels);
  printf("WHITE PIXEL : %ud\n", screen->white_pixel);
  printf("BLACK PIXEL : %ud\n", screen->black_pixel);
  printf("\n");

  uint32_t mask = 0;
  uint32_t valwin[2];

  xcb_window_t window;
  window = xcb_generate_id(connection);
  xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0,
                    0, screen->width_in_pixels, screen->height_in_pixels, 0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, 0,
                    NULL);

  xcb_map_window(connection, window);
  xcb_flush(connection);

  xcb_generic_event_t *generic_event;
  while ((generic_event = xcb_wait_for_event(connection))) {
    switch (generic_event->response_type & ~0x80) {
    case XCB_EXPOSE: {
      xcb_expose_event_t *exp_event = (xcb_expose_event_t *)generic_event;
      break;
    }
    case XCB_BUTTON_PRESS: {
      xcb_button_press_event_t *btn_event =
          (xcb_button_press_event_t *)generic_event;
      break;
    }
    default: {
      break;
    }
    }
    free(generic_event);
  }

  return 0;
}
