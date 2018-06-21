CD %~pd0

SET DST=simulation
MKDIR %DST%

DataGenerator.exe -src1 ..\..\data\logo -src2 ..\..\data\background -dst %DST%

PAUSE
