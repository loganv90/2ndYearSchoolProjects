# read the input
line = input().lower()

# solve the problem
vowels = 0
for letter in range(0,len(line)):
	if line[letter] == "a" or line[letter] == "e" or line[letter] == "i" or line[letter] == "o" or line[letter] == "u":
		vowels += 1

# output the result
print(vowels)