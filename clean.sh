#!/bin/bash

echo -e "[\033[36mINFO\033[0m] Lancement du nettoyage du projet Arcade..."

echo -e " -> Suppression de l'exécutable principal \033[33m(arcade)\033[0m"
rm -f arcade

echo -e " -> Suppression des librairies partagées générées \033[33m(lib/*.so)\033[0m"
rm -f lib/*.so

echo -e " -> Suppression de tous les dossiers de compilation \033[33m(build/)\033[0m"
find . -type d -name "build" -exec rm -rf {} +

echo -e " -> Nettoyage des caches CMake résiduels éventuels..."
find . -type d -name "CMakeFiles" -exec rm -rf {} +
find . -type f -name "CMakeCache.txt" -delete
find . -type f -name "cmake_install.cmake" -delete
find . -type f -name "Makefile" -delete
find . -type f -name "score.txt" -delete

echo -e "[\033[32mOK\033[0m] Le répertoire est totalement propre !"
