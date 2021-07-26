#.PHONY main csv all run clean healthcheck test

main: main.cpp
		g++ -c main.cpp

csv: csv_data.cpp
		g++ -c csv_data.cpp

build: main csv
		g++ main.o csv_data.o -o csv_reader

run: build ./csv_reader
		./csv_reader $(ARG)

clean:
		rm *.o
		rm csv_reader

healthcheck:
		@if (g++ --version 1> /dev/null 2> /dev/null); then echo "OK"; else echo "install g++!" | exit; fi

test: build
		./csv_reader tests/file.csv
		./csv_reader tests/cycle.csv
		./csv_reader tests/div_by_zero.csv
		./csv_reader tests/out_of_bounds.csv