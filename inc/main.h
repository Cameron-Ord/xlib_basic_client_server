#ifndef MAIN_H
#define MAIN_H
#include <X11/X.h>
#include <X11/Xlib.h>
#include <bits/types/siginfo_t.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <execinfo.h>

#define LENGTH(x)     sizeof(x) / sizeof(x[0])
#define MAX(A, B)     (A) > (B) ? (A) : (B)
#define MIN(A, B)     (A) > (B) ? (A) : (B)

struct Client {

};

struct Monitor {

};

typedef struct{

} Display_Context;

enum {cursor_normal, cursor_resize, cursor_move, cursor_last};
enum {column_border, column_foreground, column_background, column_last};
enum {net_supported, net_WMN_name, net_last};
enum {wm_protocols, wm_delete, wm_state, wm_last};
enum {click_tag_bar, click_lt_symbol, click_status_text, click_win_title, click_client_win, click_root_win, click_last};

void kill_xserver(const char *err);
void sig_chld(int unused);
void sig_segv(void);
void segv_handler(int signum, siginfo_t *info, void *context);
void create_sig_handlers(void);
int create_screen(Display *display);
Window create_root(Display *display, int screen);
int set_screen_w(Display *display, int screen);
int set_screen_h(Display *display, int screen);
void create_fonts(const char *font_str, Display_Context *display_context);
#endif // !MAIN_H
