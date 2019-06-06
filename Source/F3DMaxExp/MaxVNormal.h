#ifndef MAX_VNORMAL_H
#define MAX_VNORMAL_H

/** From MAX Sdk doc,Advanced Topics/Computing Face and Vertex Normals
The class used to compute vertex normals considering smoothing is shown below.
This class, VNormal, is similar to the RNormal class used by 3ds max internally. 
The class contains a Point3 which is the normal, a DWORD for the smoothing groups, 
and a pointer to the next normal -- this class is a linked list. The init variable 
is used as a flag to indicate if the first normal in the list has been initialized.
*/

// Linked list of vertex normals

struct NormalVerSm
{
	Point3	normal;
	DWORD	vertexIndex;
	DWORD	smGroup;
	int		faceIndex;
};

struct TangentVerSm
{
	Point3	tangent;
	DWORD	vertexIndex;
	DWORD	smGroup;
	DWORD	uv;				//bump map uv
	int		uvDir;			//bump map uv dir
	int		faceIndex;
};

class VNormal 
{
public:
	Point3 norm;
	DWORD smooth;
	VNormal *next;
	BOOL init;
	DWORD	vertexIndex;
	int		faceIndex;
	
	VNormal() 
	{
		smooth=0;next=NULL;init=FALSE;norm=Point3(0,0,0);vertexIndex=0;faceIndex=-1;
	}
	VNormal(const Point3 &n,DWORD s,DWORD vi,int faceID) 
	{
		next=NULL;init=TRUE;norm=n;smooth=s;vertexIndex=vi;faceIndex=faceID;
	}
	~VNormal() 
	{
		delete next;
	}
	
	/** Add a normal to the list if the smoothing group bits overlap,
	otherwise create a new vertex normal in the list
	*/
	void AddNormal(const Point3 &n,DWORD s,DWORD vi,int faceID)
	{
		if (init && !(s&smooth)) 
		{
			if(next)
				next->AddNormal(n,s,vi,faceID);
			else
				next = new VNormal(n,s,vi,faceID);
		}//endof if
		else 
		{
			norm += n;
			smooth |= s;
			vertexIndex=vi;
			faceIndex=faceID;
			init = TRUE;
		}//end else
	}//endof AddNormal()
	
	/** Retrieves a normal if the smoothing groups overlap or there is 
	only one in the list
	*/
	Point3 &GetNormal(DWORD s)
	{
		if (smooth&s || !next)
			return norm;
		else
			return next->GetNormal(s);
	}
	// Normalize each normal in the list
	void Normalize()
	{
		VNormal *ptr = next, *prev = this;
		
		while (ptr) 
		{
			if (ptr->smooth&smooth) 
			{
				norm += ptr->norm;
				prev->next = ptr->next;
				delete ptr;
				ptr = prev->next;
			} 
			else
			{
				prev = ptr;
				ptr = ptr->next;
			}
		}//endof while
		norm = ::Normalize(norm);
		if (next) 
			next->Normalize();
	}
};

class VTangent
{
public:
	Point3	tangent;
	DWORD	smooth;
	DWORD	vertexIndex;
	int		faceIndex;
	DWORD	uv;
	int		uvDir;
	VTangent *next;
	BOOL	init;

	VTangent() 
	{
		smooth=0;next=NULL;init=FALSE;tangent=Point3(0,0,0);vertexIndex=0;faceIndex=-1;
		uv=0;uvDir=-1;
	}
	VTangent(const Point3 &t,DWORD s,DWORD vi,int faceID,DWORD _uv,int _uvDir) 
	{
		next=NULL;init=TRUE;tangent=t;smooth=s;vertexIndex=vi;faceIndex=faceID;
		uv=_uv;uvDir=_uvDir;
	}
	~VTangent() 
	{
		delete next;
	}

	void AddTangent(const Point3 &t,DWORD s,DWORD vi,int faceID,DWORD _uv,int _uvDir)
	{
		if (init && !(s&smooth))// && uv==_uv && uvDir==_uvDir
		{
			if(next)
				next->AddTangent(t,s,vi,faceID,_uv,_uvDir);
			else
			{
				next = new VTangent(t,s,vi,faceID,_uv,_uvDir);
			}
		}//end if
		else 
		{
			tangent += t;
			smooth |= s;
			vertexIndex=vi;
			faceIndex=faceID;
			uv=_uv;
			uvDir=_uvDir;
			init = TRUE;
		}//end else
	}//end AddTangent()

	void Normalize()
	{
		VTangent *ptr = next, *prev = this;

		while (ptr) 
		{
			//切线相同的条件有
			//1、光滑组&
			//2、UV相等
			//3、UV方向一致
			if (ptr->smooth&smooth)// && ptr->uv==uv && ptr->uvDir==uvDir
			{
				tangent += ptr->tangent;
				prev->next = ptr->next;
				delete ptr;
				ptr = prev->next;
			} 
			else
			{
				prev = ptr;
				ptr = ptr->next;
			}
		}//end while
		tangent = ::Normalize(tangent);
		if (next) 
			next->Normalize();
	}
};

// Compute the face and vertex normals
inline void MAX_ComputeVertexNormals(const Mesh *mesh,std::vector<NormalVerSm>& out) 
{
	Face *face=NULL;
	Point3 *verts=NULL;
	
	Point3 v0, v1, v2;
	Tab<VNormal> vnorms;
	Tab<Point3> fnorms;
	
	face = mesh->faces;
	verts = mesh->verts;
	vnorms.SetCount(mesh->getNumVerts());
	fnorms.SetCount(mesh->getNumFaces());
	
	// Compute face and vertex surface normals
	for (int i = 0; i < mesh->getNumVerts(); i++) 
	{
		vnorms[i] = VNormal();
	}
	
	for (i = 0; i < mesh->getNumFaces(); i++, face++) 
	{
		// Calculate the surface normal
		v0 = verts[face->v[0]];
		v1 = verts[face->v[1]];
		v2 = verts[face->v[2]];
		
		fnorms[i] = (v1-v0)^(v2-v1);
		for (int j=0; j<3; j++) 
		{
			vnorms[face->v[j]].AddNormal(fnorms[i],face->smGroup,face->v[j],i);
		}
	}//endof for
	
	for (i=0; i < mesh->getNumVerts(); i++)
		vnorms[i].Normalize();
	
	out.clear();
	int normalNum=vnorms.Count();
	out.reserve(vnorms.Count());
	for (i = 0; i < vnorms.Count(); i++) 
	{
		VNormal *vn=&vnorms[i];
		while (vn)
		{
			NormalVerSm nvs;
			nvs.normal=vn->norm;
			nvs.vertexIndex=vn->vertexIndex;
			nvs.smGroup=vn->smooth;
			nvs.faceIndex=vn->faceIndex;
			out.push_back(nvs);
			vn=vn->next;
		}
	}
}

//计算顶点切线
inline void MAX_ComputeVertexTangents(const Mesh *mesh,const TVFace *pBumpFace, const Point3 *pBumpUV,
									  std::vector<TangentVerSm>& out)
{
	if (!pBumpFace || !pBumpUV)
		return;

	Face *face=NULL;
	Point3 *verts=NULL;

	Point3 v0, v1, v2;
	Tab<VTangent> vtangents;
	Tab<Point3> ftangents;

	face = mesh->faces;
	verts = mesh->verts;
	vtangents.SetCount(mesh->getNumVerts());
	ftangents.SetCount(mesh->getNumFaces());

	// Compute face and vertex surface tangents
	for (int i = 0; i < mesh->getNumVerts(); i++) 
	{
		vtangents[i] = VTangent();
	}

	Point3 P,Q,T,B,N,N1,N2;
	for (i = 0; i < mesh->getNumFaces(); i++, face++) 
	{
		// Calculate the surface tangent
		// P=v1-v0
		// Q=v2-v0
		// <s1,t1>=<sv1-sv0,tv1-tv0>	N1
		// <s2,t2>=<sv2-sv0,tv2-tv0>	N2
		// Tx=(t2Px-t1Qx)/(s1t2-s2t1)
		// Ty=(t2Py-t1Qy)/(s1t2-s2t1)
		// Tz=(t2Pz-t1Qz)/(s1t2-s2t1)
		v0 = verts[face->v[0]];
		v1 = verts[face->v[1]];
		v2 = verts[face->v[2]];
		P=v1-v0;Q=v2-v0;
		N1=pBumpUV[pBumpFace[i].t[1]]-pBumpUV[pBumpFace[i].t[0]];
		N2=pBumpUV[pBumpFace[i].t[2]]-pBumpUV[pBumpFace[i].t[0]];
		float divisor=N1.x*N2.y-N1.y*N2.x;
		ftangents[i]=(N2.y*P-N1.y*Q)/divisor;
		T=ftangents[i];
		B=(N1.x*Q-N2.x*P)/divisor;
		N=(v1-v0)^(v2-v1);
		//计算tangent、binormal和normal三个向量组成的行列式的值的符号来判断UV的方向
		//T-a1 B-a2 N-a3
		float determinant=T.x*(B.y*N.z-B.z*N.y)-T.y*(B.x*N.z-B.z*N.x)+T.z*(B.x*N.y-B.y*N.x);
		int uvDir=determinant>0?0:1;

		for (int j=0; j<3; j++) 
		{
			vtangents[face->v[j]].AddTangent(ftangents[i],face->smGroup,face->v[j],i,
				pBumpFace[i].t[j],uvDir);
		}
	}//end for

	for (i=0; i < mesh->getNumVerts(); i++)
		vtangents[i].Normalize();

	out.clear();
	int tangentNum=vtangents.Count();
	out.reserve(vtangents.Count());
	for (i=0; i<vtangents.Count(); i++) 
	{
		VTangent *vt=&vtangents[i];
		while (vt)
		{
			TangentVerSm tvs;
			tvs.tangent=vt->tangent;
			tvs.vertexIndex=vt->vertexIndex;
			tvs.smGroup=vt->smooth;
			tvs.faceIndex=vt->faceIndex;
			tvs.uv=vt->uv;
			tvs.uvDir=vt->uvDir;
			out.push_back(tvs);
			vt=vt->next;
		}
	}
}

#endif//#ifndef MAX_VNORMAL_H