.PHONY: build

build:
	@echo "Building project..."
	cmake -S . -B build
	cmake --build build
