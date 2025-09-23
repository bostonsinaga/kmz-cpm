@echo off
ninja -C ../build
@REM ninja -C ../build > build.log 2>&1

IF EXIST "..\build\cli\kmzcpm.exe" (
  COPY "..\build\cli\kmzcpm.exe" "..\test\kmzcpm.exe"
) ELSE (
  ECHO Executable does not exist!
)
