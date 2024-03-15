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

#include "pxr/imaging/hd/instancedBySchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdInstancedBySchemaTokens,
    HD_INSTANCED_BY_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdPathArrayDataSourceHandle
HdInstancedBySchema::GetPaths() const
{
    return _GetTypedDataSource<HdPathArrayDataSource>(
        HdInstancedBySchemaTokens->paths);
}

HdPathArrayDataSourceHandle
HdInstancedBySchema::GetPrototypeRoots() const
{
    return _GetTypedDataSource<HdPathArrayDataSource>(
        HdInstancedBySchemaTokens->prototypeRoots);
}

/*static*/
HdContainerDataSourceHandle
HdInstancedBySchema::BuildRetained(
        const HdPathArrayDataSourceHandle &paths,
        const HdPathArrayDataSourceHandle &prototypeRoots
)
{
    TfToken _names[2];
    HdDataSourceBaseHandle _values[2];

    size_t _count = 0;

    if (paths) {
        _names[_count] = HdInstancedBySchemaTokens->paths;
        _values[_count++] = paths;
    }

    if (prototypeRoots) {
        _names[_count] = HdInstancedBySchemaTokens->prototypeRoots;
        _values[_count++] = prototypeRoots;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdInstancedBySchema::Builder &
HdInstancedBySchema::Builder::SetPaths(
    const HdPathArrayDataSourceHandle &paths)
{
    _paths = paths;
    return *this;
}

HdInstancedBySchema::Builder &
HdInstancedBySchema::Builder::SetPrototypeRoots(
    const HdPathArrayDataSourceHandle &prototypeRoots)
{
    _prototypeRoots = prototypeRoots;
    return *this;
}

HdContainerDataSourceHandle
HdInstancedBySchema::Builder::Build()
{
    return HdInstancedBySchema::BuildRetained(
        _paths,
        _prototypeRoots
    );
}

/*static*/
HdInstancedBySchema
HdInstancedBySchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return HdInstancedBySchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                HdInstancedBySchemaTokens->instancedBy))
        : nullptr);
}

/*static*/
const TfToken &
HdInstancedBySchema::GetSchemaToken()
{
    return HdInstancedBySchemaTokens->instancedBy;
}

/*static*/
const HdDataSourceLocator &
HdInstancedBySchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
} 

PXR_NAMESPACE_CLOSE_SCOPE