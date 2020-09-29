# read in the input
x1, y1, x2, y2 = map(int, input().split())
x3, y3, x4, y4 = map(int, input().split())

# now solve the problem. good luck :D

dirx1 = x1 - x2
diry1 = y1 - y2

dirx2 = x3 - x4
diry2 = y3 - y4

if dirx1/diry1 == dirx2/diry2:
	print("parallel")

else:
	print("not parallel")