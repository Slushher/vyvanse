.PHONY: cmake compile run clean

cmake:
	cmake -B cmake

compile:
	cmake --build cmake

run:
	./bin/vyvanse

clean:
	rm cmake -rf