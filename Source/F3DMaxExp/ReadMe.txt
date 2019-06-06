
/*********************************************************************************/
½â¾ö:MeshExporter.obj : error LNK2001: unresolved external symbol "public: virtual char * __thiscall ClassDesc::GetRsrcString(long)" (?GetRsrcString@ClassDesc@@UAEPADJ@Z)
You have the correct sdk - there is a bug in the header files that ships with msvc. 
This is resolved by using the Platform sdk. The problem is the following:-

max 4 sdk development requires installation of the MS Platform SDK. You can get 
it from Microsoft website. The platform sdk is required to ensure forward compatibility
with future releases of MS operating systems (for example the IA64). More importantly, 
it is required because there is a flaw in Microsoft headers that ship with Visual C++ 6.0. 
The problem is that Visual C++ defines INT_PTR to 'long' when it is supposed to be defined 
as an 'int' (on ia32 platforms). You can either use a supported build environment by updating 
to the platform SDK, or you can use the unsupported environment by manually fixing the problem 
in the header "On or around line 123 of include\basetsd.h change:

typedef long INT_PTR, *PINT_PTR; typedef unsigned long UINT_PTR, *PUINT_PTR; to
typedef int INT_PTR, *PINT_PTR; typedef unsigned int UINT_PTR, *PUINT_PTR;


Neil 

-------------
Neil Hazzard
Sparks3D
Discreet
/*********************************************************************************/