//
// Copyright 2023 Pixar
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
////////////////////////////////////////////////////////////////////////

/* ************************************************************************** */
/* **                                                                      ** */
/* ** This file is generated by a script.                                  ** */
/* **                                                                      ** */
/* ** Do not edit it directly (unless it is within a CUSTOM CODE section)! ** */
/* ** Edit hdSchemaDefs.py instead to make changes.                        ** */
/* **                                                                      ** */
/* ************************************************************************** */

#include "hdPrman/rileyRenderViewSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdPrmanRileyRenderViewSchemaTokens,
    HD_PRMAN_RILEY_RENDER_VIEW_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdPathDataSourceHandle
HdPrmanRileyRenderViewSchema::GetRenderTarget()
#if HD_API_VERSION >= 66
                                            const
#else
                                                 
#endif
{
    return _GetTypedDataSource<HdPathDataSource>(
        HdPrmanRileyRenderViewSchemaTokens->renderTarget);
}

HdPathDataSourceHandle
HdPrmanRileyRenderViewSchema::GetCamera()
#if HD_API_VERSION >= 66
                                            const
#else
                                                 
#endif
{
    return _GetTypedDataSource<HdPathDataSource>(
        HdPrmanRileyRenderViewSchemaTokens->camera);
}

HdPathDataSourceHandle
HdPrmanRileyRenderViewSchema::GetIntegrator()
#if HD_API_VERSION >= 66
                                            const
#else
                                                 
#endif
{
    return _GetTypedDataSource<HdPathDataSource>(
        HdPrmanRileyRenderViewSchemaTokens->integrator);
}

HdPathArrayDataSourceHandle
HdPrmanRileyRenderViewSchema::GetDisplayFilters()
#if HD_API_VERSION >= 66
                                            const
#else
                                                 
#endif
{
    return _GetTypedDataSource<HdPathArrayDataSource>(
        HdPrmanRileyRenderViewSchemaTokens->displayFilters);
}

HdPathArrayDataSourceHandle
HdPrmanRileyRenderViewSchema::GetSampleFilters()
#if HD_API_VERSION >= 66
                                            const
#else
                                                 
#endif
{
    return _GetTypedDataSource<HdPathArrayDataSource>(
        HdPrmanRileyRenderViewSchemaTokens->sampleFilters);
}

HdPrmanRileyParamListSchema
HdPrmanRileyRenderViewSchema::GetParams()
#if HD_API_VERSION >= 66
                                            const
#else
                                                 
#endif
{
    return HdPrmanRileyParamListSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdPrmanRileyRenderViewSchemaTokens->params));
}

/*static*/
HdContainerDataSourceHandle
HdPrmanRileyRenderViewSchema::BuildRetained(
        const HdPathDataSourceHandle &renderTarget,
        const HdPathDataSourceHandle &camera,
        const HdPathDataSourceHandle &integrator,
        const HdPathArrayDataSourceHandle &displayFilters,
        const HdPathArrayDataSourceHandle &sampleFilters,
        const HdContainerDataSourceHandle &params
)
{
    TfToken _names[6];
    HdDataSourceBaseHandle _values[6];

    size_t _count = 0;

    if (renderTarget) {
        _names[_count] = HdPrmanRileyRenderViewSchemaTokens->renderTarget;
        _values[_count++] = renderTarget;
    }

    if (camera) {
        _names[_count] = HdPrmanRileyRenderViewSchemaTokens->camera;
        _values[_count++] = camera;
    }

    if (integrator) {
        _names[_count] = HdPrmanRileyRenderViewSchemaTokens->integrator;
        _values[_count++] = integrator;
    }

    if (displayFilters) {
        _names[_count] = HdPrmanRileyRenderViewSchemaTokens->displayFilters;
        _values[_count++] = displayFilters;
    }

    if (sampleFilters) {
        _names[_count] = HdPrmanRileyRenderViewSchemaTokens->sampleFilters;
        _values[_count++] = sampleFilters;
    }

    if (params) {
        _names[_count] = HdPrmanRileyRenderViewSchemaTokens->params;
        _values[_count++] = params;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdPrmanRileyRenderViewSchema::Builder &
HdPrmanRileyRenderViewSchema::Builder::SetRenderTarget(
    const HdPathDataSourceHandle &renderTarget)
{
    _renderTarget = renderTarget;
    return *this;
}

HdPrmanRileyRenderViewSchema::Builder &
HdPrmanRileyRenderViewSchema::Builder::SetCamera(
    const HdPathDataSourceHandle &camera)
{
    _camera = camera;
    return *this;
}

HdPrmanRileyRenderViewSchema::Builder &
HdPrmanRileyRenderViewSchema::Builder::SetIntegrator(
    const HdPathDataSourceHandle &integrator)
{
    _integrator = integrator;
    return *this;
}

HdPrmanRileyRenderViewSchema::Builder &
HdPrmanRileyRenderViewSchema::Builder::SetDisplayFilters(
    const HdPathArrayDataSourceHandle &displayFilters)
{
    _displayFilters = displayFilters;
    return *this;
}

HdPrmanRileyRenderViewSchema::Builder &
HdPrmanRileyRenderViewSchema::Builder::SetSampleFilters(
    const HdPathArrayDataSourceHandle &sampleFilters)
{
    _sampleFilters = sampleFilters;
    return *this;
}

HdPrmanRileyRenderViewSchema::Builder &
HdPrmanRileyRenderViewSchema::Builder::SetParams(
    const HdContainerDataSourceHandle &params)
{
    _params = params;
    return *this;
}

HdContainerDataSourceHandle
HdPrmanRileyRenderViewSchema::Builder::Build()
{
    return HdPrmanRileyRenderViewSchema::BuildRetained(
        _renderTarget,
        _camera,
        _integrator,
        _displayFilters,
        _sampleFilters,
        _params
    );
}

/*static*/
HdPrmanRileyRenderViewSchema
HdPrmanRileyRenderViewSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return HdPrmanRileyRenderViewSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                HdPrmanRileyRenderViewSchemaTokens->rileyRenderView))
        : nullptr);
}

/*static*/
const TfToken &
HdPrmanRileyRenderViewSchema::GetSchemaToken()
{
    return HdPrmanRileyRenderViewSchemaTokens->rileyRenderView;
}

/*static*/
const HdDataSourceLocator &
HdPrmanRileyRenderViewSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyRenderViewSchema::GetRenderTargetLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyRenderViewSchemaTokens->renderTarget);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyRenderViewSchema::GetCameraLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyRenderViewSchemaTokens->camera);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyRenderViewSchema::GetIntegratorLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyRenderViewSchemaTokens->integrator);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyRenderViewSchema::GetDisplayFiltersLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyRenderViewSchemaTokens->displayFilters);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyRenderViewSchema::GetSampleFiltersLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyRenderViewSchemaTokens->sampleFilters);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyRenderViewSchema::GetParamsLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyRenderViewSchemaTokens->params);
    return locator;
}

/*static*/
HdTokenDataSourceHandle
HdPrmanRileyRenderViewSchema::BuildFilterModeDataSource(
    const TfToken &filterMode)
{

    if (filterMode == HdPrmanRileyRenderViewSchemaTokens->importance) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(filterMode);
        return ds;
    }
    if (filterMode == HdPrmanRileyRenderViewSchemaTokens->weighted) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(filterMode);
        return ds;
    }
    // fallback for unknown token
    return HdRetainedTypedSampledDataSource<TfToken>::New(filterMode);
} 

PXR_NAMESPACE_CLOSE_SCOPE