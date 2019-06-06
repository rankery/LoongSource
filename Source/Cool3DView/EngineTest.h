	//--test camera
	Vector3 test(0,0,0);
	//POINT scrPt=m_p->m_camera.WorldToScreen(test);
	//Vector3 pt=m_p->m_camera.ScreenToWorld(0,0);
	
	//--test Noise
	/*Noise tn;
	tn.Generate(256,256);
	tn.SaveToRAW("d:\\test.raw");
	*/
	//--test transform
	/*Vector3 v(0,10,0);
	Transform t;
	t.RotateAxis('y',FLOAT_PI/2);
	t.Translate(Vector3(10,0,0));
	t.TransVec3(v);
	*/

	/*
	UINT testSize=1024*2;
	void *pTest=malloc(testSize);
	DWORD time=GetTickCount();
	int i;
	for(i=0;i<10000;i++)
		memset(pTest,0,testSize);
	DWORD time1=GetTickCount()-time;
	time=GetTickCount();
	for(i=0;i<10000;i++)
		FZeroMem(pTest,testSize);
	DWORD time2=GetTickCount()-time;
	free(pTest);
	*/
	
	/*
	for(int j=0;j<20;j++)
	{
		UINT testSize=1024*128;
		void *pSrc=malloc(testSize);
		void *pDst=malloc(testSize);

		//crt code
		DWORD time=GetTickCount();
		int i;
		for(i=0;i<10000;i++)
			FMemcpy(pDst,pSrc,testSize);
		DWORD time2=GetTickCount()-time;

		//amd code
		time=GetTickCount();
		for(i=0;i<10000;i++)
			memcpy(pDst,pSrc,testSize);
		DWORD time1=GetTickCount()-time;

		//--free
		free(pSrc);
		free(pDst);

		char szInfo[1024];
		sprintf(szInfo,"time1=%d,time2=%d\r\n",time1,time2);
		OutputDebugString(szInfo);
	}
	*/
/*	//测试基本Image功能
	Image testImg;
	bool ok=false;
	ok=testImg.LoadFromFile("girl.tga");
	ok=testImg.Scale(64,48,EIS_BestQuality);
	Image testImg2;
	ok=testImg2.Create(512,512,EPF_A8R8G8B8);
	testImg2.Blit(&testImg,0,0);
	ok=testImg2.WriteToFile("d:\\test.tga");
	*/

	//测试Image/Texture
	/*IDeviceTex *testTex=Device()->NewTexture();
	Image testImg;
	bool ok=testImg.LoadFromFile("ImgTex.bmp");
	testTex->CreateFromImage(&testImg,0);
	testTex->WriteToFile("d:\\testRGB.dds");
	*/
/*
	//测试MPreCombineBitmap
	MPreCombineBitmap mt;
	mt.BeginCombine();
	mt.AddLayer("face\\0base.tga",Color4f(1,0/255.0f,4/255.0f,50/255.0f));
	//mt.AddLayer("face\\0base.tga",Color4f(1,1,1,1));
	//mt.AddLayer("face\\1face.tga",);
	//mt.AddLayer("face\\2detail.tga",Color4f(1,1,1,1));
	mt.EndCombine();
*/

	//test bitmap
	BitMap testBitMap;
	testBitMap.Create(16,16);
	bool testB[16][16];
	int i,j;
	for(j=0;j<16;j++)
	{
		for(i=0;i<16;i++)
		{
			testB[j][i]=((i+j)%3==0);
			testBitMap.SetValue(i,j,testB[j][i]);
		}
	}
	for(j=0;j<16;j++)
	{
		for(i=0;i<16;i++)
		{
			bool t=testBitMap.GetValue(i,j);
			ASSERT(testB[j][i]==t);
		}
	}

	
	