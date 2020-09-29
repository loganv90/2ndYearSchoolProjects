# Read the input
a, b, c = map(int, input().split())


# Solve the problem
avg = (a + b + c)/3
d = abs(a - avg)
e = abs(b - avg)
f = abs(c - avg)

# Output the result
print(int(d), int(e), int(f))
