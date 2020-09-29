# read in the input
x1, y1 = map(int, input().split())
x2, y2 = map(int, input().split())



# solve the problem
width = abs(x1 - x2)
height = abs(y1 - y2)
area = width*height


# output the result
print(area)