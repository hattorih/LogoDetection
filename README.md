Logo Detection
==============

## Demo

Simulation based projection used a typical camera model:

<img src=".\Common\data\projection.jpg" width="720">

Generated training samples:

<img src=".\Common\data\sample.jpg" width="640">

## Requirement

This code is tested with the following configuration:
- OS: Windows 10, 64bit with Visual Studio 2013
- Libraries: Yolo v3, OpenCV 2.4.7
- ML Framework: Darknet (for Yolo)

## Usage

In the following commands, you can generate various sizes and angles of logos.

$ .\DataGeneration\bin\vc12_x64_Release\DataGenerator.bat

Then, you can blend them with any background images.

$ .\DataGeneration\bin\vc12_x64_Release\BlendingTool.bat

## License

Please refer to the [LICENSE](/LICENSE.md) file.

NOTE: This code refers to the following previous work:
- OpenCV
https://github.com/opencv/opencv

- Yolo
https://pjreddie.com/darknet/yolo/

- DeepLogo
https://github.com/satojkovic/DeepLogo
- Logo Detection YOLOv2
https://github.com/akarshzingade/Logo-Detection-YOLOv2
