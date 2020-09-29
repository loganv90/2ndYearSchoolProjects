vectorlength = int(input())
xvector = input().split()
yvector = input().split()

xvector = list(map(int, xvector))
yvector = list(map(int, yvector))

xvector = sorted(xvector)
yvector = sorted(yvector)

total = 0
for i in range(vectorlength):
	number = xvector[i]*yvector[i]
	total += number

print(total)