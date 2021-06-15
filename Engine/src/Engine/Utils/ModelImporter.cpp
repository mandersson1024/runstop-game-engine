#include "pch.h"
#include "ModelImporter.h"
#include "..\Assets\Model.h"
#include "..\Renderer\Mesh.h"
#include "..\Renderer\IndexBuffer.h"
#include "..\Renderer\VertexTypes.h"
#include "..\Renderer\VertexBuffer.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"
#include "assimp\scene.h"

namespace Engine
{

    std::shared_ptr<Model> ModelImporter::Import(const std::filesystem::path& assetFilepath)
    {
        TimeLog timer(std::format("Importing {}", assetFilepath.generic_string()));

        auto filepath = GetAssetsFolderPath() / assetFilepath;
        ENGINE_ASSERT(std::filesystem::exists(filepath), std::format("File '{}' not found", filepath.generic_string()));

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(filepath.generic_string(),
            //aiProcess_ConvertToLeftHanded |
            
            aiProcess_MakeLeftHanded |
            aiProcess_FlipUVs |
            aiProcess_FlipWindingOrder |

            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType |
            aiProcess_GenUVCoords |
            aiProcess_RemoveRedundantMaterials);

        ENGINE_ASSERT(scene != nullptr, std::format("Error importing {}", assetFilepath.generic_string()));

        ENGINE_LOG_INFO("The model '{}' has {} meshes, {} materials", assetFilepath.generic_string(), scene->mNumMeshes, scene->mNumMaterials);

        auto model = std::make_shared<Model>();
        model->assetFilepath = assetFilepath;

        for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
        {
            auto& mat = scene->mMaterials[i];

            ENGINE_LOG_INFO("Material {}, name: {}, numProperties {}", i, mat->GetName().C_Str(), mat->mNumProperties);

            aiColor3D color{};
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            ENGINE_LOG("diffuse color: {},{},{}", color.r, color.g, color.b);

            for (uint32_t i = 0; i < mat->mNumProperties; ++i)
            {
                auto prop = mat->mProperties[i];
                auto key = prop->mKey;
                ENGINE_LOG("Property {}: {}", i, key.C_Str());
            }
        }

        for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh* msh = scene->mMeshes[i];
            ENGINE_LOG("Mesh {}: mNumVertices={}, materialIndex={}", i, msh->mNumVertices, msh->mMaterialIndex);

            MeshInfo info
            {
                .name = msh->mName.C_Str(),
                .triangleCount = msh->mNumFaces
            };

            std::vector<float> vertexData;
            for (uint32_t i = 0; i < msh->mNumVertices; ++i)
            {
                vertexData.push_back(msh->mVertices[i].x);
                vertexData.push_back(msh->mVertices[i].y);
                vertexData.push_back(msh->mVertices[i].z);

                vertexData.push_back(msh->mNormals[i].x);
                vertexData.push_back(msh->mNormals[i].y);
                vertexData.push_back(msh->mNormals[i].z);

                if (msh->HasTangentsAndBitangents())
                {
                    vertexData.push_back(msh->mTangents[i].x);
                    vertexData.push_back(msh->mTangents[i].y);
                    vertexData.push_back(msh->mTangents[i].z);

                    // Here we negate the bitangents. If we don't, then we will have to treat the normal 
                    // maps as OpenGL (with flipped Y-direction). 
                    // 
                    // Not sure why. Maybe assimp is doing the conversion to left-handed coordinates
                    // incorrectly or maybe the shader messes up the calculations.
                    // 
                    // If the negation turns out to cause problems, then another solution could be to flip 
                    // the bitangent in the vertex shader.

                    vertexData.push_back(-msh->mBitangents[i].x);
                    vertexData.push_back(-msh->mBitangents[i].y);
                    vertexData.push_back(-msh->mBitangents[i].z);
                }
                else
                {
                    vertexData.push_back(0);
                    vertexData.push_back(0);
                    vertexData.push_back(0);
                    vertexData.push_back(0);
                    vertexData.push_back(0);
                    vertexData.push_back(0);
                }

                if (msh->HasTextureCoords(0))
                {
                    vertexData.push_back(msh->mTextureCoords[0][i].x);
                    vertexData.push_back(msh->mTextureCoords[0][i].y);
                }
                else
                {
                    vertexData.push_back(0);
                    vertexData.push_back(0);
                }
            }
            auto vBuf = std::make_shared<VertexBuffer>(vertexData, GetVertexTypeSize(VertexType::Default));

            std::vector<IndexBuffer::IndexType> indices;
            for (uint32_t i = 0; i < msh->mNumFaces; ++i)
            {
                indices.push_back(msh->mFaces[i].mIndices[0]);
                indices.push_back(msh->mFaces[i].mIndices[1]);
                indices.push_back(msh->mFaces[i].mIndices[2]);
            }
            auto iBuf = std::make_shared<IndexBuffer>(indices);

            auto mesh = std::make_shared<Mesh>(vBuf, iBuf, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, info);

            auto part = ModelPart
            {
                .name = msh->mName.C_Str(),
                .mesh = mesh,
                .materialIndex = msh->mMaterialIndex,
                .materialImportName = scene->mMaterials[msh->mMaterialIndex]->GetName().C_Str(),
            };

            model->parts.push_back(part);
        }

        return model;
    }

}

