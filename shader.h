#ifdef _WIN32
#define _(x) x
#else
#define _(x) _##x
#endif

#define SHADER_EXTERN(s) \
	extern char _(binary_##s##_vert_start); \
	extern char _(binary_##s##_vert_end); \
	extern char _(binary_##s##_frag_start); \
	extern char _(binary_##s##_frag_end);

#define SHADER_COMPILE(s) shader_compile(#s, \
	&_(binary_##s##_vert_start), \
	&_(binary_##s##_vert_end) - &_(binary_##s##_vert_start), \
	&_(binary_##s##_frag_start), \
	&_(binary_##s##_frag_end) - &_(binary_##s##_frag_start))

#define SHADER_UNIFORM(u) shader_uniform(#u)

GLuint shader_compile(char *name,
		const char *vert_code, int vert_size,
		const char *frag_code, int frag_size);

GLuint shader_uniform(char *name);
