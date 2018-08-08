CD %~pd0

SET CONFIG_DIR=..\data
ECHO %CONFIG_DIR%

SET SRC=sample.mp4

.\darknet.exe detector demo ^
%CONFIG_DIR%/coco.data ^
%CONFIG_DIR%/yolov3.cfg ^
%CONFIG_DIR%/yolov3.weights ^
-i 0 -thresh 0.25 -out_filename -dont_show %SRC%_result.mp4 %SRC%

PAUSE
