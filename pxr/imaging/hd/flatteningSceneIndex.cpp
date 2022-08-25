//
// Copyright 2021 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "pxr/imaging/hd/flatteningSceneIndex.h"
#include "pxr/imaging/hd/overlayContainerDataSource.h"
#include "pxr/imaging/hd/retainedDataSource.h"
#include "pxr/imaging/hd/tokens.h"
#include "pxr/imaging/hd/xformSchema.h"
#include "pxr/imaging/hd/purposeSchema.h"
#include "pxr/imaging/hd/visibilitySchema.h"
#include "pxr/imaging/hd/materialBindingSchema.h"
#include "pxr/base/trace/trace.h"
#include "pxr/base/work/utils.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    (model)
    (drawMode)
    (inherited)
);

static
const HdDataSourceLocator &_GetDrawModeLocator()
{
    static const HdDataSourceLocator l(_tokens->model, _tokens->drawMode);
    return l;
}

HdFlatteningSceneIndex::HdFlatteningSceneIndex(
        const HdSceneIndexBaseRefPtr &inputScene)
    : HdSingleInputFilteringSceneIndexBase(inputScene)

    , _identityXform(HdXformSchema::Builder()
        .SetMatrix(
            HdRetainedTypedSampledDataSource<GfMatrix4d>::New(
                    GfMatrix4d().SetIdentity()))
        .Build())

    , _identityVis(HdVisibilitySchema::Builder()
        .SetVisibility(
            HdRetainedTypedSampledDataSource<bool>::New(true))
        .Build())

    , _identityPurpose(HdPurposeSchema::Builder()
        .SetPurpose(
            HdRetainedTypedSampledDataSource<TfToken>::New(
                HdRenderTagTokens->geometry))
        .Build())

    , _identityDrawMode(
        HdRetainedTypedSampledDataSource<TfToken>::New(TfToken()))
{
}

HdFlatteningSceneIndex::~HdFlatteningSceneIndex() = default;

HdSceneIndexPrim
HdFlatteningSceneIndex::GetPrim(const SdfPath &primPath) const
{
    const auto it = _prims.find(primPath);
    if (it != _prims.end()) {
        return it->second.prim;
    }

    if (_GetInputSceneIndex()) {
        return _GetInputSceneIndex()->GetPrim(primPath);
    }

    return {TfToken(), nullptr};
}

SdfPathVector
HdFlatteningSceneIndex::GetChildPrimPaths(const SdfPath &primPath) const
{
    // we don't change topology so we can dispatch to input
    if (_GetInputSceneIndex()) {
        return _GetInputSceneIndex()->GetChildPrimPaths(primPath);
    }

    return {};
}

void
HdFlatteningSceneIndex::_PrimsAdded(
        const HdSceneIndexBase &sender,
        const HdSceneIndexObserver::AddedPrimEntries &entries)
{
    TRACE_FUNCTION();

    HdSceneIndexObserver::DirtiedPrimEntries dirtyEntries;

    for (const HdSceneIndexObserver::AddedPrimEntry &entry : entries) {
        // XXX immediately calls GetPrim (for now)
        HdContainerDataSourceHandle dataSource =
            _GetInputSceneIndex()->GetPrim(entry.primPath).dataSource;

        // Ensure the prim has an entry in the map.
        auto iterBoolPair =
            _prims.insert({entry.primPath, {HdSceneIndexPrim()}});
        HdSceneIndexPrim &prim = iterBoolPair.first->second.prim;

        // Always update the prim type.
        prim.primType = entry.primType;

        // If the wrapper exists, update the input datasource; otherwise,
        // create it. This is both to save an allocation if the PrimsAdded
        // message is resyncing a prim, and also to leave the cache intact for
        // _DirtyHierarchy to invalidate (since it chooses what to invalidate
        // based on what's been cached).
        if (prim.dataSource) {
            _PrimLevelWrappingDataSource::Cast(prim.dataSource)->
                UpdateInputDataSource(dataSource);
        } else {
            prim.dataSource = _PrimLevelWrappingDataSource::New(
                *this, entry.primPath, dataSource);
        }

        // If we're inserting somewhere in the existing hierarchy, we need to
        // invalidate descendant flattened attributes.
        if (!iterBoolPair.second) {
            static HdDataSourceLocatorSet locators = {
                HdXformSchema::GetDefaultLocator(),
                HdVisibilitySchema::GetDefaultLocator(),
                HdPurposeSchema::GetDefaultLocator(),
                HdDataSourceLocator(_tokens->model)
            };

            _DirtyHierarchy(entry.primPath, locators, &dirtyEntries);
        }
    }

    _SendPrimsAdded(entries);
    if (!dirtyEntries.empty()) {
        _SendPrimsDirtied(dirtyEntries);
    }
}

void
HdFlatteningSceneIndex::_PrimsRemoved(
    const HdSceneIndexBase &sender,
    const HdSceneIndexObserver::RemovedPrimEntries &entries)
{
    TRACE_FUNCTION();

    for (const HdSceneIndexObserver::RemovedPrimEntry &entry : entries) {
        if (entry.primPath.IsAbsoluteRootPath()) {
            // Special case removing the whole scene, since this is a common
            // shutdown operation.
            _prims.ClearInParallel();
            TfReset(_prims);
        } else {
            auto startEndIt = _prims.FindSubtreeRange(entry.primPath);
            for (auto it = startEndIt.first; it != startEndIt.second; ++it) {
                WorkSwapDestroyAsync(it->second.prim.dataSource);
            }
            if (startEndIt.first != startEndIt.second) {
                _prims.erase(startEndIt.first);
            }
        }
    }
    _SendPrimsRemoved(entries);
}

void
HdFlatteningSceneIndex::_PrimsDirtied(
    const HdSceneIndexBase &sender,
    const HdSceneIndexObserver::DirtiedPrimEntries &entries)
{
    TRACE_FUNCTION();

    HdSceneIndexObserver::DirtiedPrimEntries dirtyEntries;

    for (const HdSceneIndexObserver::DirtiedPrimEntry &entry : entries) {
        HdDataSourceLocatorSet locators;
        if (entry.dirtyLocators.Intersects(
                    HdXformSchema::GetDefaultLocator())) {
            locators.insert(HdXformSchema::GetDefaultLocator());
        }
        if (entry.dirtyLocators.Intersects(
                    HdVisibilitySchema::GetDefaultLocator())) {
            locators.insert(HdVisibilitySchema::GetDefaultLocator());
        }
        if (entry.dirtyLocators.Intersects(
                    HdPurposeSchema::GetDefaultLocator())) {
            locators.insert(HdPurposeSchema::GetDefaultLocator());
        }
        static const HdDataSourceLocator modelLocator(_tokens->model);
        if (entry.dirtyLocators.Intersects(
                    modelLocator)) {
            locators.insert(modelLocator);
        }

        if (!locators.IsEmpty()) {
            _DirtyHierarchy(entry.primPath, locators, &dirtyEntries);
        }
    }

    _SendPrimsDirtied(entries);
    if (!dirtyEntries.empty()) {
        _SendPrimsDirtied(dirtyEntries);
    }
}

void
HdFlatteningSceneIndex::_DirtyHierarchy(
    const SdfPath &primPath,
    const HdDataSourceLocatorSet &dirtyLocators,
    HdSceneIndexObserver::DirtiedPrimEntries *dirtyEntries)
{
    // XXX: here and elsewhere, if a parent xform is dirtied and the child has
    // resetXformStack, we could skip dirtying the child...

    auto startEndIt = _prims.FindSubtreeRange(primPath);
    auto it = startEndIt.first;
    for (; it != startEndIt.second; ) {
        _PrimEntry &entry = it->second;

        if (_PrimLevelWrappingDataSourceHandle dataSource =
                _PrimLevelWrappingDataSource::Cast(
                        entry.prim.dataSource)) {
            if (dataSource->PrimDirtied(dirtyLocators)) {
                // If we invalidated any data for any prim besides "primPath"
                // (which already has a notice), generate a new PrimsDirtied
                // notice.
                if (it->first != primPath) {
                    dirtyEntries->emplace_back(it->first, dirtyLocators);
                }
                ++it;
            } else {
                // If we didn't invalidate any data, we can safely assume that
                // no downstream prims depended on this prim for their
                // flattened result, and skip to the next subtree. This is
                // an important optimization for (e.g.) scene population,
                // where no data is cached yet...
                it = it.GetNextSubtree();
            }
        } else {
            ++it;
        }
    }
}

HdFlatteningSceneIndex::
_PrimLevelWrappingDataSource::_PrimLevelWrappingDataSource(
        const HdFlatteningSceneIndex &scene,
        const SdfPath &primPath,
        HdContainerDataSourceHandle inputDataSource)
    : _sceneIndex(scene)
    , _primPath(primPath)
    , _inputDataSource(inputDataSource)
    , _computedXformDataSource(nullptr)
    , _computedVisDataSource(nullptr)
    , _computedPurposeDataSource(nullptr)
{
}

void
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::UpdateInputDataSource(
        HdContainerDataSourceHandle inputDataSource)
{
    _inputDataSource = inputDataSource;
}

bool
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::PrimDirtied(
        const HdDataSourceLocatorSet &set)
{
    bool anyDirtied = false;
    static const HdContainerDataSourceHandle containerNull(nullptr);
    static const HdTokenDataSourceHandle tokenNull(nullptr);
    static const HdDataSourceBaseHandle baseNull(nullptr);

    if (set.Intersects(HdXformSchema::GetDefaultLocator())) {
        if (HdContainerDataSource::AtomicLoad(_computedXformDataSource)) {
            anyDirtied = true;
        }
        HdContainerDataSource::AtomicStore(
            _computedXformDataSource, containerNull);
    }
    if (set.Intersects(HdVisibilitySchema::GetDefaultLocator())) {
        if (HdContainerDataSource::AtomicLoad(_computedVisDataSource)) {
            anyDirtied = true;
        }
        HdContainerDataSource::AtomicStore(
            _computedVisDataSource, containerNull);
    }
    if (set.Intersects(HdPurposeSchema::GetDefaultLocator())) {
        if (HdContainerDataSource::AtomicLoad(_computedPurposeDataSource)) {
            anyDirtied = true;
        }
        HdContainerDataSource::AtomicStore(
            _computedPurposeDataSource, containerNull);
    }
    if (set.Intersects(_GetDrawModeLocator())) {
        anyDirtied = true;
        HdTokenDataSource::AtomicStore(
            _computedDrawModeDataSource, tokenNull);
    }
    if (set.Intersects(HdMaterialBindingSchema::GetDefaultLocator())) {
        anyDirtied = true;
        HdDataSourceBase::AtomicStore(
            _computedMaterialBindingDataSource, baseNull);
    }

    return anyDirtied;
}

bool
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::Has(
    const TfToken &name)
{
    if (name == HdXformSchemaTokens->xform) {
        return true;
    }
    if (name == HdVisibilitySchemaTokens->visibility) {
        return true;
    }
    if (name == HdPurposeSchemaTokens->purpose) {
        return true;
    }
    if (name == _tokens->model) {
        return true;
    }

    if (name == HdMaterialBindingSchemaTokens->materialBinding) {
        return true;
    }

    if (!_inputDataSource) {
        return false;
    }

    return _inputDataSource->Has(name);
}

TfTokenVector
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::GetNames()
{
    TfTokenVector result;

    if (_inputDataSource) {
        result = _inputDataSource->GetNames();
        
        bool hasXform = false;
        bool hasVis = false;
        bool hasPurpose = false;
        bool hasModel = false;
        bool hasMaterialBinding = false;
        for (const TfToken &name : result) {
            if (name == HdXformSchemaTokens->xform) {
                hasXform = true;
            }
            if (name == HdVisibilitySchemaTokens->visibility) {
                hasVis = true;
            }
            if (name == HdPurposeSchemaTokens->purpose) {
                hasPurpose = true;
            }
            if (name == _tokens->model) {
                hasModel = true;
            }
            if (name == HdMaterialBindingSchemaTokens->materialBinding) {
                hasMaterialBinding = true;
            }
            if (hasXform && hasVis && hasPurpose && hasModel
                    && hasMaterialBinding) {
                break;
            }
        }

        if (!hasXform) {
            result.push_back(HdXformSchemaTokens->xform);
        }
        if (!hasVis) {
            result.push_back(HdVisibilitySchemaTokens->visibility);
        }
        if (!hasPurpose) {
            result.push_back(HdPurposeSchemaTokens->purpose);
        }
        if (!hasModel) {
            result.push_back(_tokens->model);
        }
        if (!hasMaterialBinding) {
            result.push_back(HdMaterialBindingSchemaTokens->materialBinding);
        }
    } else {
        result.push_back(HdXformSchemaTokens->xform);
        result.push_back(HdVisibilitySchemaTokens->visibility);
        result.push_back(HdPurposeSchemaTokens->purpose);
        result.push_back(_tokens->model);
        result.push_back(HdMaterialBindingSchemaTokens->materialBinding);
    }

    return result;
}

HdDataSourceBaseHandle
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::Get(
        const TfToken &name)
{
    if (name == HdXformSchemaTokens->xform) {
        return _GetXform();
    } else if (name == HdVisibilitySchemaTokens->visibility) {
        return _GetVis();
    } else if (name == HdPurposeSchemaTokens->purpose) {
        return _GetPurpose();
    } else if (name == _tokens->model) {
        return _GetModel();
    } else if (name == HdMaterialBindingSchemaTokens->materialBinding) {
        return _GetMaterialBinding();
    } else if (_inputDataSource) {
        return _inputDataSource->Get(name);
    } else {
        return nullptr;
    }
}

HdDataSourceBaseHandle
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::_GetPurpose()
{
    HdContainerDataSourceHandle computedPurposeDataSource =
        HdContainerDataSource::AtomicLoad(_computedPurposeDataSource);

    if (computedPurposeDataSource) {
        return computedPurposeDataSource;
    }

    HdPurposeSchema inputPurpose =
        HdPurposeSchema::GetFromParent(_inputDataSource);

    if (inputPurpose) {
        if (inputPurpose.GetPurpose()) {
            computedPurposeDataSource = inputPurpose.GetContainer();
        } else {
            computedPurposeDataSource = _sceneIndex._identityPurpose;
        }
    } else {
        HdPurposeSchema parentPurpose(nullptr);
        if (_primPath.GetPathElementCount()) {
            SdfPath parentPath = _primPath.GetParentPath();
            const auto it = _sceneIndex._prims.find(parentPath);
            if (it != _sceneIndex._prims.end()) {
                parentPurpose = HdPurposeSchema::GetFromParent(
                        it->second.prim.dataSource);
            }
        }
        if (parentPurpose && parentPurpose.GetPurpose()) {
            computedPurposeDataSource = parentPurpose.GetContainer();
        } else {
            computedPurposeDataSource = _sceneIndex._identityPurpose;
        }
    }

    HdContainerDataSource::AtomicStore(
            _computedPurposeDataSource, computedPurposeDataSource);

    return computedPurposeDataSource;
}

HdDataSourceBaseHandle
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::_GetVis()
{
    HdContainerDataSourceHandle computedVisDataSource =
        HdContainerDataSource::AtomicLoad(_computedVisDataSource);

    if (computedVisDataSource) {
        return computedVisDataSource;
    }

    HdVisibilitySchema inputVis =
        HdVisibilitySchema::GetFromParent(_inputDataSource);

    if (inputVis) {
        if (inputVis.GetVisibility()) {
            computedVisDataSource = inputVis.GetContainer();
        } else {
            computedVisDataSource = _sceneIndex._identityVis;
        }
    } else {
        HdVisibilitySchema parentVis(nullptr);
        if (_primPath.GetPathElementCount()) {
            SdfPath parentPath = _primPath.GetParentPath();
            const auto it = _sceneIndex._prims.find(parentPath);
            if (it != _sceneIndex._prims.end()) {
                parentVis = HdVisibilitySchema::GetFromParent(
                        it->second.prim.dataSource);
            }
        }
        if (parentVis && parentVis.GetVisibility()) {
            computedVisDataSource = parentVis.GetContainer();
        } else {
            computedVisDataSource = _sceneIndex._identityVis;
        }
    }

    HdContainerDataSource::AtomicStore(
            _computedVisDataSource, computedVisDataSource);

    return computedVisDataSource;
}

HdDataSourceBaseHandle
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::_GetXform()
{
    HdContainerDataSourceHandle computedXformDataSource =
            HdContainerDataSource::AtomicLoad(_computedXformDataSource);

    // previously cached value
    if (computedXformDataSource) {
        return computedXformDataSource;
    }

    HdXformSchema inputXform =
            HdXformSchema::GetFromParent(_inputDataSource);

    // If this xform is fully composed, early out.
    if (inputXform) {
        HdBoolDataSourceHandle resetXformStack =
            inputXform.GetResetXformStack();
        if (resetXformStack && resetXformStack->GetTypedValue(0.0f)) {
            // Only use the local transform, or identity if no matrix was
            // provided...
            if (inputXform.GetMatrix()) {
                computedXformDataSource = inputXform.GetContainer();
            } else {
                computedXformDataSource = _sceneIndex._identityXform;
            }
            HdContainerDataSource::AtomicStore(
                _computedXformDataSource, computedXformDataSource);

            return computedXformDataSource;
        }
    }

    // Otherwise, we need to look at the parent value.
    HdXformSchema parentXform(nullptr);
    if (_primPath.GetPathElementCount()) {
        SdfPath parentPath = _primPath.GetParentPath();
        const auto it = _sceneIndex._prims.find(parentPath);
        if (it != _sceneIndex._prims.end()) {
            parentXform = HdXformSchema::GetFromParent(
                    it->second.prim.dataSource);
        }
    }

    // Attempt to compose the local matrix with the parent matrix;
    // note that since we got the parent matrix from _prims instead of
    // _inputDataSource, the parent matrix should be flattened already.
    // If either of the local or parent matrix are missing, they are
    // interpreted to be identity.
    HdMatrixDataSourceHandle parentMatrixDataSource =
        parentXform ? parentXform.GetMatrix() : nullptr;
    HdMatrixDataSourceHandle inputMatrixDataSource =
        inputXform ? inputXform.GetMatrix() : nullptr;

    if (inputMatrixDataSource && parentMatrixDataSource) {
        GfMatrix4d parentMatrix =
            parentMatrixDataSource->GetTypedValue(0.0f);
        GfMatrix4d inputMatrix =
            inputMatrixDataSource->GetTypedValue(0.0f);

        computedXformDataSource = HdXformSchema::Builder()
            .SetMatrix(
                    HdRetainedTypedSampledDataSource<GfMatrix4d>::New(
                        inputMatrix * parentMatrix))
            .Build();
    } else if (inputMatrixDataSource) {
        computedXformDataSource = inputXform.GetContainer();
    } else if (parentMatrixDataSource) {
        computedXformDataSource = parentXform.GetContainer();
    } else {
        computedXformDataSource = _sceneIndex._identityXform;
    }

    HdContainerDataSource::AtomicStore(
            _computedXformDataSource, computedXformDataSource);

    return computedXformDataSource;
}

HdDataSourceBaseHandle
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::_GetModel()
{
    HdContainerDataSourceHandle const modelContainer =
        _inputDataSource
            ? HdContainerDataSource::Cast(_inputDataSource->Get(_tokens->model))
            : nullptr;
    HdContainerDataSourceHandle const overrideContainer =
        HdRetainedContainerDataSource::New(
            _tokens->drawMode, _GetDrawMode(modelContainer));
    if (!modelContainer) {
        return overrideContainer;
    }
    return HdOverlayContainerDataSource::New(overrideContainer, modelContainer);
}

HdDataSourceBaseHandle
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::_GetDrawMode(
    const HdContainerDataSourceHandle &modelContainer)
{
    HdTokenDataSource::AtomicHandle computedDrawModeDataSource =
        HdTokenDataSource::AtomicLoad(_computedDrawModeDataSource);

    if (computedDrawModeDataSource) {
        return computedDrawModeDataSource;
    }

    computedDrawModeDataSource = _GetDrawModeUncached(modelContainer);

    HdTokenDataSource::AtomicStore(
        _computedDrawModeDataSource, computedDrawModeDataSource);

    return computedDrawModeDataSource;
}

HdTokenDataSourceHandle
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::_GetDrawModeUncached(
    const HdContainerDataSourceHandle &modelContainer)
{
    if (modelContainer) {
        if (const HdTokenDataSourceHandle src =
                HdTokenDataSource::Cast(
                    modelContainer->Get(_tokens->drawMode))) {
            const TfToken drawMode = src->GetTypedValue(0.0f);
            if (!drawMode.IsEmpty() && drawMode != _tokens->inherited) {
                return src;
            }
        }
    }

    if (_primPath.GetPathElementCount() == 0) {
        return _sceneIndex._identityDrawMode;
    }

    const SdfPath parentPath = _primPath.GetParentPath();
    const auto it = _sceneIndex._prims.find(parentPath);
    if (it == _sceneIndex._prims.end()) {
        return _sceneIndex._identityDrawMode;
    }

    if (const HdTokenDataSourceHandle src =
            HdTokenDataSource::Cast(
                HdContainerDataSource::Get(
                    it->second.prim.dataSource, _GetDrawModeLocator()))) {
        return src;
    }

    return _sceneIndex._identityDrawMode;
}

HdDataSourceBaseHandle
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::_GetMaterialBinding()
{
    HdDataSourceBaseHandle result =
        HdDataSourceBase::AtomicLoad(_computedMaterialBindingDataSource);

    if (!result) {
        result = _GetMaterialBindingUncached();
        HdDataSourceBase::AtomicStore(_computedMaterialBindingDataSource,
            result);
    }

    // The cached value of the absence of a materialBinding is a non-container
    // data source.
    return HdContainerDataSource::Cast(result);
}

HdDataSourceBaseHandle
HdFlatteningSceneIndex::_PrimLevelWrappingDataSource::
_GetMaterialBindingUncached()
{
    HdContainerDataSourceHandle inputBindings =
        HdMaterialBindingSchema::GetFromParent(_inputDataSource).GetContainer();

    HdContainerDataSourceHandle parentBindings;
    if (_primPath.GetPathElementCount()) {
        SdfPath parentPath = _primPath.GetParentPath();
        const auto it = _sceneIndex._prims.find(parentPath);
        if (it != _sceneIndex._prims.end()) {
            parentBindings = HdMaterialBindingSchema::GetFromParent(
                    it->second.prim.dataSource).GetContainer();
        }
    }

    if (inputBindings) {
        if (parentBindings) {
            // Parent and local bindings might have unique fields so we must
            // overlay them. If we are concerned about overlay depth, we could
            // compare GetNames() results to decide whether the child bindings
            // completely mask the parent.
            return HdOverlayContainerDataSource::New(
                inputBindings, parentBindings);
        }

        return inputBindings;
    } else {
        if (parentBindings) {
            return parentBindings;
        } else {
            // Cache the absence of value by storing a non-container which will
            // fail the cast on return. Using retained "false" because its New
            // returns a shared instance rather than a new allocation.
            return HdRetainedTypedSampledDataSource<bool>::New(false);
        }
    }
}

PXR_NAMESPACE_CLOSE_SCOPE
