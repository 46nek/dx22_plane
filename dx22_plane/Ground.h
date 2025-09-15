#pragma once

#include "Object.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "Texture.h"

//-----------------------------------------------------------------------------
//TestPlaneクラス
//-----------------------------------------------------------------------------
class Ground :public Object 
{
private:
	// 描画の為の情報（メッシュに関わる情報）
	IndexBuffer	 m_IndexBuffer; // インデックスバッファ
	VertexBuffer<VERTEX_3D>	m_VertexBuffer; // 頂点バッファ

	// 描画の為の情報（見た目に関わる部分）
	Texture m_Texture;//テクスチャ
	std::unique_ptr<Material> m_Materiale;

	int m_SizeX;
	int m_SizeZ;
	std::vector<VERTEX_3D>m_Vertices;

public:

	Ground(Camera* cam);
	~Ground();
	void Init();
	void Draw();
	void Update();
	void Uninit();

	std::vector<VERTEX_3D>GetVertices();
};