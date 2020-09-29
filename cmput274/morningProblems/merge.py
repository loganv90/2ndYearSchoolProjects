# Get the input
leftlane = input().split()

# now do something similar to get the list of vehicles in the right lane
rightlane = input().split()

# Solve the problem
if len(leftlane) > len(rightlane): 
	cars = len(rightlane)
	endstr = ' '.join(leftlane[cars:])
else:
	cars = len(leftlane)
	endstr = ' '.join(rightlane[cars:])

mylist = [0] * cars

for i in range(cars):
	mylist[i] = leftlane[i] + " " + rightlane[i] + " "

strstr = ''.join(mylist)

newstr = (strstr + endstr).strip()

# Print the result
print(newstr)