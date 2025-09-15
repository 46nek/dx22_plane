#include <memory>
#include "GolfBall.h"
#include "StaticMesh.h"
#include "utility.h"
#include "Collision.h"
#include "input.h"
#include "Game.h"
#include "Ground.h"
#include "Pole.h"

using namespace std;
using namespace DirectX::SimpleMath;

//コンストラクタ
GolfBall::GolfBall(Camera* cam) :Object(cam)
{

}
//デストラクタ
GolfBall::~GolfBall()
{

}

void GolfBall::Init()
{
	// メッシュ読み込み
	StaticMesh staticmesh;

	//3Dモデルデータ
	std::u8string modelFile = u8"assets/model/golfball2/golf_ball.obj";

	//テクスチャディレクトリ
	std::string texDirectory = "assets/model/golf";

	//Meshを読み込む
	std::string tmpStr1(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
	staticmesh.Load(tmpStr1, texDirectory);

	m_MeshRenderer.Init(staticmesh);

	// シェーダオブジェクト生成
	m_Shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

	// サブセット情報取得
	m_subsets = staticmesh.GetSubsets();

	// テクスチャ情報取得
	m_Textures = staticmesh.GetTextures();

	// マテリアル情報取得	
	std::vector<MATERIAL> materials = staticmesh.GetMaterials();

	// マテリアル数分ループ
	for (int i = 0; i < materials.size(); i++)
	{
		// マテリアルオブジェクト生成
		std::unique_ptr<Material> m = std::make_unique<Material>();

		// マテリアル情報をセット
		m->Create(materials[i]);

		// マテリアルオブジェクトを配列に追加
		m_Materiales.push_back(std::move(m));
	}

	//モデルによってスケールを調整
	m_Scale.x = 1;
	m_Scale.y = 1;
	m_Scale.z = 1;
	m_Position.y = 7.0f;
	m_Position.x =50.0f;
}

void GolfBall::Update()
{
	if (m_State != 0) return;
	Vector3 oldPos = m_Position;

	//速度が0に近づいたら停止
	if (m_Velocity.LengthSquared() < 0.03f)
	{
		m_StopCount++;
	}
	else
	{
		m_StopCount = 0;
		//源速度(1フレームあたりどれくらい減速するか)
		float decelerationPower = 0.04f;

		Vector3 deceleration = -m_Velocity;	//速度の逆ベクトルを計算
		deceleration.Normalize();//ベクトルを正規化
		m_Acceralation = deceleration * decelerationPower;

		//加速度を速度に加算
		m_Velocity += m_Acceralation;
	}
	if (m_StopCount > 10)
	{
		m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
		m_State = 1;
	}
	//重力
	const float gravity =0.05f;
	m_Velocity.y -= gravity;

	//速度を座標に加算
	m_Position += m_Velocity;

	m_Rotation.y += 1.0f;

	float radius = 1.0f;

	//groundの頂点データを取得
	vector<Ground*>grounds = Game::GetInstance()->GetObjects<Ground>();
	vector<VERTEX_3D>vertices;
	for (auto& g : grounds)
	{
		vector<VERTEX_3D> vecs = g->GetVertices();
		for (auto& v : vecs)
		{
			vertices.emplace_back(v);
		}
	}

	float moveDistance = 9999;
	Vector3 contactPoint;
	Vector3 normal;

	//球体とポリゴンの当たり判定
	for (int i = 0; i < vertices.size(); i += 3) {
		//三角形ポリゴン
		Collision::Polygon collisionPolygon = {
			vertices[i + 0].position,
			vertices[i + 1].position,
			vertices[i + 2].position
		};
		Vector3 cp;
		Collision::Sphere collisionSphere = { m_Position,radius };
		if (Collision::CheckHit(collisionSphere, collisionPolygon, cp))
		{
			float md = 0;
			Vector3 np = Collision::moveSphere(collisionSphere, collisionPolygon, cp);
			md = (np - oldPos).Length();
			if (moveDistance > md)
			{
				moveDistance = md;
				m_Position = np;
				contactPoint = cp;
				normal = Collision::GetNormal(collisionPolygon);
			}
		}
	}
	if (moveDistance != 9999)
	{
		m_Velocity.y = -gravity;

		float velocityNormal = Collision::Dot(m_Velocity, normal);
		Vector3 v1 = velocityNormal * normal;
		Vector3 v2 = m_Velocity - v1;

		const float restitution = 0.5f;

		Vector3 reflectedVelocity = v2 - restitution * v1;

		m_Velocity = reflectedVelocity;
	}
	if (m_Position.y < -100)
	{
		m_Position = Vector3(0.0f, 50.0f, 0.0f);
		m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
	}

	vector<Pole*> pole = Game::GetInstance()->GetObjects<Pole>();
	if (pole.size() > 0) {
		Vector3 polePos = pole[0]->GetPosition();
		Collision::Sphere balCollision = { m_Position,radius };
		Collision::Sphere poleCollision = { polePos,0.5f };
	
		if (Collision::CheckHit(balCollision, poleCollision)) {
			m_State = 2;
		}
	}
}

void GolfBall::Draw()
{
	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	m_Shader.SetGPU();

	// インデックスバッファ・頂点バッファをセット
	m_MeshRenderer.BeforeDraw();

	//カメラの設定を指定
	m_Camera->SetCamera(0);
	//マテリアル数分ループ 
	for (int i = 0; i < m_subsets.size(); i++)
	{
		// マテリアルをセット(サブセット情報の中にあるマテリアルインデックスを使用)
		m_Materiales[m_subsets[i].MaterialIdx]->SetGPU();

		if (m_Materiales[m_subsets[i].MaterialIdx]->isTextureEnable())
		{
			m_Textures[m_subsets[i].MaterialIdx]->SetGPU();
		}

		m_MeshRenderer.DrawSubset(
			m_subsets[i].IndexNum,		// 描画するインデックス数
			m_subsets[i].IndexBase,		// 最初のインデックスバッファの位置	
			m_subsets[i].VertexBase);	// 頂点バッファの最初から使用
	}
}

void GolfBall::Uninit()
{

}
void GolfBall :: SetState(int s) { m_State = s; }
int GolfBall :: GetState() { return m_State; }

void GolfBall::Shot(Vector3 v) { m_Velocity = v; }