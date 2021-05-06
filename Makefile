# ============= SERIAL UNIT TESTS =======================================

buildUnitMatSer:
	gcc test/unitTestMatSer.c code/materialSer.c -o obj/unitTestMatSer -lm

runUnitMatSer:
	./obj/unitTestMatSer

buildUnitChkPtSer:
	gcc test/unitTestCheckPtSer.c code/materialSer.c code/checkPtSer.c code/simulationSer.c -o obj/unitTestChkPtSer -lm

runUnitChkPtSer:
	./obj/unitTestChkPtSer

buildUnitSimSer:
	gcc test/unitTestSimSer.c code/materialSer.c code/checkPtSer.c code/simulationSer.c -o obj/unitTestSimSer -lm

runUnitSimSer:
	./obj/unitTestSimSer

buildSerialUnitTests:
	make buildUnitMatSer
	make buildUnitChkPtSer
	make buildUnitSimSer

runSerialUnitTests:
	make runUnitMatSer
	make runUnitChkPtSer
	make runUnitSimSer

# ============= SERIAL AND PARALLEL SMALL TEST EXAMPLE ================================

buildPointSimSer:
	gcc test/pointSimSer.c code/materialSer.c code/checkPtSer.c code/simulationSer.c -o obj/pointSimSer -lm

runPointSimSer:
	./obj/pointSimSer

# be sure to load openmpi and have an interaction session with at least 4 cores before this
buildPointSimPar: 
	mpicc test/pointSimPar.c code/materialPar.c code/checkPtPar.c code/simulationPar.c -o obj/pointSimPar -lm

runPointSimPar:
	mpirun -np 4 ./obj/pointSimPar

# The rule below builds and runs both the serial and parallel examples and compares their outputs.
# We expect there to be no differences in the outputs
wholeComparison:
	make buildPointSimSer
	make runPointSimSer
	make buildPointSimPar
	make runPointSimPar
	echo If any differences between serial and parallel simulation they are listed in results/diff.txt:
	touch results/diff.txt
	diff results/pointTestSer.txt results/pointTestPar.txt >results/diff.txt

# before you run the rule below, make sure you have X forwarding enabled (-X flag when you ssh in), and have loaded the Anaconda module
plotPointSims:
	python test/readPlotSnaps.py results/pointTestSer.txt results/pointPlotsSer
	python test/readPlotSnaps.py results/pointTestPar.txt results/pointPlotsPar


# ========== SERIAL AND PARALLEL SMALL EXAMPLE SKIPPING TIMES ====================

buildPointSkipSer:
	mpicc test/pointSimSkipSer.c code/materialSer.c code/checkPtSer.c code/simulationSer.c -o obj/pointSkipSer -lm

runPointSkipSer:
	./obj/pointSkipSer

buildPointSkipPar:
	mpicc test/pointSimSkipPar.c code/materialPar.c code/checkPtPar.c code/simulationPar.c -o obj/pointSkipPar -lm

runPointSkipPar:
	mpirun -np 4 ./obj/pointSkipPar

# The rule below builds and runs both the serial and parallel examples and compares their outputs.
# We expect there to be no differences in the outputs
wholeComparisonSkip:
	make buildPointSkipSer
	make runPointSkipSer
	make buildPointSkipPar
	make runPointSkipPar
	echo If any differences between serial and parallel simulation they are listed in results/diffSkip.txt
	diff results/pointTestSer.txt results/pointTestPar.txt >results/diffSkip.txt

# before you run the rule below, make sure you have loaded the Anaconda module
plotPointSkips:
	python test/readPlotSnaps.py results/pointTestSkipSer.txt results/skipPlotsSer
	python test/readPlotSnaps.py results/pointTestSkipPar.txt results/skipPlotsPar


# ============RULE TO BUILD BIG SIMULATION ======================================
buildBigSim:
	mpicc test/bigSim.c code//materialPar.c code/checkPtPar.c code/simulationPar.c -o obj/bigSim -lm
	
# just use this example so you can see how to call the code with 100 columns, 400 rows and 5 steps per checkpoint
exampleRunBigSim: 
	mpirun -np 4 ./obj/bigSim 100 400 5
	
# ===========RULES TO BUILD AND RUN ALL TEST CASES ABOVE =======================

# note: need to have openmpi loaded before parallel code builds
buildAll:
	make buildUnitMatSer
	make buildUnitChkPtSer
	make buildUnitSimSer
	make buildPointSimSer
	make buildPointSkipSer
	make buildPointSimPar
	make buildPointSkipPar
	make buildBigSim

# note: need to have openmpi loaded before parallel runs and Anaconda loaded before plotting scripts run
runAll:
	make runUnitMat
	make runUnitChkPt
	make runUnitSim
	make wholeComparison
	make plotPointSims
	make wholeComparisonSkip
	make plotPointSkips
	make exampleRunBigSim

clean:
	rm -f obj/unitTestMatSer
	rm -f obj/unitTestChkPtSer
	rm -f obj/unitTestSimSer
	rm -f obj/pointSimSer
	rm -f obj/pointSimPar
	rm -f obj/pointSimSkipSer
	rm -f obj/pointSimSkipPar
	rm -f obj/bigSim
	rm -f results/*.txt
	rm -f results/*.png
