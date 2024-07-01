#include <Novice.h>
#include "GeometryUtility.h"
#include <imgui.h>

const char kWindowTitle[] = "学籍番号";

GeometryUtility geometryUtility;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 rotate{ 0.0f,0.0f,0.0f };

	Vector3 translate{ 0.0f,0.0f,0.0f };

	Vector3 translates[3] = {
	{ 0.2f,1.0f,0.0f },
	{ 0.4f,0.0f,0.0f },
	{ 0.3f,0.0f,0.0f },
	};

	Vector3 rotates[3] = {
	{ 0.0f,0.0f,-6.8f },
	{ 0.0f,0.0f,-1.4f },
	{ 0.0f,0.0f,0.0f },
	};
	Vector3 scales[3] = {
	{ 1.0f,1.0f,1.0f },
	{ 1.0f,1.0f,1.0f },
	{ 1.0f,1.0f,1.0f },
	};



	int kWindowWidth = 1280;
	int kWindowHeight = 720;

	float radius = 10.0f;
	float theta = -3.46f;
	float phi = 1.5f;

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Vector3 cameraPosition = geometryUtility.SphericalToCartesian(radius, theta, phi);
		Vector3 cameraTarget = { 0.0f, 0.0f, 0.0f };
		Vector3 cameraUp = { 0.0f, 1.0f, 0.0f };

		Matrix4x4 viewMatrix = geometryUtility.MakeLookAtMatrix4x4(cameraPosition, cameraTarget, cameraUp);
		Matrix4x4 worldMatrix = geometryUtility.MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 projectionMatrix = geometryUtility.MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = geometryUtility.Multiply(worldMatrix, geometryUtility.Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewProjectionMatrix = geometryUtility.Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = geometryUtility.MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);


		Matrix4x4 shoulderMatrix = geometryUtility.MakeAffineMatrix(scales[0], rotates[0], translates[0]);
		Matrix4x4 elbowMatrix = geometryUtility.MakeAffineMatrix(scales[1], rotates[1], translates[1]);
		Matrix4x4 handMatrix = geometryUtility.MakeAffineMatrix(scales[2], rotates[2], translates[2]);

		Matrix4x4 shoulderWorldMatrix = shoulderMatrix;
		Matrix4x4 elbowWorldMatrix = geometryUtility.Multiply(elbowMatrix, shoulderWorldMatrix);
		Matrix4x4 handWorldMatrix = geometryUtility.Multiply(handMatrix, elbowWorldMatrix);

		Vector3 shoulderPosition = { shoulderWorldMatrix.m[3][0], shoulderWorldMatrix.m[3][1], shoulderWorldMatrix.m[3][2] };
		Vector3 elbowPosition = { elbowWorldMatrix.m[3][0], elbowWorldMatrix.m[3][1], elbowWorldMatrix.m[3][2] };
		Vector3 handPosition = { handWorldMatrix.m[3][0], handWorldMatrix.m[3][1], handWorldMatrix.m[3][2] };

		Vector3 shoulderPositionP = geometryUtility.Transform(geometryUtility.Transform(shoulderPosition, viewProjectionMatrix), viewportMatrix);
		Vector3 elbowPositionP = geometryUtility.Transform(geometryUtility.Transform(elbowPosition, viewProjectionMatrix), viewportMatrix);
		Vector3 handPositionP = geometryUtility.Transform(geometryUtility.Transform(handPosition, viewProjectionMatrix), viewportMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		geometryUtility.DrawGrid(viewProjectionMatrix, viewportMatrix);

		geometryUtility.DrawSphere({ shoulderPosition, 0.1f }, viewProjectionMatrix, viewportMatrix, 0xFF0000FF); 
		geometryUtility.DrawSphere({ elbowPosition, 0.1f }, viewProjectionMatrix, viewportMatrix, 0x00FF00FF);    
		geometryUtility.DrawSphere({ handPosition, 0.1f }, viewProjectionMatrix, viewportMatrix, 0x0000FFFF);   

		Novice::DrawLine(int(shoulderPositionP.x), int(shoulderPositionP.y), int(elbowPositionP.x), int(elbowPositionP.y), 0xFFFFFFFF);
		Novice::DrawLine(int(elbowPositionP.x), int(elbowPositionP.y), int(handPositionP.x), int(handPositionP.y), 0xFFFFFFFF);

		ImGui::Begin("Window");
		ImGui::DragFloat("theta", &theta, 0.01f);
		ImGui::DragFloat("phi", &phi, 0.01f);
		ImGui::DragFloat3("translates[0]", &translates[0].x, 0.01f);
		ImGui::DragFloat3("rotates[0]", &rotates[0].x, 0.01f);
		ImGui::DragFloat3("scales[0]", &scales[0].x, 0.01f);
		ImGui::DragFloat3("translates[1]", &translates[1].x, 0.01f);
		ImGui::DragFloat3("rotates[1]", &rotates[1].x, 0.01f);
		ImGui::DragFloat3("scales[1]", &scales[1].x, 0.01f);
		ImGui::DragFloat3("translates[2]", &translates[2].x, 0.01f);
		ImGui::DragFloat3("rotates[2]", &rotates[2].x, 0.01f);
		ImGui::DragFloat3("scales[2]", &scales[2].x, 0.01f);
		ImGui::End();

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
