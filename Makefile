.SILENT: message


parse:
	python3 ./state_machines/parser.py ./state_machines/automata_compiler.jff > ./data/transition_matrix.csv
all:
	gcc-15 -o ./main ./src/main.c ./src/transition_matrix_creator.c ./src/hash_table.c ./src/procedures.c -g

message:
	echo output saved to 'output.txt':

run_analyser: all
	./main  $(FILE_PATH)

run:    run_analyser message

run_val:    all
	valgrind --leak-check=full --show-leak-kinds=all ./main $(FILE_PATH)
