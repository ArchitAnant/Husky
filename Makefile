.PHONY: all run clean

all:
	cmake -B build
	cmake --build build

run: all
	cmake --build build --target run

clean:
	rm -rf build
	rm -f os.elf
