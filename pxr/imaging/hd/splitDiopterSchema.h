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

#ifndef PXR_IMAGING_HD_SPLIT_DIOPTER_SCHEMA_H
#define PXR_IMAGING_HD_SPLIT_DIOPTER_SCHEMA_H

/// \file

#include "pxr/imaging/hd/api.h"

#include "pxr/imaging/hd/schema.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

// --(BEGIN CUSTOM CODE: Declares)--
// --(END CUSTOM CODE: Declares)--

#define HD_SPLIT_DIOPTER_SCHEMA_TOKENS \
    (splitDiopter) \
    (count) \
    (angle) \
    (offset1) \
    (width1) \
    (focusDistance1) \
    (offset2) \
    (width2) \
    (focusDistance2) \

TF_DECLARE_PUBLIC_TOKENS(HdSplitDiopterSchemaTokens, HD_API,
    HD_SPLIT_DIOPTER_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------


class HdSplitDiopterSchema : public HdSchema
{
public:
    /// \name Schema retrieval
    /// @{

    HdSplitDiopterSchema(HdContainerDataSourceHandle container)
      : HdSchema(container) {}

    /// Retrieves a container data source with the schema's default name token
    /// "splitDiopter" from the parent container and constructs a
    /// HdSplitDiopterSchema instance.
    /// Because the requested container data source may not exist, the result
    /// should be checked with IsDefined() or a bool comparison before use.
    HD_API
    static HdSplitDiopterSchema GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer);

    /// @}

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

    /// \name Member accessor
    /// @{

    HD_API
    HdIntDataSourceHandle GetCount() const;

    HD_API
    HdFloatDataSourceHandle GetAngle() const;

    HD_API
    HdFloatDataSourceHandle GetOffset1() const;

    HD_API
    HdFloatDataSourceHandle GetWidth1() const;

    HD_API
    HdFloatDataSourceHandle GetFocusDistance1() const;

    HD_API
    HdFloatDataSourceHandle GetOffset2() const;

    HD_API
    HdFloatDataSourceHandle GetWidth2() const;

    HD_API
    HdFloatDataSourceHandle GetFocusDistance2() const; 

    /// @}

    /// \name Schema location
    /// @{

    /// Returns a token where the container representing this schema is found in
    /// a container by default.
    HD_API
    static const TfToken &GetSchemaToken();

    /// Returns an HdDataSourceLocator (relative to the prim-level data source)
    /// where the container representing this schema is found by default.
    HD_API
    static const HdDataSourceLocator &GetDefaultLocator();

    /// @} 

    /// \name Schema construction
    /// @{

    /// \deprecated Use Builder instead.
    ///
    /// Builds a container data source which includes the provided child data
    /// sources. Parameters with nullptr values are excluded. This is a
    /// low-level interface. For cases in which it's desired to define
    /// the container with a sparse set of child fields, the Builder class
    /// is often more convenient and readable.
    HD_API
    static HdContainerDataSourceHandle
    BuildRetained(
        const HdIntDataSourceHandle &count,
        const HdFloatDataSourceHandle &angle,
        const HdFloatDataSourceHandle &offset1,
        const HdFloatDataSourceHandle &width1,
        const HdFloatDataSourceHandle &focusDistance1,
        const HdFloatDataSourceHandle &offset2,
        const HdFloatDataSourceHandle &width2,
        const HdFloatDataSourceHandle &focusDistance2
    );

    /// \class HdSplitDiopterSchema::Builder
    /// 
    /// Utility class for setting sparse sets of child data source fields to be
    /// filled as arguments into BuildRetained. Because all setter methods
    /// return a reference to the instance, this can be used in the "builder
    /// pattern" form.
    class Builder
    {
    public:
        HD_API
        Builder &SetCount(
            const HdIntDataSourceHandle &count);
        HD_API
        Builder &SetAngle(
            const HdFloatDataSourceHandle &angle);
        HD_API
        Builder &SetOffset1(
            const HdFloatDataSourceHandle &offset1);
        HD_API
        Builder &SetWidth1(
            const HdFloatDataSourceHandle &width1);
        HD_API
        Builder &SetFocusDistance1(
            const HdFloatDataSourceHandle &focusDistance1);
        HD_API
        Builder &SetOffset2(
            const HdFloatDataSourceHandle &offset2);
        HD_API
        Builder &SetWidth2(
            const HdFloatDataSourceHandle &width2);
        HD_API
        Builder &SetFocusDistance2(
            const HdFloatDataSourceHandle &focusDistance2);

        /// Returns a container data source containing the members set thus far.
        HD_API
        HdContainerDataSourceHandle Build();

    private:
        HdIntDataSourceHandle _count;
        HdFloatDataSourceHandle _angle;
        HdFloatDataSourceHandle _offset1;
        HdFloatDataSourceHandle _width1;
        HdFloatDataSourceHandle _focusDistance1;
        HdFloatDataSourceHandle _offset2;
        HdFloatDataSourceHandle _width2;
        HdFloatDataSourceHandle _focusDistance2;

    };

    /// @}
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif