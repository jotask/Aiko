#include "skinned_mesh_importer.h"

#include "animation/skeleton_pose.h"
#include "skinned_mesh_importer.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <limits>
#include <utility>

namespace aiko
{

    AssetImportResult<ImportedModel> SkinnedMeshImporter::importSkinnedModel(const aiScene& scene, ImportedModel imported)
    {
        auto skeletonResult = buildSkeleton(scene);

        if (!skeletonResult)
        {
            return AssetImportResult<ImportedModel>::failure(skeletonResult.error(), std::string(skeletonResult.message()));
        }

        imported.skeleton = std::move(*skeletonResult);

        SkeletonAsset& skeleton = *imported.skeleton;
        if (imported.submeshes.size() != scene.mNumMeshes)
        {
            return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Imported submesh count does not match Assimp scene");
        }

        std::unordered_map<std::string, uint32_t> jointLookup;
        jointLookup.reserve(skeleton.joints.size());

        for (size_t i = 0; i < skeleton.joints.size(); ++i)
        {
            const auto [it, inserted] = jointLookup.emplace(skeleton.joints[i].name, static_cast<uint32_t>(i));

            if (!inserted)
            {
                return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Duplicate skeleton joint: " + skeleton.joints[i].name);
            }
        }

        std::vector<mat4> meshBindTransforms;
        const std::string transformError = collectMeshBindTransforms(scene, meshBindTransforms);
        if (!transformError.empty())
        {
            return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, transformError);
        }

        const std::string bindError = rebuildBindPoseFromOffsets(scene, meshBindTransforms, jointLookup, skeleton);
        if (!bindError.empty())
        {
            return AssetImportResult<ImportedModel>::failure(
                AssetImportError::InvalidData,
                bindError);
        }

        for (unsigned int meshIndex = 0; meshIndex < scene.mNumMeshes; ++meshIndex)
        {
            const aiMesh& source = *scene.mMeshes[meshIndex];
            MeshAsset& destination = imported.submeshes[meshIndex].mesh;
            const mat4& meshBind = meshBindTransforms[meshIndex];

            if (!isFiniteMatrix(meshBind))
            {
                return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Invalid mesh bind transform");
            }

            const mat4 inverseMeshBind = math::inverse(meshBind);

            if (!isFiniteMatrix(inverseMeshBind))
            {
                return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Non-invertible mesh bind transform");
            }

            if (source.HasBones())
            {
                // Validate Assimp's bind-space relationship.
                for (unsigned int i = 0; i < source.mNumBones; ++i)
                {
                    const aiBone* bone = source.mBones[i];

                    if (bone == nullptr)
                    {
                        return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Null bone in skinned mesh");
                    }

                    const auto joint = jointLookup.find(bone->mName.C_Str());

                    if (joint == jointLookup.end())
                    {
                        return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Unknown bone: " + std::string(bone->mName.C_Str()));
                    }

                    const mat4 expectedOffset = skeleton.joints[joint->second].inverseGlobalBindTransform * meshBind;
                    const mat4 actualOffset = toAikoMatrix(bone->mOffsetMatrix);

                    if (!matricesNearlyEqual(expectedOffset, actualOffset))
                    {
                        return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Bone offset mismatch: " + std::string(bone->mName.C_Str()));
                    }
                }

                const std::string weightError = importVertexWeights(source, jointLookup, destination);

                if (!weightError.empty())
                {
                    return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Mesh " + std::to_string(meshIndex) + ": " + weightError);
                }
            }

            // Mesh-local position -> model-space position.
            for (vec3& vertex : destination.m_vertices)
            {
                vertex = math::transformPoint(meshBind, vertex);
            }

            // Transform normals without applying translation.
            const mat4 normalMatrix = math::transpose(inverseMeshBind);

            for (vec3& normal : destination.m_normals)
            {
                const vec4 transformed = normalMatrix * vec4{
                    normal.x,
                    normal.y,
                    normal.z,
                    0.0f
                };

                normal = math::normalize(vec3{
                    transformed.x,
                    transformed.y,
                    transformed.z
                });
            }

            // Transform morph targets into the same space as the base mesh.
            for (MorphTargetAsset& morph : destination.m_morphTargets)
            {
                if (morph.positionDeltas.size() != destination.m_vertices.size())
                {
                    return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Invalid morph position count: " + morph.name);
                }

                // Positions are displacement vectors, so w = 0.
                // Translation must never affect a morph delta.
                for (vec3& delta : morph.positionDeltas)
                {
                    const vec4 transformed = meshBind * vec4{ delta.x, delta.y, delta.z, 0.0f };
                    delta = {
                        transformed.x,
                        transformed.y,
                        transformed.z
                    };

                    if (!math::isfinite(delta))
                    {
                        return AssetImportResult<ImportedModel>::failure( AssetImportError::InvalidData, "Invalid transformed morph position: " + morph.name);
                    }
                }

                if (morph.normalDeltas.empty())
                {
                    continue;
                }

                if (source.mNormals == nullptr || morph.normalDeltas.size() != destination.m_normals.size())
                {
                    return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Invalid morph normal count: " + morph.name);
                }

                for (size_t v = 0; v < morph.normalDeltas.size(); ++v)
                {
                    const aiVector3D& original = source.mNormals[v];

                    const vec3 originalNormal = { original.x, original.y, original.z };

                    // Reconstruct the target normal before changing spaces.
                    const vec3 targetNormal = originalNormal + morph.normalDeltas[v];

                    const vec4 transformed = normalMatrix * vec4{ targetNormal.x, targetNormal.y, targetNormal.z, 0.0f };

                    const vec3 transformedNormal = { transformed.x, transformed.y, transformed.z};

                    if (!math::isfinite(transformedNormal) || math::length(transformedNormal) <= 1e-6f)
                    {
                        return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Invalid transformed morph normal: " + morph.name);
                    }

                    // Base normals have already been normalized above.
                    // Store the difference between normalized target and base.
                    morph.normalDeltas[v] = math::normalize(transformedNormal) - destination.m_normals[v];

                    if (!math::isfinite(morph.normalDeltas[v]))
                    {
                        return AssetImportResult<ImportedModel>::failure(AssetImportError::InvalidData, "Non-finite morph normal delta: " + morph.name);
                    }
                }
            }
        }

        auto animations = importAnimations(scene, skeleton);

        if (!animations)
        {
            return AssetImportResult<ImportedModel>::failure(animations.error(), std::string(animations.message()));
        }

        imported.animations = std::move(*animations);

        logger::Log::infoFmt("Imported {} skeletal animation clips", imported.animations.size());
        logger::Log::infoFmt("Skinned model imported: {} joints, {} submeshes", skeleton.joints.size(), imported.submeshes.size());

        size_t totalMorphTargets = 0;

        for (size_t meshIndex = 0; meshIndex < imported.submeshes.size(); ++meshIndex)
        {
            const auto& targets = imported.submeshes[meshIndex].mesh.m_morphTargets;

            if (targets.empty())
            {
                continue;
            }

            totalMorphTargets += targets.size();

            logger::Log::infoFmt("Morph mesh [{}] '{}': {} targets", meshIndex, scene.mMeshes[meshIndex]->mName.C_Str(), targets.size());

            for (const MorphTargetAsset& morph : targets)
            {
                if (morph.name == "jawOpen" || morph.name == "eyeBlinkLeft")
                {
                    logger::Log::infoFmt("  Found '{}' ({} position deltas)", morph.name, morph.positionDeltas.size());
                }
            }
        }

        logger::Log::infoFmt("Imported {} morph target entries", totalMorphTargets);

        return AssetImportResult<ImportedModel>::success(std::move(imported));
    }

    bool SkinnedMeshImporter::hasSkinnedMeshes(const aiScene& scene)
    {
        for (unsigned int i = 0; i < scene.mNumMeshes; ++i)
        {
            const aiMesh* mesh = scene.mMeshes[i];
            if (mesh != nullptr && mesh->HasBones())
            {
                return true;
            }
        }
        return false;
    }

    mat4 SkinnedMeshImporter::toAikoMatrix(const aiMatrix4x4& source)
    {
        mat4 result{};

        result(0, 0) = source.a1;
        result(0, 1) = source.a2;
        result(0, 2) = source.a3;
        result(0, 3) = source.a4;

        result(1, 0) = source.b1;
        result(1, 1) = source.b2;
        result(1, 2) = source.b3;
        result(1, 3) = source.b4;

        result(2, 0) = source.c1;
        result(2, 1) = source.c2;
        result(2, 2) = source.c3;
        result(2, 3) = source.c4;

        result(3, 0) = source.d1;
        result(3, 1) = source.d2;
        result(3, 2) = source.d3;
        result(3, 3) = source.d4;

        return result;
    }

    void SkinnedMeshImporter::appendSkeletonJoints(const aiNode& node, int32_t parentIndex, const std::unordered_set<const aiNode*>& requiredNodes, SkeletonAsset& skeleton)
    {
        if (requiredNodes.contains(&node) == false)
        {
            return;
        }

        const int32_t jointIndex = static_cast<int32_t>(skeleton.joints.size());

        SkeletonJoint joint{};
        joint.name = node.mName.C_Str();
        joint.parentIndex = parentIndex;
        joint.localBindTransform = toAikoMatrix(node.mTransformation);

        skeleton.joints.push_back(std::move(joint));

        for (unsigned int i = 0; i < node.mNumChildren; ++i)
        {
            appendSkeletonJoints(*node.mChildren[i], jointIndex, requiredNodes, skeleton);
        }
    }

    std::string SkinnedMeshImporter::rebuildBindPoseFromOffsets(const aiScene& scene, const std::vector<mat4>& meshBindTransforms, const std::unordered_map<std::string, uint32_t>& jointLookup, SkeletonAsset& skeleton)
    {
         if (meshBindTransforms.size() != scene.mNumMeshes)
        {
            return "Invalid mesh bind transform count";
        }

        const size_t jointCount = skeleton.joints.size();

        std::vector<mat4> weightedGlobalBind(jointCount, mat4{1.0f});

        std::vector<bool> hasWeightedBind(jointCount, false);

        // Recover each weighted joint's model-space bind matrix
        // from the mesh transform and Assimp's bone offset.
        for (unsigned int meshIndex = 0; meshIndex < scene.mNumMeshes; ++meshIndex)
        {
            const aiMesh* mesh = scene.mMeshes[meshIndex];
            if (mesh == nullptr)
            {
                return "Invalid mesh";
            }

            const mat4& meshBind = meshBindTransforms[meshIndex];
            if (!isFiniteMatrix(meshBind))
            {
                return "Invalid mesh bind transform";
            }

            for (unsigned int i = 0; i < mesh->mNumBones; ++i)
            {
                const aiBone* bone = mesh->mBones[i];
                if (bone == nullptr)
                {
                    return "Invalid bone";
                }

                const auto joint = jointLookup.find(bone->mName.C_Str());

                if (joint == jointLookup.end())
                {
                    return "Unknown bone: " + std::string(bone->mName.C_Str());
                }

                const mat4 offset = toAikoMatrix(bone->mOffsetMatrix);
                const mat4 inverseOffset = math::inverse(offset);

                if (!isFiniteMatrix(offset) || !isFiniteMatrix(inverseOffset))
                {
                    return "Invalid bone offset: " + std::string(bone->mName.C_Str());
                }

                const mat4 bindGlobal = meshBind * inverseOffset;
                if (!isFiniteMatrix(bindGlobal))
                {
                    return "Invalid global bind matrix";
                }

                const uint32_t index = joint->second;

                if (hasWeightedBind[index])
                {
                    // The same joint may influence multiple meshes.
                    // All of them must describe a compatible bind pose.
                    if (!matricesNearlyEqual(weightedGlobalBind[index], bindGlobal))
                    {
                        return "Inconsistent bind pose across meshes: " + std::string(bone->mName.C_Str());
                    }
                }
                else
                {
                    weightedGlobalBind[index] = bindGlobal;
                    hasWeightedBind[index] = true;
                }
            }
        }

        // Reconstruct local transforms in parent-first order.
        // Unweighted intermediary nodes retain their node transforms.
        std::vector<mat4> globalBind(jointCount, mat4{1.0f});

        for (size_t i = 0; i < jointCount; ++i)
        {
            SkeletonJoint& joint = skeleton.joints[i];

            const mat4 parentGlobal = joint.parentIndex < 0 ? mat4{1.0f} : globalBind[static_cast<size_t>( joint.parentIndex)];

            if (hasWeightedBind[i])
            {
                const mat4 inverseParent = math::inverse(parentGlobal);

                if (!isFiniteMatrix(inverseParent))
                {
                    return "Invalid parent bind transform: " + joint.name;
                }

                joint.localBindTransform = inverseParent * weightedGlobalBind[i];
            }

            globalBind[i] = parentGlobal * joint.localBindTransform;

            if (!isFiniteMatrix(globalBind[i]))
            {
                return "Invalid reconstructed bind transform: " + joint.name;
            }

            if (hasWeightedBind[i] && !matricesNearlyEqual( globalBind[i], weightedGlobalBind[i]))
            {
                return "Failed to reconstruct bind pose: " + joint.name;
            }

            const mat4 inverseGlobal =math::inverse(globalBind[i]);
            if (!isFiniteMatrix(inverseGlobal))
            {
                return "Non-invertible bind transform: " + joint.name;
            }

            joint.inverseGlobalBindTransform = inverseGlobal;
        }

        return {};
    }

    AssetImportResult<SkeletonAsset> SkinnedMeshImporter::buildSkeleton(const aiScene& scene)
    {
        if (scene.mRootNode == nullptr)
        {
            return AssetImportResult<SkeletonAsset>::failure(AssetImportError::InvalidData, "Skinned model has no scene root");
        }

        std::unordered_set<std::string> boneNames;
        for (unsigned int meshIndex = 0; meshIndex < scene.mNumMeshes; ++meshIndex)
        {
            const aiMesh* mesh = scene.mMeshes[meshIndex];
            if (mesh == nullptr || (mesh->mNumBones > 0 && mesh->mBones == nullptr))
            {
                return AssetImportResult<SkeletonAsset>::failure(AssetImportError::InvalidData, "Invalid skinned mesh or bone array");
            }

            for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
            {
                const aiBone* bone = mesh->mBones[boneIndex];
                if (bone == nullptr || bone->mName.length == 0)
                {
                    return AssetImportResult<SkeletonAsset>::failure(AssetImportError::InvalidData, "Invalid or unnamed bone");
                }

                boneNames.insert(bone->mName.C_Str());
            }
        }

        if (boneNames.empty())
        {
            return AssetImportResult<SkeletonAsset>::failure(AssetImportError::InvalidData, "Model contains no bones");
        }

        // Find the scene node corresponding to each weighted bone.
        std::unordered_map<std::string, const aiNode*> boneNodes;

        bool invalidHierarchy = false;
        bool duplicateBoneNode = false;

        auto findNodes = [&](auto&& self, const aiNode* node) -> void
        {
            if (node == nullptr)
            {
                invalidHierarchy = true;
                return;
            }

            const std::string name = node->mName.C_Str();

            if (boneNames.contains(name))
            {
                const auto [it, inserted] = boneNodes.emplace(name, node);
                if (!inserted && it->second != node)
                {
                    duplicateBoneNode = true;
                }
            }

            if (node->mNumChildren > 0 && node->mChildren == nullptr)
            {
                invalidHierarchy = true;
                return;
            }

            for (unsigned int i = 0; i < node->mNumChildren; ++i)
            {
                self(self, node->mChildren[i]);
            }
        };

        findNodes(findNodes, scene.mRootNode);

        if (invalidHierarchy || duplicateBoneNode || boneNodes.size() != boneNames.size())
        {
            return AssetImportResult<SkeletonAsset>::failure(AssetImportError::InvalidData, "Missing, duplicate, or invalid skeleton nodes");
        }

        // Include all weighted nodes and their ancestors.
        // This preserves intermediate transforms.
        std::unordered_set<const aiNode*> requiredNodes;

        for (const auto& [name, node] : boneNodes)
        {
            for (const aiNode* current = node; current != nullptr; current = current->mParent)
            {
                requiredNodes.insert(current);
            }
        }

        if (requiredNodes.size() > static_cast<size_t>(std::numeric_limits<int32_t>::max()))
        {
            return AssetImportResult<SkeletonAsset>::failure(AssetImportError::InvalidData, "Skeleton exceeds supported joint count");
        }

        SkeletonAsset skeleton{};
        skeleton.joints.reserve(requiredNodes.size());

        appendSkeletonJoints(*scene.mRootNode, -1, requiredNodes, skeleton);

        if (!SkeletonPoseUtils::validateSkeleton(skeleton))
        {
            return AssetImportResult<SkeletonAsset>::failure(AssetImportError::InvalidData, "Invalid imported skeleton hierarchy");
        }

        // Calculate global bind matrices from local bind transforms.
        SkeletonPose bindPose{};
        SkeletonPoseUtils::resetToBindPose(skeleton, bindPose);

        std::vector<mat4> globalBindTransforms(skeleton.joints.size());
        SkeletonPoseUtils::calculateGlobalTransforms(skeleton, bindPose, globalBindTransforms);

        // The inverse global bind matrices are used by skinning.
        for (size_t i = 0; i < skeleton.joints.size(); ++i)
        {
            const mat4 inverseBind = math::inverse(globalBindTransforms[i]);
            for (int element = 0; element < 16; ++element)
            {
                if (!std::isfinite(inverseBind.data()[element]))
                {
                    return AssetImportResult<SkeletonAsset>::failure(AssetImportError::InvalidData, "Skeleton contains a non-invertible bind transform");
                }
            }
            skeleton.joints[i].inverseGlobalBindTransform = inverseBind;
        }

        return AssetImportResult<SkeletonAsset>::success(std::move(skeleton));
    }

    bool SkinnedMeshImporter::isFiniteMatrix(const mat4& matrix)
    {
        for (int i = 0; i < 16; ++i)
        {
            if (!std::isfinite(matrix.data()[i]))
            {
                return false;
            }
        }
        return true;
    }

    bool SkinnedMeshImporter::matricesNearlyEqual(const mat4& a, const mat4& b)
    {
        for (int i = 0; i < 16; ++i)
        {
            const float x = a.data()[i];
            const float y = b.data()[i];
            if (!std::isfinite(x) || !std::isfinite(y))
            {
                return false;
            }

            const float tolerance = 1e-3f + 1e-4f * std::max(std::fabs(x), std::fabs(y));
            if (std::fabs(x - y) > tolerance)
            {
                return false;
            }
        }

        return true;
    }

    // Returns an empty string on success, otherwise an error.
    std::string SkinnedMeshImporter::importVertexWeights(const aiMesh& source, const std::unordered_map<std::string, uint32_t>& jointLookup, MeshAsset& destination)
    {
        std::vector<std::vector<VertexInfluence>> influences(source.mNumVertices);

        for (unsigned int boneIndex = 0; boneIndex < source.mNumBones; ++boneIndex)
        {
            const aiBone* bone = source.mBones[boneIndex];
            if (bone == nullptr || (bone->mNumWeights > 0 && bone->mWeights == nullptr))
            {
                return "Invalid bone or bone-weight array";
            }

            const auto joint = jointLookup.find( bone->mName.C_Str());
            if (joint == jointLookup.end())
            {
                return "Bone not found in skeleton: " + std::string(bone->mName.C_Str());
            }

            for (unsigned int i = 0; i < bone->mNumWeights; ++i)
            {
                const aiVertexWeight& influence = bone->mWeights[i];
                if (influence.mVertexId >= source.mNumVertices)
                {
                    return "Bone references an invalid vertex";
                }

                const float weight = influence.mWeight;
                if (!std::isfinite(weight) || weight < 0.0f)
                {
                    return "Invalid bone influence weight";
                }

                if (weight == 0.0f)
                {
                    continue;
                }

                auto& vertexInfluences = influences[influence.mVertexId];

                // Combine duplicate influences for the same joint.
                const auto existing = std::find_if(
                    vertexInfluences.begin(),
                    vertexInfluences.end(),
                    [&](const VertexInfluence& entry)
                    {
                        return entry.jointIndex == joint->second;
                    });

                if (existing != vertexInfluences.end())
                {
                    existing->weight += weight;
                    if (!std::isfinite(existing->weight))
                    {
                        return "Accumulated bone weight is invalid";
                    }
                }
                else
                {
                    vertexInfluences.push_back({joint->second, weight});
                }
            }
        }

        destination.m_jointIndices.assign(source.mNumVertices, uvec4{0u});
        destination.m_jointWeights.assign(source.mNumVertices, vec4{0.0f});

        for (size_t vertex = 0; vertex < influences.size(); ++vertex)
        {
            auto& weights = influences[vertex];

            if (weights.empty())
            {
                return "Skinned vertex has no bone influences: " + std::to_string(vertex);
            }

            std::sort(
                weights.begin(),
                weights.end(),
                [](const VertexInfluence& a,
                   const VertexInfluence& b)
                {
                    if (a.weight != b.weight)
                    {
                        return a.weight > b.weight;
                    }

                    return a.jointIndex < b.jointIndex;
                });

            // Hardware skinning uses at most four influences.
            if (weights.size() > 4)
            {
                weights.resize(4);
            }

            float totalWeight = 0.0f;

            for (const VertexInfluence& influence : weights)
            {
                totalWeight += influence.weight;
            }

            if (!std::isfinite(totalWeight) || totalWeight <= 0.0f)
            {
                return "Invalid total vertex weight";
            }

            uint32_t* indices = destination.m_jointIndices[vertex].data();

            float* normalizedWeights = destination.m_jointWeights[vertex].data();

            for (size_t i = 0; i < weights.size(); ++i)
            {
                indices[i] = weights[i].jointIndex;
                normalizedWeights[i] = weights[i].weight / totalWeight;
            }
        }

        return {};
    }

    // Calculate one model-space bind transform per mesh.
    // For this milestone, shared mesh instances are rejected.
    std::string SkinnedMeshImporter::collectMeshBindTransforms(const aiScene& scene, std::vector<mat4>& transforms)
    {
        if (scene.mRootNode == nullptr)
        {
            return "Scene has no root node";
        }

        transforms.assign(scene.mNumMeshes, mat4{1.0f});

        std::vector<bool> visited(scene.mNumMeshes, false);
        std::string error;

        auto visit = [&](auto&& self, const aiNode* node, const mat4& parentGlobal) -> bool
        {
            if (node == nullptr)
            {
                error = "Scene contains a null node";
                return false;
            }

            const mat4 global = parentGlobal * toAikoMatrix(node->mTransformation);

            if (node->mNumMeshes > 0 && node->mMeshes == nullptr)
            {
                error = "Scene node has an invalid mesh array";
                return false;
            }

            for (unsigned int i = 0; i < node->mNumMeshes; ++i)
            {
                const unsigned int meshIndex = node->mMeshes[i];

                if (meshIndex >= scene.mNumMeshes)
                {
                    error = "Scene node references an invalid mesh";
                    return false;
                }

                if (visited[meshIndex])
                {
                    error = "Instanced meshes are not supported yet";
                    return false;
                }

                visited[meshIndex] = true;
                transforms[meshIndex] = global;
            }

            if (node->mNumChildren > 0 && node->mChildren == nullptr)
            {
                error = "Scene node has an invalid child array";
                return false;
            }

            for (unsigned int i = 0; i < node->mNumChildren; ++i)
            {
                if (!self(self, node->mChildren[i], global))
                {
                    return false;
                }
            }

            return true;
        };

        if (!visit(visit, scene.mRootNode, mat4{1.0f}))
        {
            return error;
        }

        for (bool found : visited)
        {
            if (!found)
            {
                return "A mesh has no scene-node reference";
            }
        }

        return {};
    }

    AssetImportResult<std::vector<AnimationClipAsset>> SkinnedMeshImporter::importAnimations(const aiScene& scene, const SkeletonAsset& skeleton)
    {
        std::vector<AnimationClipAsset> result;

        if (scene.mNumAnimations == 0)
        {
            return AssetImportResult<std::vector<AnimationClipAsset>>::success({});
        }

        if (!scene.mAnimations || !scene.mRootNode)
        {
            return AssetImportResult<std::vector<AnimationClipAsset>>::failure(AssetImportError::InvalidData, "Invalid animation scene");
        }

        std::unordered_map<std::string, uint32_t> jointLookup;

        for (uint32_t i = 0; i < skeleton.joints.size(); ++i)
        {
            jointLookup.emplace(skeleton.joints[i].name, i);
        }

        for (unsigned int i = 0; i < scene.mNumAnimations; ++i)
        {
            const aiAnimation* source = scene.mAnimations[i];

            if (!source || (source->mNumChannels > 0 && !source->mChannels))
            {
                return AssetImportResult<std::vector<AnimationClipAsset>>::failure(AssetImportError::InvalidData, "Invalid FBX animation");
            }

            const double ticksPerSecond = source->mTicksPerSecond > 0.0 ? source->mTicksPerSecond : 25.0;

            if (!std::isfinite(ticksPerSecond) || !std::isfinite(source->mDuration) || source->mDuration < 0.0)
            {
                return AssetImportResult<std::vector<AnimationClipAsset>>::failure(AssetImportError::InvalidData, "Invalid animation timing");
            }

            AnimationClipAsset clip{};

            clip.name = source->mName.length > 0 ? source->mName.C_Str() : "Animation_" + std::to_string(i);

            clip.durationSeconds = source->mDuration / ticksPerSecond;

            std::unordered_set<uint32_t> usedJoints;

            for (unsigned int j = 0; j < source->mNumChannels; ++j)
            {
                const aiNodeAnim* sourceChannel = source->mChannels[j];

                if (!sourceChannel)
                {
                    return AssetImportResult<std::vector<AnimationClipAsset>>::failure(AssetImportError::InvalidData, "Null animation channel");
                }

                const auto joint = jointLookup.find( sourceChannel->mNodeName.C_Str());

                if (joint == jointLookup.end())
                {
                    // Ignore channels outside the imported skeleton.
                    continue;
                }

                if (!usedJoints.insert(joint->second).second)
                {
                    return AssetImportResult<std::vector<AnimationClipAsset>>::failure(AssetImportError::InvalidData, "Duplicate joint animation channel");
                }

                const aiNode* node = scene.mRootNode->FindNode(sourceChannel->mNodeName);

                if (!node ||
                    (sourceChannel->mNumPositionKeys &&
                     !sourceChannel->mPositionKeys) ||
                    (sourceChannel->mNumRotationKeys &&
                     !sourceChannel->mRotationKeys) ||
                    (sourceChannel->mNumScalingKeys &&
                     !sourceChannel->mScalingKeys))
                {
                    return AssetImportResult<std::vector<AnimationClipAsset>>::failure(AssetImportError::InvalidData, "Invalid animation node or keyframes");
                }

                aiVector3D defaultScale;
                aiVector3D defaultPosition;
                aiQuaternion defaultRotation;

                node->mTransformation.Decompose(defaultScale, defaultRotation, defaultPosition);

                AnimationChannel channel{};
                channel.jointIndex = joint->second;

                channel.defaultTranslation = {
                    defaultPosition.x,
                    defaultPosition.y,
                    defaultPosition.z
                };

                channel.defaultRotation = {
                    defaultRotation.x,
                    defaultRotation.y,
                    defaultRotation.z,
                    defaultRotation.w
                };

                channel.defaultScale = {
                    defaultScale.x,
                    defaultScale.y,
                    defaultScale.z
                };

                const mat4 sourceBind = toAikoMatrix(node->mTransformation);

                channel.bindCorrection = skeleton.joints[channel.jointIndex].localBindTransform * math::inverse(sourceBind);

                for (unsigned int k = 0; k < sourceChannel->mNumPositionKeys; ++k)
                {
                    const aiVectorKey& key = sourceChannel->mPositionKeys[k];

                    channel.positionKeys.push_back({
                        key.mTime / ticksPerSecond,
                        {key.mValue.x, key.mValue.y, key.mValue.z}
                    });
                }

                for (unsigned int k = 0; k < sourceChannel->mNumRotationKeys; ++k)
                {
                    const aiQuatKey& key = sourceChannel->mRotationKeys[k];

                    channel.rotationKeys.push_back({
                        key.mTime / ticksPerSecond,
                        {
                            key.mValue.x,
                            key.mValue.y,
                            key.mValue.z,
                            key.mValue.w
                        }
                    });
                }

                for (unsigned int k = 0; k < sourceChannel->mNumScalingKeys; ++k)
                {
                    const aiVectorKey& key = sourceChannel->mScalingKeys[k];
                    channel.scalingKeys.push_back({
                        key.mTime / ticksPerSecond,
                        {key.mValue.x, key.mValue.y, key.mValue.z}
                    });
                }

                const auto byTime = [](const auto& a, const auto& b)
                {
                    return a.timeSeconds < b.timeSeconds;
                };

                std::sort(channel.positionKeys.begin(), channel.positionKeys.end(), byTime);
                std::sort(channel.rotationKeys.begin(), channel.rotationKeys.end(), byTime);
                std::sort(channel.scalingKeys.begin(), channel.scalingKeys.end(), byTime);

                const auto extendDuration = [&](const auto& keys)
                {
                    if (!keys.empty())
                    {
                        clip.durationSeconds = std::max(clip.durationSeconds, keys.back().timeSeconds);
                    }
                };

                extendDuration(channel.positionKeys);
                extendDuration(channel.rotationKeys);
                extendDuration(channel.scalingKeys);

                clip.channels.push_back(std::move(channel));
            }

            if (!clip.channels.empty())
            {
                result.push_back(std::move(clip));
            }
        }

        return AssetImportResult<std::vector<AnimationClipAsset>>::success(std::move(result));
    }

}
