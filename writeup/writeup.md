#Writeup
#Mohamed Naji
##Github
1.) My paralell and serial vales are different for the later values of the array. My python figures were created however, github would not let me push after bigSim was created.

2.) The MPI_Gatherv call gives variabilty with the count of data from each process and alows to choose where data is placed on the root procces. First the parts of the snapshots are gathered into the root rank. Then the file is opened. Then each snapshot is written into the file and array storing the snaps is cleared. All other ranks just participate in the gather of snapshots.

3.)![/Users/asuka/Desktop/untitled folder](/Users/asuka/Desktop/untitled folder/Weak_Scaling.png)
Wall Time: 2-23sec 4-60sec 8-60sec 16-107sec 32-201 sec
There is a loss of efficiency when working with a larger problem most likley caused by the writing of the file.

4.) My script would not work and kept resulting in an error. I think that checkpointing and saving takes the longest and this could be fixed by checkpointing less.

5.) Since my paralell and serial data were different there i definitley had a bug in my code. I have pinned it down to the condition in oneStepLoc to apply the formua to the non boundry conditions. I believe my range for boundry values is too large thus not applying the formula to some points. I have attempted to change the conditional and even removed it at one point which resulted in the formula being applied to most of the points, however this obviosuly did not include the boundry values. Another issue I had was I couldn push after bigSim.txt was created making it very hard to get my text files and impossible to get my python pointSim plots. I tried deleting the file and using various git commands however none of them worked.