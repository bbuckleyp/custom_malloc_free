default:
	gcc -g -std=gnu99 -o project2 -Wall -Wextra -Wno-unused-parameter psumemory.c large_asymmetric_worst.c
clean:
	rm project2
