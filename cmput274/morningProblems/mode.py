stringlist = input().split()


numberlist = [0]*len(stringlist)

for i in range(len(stringlist)):
	for j in range(len(stringlist)):
		if stringlist[i] == stringlist[j]:
			numberlist[i] += 1

maxnum = max(numberlist)


outlist = []
for i in range(len(stringlist)):
	if numberlist[i] == maxnum:
		outlist.append(stringlist[i])

outlist.sort()

for i in range(len(outlist) - 1):
	if outlist[i] == outlist[i + 1]:
		outlist[i] = 0

for i in range(len(outlist)):
	if outlist[i] != 0:
		print(outlist[i])

