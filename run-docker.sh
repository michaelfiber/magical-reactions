#!/usr/bin/env bash

docker run --rm -v $(pwd):/app/project -u $(id -u):$(id -g) michaelfiber/mikes-raylib-wasm-builder:latest