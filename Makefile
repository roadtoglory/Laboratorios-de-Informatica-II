etapa3: main.c logica.c interface.c leitor.c
	gcc -o etapa3 main.c logica.c interface.c leitor.c

clean:
	rm -f etapa3 testes *.o save.txt
