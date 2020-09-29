n, m = map(int, input().split())
bits = str(input())
outputs = [0]*m


for i in range(m):
	string = ''
	answer = ''
	s, e = map(int, input().split())
	first = s-1
	last = e-1

	actualbits = []
	item = first
	for j in range(first, last+1):
		actualbits.append(bits[item])
		item += 1
	string = ''.join(actualbits)

	if (str(1) in string) and (str(0) in string):
		answer = 'both'
	elif str(1) in string:
		answer = 'one'
	elif str(0) in string:
		answer = 'zero'

	outputs[i] = answer

for k in range(len(outputs)):
	print(outputs[k])