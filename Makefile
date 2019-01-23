PLAT=none
PLATS=mingw linux chip8 emscripten

EXTENSIONS=BaseExtension

export SRC=src/*.cpp $(foreach ext,$(EXTENSIONS),src/$(ext)/*.cpp)
export SRC_H=src/*.h $(foreach ext,$(EXTENSIONS),src/$(ext)/*.h)

all: $(PLAT)

$(PLATS) clean:
	$(MAKE) -f Makefile.$@

none:
	@echo Build with 'make PLATFORM'. The supported platforms are:
	@echo   $(PLATS)
