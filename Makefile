TERMINAL ?= ghostty
.PHONY: dev run test build

dev:
	cmake -S . -B build-hot -DCMAKE_BUILD_TYPE=Release -DWHEEL_OF_FEAR_HOT_RELOAD=ON
	cmake --build build-hot -j
	$(TERMINAL) -e ./watch.sh $$$$ & \
	exec ./build-hot/wheel_of_fear

run: build
	./build/wheel_of_fear

test: build
	./build/wheel_of_fear_tests

build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build -j
