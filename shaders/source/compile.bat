:: Requires shadercross CLI installed from SDL_shadercross
:: Converts my hlsl shaders to the compatible shader format
@echo off
setlocal enabledelayedexpansion

for %%f in (*.vert.hlsl) do (
    if exist "%%f" (
        shadercross "%%f" -o "..\compiled\SPIRV\%%~nf.spv"
        shadercross "%%f" -o "..\compiled\MSL\%%~nf.msl"
        shadercross "%%f" -o "..\compiled\DXIL\%%~nf.dxil"
    )
)

for %%f in (*.frag.hlsl) do (
    if exist "%%f" (
        shadercross "%%f" -o "..\compiled\SPIRV\%%~nf.spv"
        shadercross "%%f" -o "..\compiled\MSL\%%~nf.msl"
        shadercross "%%f" -o "..\compiled\DXIL\%%~nf.dxil"
    )
)

for %%f in (*.comp.hlsl) do (
    if exist "%%f" (
        shadercross "%%f" -o "..\compiled\SPIRV\%%~nf.spv"
        shadercross "%%f" -o "..\compiled\MSL\%%~nf.msl"
        shadercross "%%f" -o "..\compiled\DXIL\%%~nf.dxil"
    )
)
