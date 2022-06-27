#pragma once

#include "../../Core/Interfaces/Interfaces.hpp"
#include "../../Components/Light/Light.hpp"

#include <unordered_map>
#include <vector>

namespace Pine
{
    class Model;
    class ModelRenderer;

    class IShadowManager : public IInterface {
    public:

        virtual void PrepareScene( ) = 0;

        virtual void PrepareLight( Pine::Light* light ) = 0;

        virtual void Render( std::unordered_map<Model*, std::vector<ModelRenderer*>>& renderBatch ) = 0;

    };
}