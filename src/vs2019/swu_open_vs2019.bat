@echo off
REM note:
REM we assume GSTREAMER_1_0_ROOT_MSVC_X86_64 env variable is aldready 
REM defined in your windows system when during installing GStreamer
REM

REM set GSTREAMER_1_0_ROOT_MSVC_X86_64 = already-defined

REM set CUDA_INC="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.0\include"
REM set CUDA_LIB="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.0\lib\x64"

REM set CV_INC=C:\pkg\opencv\opencv-4.1.1\build-vs2019\install\include
REM set CV_LIB=C:\pkg\opencv\opencv-4.1.1\build-vs2019\install\x64\vc16\lib

REM set BOOST_INC=C:\pkg\boost\boost_1_71_0
REM set BOOST_LIB=C:\pkg\boost\boost_1_71_0\lib64-msvc-14.2

set TUTORIAL_ROOT=C:\Users\wus1\Projects\swu-personal\gstTutorial\src
set TUTORIAL_BUILD_INT=C:\Users\wus1\Projects\swu-personal\gstTutorial\build-vs2019\int
set TUTORIAL_BUILD_BIN=C:\Users\wus1\Projects\swu-personal\gstTutorial\build-vs2019\bin

"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.exe" %TUTORIAL_ROOT%\vs2019\swu_vs2019.sln

REM ---eof---/Users/wus1/Projects/2020/p803/software/payload-cpu/pyxis-analysis/vs2017/pyxisAnalysis.props
