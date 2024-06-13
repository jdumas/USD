//
// Copyright 2024 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//

#include "pxr/usd/sdr/shaderProperty.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/property.h"
#include "pxr/usd/usd/relationship.h"
#include "pxr/usd/usd/schemaRegistry.h"
#include "pxr/usd/usd/validationError.h"
#include "pxr/usd/usd/validationRegistry.h"
#include "pxr/usd/usd/validator.h"
#include "pxr/usd/sdr/registry.h"
#include "pxr/base/tf/token.h"
#include "pxr/base/tf/stringUtils.h"
#include "pxr/usd/usdGeom/subset.h"
#include "pxr/usd/usdShade/materialBindingAPI.h"
#include "pxr/usd/usdShade/shader.h"
#include "pxr/usd/usdShade/tokens.h"
#include "pxr/usd/usdShade/validatorTokens.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

static
UsdValidationErrorVector
_ShaderPropertyTypeConformance(const UsdPrim &usdPrim)
{
    if (!(usdPrim && usdPrim.IsInFamily<UsdShadeShader>(
        UsdSchemaRegistry::VersionPolicy::All))) {
        return {};
    }
    UsdShadeShader shader(usdPrim);
    if (!shader) {
        return {};
    }

    const TfTokenVector expectedImplSource =
        {UsdShadeTokens->id, UsdShadeTokens->sourceAsset, 
            UsdShadeTokens->sourceCode};

    const TfToken implSource = shader.GetImplementationSource();
    if (std::find(expectedImplSource.begin(), expectedImplSource.end(),
                  implSource) == expectedImplSource.end()) {
        const UsdValidationErrorSites implSourceErrorSite = {
            UsdValidationErrorSite(
                usdPrim.GetStage(), 
                shader.GetImplementationSourceAttr().GetPath()) };
        return {UsdValidationError(UsdValidationErrorType::Error, 
                            implSourceErrorSite,
                            TfStringPrintf("Shader <%s> has invalid "
                                           "implementation source '%s'.", 
                                           usdPrim.GetPath().GetText(), 
                                           implSource.GetText()))};
    }

    const std::vector<std::string> sourceTypes = shader.GetSourceTypes();
    if (sourceTypes.empty() && implSource != UsdShadeTokens->id) {
        const UsdValidationErrorSites primErrorSite = {
            UsdValidationErrorSite(usdPrim.GetStage(), 
                                   usdPrim.GetPath()) };
        return {UsdValidationError(
            UsdValidationErrorType::Error, 
            primErrorSite, 
            TfStringPrintf("Shader <%s> has no sourceType.", 
                           usdPrim.GetPath().GetText()))};
    }

    UsdValidationErrorVector errors;
    std::unordered_map<TfToken, SdrShaderPropertyConstPtr,
        TfToken::HashFunctor> propNameToPropertyMap;
    if (sourceTypes.empty() && implSource == UsdShadeTokens->id) {
        TfToken shaderId;
        if (shader.GetShaderId(&shaderId)) {
            // Single shaderNode, just emplace all properties, no need to find
            // anything.
            if (SdrShaderNodeConstPtr sdrShaderNode = 
                SdrRegistry::GetInstance().GetShaderNodeByIdentifier(
                    shaderId)) {
                for (const TfToken &propName : sdrShaderNode->GetInputNames()) {
                    if (const SdrShaderPropertyConstPtr sdrProp =
                        sdrShaderNode->GetShaderInput(propName)) {
                        propNameToPropertyMap.emplace(propName, sdrProp);
                    }
                }
            } else {
                const UsdValidationErrorSites shaderIdErrorSite = {
                    UsdValidationErrorSite(usdPrim.GetStage(), 
                                           shader.GetIdAttr().GetPath()) };
                return {UsdValidationError(
                    UsdValidationErrorType::Error,
                    shaderIdErrorSite,
                    TfStringPrintf("shaderId '%s' specified on shader prim "
                                   "<%s> not found in sdrRegistry.",
                                   shaderId.GetText(), 
                                   usdPrim.GetPath().GetText()))};
            }
        }
    } else {
        // Use the SdrShaderNode::CheckPropertyCompliance to find if these do 
        // not match, then report a ValidationError as a warning, since asset 
        // authors have no control on fixing the shaders.
        std::vector<SdrShaderNodeConstPtr> shaderNodesFromSourceTypes;

        // We need to gather all unique inputs from all sdrShaderNodes queried
        // using multiple sourceTypes.
        for (const auto& sourceType : sourceTypes) {
            if (SdrShaderNodeConstPtr sdrShaderNode = 
                shader.GetShaderNodeForSourceType(TfToken(sourceType))) {
                shaderNodesFromSourceTypes.push_back(sdrShaderNode);

                for (const TfToken &propName : sdrShaderNode->GetInputNames()) {
                    // Check if property has already been added to the map.
                    if (propNameToPropertyMap.find(propName) == 
                        propNameToPropertyMap.end()) {
                        if (const SdrShaderPropertyConstPtr sdrProp =
                            sdrShaderNode->GetShaderInput(propName)) {
                            propNameToPropertyMap.emplace(propName, sdrProp);
                        }
                    }
                }
            } else {
                UsdValidationErrorSites sourceTypeSites;
                for (const auto& sourceTypeProp : 
                         usdPrim.GetPropertiesInNamespace(
                             SdfPath::JoinIdentifier("info", sourceType))) {
                    sourceTypeSites.emplace_back(usdPrim.GetStage(), 
                                                 sourceTypeProp.GetPath());
                }
                errors.emplace_back(
                    UsdValidationErrorType::Error,
                    sourceTypeSites,
                    TfStringPrintf("sourceType '%s' specified on shader prim "
                                   "<%s> not found in sdrRegistry.",
                                   sourceType.c_str(), 
                                   usdPrim.GetPath().GetText()));
            }
        }
        SdrShaderNode::ComplianceResults sdrShaderComplianceResults = 
            SdrShaderNode::CheckPropertyCompliance(shaderNodesFromSourceTypes);
        const UsdValidationErrorSites sdrWarnSite = {
                UsdValidationErrorSite(usdPrim.GetStage(), usdPrim.GetPath())
            };
        for (const auto& entry : sdrShaderComplianceResults) {
            // need this for getting the error message!
            std::vector<std::string> shaderNames;
            shaderNames.reserve(entry.second.size());
            for (const auto& shaderName : entry.second) {
                shaderNames.push_back(shaderName.GetString());
            }
            errors.emplace_back(
                UsdValidationErrorType::Warn, sdrWarnSite,
                TfStringPrintf("Shader nodes '%s' have incompatible property "
                               "'%s'.", TfStringJoin(shaderNames).c_str(), 
                                   entry.first.GetText()));
        }
    }

    // Get ground truth data about inputName to types from sdrNode
    const auto sdrPropnameToSdfType = [&propNameToPropertyMap]() {
        std::unordered_map<TfToken, SdfValueTypeName, TfToken::HashFunctor> map;
        for (const auto& prop : propNameToPropertyMap) {
            map.emplace(prop.first, prop.second->GetTypeAsSdfType().first);
        }
        return map;
    }();

    // Compare ground truth data with the inputs on UsdShadeShader prim
    for (const UsdShadeInput &input : shader.GetInputs(false)) {
        const TfToken baseName = input.GetBaseName();
        if (sdrPropnameToSdfType.find(baseName) != 
                sdrPropnameToSdfType.end()) {
            const SdfValueTypeName &expectedSdrInputType = 
                sdrPropnameToSdfType.at(baseName);
            const SdfValueTypeName usdInputType = input.GetTypeName();
            if (usdInputType != expectedSdrInputType) {
                const UsdValidationErrorSites inputErrorSite = {
                        UsdValidationErrorSite(usdPrim.GetStage(),
                                               input.GetAttr().GetPath())
                    };
                errors.emplace_back(
                    UsdValidationErrorType::Error,
                    inputErrorSite,
                    TfStringPrintf("Incorrect type for %s. "
                                   "Expected '%s'; got '%s'.",
                                   input.GetAttr().GetPath().GetText(),
                                   expectedSdrInputType.GetAsToken().GetText(),
                                   usdInputType.GetAsToken().GetText()));
            }
        }
    }
    return errors;
}

static
UsdValidationErrorVector
_SubsetMaterialBindFamilyName(const UsdPrim& usdPrim)
{
    if (!(usdPrim && usdPrim.IsInFamily<UsdGeomSubset>(
            UsdSchemaRegistry::VersionPolicy::All))) {
        return {};
    }

    const UsdGeomSubset subset(usdPrim);
    if (!subset) {
        return {};
    }

    size_t numMatBindingRels = 0u;

    const std::vector<UsdProperty> matBindingProperties =
        usdPrim.GetProperties(
            /* predicate = */ [](const TfToken& name) {
                return UsdShadeMaterialBindingAPI::CanContainPropertyName(
                    name);
            }
        );
    for (const UsdProperty& matBindingProperty : matBindingProperties) {
        if (matBindingProperty.Is<UsdRelationship>()) {
            ++numMatBindingRels;
        }
    }

    if (numMatBindingRels < 1u) {
        return {};
    }

    if (subset.GetFamilyNameAttr().HasAuthoredValue()) {
        return {};
    }

    const UsdValidationErrorSites primErrorSites = {
        UsdValidationErrorSite(usdPrim.GetStage(), usdPrim.GetPath())
    };

    return {
        UsdValidationError(
            UsdValidationErrorType::Error,
            primErrorSites,
            TfStringPrintf(
                "GeomSubset prim <%s> with material bindings applied but "
                "no authored family name should set familyName to '%s'.",
                usdPrim.GetPath().GetText(),
                UsdShadeTokens->materialBind.GetText())
        )
    };
}

TF_REGISTRY_FUNCTION(UsdValidationRegistry)
{
    UsdValidationRegistry &registry = UsdValidationRegistry::GetInstance();

    registry.RegisterPluginValidator(
        UsdShadeValidatorNameTokens->shaderSdrCompliance, 
        _ShaderPropertyTypeConformance);

    registry.RegisterPluginValidator(
        UsdShadeValidatorNameTokens->subsetMaterialBindFamilyName,
        _SubsetMaterialBindFamilyName);
}

PXR_NAMESPACE_CLOSE_SCOPE
