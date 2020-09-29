slimes = int(input())

row = []
for i in range(slimes):
	row.append(1)
	if len(row) > 1:
		for j in range(len(row)-1):
			if row[-1] == row[-2]:
				row[-2] += 1
				row.pop()

for k in range(len(row)):
	print(row[k], end='')
	print(" ", end='')