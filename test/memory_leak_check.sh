#!/usr/bin/env bash
valgrind --leak-check=full --show-leak-kinds=all  --track-origins=yes --verbose --log-file=valgrind-out.txt ./test.out ../thread-pool.h ../thread-pool.c test.c