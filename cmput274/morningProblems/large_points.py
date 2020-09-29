import math

n = int(input())

x, y, r = [], [], []
# x, y, r are empty lists

for i in range(n):
    x_in, y_in, r_in = map(float, input().split())
    x.append(x_in)
    y.append(y_in)
    r.append(r_in)

# Now read in the number of queries and then the
# queries themselves.
# You do not have to store all queries, you can
# output the answer for one query before reading
# in the next query point.

points = int(input())

pointx, pointy = [], []

for i in range(points):
	px, py = map(float, input().split())
	pointx.append(px)
	pointy.append(py)


pointstring = ["Small"] * points
for i in range(n):
	for j in range(points):
		newx = x[i] - pointx[j]
		newy = y[i] - pointy[j]
		if math.sqrt(newx**2 + newy**2) <= r[i]:
			pointstring[j] = "Large"

for i in range(len(pointstring)):
	print(pointstring[i])
