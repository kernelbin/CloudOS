:: checking utils
if exist tools\utils\ (
set "PATH=%~dp0tools\utils\;%PATH%"
)

:: check if we've already have curl and 7z
where /q curl || echo Cound not find curl. && EXIT /B
where /q 7z || echo Cound not find 7z. && EXIT /B

:: checking make
if exist tools\make\ (
set "PATH=%~dp0tools\make\;%PATH%"
)

:: check if we got make
where /q make || echo Cound not find make. && EXIT /B

:: checking nasm
if exist tools\nasm\ (
set "PATH=%~dp0tools\nasm\;%PATH%"
)

:: check if we got nasm
where /q nasm || echo Cound not find nasm. && EXIT /B

:: checking fat_imgen
if exist tools\fat_imgen\ (
set "PATH=%~dp0tools\fat_imgen\;%PATH%"
)

where /q fat_imgen || echo Cound not find fat_imgen. && EXIT /B

:: checking gcc stuff
if exist tools\i686-elf-tools-windows\ (
set "PATH=%~dp0tools\i686-elf-tools-windows\bin\;%PATH%"
) else (
:: download it from https://github.com/lordmilko/i686-elf-tools
mkdir download_tmp
curl --location https://github.com/lordmilko/i686-elf-tools/releases/download/7.1.0/i686-elf-tools-windows.zip^
  --output .\download_tmp\i686-elf-tools-windows.zip || echo failed to download i686-elf-tools-windows! && EXIT /B
7z x .\download_tmp\i686-elf-tools-windows.zip -o.\tools\i686-elf-tools-windows\ || echo failed to unzip i686-elf-tools-windows! && EXIT /B
rd /S /Q .\download_tmp\
set "PATH=%~dp0tools\i686-elf-tools-windows\bin\;%PATH%"
)
