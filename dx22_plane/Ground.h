#pragma once

#include "Object.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "Texture.h"

//-----------------------------------------------------------------------------
//TestPlane�N���X
//-----------------------------------------------------------------------------
class Ground :public Object 
{
private:
	// �`��ׂ̈̏��i���b�V���Ɋւ����j
	IndexBuffer	 m_IndexBuffer; // �C���f�b�N�X�o�b�t�@
	VertexBuffer<VERTEX_3D>	m_VertexBuffer; // ���_�o�b�t�@

	// �`��ׂ̈̏��i�����ڂɊւ�镔���j
	Texture m_Texture;//�e�N�X�`��
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