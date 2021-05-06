import sys
import numpy as np
import csv 
import matplotlib.pyplot as plt
plt.switch_backend('agg')

# get the input filename from the command line call of this
checkPtFilename = sys.argv[1]
# read header (metadata) from file
f = open(checkPtFilename,'r')
Nx = int((f.readline()).strip()) # read 1st line, strip off white space and newlines, cast to integer
Ny = int((f.readline()).strip()) # do same for 2nd line
NSnaps = int((f.readline()).strip()) # do same for 3rd line
f.close()
# may find it helpful to make sure dimensions are interpreted right by uncommenting next line
#print("Nx = "+str(Nx)+" , Ny = "+str(Ny)+" , NSnaps = "+str(NSnaps))

# read data and times from file
flatData = np.genfromtxt(checkPtFilename, delimiter=',',skip_header=3)
times = flatData[:,0] # first entry of each row is the time
snapshotsFlat = flatData[:,1:-1] # last entry of each row is just a comma (shows up as nan)
snapshots = np.reshape(snapshotsFlat,(NSnaps,Ny,Nx))


# define a function to take any integer (idx) between 0 and 999 and turn it into a filename beginning with start 
def genFilename(start, idx):
	strIdx = str(idx)
	if idx < 100:
		strIdx = '0'+strIdx
		if idx < 10:
			strIdx = '0'+strIdx
	filename = start + strIdx
	return filename

# going to plot on a log10 scale
# all the plots need to have the same colorbar, so find the min and max values overall
MinClip = np.log10(np.min(snapshots))
MaxClip = np.log10(np.max(snapshots))

# get start of output filename from the command line
outFilenameStart = sys.argv[2]
for i in range(NSnaps):
	filename = genFilename(outFilenameStart,i) # generate the filename
	# plot the image
	plt.imshow(np.log10(snapshots[i,:,:]),aspect='auto',vmin=MinClip,vmax=MaxClip,interpolation='nearest',cmap='inferno')
	plt.xlabel('x index',fontsize=12)
	plt.ylabel('y index',fontsize=12)
	plt.title('log10(Temperature) at '+str(times[i])+' seconds')
	plt.colorbar()
	plt.savefig(filename)
	plt.clf()
