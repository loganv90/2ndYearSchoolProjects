"""
Name: LOGAN VAUGHAN
ID #:  1577489
CMPUT  274  EA1

Exercise  1: Password Validator and Generator
"""

from random import randrange
import random

#Creates global variables
symbols = "!#$%&'()*+,./:;<=>?@[]^`{|}~"
digits = "0123456789"
lower = "abcdefghijklmnopqrstuvwxyz"
upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
characters = [symbols, digits, upper, lower]



def validate(password):

	#If password is not long enough or is using restricted characters this will print "Password Invalid"
	if len(password) < 8 or " " in password or "-" in password or "_" in password:
		print("Password Invalid")


	#Checks if the password uses all required characters and prints "Password Secure" if so or "Password Insecure" otherwise
	#Checks password by comparing each character in the password string with each character in the global variables strings
	#When a character type is found in the password, a position in the key will be changed to 1
	#If all 4 positions in the key are 1 then the password is secure
	#The while loops are created to short circuit after they find different character types
	else:
		key = [0, 0, 0, 0]

		for i in range(0, 4):
			k = 0
			while k < len(password):
				j = 0
				while j < len(characters[i]):
					if password[k] == characters[i][j]:
						key[i] = 1
						j = len(characters[i])
						k = len(password)
					j += 1
				k += 1

		if sum(key) == 4:
			print("Password Secure")
		else:
			print("Password Insecure")



def generate(n):

	#According to password length, finds a random number of each different character type to use in password
	letters = randrange(2, n - 1)
	other = n - letters
	NumLo = randrange(1, letters)
	NumUp = letters - NumLo
	NumSym = randrange(1, other)
	NumDig = other - NumSym

	#Finds a, previously defined, number of random charcters from each different character type and puts them in a string
	string = ""
	for i in range(0, NumSym):
		char = symbols[randrange(0,28)]
		string += char

	for i in range(0, NumDig):
		char = digits[randrange(0,10)]
		string += char

	for i in range(0, NumLo):
		char = lower[randrange(0,26)]
		string += char

	for i in range(0, NumUp):
		char = upper[randrange(0,26)]
		string += char


	#Shuffles the string and prints the generated password
	listChars = list(string)
	random.shuffle(listChars)
	creation = ''.join(listChars)

	print("Generated Password: " + creation)



if __name__ == "__main__":

	#User can input a password to be validated
	word = input("Enter Password for Validation: ")

	validate(password = word)


	#Allows user to choose length of generated password and makes sure input is valid
	x = 1
	while x == 1:
		number = input("Enter the Number of Characters for a Randomly Generated Secure Password: ")
		if number.isdigit() and int(number) >= 8: 
			num = int(number)
			x = 2
		else:
			print("You must enter an integer of at least 8.")

	generate(n = num)