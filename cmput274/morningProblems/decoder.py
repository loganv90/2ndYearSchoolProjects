# Read in the input

# Construct a dictionary mapping binary strings to English words

# Use the dictionary to decode the binary string

wordcount = int(input())

dictionary = {}

for i in range(wordcount):
	code, word = map(str, input().split())
	dictionary[code] = word

sequence = input()
endlist = []
string = ''

for i in range(len(sequence)):
	string += sequence[i]
	for key, value in dictionary.items():
		if string == key:
			endlist.append(value)
			string = ''

out = ' '.join(endlist)

print(out)