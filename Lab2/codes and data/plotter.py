import numpy
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
pi_values=[]
time,serial = [],[]
final_time,speedup,final_serial =[0,0,0,0], [0,0,0,0],[0,0,0,0]

with open('op_parallel_cores.txt','r') as op_parallel:
	with open('op_serial_cores.txt','r') as op_serial:
		for line in op_parallel:	
			data = line.rstrip().split(' ')
			pi_values.append(data[1])
			time.append(data[3])
		for line in op_serial:
			data = line.rstrip().split(' ')
			serial.append(data[3])

print(len(time))

for i in range(len(time)):
	if(i%4 == 0):
		final_time[0]+=float(time[i])
		final_serial[0]+=float(serial[i])
	if(i%4 == 1):
		final_time[1]+=float(time[i])
		final_serial[1]+=float(serial[i])
	if(i%4 == 2):
		final_time[2]+=float(time[i])
		final_serial[2]+=float(serial[i])
	if(i%4 == 3):
		final_time[3]+=float(time[i])
		final_serial[3]+=float(serial[i])
	#if(i%5 == 4):
	#	final_time[4]+=float(time[i])
#		final_serial[4]+=float(serial[i])


#with open('output_serial.txt','r') as op:
#	for line in op:	
#		data = line.rstrip().split(' ')
#		serial.append(float(data[3]))

for i in range(4):
	final_time[i] = final_time[i]/10
	final_serial[i] = final_serial[i]/10
	speedup[i] = final_serial[i]/final_time[i]

print(speedup)
x = [1, 2, 3, 4]

plt.figure()
plt.plot(x, speedup, 'k')
plt.xlabel('# of cores')
plt.ylabel('speedup')
plt.grid(True)
plt.show()
