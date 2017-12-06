import matplotlib.pyplot as plt
import numpy as np
import sys

def loadData(fname):
    inputSize, elapsedTime = [], []
    try:
        myData = open(fname, 'r')
    except IOError:
        print "File ", fname , "not found."
    for l in myData:
        l = l.split()
        l = [float(num) for num in l]
        inputSize.append(l[0])
        elapsedTime.append(l[2])

    inputSize = np.array(inputSize)
    elapsedTime = np.array(elapsedTime)
    return inputSize, elapsedTime

#max number of cores in the pc
cores = 12

#files          => list of files to load data from
#inputSize[i]   => input sizes numpy array of the ith file
#elapsedTime[i] => elapsed time numpy array of the ith file
files, inputSize, elapsedTime = [], [], []

#Append names of all data files
files.append("serial")
for c in xrange(1,cores+1):
    files.append("parallel"+str(c))

#loadData from files
for f in files:
    tmpInputSize, tmpElapsedTime = loadData(f)
    inputSize.append(tmpInputSize)
    elapsedTime.append(tmpElapsedTime)
    print "loaded", f
inputSize = np.array(inputSize)
elapsedTime = np.array(elapsedTime)
#plot parallel overhead
plt.plot(inputSize[0], ((elapsedTime[1] - elapsedTime[0])*100)/elapsedTime[0], 'ko-')
plt.xscale("log")
plt.xlabel("input size")
plt.ylabel("% of parallel overhead")
plt.grid(True)
plt.savefig("plots/overhead.png", format = "png", dpi = 350)
plt.close()

#plot input-size vs time
plt.plot(inputSize[0], elapsedTime[0], 'k*:', label = "serial", markersize=8)
plt.plot(inputSize[2], elapsedTime[2], 'k^--', label = "2 Cores")
plt.plot(inputSize[4], elapsedTime[4], 'ks-', label = "4 Cores") 
plt.plot(inputSize[8], elapsedTime[8], 'ko-', linewidth = 2, label = "8 Cores")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("input size")
plt.ylabel("time taken")
plt.legend()
plt.grid(True)
plt.savefig("plots/time.png", format = "png", dpi = 350)
plt.close()

#plot input-size vs speedup
plt.plot(inputSize[0], elapsedTime[0]/elapsedTime[12], 'ko-', label = "12 Cores")
plt.plot(inputSize[0], elapsedTime[0]/elapsedTime[8], 'ks--', label = "8 Cores")
plt.plot(inputSize[0], elapsedTime[0]/elapsedTime[4], 'k*:', label = "4 Cores", markersize = 8)
plt.grid(True)
plt.legend()
plt.xscale("log")
plt.xlabel("input size")
plt.ylabel("speedup")
plt.savefig("plots/speedup_size.png", format = "png", dpi = 350)
plt.close()

#plot cores vs speedup
plt.plot(np.arange(0, cores+1), elapsedTime[0][6]/elapsedTime[:,6], 'ko-', label = r"$10^9$ elements")
plt.plot(np.arange(0, cores+1), elapsedTime[0][2]/elapsedTime[:,2], 'ks--', label = r"$10^5$ elements")
plt.plot(np.arange(0, cores+1), elapsedTime[0][0]/elapsedTime[:,0], 'k*:', label = r"$10^3$ elements", markersize = 8)
plt.grid(True)
plt.legend()
plt.xlabel("number of cores")
plt.ylabel("speedup")
plt.savefig("plots/speedup_cores.png", format = "png", dpi = 350)
plt.close()

#plot efficiency vs cores
plt.plot(np.arange(1, cores+1), elapsedTime[0][6]/(elapsedTime[1:,6]*np.arange(1, cores+1)), 'ko-', label = r"$10^9$ elements")
plt.plot(np.arange(1, cores+1), elapsedTime[0][2]/(elapsedTime[1:,2]*np.arange(1, cores+1)), 'ks--', label = r"$10^5$ elements")
plt.plot(np.arange(1, cores+1), elapsedTime[0][0]/(elapsedTime[1:,0]*np.arange(1, cores+1)), 'k*:', label = r"$10^3$ elements", markersize = 8)
plt.grid(True)
plt.legend()
plt.xlabel("number of cores")
plt.ylabel("efficiency")
plt.savefig("plots/efficiency_cores.png", format = "png", dpi = 350)
plt.close()

#plot efficiency vs input size
plt.plot(inputSize[0], elapsedTime[0]/(elapsedTime[12]*12), 'ko-', label = "12 Cores")         
plt.plot(inputSize[0], elapsedTime[0]/(elapsedTime[8]*8), 'ks--', label = "8 Cores")          
plt.plot(inputSize[0], elapsedTime[0]/(elapsedTime[4]*4), 'k*:', label = "4 Cores", markersize = 8)              
plt.grid(True)                                                                            
plt.legend()                                                                                 
plt.xscale("log")                                                                         
plt.xlabel("input size")                                                                  
plt.ylabel("efficiency")                                                                     
plt.savefig("plots/efficiency_size.png.png", format = "png", dpi = 350)
plt.close()
