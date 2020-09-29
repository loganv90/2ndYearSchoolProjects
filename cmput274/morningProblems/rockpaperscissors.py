num_matches = int(input())

matches = [0] * num_matches
for i in range(num_matches):
    matches[i] = input().split()

matchbob = 0
matchalice = 0
for a in range(len(matches)):
	bob = 0
	alice = 0
	for j in range(len(matches[a])):
		if matches[a][j] == "RS" or matches[a][j] == "SP" or matches[a][j] == "PR":
			alice += 1
		elif matches[a][j] == "SR" or matches[a][j] == "PS" or matches[a][j] == "RP":
			bob += 1
	if bob > alice:
		matchbob += 1
	elif alice > bob:
		matchalice += 1

if matchbob > matchalice:
	winner = "Bob"
	matchwinner = matchbob
else:
	winner = "Alice"
	matchwinner = matchalice
    

# print here whoever is the overall winner of all the matches and
# how many matches the winner won
print(winner + " " + str(matchwinner))