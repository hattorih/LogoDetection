CD %~pd0

SET DST=simulation
SET DST_MASK=%DST%_mask

MKDIR %DST%
MKDIR %DST_MASK%

DataGenerator.exe -src1 ..\..\data\logo\google\google2 -src2 ..\..\data\background -dst %DST% -r 255 -g 255 -b 255 -no_frame 1
DataGenerator.exe -src1 ..\..\data\logo\google\google2_mask -src2 ..\..\data\background -dst %DST_MASK% -r 0 -g 0 -b 0 -no_frame 1

PAUSE
