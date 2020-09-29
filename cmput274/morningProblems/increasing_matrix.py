n, m = map(int, input().split())
# You can access the j-th element in the i-th row of matrix A using a[i][j].
# Works the same for B. Just make sure 0 <= j < n and 0 <= i < m.
a = [[int(val) for val in input().split()] for _ in range(n)]
b = [[int(val) for val in input().split()] for _ in range(n)]

# Good luck! I believe in you! :)

impossible = False

for row in range(n):
	for col in range(m - 1):
		bothdw = (a[row][col], b[row][col])
		bothup = (a[row][col+1], b[row][col+1])
		if ((bothdw[0] >= bothup[0]) or (bothdw[1] >= bothup[1])) and ((bothdw[0] >= bothup[1]) or (bothdw[1] >= bothup[0])):
			impossible = True

for row in range(n - 1):
	for col in range(m):
		bothdw = (a[row][col], b[row][col])
		bothup = (a[row+1][col], b[row+1][col])
		if ((bothdw[0] >= bothup[0]) or (bothdw[1] >= bothup[1])) and ((bothdw[0] >= bothup[1]) or (bothdw[1] >= bothup[0])):
			impossible = True

if impossible == True:
	print("Impossible")
else:
	print("Possible")
