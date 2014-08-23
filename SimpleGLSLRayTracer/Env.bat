set OPENCV_ROOT=F:\Tech Zone\Abhishek_LRC\Third_Party\OpenCV\build
set OPENCV_PATH=%OPENCV_ROOT%\x86\vc10\bin\
set OPENCV_LIB=%OPENCV_ROOT%\x86\vc10\lib\
set OPENCV_INCLUDE=%OPENCV_ROOT%\include\

set GLUT_ROOT=F:\Tech Zone\Abhishek_LRC\Third_Party\glut
set GLUT_PATH=%GLUT_ROOT%\bin\x86\release
set GLUT_LIB=%GLUT_ROOT%\lib\x86\release
set GLUT_INCLUDE=%GLUT_ROOT%\include\

set GLEW_ROOT=F:\Tech Zone\Abhishek_LRC\Third_Party\glew
set GLEW_PATH=%GLEW_ROOT%\bin\x86\
set GLEW_LIB=%GLEW_ROOT%\lib\x86\
set GLEW_INCLUDE=%GLEW_ROOT%\include\

set GLUI_ROOT=F:\Tech Zone\Abhishek_LRC\Third_Party\glui235
set GLUI_PATH=%GLUI_ROOT%\bin\x86\
set GLUI_LIB=%GLUI_ROOT%\lib\x86\
set GLUI_INCLUDE=%GLUI_ROOT%\include\

set TBB_ROOT=F:\Tech Zone\Abhishek_LRC\Third_Party\tbb41_20120718oss
set TBB_PATH=%TBB_ROOT%\bin\x86\vc9
set TBB_LIB=%TBB_ROOT%\lib\x86\vc9
set TBB_INCLUDE=%TBB_ROOT%\include\

set PATH=%PATH%;%OPENCV_PATH%;%GLUT_PATH%;%GLUI_PATH%;%TBB_PATH%;%GLEW_PATH%

::SimpleGLSLRayTracer.sln