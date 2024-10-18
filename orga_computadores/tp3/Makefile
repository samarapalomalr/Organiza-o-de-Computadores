# Nome do executável
EXEC = tp3

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g3

# Arquivos fonte
SRC = tp3.c \
      tratador_interrupcao.c \
      memoriaExterna_binario.c

# Regra padrão para compilar o executável
all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

# Limpeza dos arquivos gerados
clean:
	rm -f $(EXEC) *.o
