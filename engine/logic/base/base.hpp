
#ifndef _BASE_HPP_
#define _BASE_HPP_

#include <vector>
#include <string>

#include "../../render/objects/animator.hpp"
#include "../../render/objects/skybox.hpp"
#include "../../render/lighting/shadows/shadow.hpp"

class Window;

namespace Logical
{
	enum
	{
		ENT_BASIC,
		ENT_MESH,
		ENT_MODEL,
		ENT_SCENE,
		ENT_BONE,
		ENT_LIGHT,
		ENT_CAMERA,
		ENT_SHADOW_ORTHO,
		ENT_COLLECTION_BASE,
		ENT_COLLECTION_LOD
	};

	class Transform
	{
	public:
		glm::mat4 model;

		Transform()
		{
			model = glm::mat4(1);

			position = glm::vec3(0);
			rotation = glm::vec3(0);
			scale = glm::vec3(1);
		}

		void update()
		{
			model = glm::mat4(1);

			model = glm::translate(model, position);

			model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
			model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
			model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));

			model = glm::scale(model, scale);
		}

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	};

	class Entity;

	class Script
	{
	public:
		Entity *ent;

		virtual void Setup() {}
		virtual void Update(float deltaTime) {}
	};

	class Entity
	{
	public:
		Script *script;
		std::string name;

		bool enabled;

		Entity *parent;
		std::vector<Entity*> children;

		int type;
		
		Transform *transform;

		struct LODproperties
		{
			float LOD_distanceMin; // distance model appears
			float LOD_distanceMax; // distance model disappears
		};

		LODproperties LOD_properties;

		Entity(std::string name) : name(name)
		{
			type = ENT_BASIC;
			enabled = true;
			parent = NULL;
			script = NULL;

			transform = new Transform();
		}

		virtual ~Entity()
		{
			delete transform;
		}

		void addChild(Entity *ent)
		{
			ent->parent = this;
			children.push_back(ent);
		}

		void attachScript(Script *_script)
		{
			script = _script;
			_script->ent = this;
		}

		/*
		void findEntity(std::string name);
		void findEntityRecursive(std::string name);
		*/
	};

	class EModel : public Entity
	{
	private:
		bool castShadow;

	public:
		Model *mdl;
		Animator *animator;

		EModel(std::string name) : Entity(name)
		{
			type = ENT_MODEL;

			script = NULL;
			castShadow = true;
			animator = new Animator();
		
			LOD_properties = {
				.LOD_distanceMin = 0.0f,
				.LOD_distanceMax = 100.0f
			};
		}


		~EModel() override
		{
			delete animator;
		}
	};

	class ECollection : public Entity
	{
	public:
		ECollection(std::string name) : Entity(name)
		{
			type = ENT_COLLECTION_BASE;
		}
	};

	class ECollectionLOD : public ECollection
	{
	public:
		ECollectionLOD(std::string name) : ECollection(name)
		{
			type = ENT_COLLECTION_LOD;
		}
	};

	class ELight_point : public Entity
	{
	public:
		struct LightProperties
		{
			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;

			float constant;
			float linear;
			float quadratic;
		};

		LightProperties properties;

		ELight_point(std::string name) : Entity(name)
		{
			type = ENT_LIGHT;
			
			properties = {
				glm::vec3(1),
				glm::vec3(1),
				glm::vec3(1),
				1.0f,
				0.052,
				0.0029
			};
		}
	};

	class ECameraBase : public Entity
	{
	public:
		glm::mat4 rotation;

		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;

		ECameraBase(std::string name) : Entity(name)
		{
			type = ENT_CAMERA;
			update();
		}

		void update()
		{
			rotation = glm::mat4(1);
			
			rotation = glm::rotate(rotation, glm::radians(transform->rotation.z), glm::vec3(0, 0, 1));
			rotation = glm::rotate(rotation, glm::radians(transform->rotation.y), glm::vec3(0, 1, 0));
			rotation = glm::rotate(rotation, glm::radians(transform->rotation.x), glm::vec3(1, 0, 0));

			front = glm::vec3(rotation * glm::vec4(0,0,-1,1));
			right = glm::vec3(rotation * glm::vec4(1,0,0,1));
			up = glm::vec3(rotation * glm::vec4(0,1,0,1));
		}

		virtual glm::mat4 getView()
		{
			return glm::mat4(1);
		}

		virtual glm::mat4 getProjection()
		{
			return glm::mat4(1);
		}

		virtual glm::mat4 getProjection(float aspect)
		{
			return glm::mat4(1);
		}
	};

	class ECameraPersp : public ECameraBase
	{
	private:
		float fov;

	public:
		ECameraPersp(std::string name, float fov) : ECameraBase(name), fov(glm::radians(fov))
		{}

		void setFov(float fov)
		{
			this->fov = fov;
		}

		glm::mat4 getView()
		{
			return glm::lookAt(transform->position, transform->position + front, up);
		}
		
		glm::mat4 getProjection(float aspect)
		{
			return glm::perspective(fov, aspect, 0.1f, 1000.0f);
		}
	};

	/*
		Not finished
	*/
	class ECameraOrtho
	{
	private:

	public:
		struct CameraOrthoPrperties
		{
			float zNear;
			float zFar;
			float sizeX;
			float sizeY;
		};

		
	};

	class EShadowCasterDirectOrtho : public Entity, public ShadowCasterDirectBasic
	{
	private:
		glm::mat4 rotation;

		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;

	public:
		
		struct ShadowCasterProperties
		{
			float zNear;
			float zFar;
			float sizeX;
			float sizeY;
		};

		ShadowCasterProperties properties;

		EShadowCasterDirectOrtho(std::string name, int width_l, int height_l) : Entity(name), ShadowCasterDirectBasic(width_l, height_l)
		{
			rotation = glm::mat4(1);

			type = ENT_SHADOW_ORTHO;
			properties = {
				1,
				100,
				10,
				10
			};
		}

		void update()
		{
			rotation = glm::mat4(1);
			
			rotation = glm::rotate(rotation, glm::radians(transform->rotation.z), glm::vec3(0, 0, 1));
			rotation = glm::rotate(rotation, glm::radians(transform->rotation.y), glm::vec3(0, 1, 0));
			rotation = glm::rotate(rotation, glm::radians(transform->rotation.x), glm::vec3(1, 0, 0));

			front = glm::vec3(rotation * glm::vec4(0,0,-1,1));
			right = glm::vec3(rotation * glm::vec4(1,0,0,1));
			up = glm::vec3(rotation * glm::vec4(0,1,0,1));
		}

		glm::mat4 getView()
		{
			return glm::lookAt(transform->position, transform->position + front, up);
		}
		
		glm::mat4 getProjection()
		{
			return glm::ortho(
				-properties.sizeY / 2,
				properties.sizeY / 2,
				-properties.sizeX / 2,
				properties.sizeX / 2,
				properties.zNear,
				properties.zFar
			);
		}
	};

	class ShadowCasterDirectPersp : public ECameraPersp
	{

	};

	class EScene : public Entity
	{
	public:
		Skybox *skybox;

		ECameraBase *activeCamera;

		EScene(std::string name) : Entity(name)
		{
			type = ENT_SCENE;
			skybox = nullptr;
		}
	};

};


namespace Debug
{
	void printMatrix(glm::mat4 matrix)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; ++j)
			{
				printf("%f ", matrix[i][j]);
			}
			printf("\n");
		}
	}
};

#endif //_BASE_HPP_
