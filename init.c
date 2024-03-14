#include <stdio.h>
#include <stdlib.h>

#include <xcb/xcb.h>

void print_modifiers(uint32_t mask) {
  const char **mod,
      *mods[] = {"Shift",   "Lock",    "Ctrl",   "Alt",     "Mod2",
                 "Mod3",    "Mod4",    "Mod5",   "Button1", "Button2",
                 "Button3", "Button4", "Button5"};
  printf("Modifier mask: ");
  for (mod = mods; mask; mask >>= 1, mod++)
    if (mask & 1)
      printf("MOD : %s", *mod);
  putchar('\n');
}

/*
 Entry of code. A good thing to remember is that Xorg is largely limited to
 int16_t for performance reasons
 */
int main() {
  xcb_connection_t *c;
  xcb_screen_t *screen;
  xcb_window_t win;
  xcb_gcontext_t foreground;
  xcb_generic_event_t *e;
  uint32_t mask = 0;
  uint32_t values[2];

  /* geometric objects */
  xcb_point_t points[] = {{10, 10}, {10, 20}, {20, 10}, {20, 20}};

  xcb_point_t polyline[] = {{50, 10},
                            {5, 20}, /* rest of points are relative */
                            {25, -20},
                            {10, 10}};

  xcb_segment_t segments[] = {{100, 10, 140, 30}, {110, 25, 130, 60}};

  xcb_rectangle_t rectangles[] = {{10, 50, 40, 20}, {80, 50, 10, 40}};

  xcb_arc_t arcs[] = {{10, 100, 60, 40, 0, 90 << 6},
                      {90, 100, 55, 40, 0, 270 << 6}};

  /* Open the connection to the X server */
  c = xcb_connect(NULL, NULL);

  /* Get the first screen */
  screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
  win = screen->root;

  /* Graphic context creation*/
  foreground = xcb_generate_id(c);
  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = screen->black_pixel;
  values[1] = 0;
  xcb_create_gc(c, foreground, win, mask, values);

  /* Ask for our window's Id */
  win = xcb_generate_id(c);

  /* Create the window */
  /* Setting masks using bitwise operators */
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = screen->white_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
              XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
              XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
              XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;
  xcb_create_window(c,                             /* Connection          */
                    0,                             /* depth               */
                    win,                           /* window Id           */
                    screen->root,                  /* parent window       */
                    0, 0,                          /* x, y                */
                    150, 150,                      /* width, height       */
                    10,                            /* border_width        */
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                    screen->root_visual,           /* visual              */
                    mask, values);                 /* masks */

  /* Map the window on the screen */
  xcb_map_window(c, win);

  xcb_flush(c);

  while ((e = xcb_wait_for_event(c))) {
    switch (e->response_type & ~0x80) {
    case XCB_EXPOSE: {
      xcb_expose_event_t *ev = (xcb_expose_event_t *)e;
      xcb_poly_point(c, XCB_COORD_MODE_ORIGIN, win, foreground, 4, points);

      /* We draw the polygonal line */
      xcb_poly_line(c, XCB_COORD_MODE_PREVIOUS, win, foreground, 4, polyline);

      /* We draw the segements */
      xcb_poly_segment(c, win, foreground, 2, segments);

      /* We draw the rectangles */
      xcb_poly_rectangle(c, win, foreground, 2, rectangles);

      /* We draw the arcs */
      xcb_poly_arc(c, win, foreground, 2, arcs);

      /* We flush the request */
      xcb_flush(c);
      printf("Window %d exposed. Region to be redrawn at location (%d,%d), "
             "with dimension (%d,%d)\n",
             ev->window, ev->x, ev->y, ev->width, ev->height);
      break;
    }
    case XCB_BUTTON_PRESS: {
      xcb_button_press_event_t *ev = (xcb_button_press_event_t *)e;
      print_modifiers(ev->state);

      switch (ev->detail) {
      case 4:
        printf("Wheel Button up in window %d, at coordinates (%d,%d)\n",
               ev->event, ev->event_x, ev->event_y);
        break;
      case 5:
        printf("Wheel Button down in window %d, at coordinates (%d,%d)\n",
               ev->event, ev->event_x, ev->event_y);
        break;
      default:
        printf("Button %d pressed in window %d, at coordinates (%d,%d)\n",
               ev->detail, ev->event, ev->event_x, ev->event_y);
      }
      break;
    }
    case XCB_BUTTON_RELEASE: {
      xcb_button_release_event_t *ev = (xcb_button_release_event_t *)e;
      print_modifiers(ev->state);

      printf("Button %d released in window %d, at coordinates (%d,%d)\n",
             ev->detail, ev->event, ev->event_x, ev->event_y);
      break;
    }
    case XCB_MOTION_NOTIFY: {
      xcb_motion_notify_event_t *ev = (xcb_motion_notify_event_t *)e;

      printf("Mouse moved in window %d, at coordinates (%d,%d)\n", ev->event,
             ev->event_x, ev->event_y);
      break;
    }
    case XCB_ENTER_NOTIFY: {
      xcb_enter_notify_event_t *ev = (xcb_enter_notify_event_t *)e;

      printf("Mouse entered window %d, at coordinates (%d,%d)\n", ev->event,
             ev->event_x, ev->event_y);
      break;
    }
    case XCB_LEAVE_NOTIFY: {
      xcb_leave_notify_event_t *ev = (xcb_leave_notify_event_t *)e;

      printf("Mouse left window %d, at coordinates (%d,%d)\n", ev->event,
             ev->event_x, ev->event_y);
      break;
    }
    case XCB_KEY_PRESS: {
      xcb_key_press_event_t *ev = (xcb_key_press_event_t *)e;
      print_modifiers(ev->state);

      printf("Key pressed in window %d\n", ev->event);
      break;
    }
    case XCB_KEY_RELEASE: {
      xcb_key_release_event_t *ev = (xcb_key_release_event_t *)e;
      print_modifiers(ev->state);

      printf("Key released in window %d\n", ev->event);
      break;
    }
    default:
      /* Unknown event type, ignore it */
      printf("Unknown event: %d\n", e->response_type);
      break;
    }
    /* Free the Generic Event */
    free(e);
  }

  return 0;
}
