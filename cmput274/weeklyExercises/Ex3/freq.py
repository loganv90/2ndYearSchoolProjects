"""
Name: LOGAN VAUGHAN
ID #:  1577489
CMPUT  274  EA1

Exercise  3: Word Frequency
"""

import sys


def error_check(arguments):
	# If the user does not enter a second argument (for the text file) when calling this
	# program or enters too many arguments then this function will print an error message 
	# and end the program.
	#
	# Input variables: arguments is the number of input arguments entered by the user
	# Return: no return. If there's an error this function will print to the console

	if arguments > 2:
		print("Too many input arguments!")
		print("You can only call upon one text file when runing this program.")
		print("To successfully run this program use this command: python3 freq.py <textfile>")
		print("Where <textfile> is the name of a text file in the same directory as freq.py")
		sys.exit()
	elif arguments < 2:
		print("Too few input arguments!")
		print("You must call upon a text file when runing this program.")
		print("To successfully run this program use this command: python3 freq.py <textfile>")
		print("Where <textfile> is the name of a text file in the same directory as freq.py")
		sys.exit()


def read_file(filename):
	# Reads the words out of a text file and puts them in a dictionary along with the 
	# number of times each word was used in the file.
	#
	# Input variables: filename is the file that will be analysed by the funciton
	# Return: dictionary is a dictionary containing the words of the text file and the
	#		  number of times each word was used.
	#		  wordcount is the total number of words in the text file.

	fin = open(filename, "r")

	dictionary = {}
	wordcount = 0
	for line in fin:
		words = line.split()
		for i in range(len(words)):
			if words[i] in dictionary:
				dictionary[words[i]] += 1
			else:
				dictionary[words[i]] = 1
			wordcount += 1

	fin.close()

	return dictionary, wordcount


def write_output(d, infile, totalwords):
	# Creates a new text file with the same name as the original but with .out appended
	# to the end. In this file this function writes a alphabetically sorted list of all 
	# the words in the original file and sorts then from low to high using the ASCII
	# character set. Each line in the new file contains a word followed by the number of 
	# times it was used in the original file and then the frequency of its usage in the
	# original file.
	#
	# Input variables: d is a dictionary containing the words from original file and the
	#				   number of times they were used.
	#				   infile is the name of the original file
	#				   totalwords is the total number of words from the original file
	# Return: no return. This function only creates a text file

	fout = open(infile + ".out", "w")

	itemlist = [(0, 0)] * len(d)
	itemlist = sorted(d.items())

	for i in range(len(d)):
		fout.write("{} {} {}\n".format(itemlist[i][0], itemlist[i][1], round(itemlist[i][1]/totalwords, 3)))

	fout.close()


if __name__ == "__main__":

	error_check(len(sys.argv))

	dict_of_words, number_of_words = read_file(sys.argv[1])

	write_output(d = dict_of_words, infile = sys.argv[1], totalwords = number_of_words)