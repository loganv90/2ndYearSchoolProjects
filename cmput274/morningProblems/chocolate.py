m, n = map(int, input().split())

room = [0] * n
jars = 0
for i in range(n):

	jarmin, jarmax = map(int, input().split())

	room[i] = abs(jarmax - jarmin)

	if room[i] >= m:
		jars += 1

print(jars)