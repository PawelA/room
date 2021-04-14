# Generated using make_deps
$(MAIN): alloc.o buffer.o fps.o gl.o lin.o loadgl.o main.o model.o shader.o depth.o phong.o 
alloc.o: alloc.c 
buffer.o: buffer.c gl.h 
fps.o: fps.c 
gl.o: gl.c gl.h 
lin.o: lin.c lin.h 
loadgl.o: loadgl.c gl.h 
loadgl_win32.o: loadgl_win32.c loadgl.h 
loadgl_wl.o: loadgl_wl.c loadgl.h 
loadgl_x11.o: loadgl_x11.c loadgl.h 
main.o: main.c gl.h loadgl.h shader.h window.h keys.h lin.h fps.h model.h 
model.o: model.c model.h alloc.h gl.h 
shader.o: shader.c gl.h 
window_win32.o: window_win32.c window.h keys.h 
window_wl.o: window_wl.c window.h 
window_x11.o: window_x11.c window.h keys.h 
depth.o: depth.vert depth.frag
phong.o: phong.vert phong.frag
ifeq ($(ARCH), x11)
$(MAIN): loadgl_x11.o window_x11.o 
loadgl_x11.o: loadgl_x11.c loadgl.h 
window_x11.o: window_x11.c window.h keys.h 
endif
ifeq ($(ARCH), win32)
$(MAIN): loadgl_win32.o window_win32.o 
loadgl_win32.o: loadgl_win32.c loadgl.h 
window_win32.o: window_win32.c window.h keys.h 
endif
ifeq ($(ARCH), wl)
$(MAIN): loadgl_wl.o window_wl.o 
loadgl_wl.o: loadgl_wl.c loadgl.h 
window_wl.o: window_wl.c window.h 
endif
