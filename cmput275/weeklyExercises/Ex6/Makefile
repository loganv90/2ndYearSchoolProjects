all: student_hash complex_postfix

student_hash: student_hash.o
	g++ student_hash.o -o student_hash

student_hash.o: student_hash.cpp
	g++ student_hash.cpp -o student_hash.o -c -Wall

complex_postfix: complex_postfix.o
	g++ complex_postfix.o -o complex_postfix

complex_postfix.o: complex_postfix.cpp
	g++ complex_postfix.cpp -o complex_postfix.o -c -Wall

clean:
	rm -f student_hash.o student_hash complex_postfix.o complex_postfix