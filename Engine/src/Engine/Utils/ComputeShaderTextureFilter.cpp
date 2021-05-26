#include "pch.h"
#include "ComputeShaderTextureFilter.h"
#include "..\Application\Application.h"
#include "..\Renderer\SamplerState.h"
#include "..\Renderer\Shader.h"
#include "..\Renderer\Graphics.h"
#include "..\Renderer\Texture2D.h"
#include "..\Renderer\TextureCube.h"
#include "..\Assets\Assets.h"

namespace Engine
{

    std::shared_ptr<TextureCube> ComputeShaderTextureFilter::EquirectangularToCube(const Texture2D& source, uint16_t resolution) const
    {
        TimeLog timer("Compute Shader (EquirectangularToCube)");

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;

        // The texture cube will have the same format as the source texture
        D3D11_TEXTURE2D_DESC imageDesc;
        source.GetTexture()->GetDesc(&imageDesc);
        const DXGI_FORMAT format = imageDesc.Format;

        {
            //
            // CREATE THE OUTPUT TEXTURECUBE
            //

            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = resolution;
            desc.Height = resolution;
            desc.MipLevels = 1;
            desc.ArraySize = 6;
            desc.Format = format;
            desc.CPUAccessFlags = 0;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

            DX::ThrowIfFailed(Graphics::GetDevice()->CreateTexture2D(&desc, NULL, &texture));
        }

        {
            //
            // CREATE THE OUTPUT UNORDERED_ACCESS_VIEW
            //

            D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.ArraySize = 6;
            
            DX::ThrowIfFailed(Graphics::GetDevice()->CreateUnorderedAccessView(texture.Get(), &desc, &uav));
        }

        {
            //
            // RUN THE COMPUTE SHADER
            //

            Assets::GetComputeShader("equirectangular_to_cube")->Bind();
            Assets::GetSamplerState("compute")->BindCS(0);
            source.BindCS(0);

            WITH_GUARDED_GRAPHICS_CONTEXT(
                Graphics::GetContext()->CSSetUnorderedAccessViews(0, 1, uav.GetAddressOf(), NULL);
                Graphics::GetContext()->Dispatch(resolution, resolution, 6);
            );
        }

        {
            //
            // UNBIND THE UAV
            //

            // We now want to create a shader resource view from the same texture
            // that we used for the unordered access view.
            //
            // Before actually using the SRV in the pixel shader, we need to GPU to
            // flush the surface. We do this now by setting a null resource.
            //
            // TODO: This could be abstracted to an Unbind() function in some class.

            ID3D11UnorderedAccessView* NullUav = nullptr;
            WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->CSSetUnorderedAccessViews(0, 1, &NullUav, nullptr));
        }

        return std::make_shared<TextureCube>(texture);
    }

}

