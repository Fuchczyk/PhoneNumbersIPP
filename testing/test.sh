#!bin/bash

cd rust_generator
cargo run --release $1 >t.in
cd ..
valgrind --leak-check=full ./test <rust_generator/t.in