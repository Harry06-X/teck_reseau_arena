#!/bin/bash

echo -e "[\033[36mINFO\033[0m] Nettoyage du projet Arena Command..."

echo -e " -> Suppression des exécutables \033[33m(arena_server, arena_client)\033[0m"
rm -f arena_server arena_client

echo -e " -> Suppression du dossier de compilation \033[33m(build/)\033[0m"
rm -rf build

echo -e "[\033[32mOK\033[0m] Le répertoire est propre !"
