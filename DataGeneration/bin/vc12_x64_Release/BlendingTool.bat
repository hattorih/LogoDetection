CD %~pd0

SET SRC=simulation
SET SRC_MASK=%SRC%_mask
SET DST=result

MKDIR %SRC%
MKDIR %SRC_MASK%
MKDIR %DST%

BlendingTool.exe ^
-src %SRC% ^
-src_mask %SRC_MASK% ^
-src_background ..\..\..\Common\data\background ^
-dst %DST% -r 255 -g 255 -b 255 -no_frame 1

PAUSE
