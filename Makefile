make:
	gcc -I./include -o game src/main.c -lraylib -lraygui -lGL -lm -lpthread -ldl -lrt -lX11 -lcurl -lzip

run:
	./game

clean:
	rm game

