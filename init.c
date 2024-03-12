#include "inc/main.h"

const char *font_str = "-*-dejavu sans mono-medium-r-normal-*-12-*-*-*-*-*-iso10646-1";
struct sigaction sa;

void kill_xserver(const char *err){
  printf("%c\n", *err);
  exit(EXIT_FAILURE);
}

int main() {
  Display* display;
  if (!(display = XOpenDisplay(NULL))) {
      kill_xserver("Could not create X display");
  }
  create_sig_handlers();
  Display_Context display_context;
  int screen = create_screen(display);
  Window root = create_root(display, screen);
  int dpy_scr_width = set_screen_w(display, screen);
  int dpy_scr_height = set_screen_h(display, screen);
  create_fonts(font_str, &display_context);
  return 0;
}

void create_sig_handlers(void){
  sig_chld(0);
  sig_segv();
}

void sig_segv(){
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = segv_handler;
  if(sigaction(SIGSEGV, &sa, NULL) == -1){
    perror("SIG SEGV ACTIONED");
  }
}

void segv_handler(int signum, siginfo_t *info, void *context){
  printf("Faulting address %p\n", info->si_addr);
  void *buffer[10];
  int nptrs = backtrace(buffer, 10);
  backtrace_symbols_fd(buffer, nptrs, STDERR_FILENO);

  printf("----------------------------\n");
  kill_xserver("SIG SEGV BACKTRACE PRINTED\n");
  printf("----------------------------\n");
}

void sig_chld(int unused) {
  if (signal(SIGCHLD, sig_chld) == SIG_ERR) {
      kill_xserver("Can't use SIGCHLD handler");
  }
  while (0 < waitpid(-1, NULL, WNOHANG));
}

int create_screen(Display *display) {
  int screen = DefaultScreen(display);
  return screen;
}

Window create_root(Display *display, int screen){
  Window root = RootWindow(display, screen);
  return root;
}


int set_screen_w(Display *display, int screen){
  int screen_width = DisplayWidth(display, screen);
  return screen_width;
}

int set_screen_h(Display *display, int screen){
  int screen_height = DisplayHeight(display, screen);
  return screen_height;
}

void create_fonts(const char *font_str, Display_Context *display_context){

}

