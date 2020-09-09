#pragma once
#include "../IComponent/IComponent.hpp"
#include "../../Assets/Model/Model.hpp"

namespace Pine {

	class ModelRenderer : public IComponent {
	private:
		Pine::Model* m_TargetModel = nullptr;
	public:
		ModelRenderer();

		Pine::Model* GetTargetModel() const;
		void SetTargetModel(Pine::Model* mdl);

		void OnSetup() override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;

		void SaveToJson(nlohmann::json& j) override;
		void LoadFromJson(nlohmann::json& j) override;
		IComponent* Clone() override;
	};

}