md e:\F3DShare\work\F3D
md e:\F3DShare\work\Bin
xcopy d:\work\f3d\*.h e:\F3DShare\work\F3D\ /s
xcopy d:\work\public\*.h e:\F3DShare\work\public\ /s

xcopy d:\work\xengine\*.h e:\F3DShare\work\xengine\ /s /r
xcopy d:\work\xengine\intermediate\*.lib e:\F3DShare\work\xengine\intermediate\ /s /r
xcopy d:\work\xengine\script\*.lib e:\F3DShare\work\xengine\script\ /s /r
copy d:\work\bin\xengine.* e:\F3DShare\work\bin


del e:\F3DShare\work\F3D\Device\DX9Device /s /q
rd e:\F3DShare\work\F3D\Device\DX9Device /s /q
del e:\F3DShare\work\F3D\Image\IL /s /q
rd e:\F3DShare\work\F3D\Image\IL /s /q

copy d:\work\bin\f3d.* e:\F3DShare\work\Bin
copy d:\work\bin\lua*.* e:\F3DShare\work\Bin
copy d:\work\bin\lua*.* e:\F3DShare\work\Bin
copy d:\work\bin\SpeedTreeRT*.* e:\F3DShare\work\Bin
copy d:\work\bin\FTerrainEditor.* e:\F3DShare\work\Bin
copy d:\work\bin\symbol\*.* e:\F3DShare\work\Bin\symbol\
