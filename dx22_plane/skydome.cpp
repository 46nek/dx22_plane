#include "Skydome.h"
#include "Renderer.h" // Renderer.h�Œ�`����ECullMode���g�����߂ɕK�v
#include "Texture.h"
#include <vector>

// ���N���X(Object)�̃R���X�g���N�^���Ăяo��
Skydome::Skydome(Camera* cam) : Object(cam)
{
}

Skydome::~Skydome()
{
}

void Skydome::Init()
{
	// �V�F�[�_�[�̓ǂݍ��� (���C�e�B���O�̉e�����󂯂Ȃ��V�F�[�_�[���g�p)
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/unlitTexturePS.cso");

	// �e�N�X�`���̓ǂݍ���
	m_Texture = new Texture();
	m_Texture->Load("assets/texture/sky.png"); 

	// ���̃��b�V���̒��_�ƃC���f�b�N�X�𐶐�
	const int sliceCount = 36;
	const int stackCount = 18;
	const float radius = 1.0f;

	std::vector<VERTEX_3D> vertices;
	for (int i = 0; i <= stackCount; ++i)
	{
		float v = (float)i / (float)stackCount;
		float phi = v * DirectX::XM_PI;
		for (int j = 0; j <= sliceCount; ++j)
		{
			float u = (float)j / (float)sliceCount;
			float theta = u * DirectX::XM_2PI;
			VERTEX_3D vertex;
			vertex.position.x = radius * sinf(phi) * cosf(theta);
			vertex.position.y = radius * cosf(phi);
			vertex.position.z = radius * sinf(phi) * sinf(theta);
			vertex.uv.x = u;
			vertex.uv.y = v;
			vertex.normal = vertex.position;
			vertex.normal.Normalize();
			vertex.color = { 1, 1, 1, 1 };
			vertices.push_back(vertex);
		}
	}

	std::vector<unsigned int> indices;
	for (int i = 0; i < stackCount; ++i)
	{
		for (int j = 0; j < sliceCount; ++j)
		{
			int first = (i * (sliceCount + 1)) + j;
			int second = first + sliceCount + 1;
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);
			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}
	m_IndexCount = (unsigned int)indices.size();

	// ���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * (UINT)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertices.data();
	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	// �C���f�b�N�X�o�b�t�@�쐬
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int) * m_IndexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	sd.pSysMem = indices.data();
	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);

	// Object�N���X�̃����o�ϐ����g���ăX�P�[����ݒ�
	m_Scale = { 500.0f, 500.0f, 500.0f };
}

void Skydome::Uninit()
{
	// Init�Ŋm�ۂ������\�[�X�����
	if (m_VertexBuffer) m_VertexBuffer->Release();
	if (m_IndexBuffer) m_IndexBuffer->Release();
	if (m_Texture) delete m_Texture;
	if (m_VertexShader) m_VertexShader->Release();
	if (m_PixelShader) m_PixelShader->Release();
	if (m_VertexLayout) m_VertexLayout->Release();
}

void Skydome::Update()
{
	// ���N���X�����J�����|�C���^(m_Camera)���g���A�J�����̈ʒu�ɒǏ]������
	if (m_Camera)
	{
		m_Position = m_Camera->GetPosition();
	}
}

void Skydome::Draw()
{
	// ���[���h�s���SRT����蓮�Ōv�Z
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_Scale);
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_Position);
	DirectX::SimpleMath::Matrix world = scale * rot * trans;
	Renderer::SetWorldMatrix(&world);

	// �V�F�[�_�[���Z�b�g
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	// �o�b�t�@���Z�b�g
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �v���~�e�B�u�^�C�v��ݒ�
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Texture�N���X�̐������֐� GetSRV() ���g���ăe�N�X�`�����Z�b�g
	ID3D11ShaderResourceView* srv = m_Texture->GetSRV();
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &srv);

	// === �X�J�C�h�[����p�`��ݒ� ===
	Renderer::SetCullMode(C_FRONT);   // �|���S���̓�����`��
	Renderer::SetDepthEnable(false);   // �[�x�𖳎����ď�Ɉ�ԉ��ɕ`��

	// �`����s
	Renderer::GetDeviceContext()->DrawIndexed(m_IndexCount, 0, 0);

	// === �`��ݒ�����ɖ߂� ===
	Renderer::SetCullMode(C_BACK);
	Renderer::SetDepthEnable(true);
}