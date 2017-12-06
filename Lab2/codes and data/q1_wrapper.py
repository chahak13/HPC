import os

for i in range(10):
	if (os.fork() == 0):
		os.execv("a.out",['a.out'])
	else:
		os.wait()
