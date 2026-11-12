##
## EPITECH PROJECT, 2026
## teck_reseau_arena
## File description:
## Raccourcis autour de CMake
##

all:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build -j

clean:
	rm -rf build

fclean: clean
	rm -f arena_server arena_client

re: fclean all

tests_run: all
	python3 tests/e2e_test.py

.PHONY: all clean fclean re tests_run
