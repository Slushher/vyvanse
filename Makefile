.PHONY: cmake compile run clean

cmake:
	cmake -B cmake

compile:
	cmake --build cmake

multi:
	$(MAKE) compile -j 16

run:
	./bin/vyvanse

clean:
	rm cmake -rf