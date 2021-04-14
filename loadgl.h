typedef void *(*loadgl_fn)(const char *);

void loadgl(void);
void loadgl_loader(loadgl_fn loader);
