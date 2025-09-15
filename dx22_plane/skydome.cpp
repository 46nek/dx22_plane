#include "Skydome.h"
#include "Renderer.h" // Renderer.hで定義したECullModeを使うために必要
#include "Texture.h"
#include <vector>

// 基底クラス(Object)のコンストラクタを呼び出す
Skydome::Skydome(Camera* cam) : Object(cam)
{
}

Skydome::~Skydome()
{
}

void Skydome::Init()
{
	// シェーダーの読み込み (ライティングの影響を受けないシェーダーを使用)
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/unlitTexturePS.cso");

	// テクスチャの読み込み
	m_Texture = new Texture();
	m_Texture->Load("assets/texture/sky.png"); 

	// 球体メッシュの頂点とインデックスを生成
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

	// 頂点バッファ作成
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * (UINT)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertices.data();
	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	// インデックスバッファ作成
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int) * m_IndexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	sd.pSysMem = indices.data();
	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);

	// Objectクラスのメンバ変数を使ってスケールを設定
	m_Scale = { 500.0f, 500.0f, 500.0f };
}

void Skydome::Uninit()
{
	// Initで確保したリソースを解放
	if (m_VertexBuffer) m_VertexBuffer->Release();
	if (m_IndexBuffer) m_IndexBuffer->Release();
	if (m_Texture) delete m_Texture;
	if (m_VertexShader) m_VertexShader->Release();
	if (m_PixelShader) m_PixelShader->Release();
	if (m_VertexLayout) m_VertexLayout->Release();
}

void Skydome::Update()
{
	// 基底クラスが持つカメラポインタ(m_Camera)を使い、カメラの位置に追従させる
	if (m_Camera)
	{
		m_Position = m_Camera->GetPosition();
	}
}

void Skydome::Draw()
{
	// ワールド行列をSRTから手動で計算
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_Scale);
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_Position);
	DirectX::SimpleMath::Matrix world = scale * rot * trans;
	Renderer::SetWorldMatrix(&world);

	// シェーダーをセット
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	// バッファをセット
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// プリミティブタイプを設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Textureクラスの正しい関数 GetSRV() を使ってテクスチャをセット
	ID3D11ShaderResourceView* srv = m_Texture->GetSRV();
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &srv);

	// === スカイドーム専用描画設定 ===
	Renderer::SetCullMode(C_FRONT);   // ポリゴンの内側を描画
	Renderer::SetDepthEnable(false);   // 深度を無視して常に一番奥に描画

	// 描画実行
	Renderer::GetDeviceContext()->DrawIndexed(m_IndexCount, 0, 0);

	// === 描画設定を元に戻す ===
	Renderer::SetCullMode(C_BACK);
	Renderer::SetDepthEnable(true);
}