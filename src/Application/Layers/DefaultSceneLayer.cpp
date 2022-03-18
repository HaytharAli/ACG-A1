#include "DefaultSceneLayer.h"
#pragma region includes
// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)

#include <filesystem>

// Graphics
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"
#include "Graphics/Framebuffer.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Gameplay/InputEngine.h"

#include "Application/Application.h"
#include "Gameplay/Components/ParticleSystem.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Textures/Texture1D.h"
#pragma endregion

DefaultSceneLayer::DefaultSceneLayer() :
	ApplicationLayer()
{
	Name = "Default Scene";
	Overrides = AppLayerFunctions::OnAppLoad;
}

DefaultSceneLayer::~DefaultSceneLayer() = default;

void DefaultSceneLayer::OnAppLoad(const nlohmann::json& config) {
	_CreateScene();
}

void DefaultSceneLayer::_CreateScene()
{
	using namespace Gameplay;
	using namespace Gameplay::Physics;

	Application& app = Application::Get();

	bool loadScene = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene && std::filesystem::exists("scene.json")) {
		app.LoadScene("scene.json");
	} else {
		// Create an empty scene
		Scene::Sptr scene = std::make_shared<Scene>();

#pragma region ShaderCompilation
		// This time we'll have 2 different shaders, and share data between both of them using the UBO
		// This shader will handle reflective materials 
		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr basicShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_textured.glsl" }
		});
		basicShader->SetDebugName("PS1");
#pragma endregion
#pragma region LoadMeshes
		// Load in the meshes
		//MeshResource::Sptr monkeyMesh = ResourceManager::CreateAsset<MeshResource>("Monkey.obj");
		MeshResource::Sptr THR1 = ResourceManager::CreateAsset<MeshResource>("THR0001F.obj");
		MeshResource::Sptr THR2 = ResourceManager::CreateAsset<MeshResource>("THR0002F.obj");
		MeshResource::Sptr THR3 = ResourceManager::CreateAsset<MeshResource>("THR0003F.obj");
		MeshResource::Sptr THR4 = ResourceManager::CreateAsset<MeshResource>("THR0004F.obj");
		MeshResource::Sptr THF1 = ResourceManager::CreateAsset<MeshResource>("THR1301F.obj");
		MeshResource::Sptr THF2 = ResourceManager::CreateAsset<MeshResource>("THR1401F.obj");
		MeshResource::Sptr THF3 = ResourceManager::CreateAsset<MeshResource>("THR1801F.obj");
#pragma endregion
#pragma region LoadTextures
		Texture2D::Sptr TexTHR1 = ResourceManager::CreateAsset<Texture2D>("textures/THR0001F.png");
		TexTHR1->SetMinFilter(MinFilter::Unknown);
		TexTHR1->SetMagFilter(MagFilter::Nearest);
		Texture2D::Sptr TexTHR2 = ResourceManager::CreateAsset<Texture2D>("textures/THR0002F.png");
		TexTHR2->SetMinFilter(MinFilter::Unknown);
		TexTHR2->SetMagFilter(MagFilter::Nearest);
		Texture2D::Sptr TexTHR3 = ResourceManager::CreateAsset<Texture2D>("textures/THR0003F.png");
		TexTHR3->SetMinFilter(MinFilter::Unknown);
		TexTHR3->SetMagFilter(MagFilter::Nearest);
		Texture2D::Sptr TexTHR4 = ResourceManager::CreateAsset<Texture2D>("textures/THR0004F.png");
		TexTHR4->SetMinFilter(MinFilter::Unknown);
		TexTHR4->SetMagFilter(MagFilter::Nearest);
		Texture2D::Sptr TexTHF1 = ResourceManager::CreateAsset<Texture2D>("textures/THR1301F.png");
		TexTHF1->SetMinFilter(MinFilter::Unknown);
		TexTHF1->SetMagFilter(MagFilter::Nearest);
		Texture2D::Sptr TexTHF2 = ResourceManager::CreateAsset<Texture2D>("textures/THR1401F.png");
		TexTHF2->SetMinFilter(MinFilter::Unknown);
		TexTHF2->SetMagFilter(MagFilter::Nearest);
		Texture2D::Sptr TexTHF3 = ResourceManager::CreateAsset<Texture2D>("textures/THR1801F.png");
		TexTHF3->SetMinFilter(MinFilter::Unknown);
		TexTHF3->SetMagFilter(MagFilter::Nearest);

		Texture1D::Sptr toonLut = ResourceManager::CreateAsset<Texture1D>("luts/toon-1D.png");
		toonLut->SetWrap(WrapMode::ClampToEdge);
#pragma endregion
#pragma region Skybox
		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/ocean/ocean.jpg");
		ShaderProgram::Sptr      skyboxShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/skybox_frag.glsl" }
		});

		// Setting up our enviroment map
		scene->SetSkyboxTexture(testCubemap);
		scene->SetSkyboxShader(skyboxShader);
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up 
		scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));
#pragma endregion
#pragma region LUTs
		// Loading in a color lookup table
		Texture3D::Sptr lut = ResourceManager::CreateAsset<Texture3D>("luts/cool.cube");
		Texture3D::Sptr lut2 = ResourceManager::CreateAsset<Texture3D>("luts/warmo.cube");
		Texture3D::Sptr lut3 = ResourceManager::CreateAsset<Texture3D>("luts/Custom.cube");

		// Configure the color correction LUT
		scene->SetColorLUT(lut, 1);
		scene->SetColorLUT(lut2, 2);
		scene->SetColorLUT(lut3, 3);
#pragma endregion
#pragma region CreateMaterials
		Material::Sptr THR1Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			THR1Mat->Name = "THR1";
			THR1Mat->Set("u_Material.Diffuse", TexTHR1);
			THR1Mat->Set("u_Material.Shininess", 0.0f);
			THR1Mat->Set("u_Material.toonTex", toonLut);
		}

		Material::Sptr THR2Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			THR2Mat->Name = "THR2";
			THR2Mat->Set("u_Material.Diffuse", TexTHR2);
			THR2Mat->Set("u_Material.Shininess", 0.0f);
			THR2Mat->Set("u_Material.toonTex", toonLut);
		}

		Material::Sptr THR3Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			THR3Mat->Name = "THR3";
			THR3Mat->Set("u_Material.Diffuse", TexTHR3);
			THR3Mat->Set("u_Material.Shininess", 0.0f);
			THR3Mat->Set("u_Material.toonTex", toonLut);
		}

		Material::Sptr THR4Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			THR4Mat->Name = "THR4";
			THR4Mat->Set("u_Material.Diffuse", TexTHR4);
			THR4Mat->Set("u_Material.Shininess", 0.0f);
			THR4Mat->Set("u_Material.toonTex", toonLut);
		}

		Material::Sptr THF1Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			THF1Mat->Name = "THF1";
			THF1Mat->Set("u_Material.Diffuse", TexTHF1);
			THF1Mat->Set("u_Material.Shininess", 0.0f);
			THF1Mat->Set("u_Material.toonTex", toonLut);
		}

		Material::Sptr THF2Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			THF2Mat->Name = "THF2";
			THF2Mat->Set("u_Material.Diffuse", TexTHF2);
			THF2Mat->Set("u_Material.Shininess", 0.0f);
			THF2Mat->Set("u_Material.toonTex", toonLut);
		}

		Material::Sptr THF3Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			THF3Mat->Name = "THF3";
			THF3Mat->Set("u_Material.Diffuse", TexTHF3);
			THF3Mat->Set("u_Material.Shininess", 0.0f);
			THF3Mat->Set("u_Material.toonTex", toonLut);
		}
#pragma endregion
#pragma region Lighting

		// Create some lights for our scene
		scene->Lights.resize(3);
		scene->Lights[0].Position = glm::vec3(0.0f, 1.0f, 3.0f);
		scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[0].Range = 100.0f;

		scene->Lights[1].Position = glm::vec3(1.0f, 0.0f, 3.0f);
		scene->Lights[1].Color = glm::vec3(0.2f, 0.8f, 0.1f);

		scene->Lights[2].Position = glm::vec3(0.0f, 1.0f, 3.0f);
		scene->Lights[2].Color = glm::vec3(1.0f, 0.2f, 0.1f);

#pragma endregion
#pragma region CreateObjects

		// Set up the scene's camera
		GameObject::Sptr camera = scene->MainCamera->GetGameObject()->SelfRef();
		{
			camera->SetPostion({ 0, 0, 0.23f });
			camera->LookAt(glm::vec3(0.0f));
			camera->SetRotation(glm::vec3(75, 0, 0));

			camera->Add<SimpleCameraControl>();

			// This is now handled by scene itself!
			//Camera::Sptr cam = camera->Add<Camera>();
			// Make sure that the camera is set as the scene's main camera!
			//scene->MainCamera = cam;
		}

		GameObject::Sptr THR1Object = scene->CreateGameObject("THR1Object");
		{
			THR1Object->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));
			THR1Object->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
			THR1Object->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

			RenderComponent::Sptr renderer = THR1Object->Add<RenderComponent>();
			renderer->SetMesh(THR1);
			renderer->SetMaterial(THR1Mat);
		}

		GameObject::Sptr THR2Object = scene->CreateGameObject("THR2Object");
		{
			THR2Object->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));
			THR2Object->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
			THR2Object->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

			RenderComponent::Sptr renderer = THR2Object->Add<RenderComponent>();
			renderer->SetMesh(THR2);
			renderer->SetMaterial(THR2Mat);
		}

		GameObject::Sptr THR3Object = scene->CreateGameObject("THR3Object");
		{
			THR3Object->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));
			THR3Object->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
			THR3Object->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

			RenderComponent::Sptr renderer = THR3Object->Add<RenderComponent>();
			renderer->SetMesh(THR3);
			renderer->SetMaterial(THR3Mat);
		}

		GameObject::Sptr THR4Object = scene->CreateGameObject("THR4Object");
		{
			THR4Object->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));
			THR4Object->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
			THR4Object->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

			RenderComponent::Sptr renderer = THR4Object->Add<RenderComponent>();
			renderer->SetMesh(THR4);
			renderer->SetMaterial(THR4Mat);
		}

		GameObject::Sptr THF1Object = scene->CreateGameObject("THF1Object");
		{
			THF1Object->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));
			THF1Object->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
			THF1Object->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

			RenderComponent::Sptr renderer = THF1Object->Add<RenderComponent>();
			renderer->SetMesh(THF1);
			renderer->SetMaterial(THF1Mat);
		}

		GameObject::Sptr THF2Object = scene->CreateGameObject("THF2Object");
		{
			THF2Object->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));
			THF2Object->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
			THF2Object->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

			RenderComponent::Sptr renderer = THF2Object->Add<RenderComponent>();
			renderer->SetMesh(THF2);
			renderer->SetMaterial(THF2Mat);
		}

		GameObject::Sptr THF3Object = scene->CreateGameObject("THF3Object");
		{
			THF3Object->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));
			THF3Object->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
			THF3Object->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

			RenderComponent::Sptr renderer = THF3Object->Add<RenderComponent>();
			renderer->SetMesh(THF3);
			renderer->SetMaterial(THF3Mat);
		}

		/*
		// Set up all our sample objects
		GameObject::Sptr plane = scene->CreateGameObject("Plane");
		{
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(boxMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = plane->Add<RigidBody>(static by default);
			physics->AddCollider(BoxCollider::Create(glm::vec3(50.0f, 50.0f, 1.0f)))->SetPosition({ 0,0,-1 });
		}

		GameObject::Sptr monkey1 = scene->CreateGameObject("Monkey 1");
		{
			// Set position in the scene
			monkey1->SetPostion(glm::vec3(1.5f, 0.0f, 1.0f));

			// Add some behaviour that relies on the physics body
			monkey1->Add<JumpBehaviour>();

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = monkey1->Add<RenderComponent>();
			renderer->SetMesh(monkeyMesh);
			renderer->SetMaterial(monkeyMaterial);

			// Example of a trigger that interacts with static and kinematic bodies as well as dynamic bodies
			TriggerVolume::Sptr trigger = monkey1->Add<TriggerVolume>();
			trigger->SetFlags(TriggerTypeFlags::Statics | TriggerTypeFlags::Kinematics);
			trigger->AddCollider(BoxCollider::Create(glm::vec3(1.0f)));

			monkey1->Add<TriggerVolumeEnterBehaviour>();
		}

		GameObject::Sptr demoBase = scene->CreateGameObject("Demo Parent");

		// Box to showcase the specular material
		GameObject::Sptr specBox = scene->CreateGameObject("Specular Object");
		{
			MeshResource::Sptr boxMesh = ResourceManager::CreateAsset<MeshResource>();
			boxMesh->AddParam(MeshBuilderParam::CreateCube(ZERO, ONE));
			boxMesh->GenerateMesh();

			// Set and rotation position in the scene
			specBox->SetPostion(glm::vec3(0, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = specBox->Add<RenderComponent>();
			renderer->SetMesh(boxMesh);
			renderer->SetMaterial(testMaterial);

			demoBase->AddChild(specBox);
		}

		// sphere to showcase the foliage material
		GameObject::Sptr foliageBall = scene->CreateGameObject("Foliage Sphere");
		{
			// Set and rotation position in the scene
			foliageBall->SetPostion(glm::vec3(-4.0f, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = foliageBall->Add<RenderComponent>();
			renderer->SetMesh(sphere);
			renderer->SetMaterial(foliageMaterial);

			demoBase->AddChild(foliageBall);
		}

		// Box to showcase the foliage material
		GameObject::Sptr foliageBox = scene->CreateGameObject("Foliage Box");
		{
			MeshResource::Sptr box = ResourceManager::CreateAsset<MeshResource>();
			box->AddParam(MeshBuilderParam::CreateCube(glm::vec3(0, 0, 0.5f), ONE));
			box->GenerateMesh();

			// Set and rotation position in the scene
			foliageBox->SetPostion(glm::vec3(-6.0f, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = foliageBox->Add<RenderComponent>();
			renderer->SetMesh(box);
			renderer->SetMaterial(foliageMaterial);

			demoBase->AddChild(foliageBox);
		}

		// Box to showcase the specular material
		GameObject::Sptr toonBall = scene->CreateGameObject("Toon Object");
		{
			// Set and rotation position in the scene
			toonBall->SetPostion(glm::vec3(-2.0f, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = toonBall->Add<RenderComponent>();
			renderer->SetMesh(sphere);
			renderer->SetMaterial(toonMaterial);

			demoBase->AddChild(toonBall);
		}

		GameObject::Sptr displacementBall = scene->CreateGameObject("Displacement Object");
		{
			// Set and rotation position in the scene
			displacementBall->SetPostion(glm::vec3(2.0f, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = displacementBall->Add<RenderComponent>();
			renderer->SetMesh(sphere);
			renderer->SetMaterial(displacementTest);

			demoBase->AddChild(displacementBall);
		}

		GameObject::Sptr multiTextureBall = scene->CreateGameObject("Multitextured Object");
		{
			// Set and rotation position in the scene 
			multiTextureBall->SetPostion(glm::vec3(4.0f, -4.0f, 1.0f));

			// Add a render component 
			RenderComponent::Sptr renderer = multiTextureBall->Add<RenderComponent>();
			renderer->SetMesh(sphere);
			renderer->SetMaterial(multiTextureMat);

			demoBase->AddChild(multiTextureBall);
		}

		GameObject::Sptr normalMapBall = scene->CreateGameObject("Normal Mapped Object");
		{
			// Set and rotation position in the scene 
			normalMapBall->SetPostion(glm::vec3(6.0f, -4.0f, 1.0f));

			// Add a render component 
			RenderComponent::Sptr renderer = normalMapBall->Add<RenderComponent>();
			renderer->SetMesh(sphere);
			renderer->SetMaterial(normalmapMat);

			demoBase->AddChild(normalMapBall);
		}

		// Create a trigger volume for testing how we can detect collisions with objects!
		GameObject::Sptr trigger = scene->CreateGameObject("Trigger");
		{
			TriggerVolume::Sptr volume = trigger->Add<TriggerVolume>();
			CylinderCollider::Sptr collider = CylinderCollider::Create(glm::vec3(3.0f, 3.0f, 1.0f));
			collider->SetPosition(glm::vec3(0.0f, 0.0f, 0.5f));
			volume->AddCollider(collider);

			trigger->Add<TriggerVolumeEnterBehaviour>();
		}
		*/

		/////////////////////////// UI //////////////////////////////
		/*
		GameObject::Sptr canvas = scene->CreateGameObject("UI Canvas");
		{
			RectTransform::Sptr transform = canvas->Add<RectTransform>();
			transform->SetMin({ 16, 16 });
			transform->SetMax({ 256, 256 });

			GuiPanel::Sptr canPanel = canvas->Add<GuiPanel>();


			GameObject::Sptr subPanel = scene->CreateGameObject("Sub Item");
			{
				RectTransform::Sptr transform = subPanel->Add<RectTransform>();
				transform->SetMin({ 10, 10 });
				transform->SetMax({ 128, 128 });

				GuiPanel::Sptr panel = subPanel->Add<GuiPanel>();
				panel->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

				panel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/upArrow.png"));

				Font::Sptr font = ResourceManager::CreateAsset<Font>("fonts/Roboto-Medium.ttf", 16.0f);
				font->Bake();

				GuiText::Sptr text = subPanel->Add<GuiText>();
				text->SetText("Hello world!");
				text->SetFont(font);

				monkey1->Get<JumpBehaviour>()->Panel = text;
			}

			canvas->AddChild(subPanel);
		}
		*/

		GameObject::Sptr particles = scene->CreateGameObject("Particles");
		{
			ParticleSystem::Sptr particleManager = particles->Add<ParticleSystem>();  
			particleManager->AddEmitter(glm::vec3(0.01f, 0.0f, 4.0f), glm::vec3(-0.01f, 1.0f, -0.5f), 10.0f, glm::vec4(0.5608f, 0.4706f, 0.5323f, 1.0f));
			particleManager->AddEmitter(glm::vec3(-0.1f, 0.0f, 4.0f), glm::vec3(0.1f, 1.0f, -0.5f), 10.0f, glm::vec4(0.5608f, 0.4706f, 0.5323f, 1.0f));
			particleManager->AddEmitter(glm::vec3(0.1f, 0.0f, 4.0f), glm::vec3(-0.1f, 1.0f, -0.5f), 10.0f, glm::vec4(0.5608f, 0.4706f, 0.5323f, 1.0f));
			particleManager->AddEmitter(glm::vec3(-0.5f, 0.0f, 4.0f), glm::vec3(0.5f, 1.0f, -0.5f), 10.0f, glm::vec4(0.5608f, 0.4706f, 0.5323f, 1.0f));
			particleManager->AddEmitter(glm::vec3(0.5f, 0.0f, 4.0f), glm::vec3(-0.5f, 1.0f, -0.5f), 10.0f, glm::vec4(0.5608f, 0.4706f, 0.5323f, 1.0f));

			//camera->AddChild();
		}

#pragma endregion

		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui-sprite.png"));
		GuiBatcher::SetDefaultBorderRadius(8);

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("scene-manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");

		// Send the scene to the application
		app.LoadScene(scene);
	}
}
