#define WINDOW_REGULAR    1
#define WINDOW_FLOATING   2
#define WINDOW_FULLSCREEN 3

#define WINDOW_KEY_DOWN    1
#define WINDOW_KEY_UP      2
#define WINDOW_BUTTON_DOWN 3
#define WINDOW_BUTTON_UP   4
#define WINDOW_MOUSE_MOVE  5
#define WINDOW_FOCUS       6
#define WINDOW_RESIZE      7
#define WINDOW_CLOSE       8

struct window_evt {
	int key;
	int button;
	int x;
	int y;
	int focused;
	int width;
	int height;
	int type;
};

int window_init(void);
int window_create(int width, int height, int mode);

void window_map(void);
void window_unmap(void);
void window_swap_buffers(void);

void window_show_cursor(void);
void window_hide_cursor(void);
void window_trap_cursor(void);
void window_untrap_cursor(void);
void window_move_cursor(int x, int y);

int window_get_evt(struct window_evt *evt);
