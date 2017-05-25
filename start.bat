@echo off

:: opens a file dialogue
set dialog="about:<input type=file id=FILE multiple></input><script>FILE.click();new ActiveXObject
set dialog=%dialog%('Scripting.FileSystemObject').GetStandardStream(1).WriteLine(FILE.value);
set dialog=%dialog%close();resizeTo(0,0);</script>"

:: parse file paths
for /f "tokens=* delims=" %%p in ('mshta.exe %dialog%') do (
	set file=%%p
	set ftype=%%~xp
)

:: list of file extensions that are compatible with SDL2_Image
set validExts[1]=.bmp
set validExts[2]=.dib
set validExts[3]=.gif
set validExts[4]=.jpg
set validExts[5]=.jpeg
set validExts[6]=.jpe
set validExts[7]=.jif
set validExts[8]=.jfif
set validExts[9]=.jfi
set validExts[10]=.iff
set validExts[11]=.lbm
set validExts[12]=.pcx
set validExts[13]=.png
set validExts[14]=.pbm
set validExts[15]=.pgm
set validExts[16]=.ppm
set validExts[17]=.pnm
set validExts[18]=.tga
set validExts[19]=.icb
set validExts[20]=.vda
set validExts[21]=.vst
set validExts[22]=.tif
set validExts[23]=.tiff
set validExts[24]=.webp
set validExts[25]=.xcf
set validExts[26]=.xpm
set validExts[27]=.xv

:: check if file is compatible
setlocal enabledelayedexpansion
set "compatible=0"

for /L %%i in (1,1,26) do (
	if /I "%ftype%" equ "!validExts[%%i]!" SET "compatible=1"
)
if %compatible% neq 0 goto :success

:: incompatible file extension
setlocal disabledelayedexpansion
echo "nope"
pause
exit

:: compatible file extension
:success
setlocal disabledelayedexpansion
echo "selected  file is : %file%"
echo "its extension is : %ftype%"
pause 
exit /b 0


