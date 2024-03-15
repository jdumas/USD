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

#ifndef EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_RILEY_PRIMVAR_SCHEMA_H
#define EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_RILEY_PRIMVAR_SCHEMA_H

/// \file

#include "hdPrman/api.h"

#include "pxr/imaging/hd/schema.h"
#include "pxr/imaging/hd/version.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

// --(BEGIN CUSTOM CODE: Declares)--
// --(END CUSTOM CODE: Declares)--

#define HD_PRMAN_RILEY_PRIMVAR_SCHEMA_TOKENS \
    (value) \
    (detailType) \
    (role) \
    (constant) \
    (uniform) \
    (vertex) \
    (varying) \
    (facevarying) \
    (reference) \

TF_DECLARE_PUBLIC_TOKENS(HdPrmanRileyPrimvarSchemaTokens, HDPRMAN_API,
    HD_PRMAN_RILEY_PRIMVAR_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------


class HdPrmanRileyPrimvarSchema : public HdSchema
{
public:
    /// \name Schema retrieval
    /// @{

    HdPrmanRileyPrimvarSchema(HdContainerDataSourceHandle container)
      : HdSchema(container) {}

    /// @}

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

    /// \name Member accessor
    /// @{

    HDPRMAN_API
    HdSampledDataSourceHandle GetValue()
#if HD_API_VERSION >= 66
                                            const;
#else
                                                 ;
#endif


    HDPRMAN_API
    HdTokenDataSourceHandle GetDetailType()
#if HD_API_VERSION >= 66
                                            const;
#else
                                                 ;
#endif


    HDPRMAN_API
    HdTokenDataSourceHandle GetRole()
#if HD_API_VERSION >= 66
                                            const;
#else
                                                 ;
#endif
 

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
    HDPRMAN_API
    static HdContainerDataSourceHandle
    BuildRetained(
        const HdSampledDataSourceHandle &value,
        const HdTokenDataSourceHandle &detailType,
        const HdTokenDataSourceHandle &role
    );

    /// \class HdPrmanRileyPrimvarSchema::Builder
    /// 
    /// Utility class for setting sparse sets of child data source fields to be
    /// filled as arguments into BuildRetained. Because all setter methods
    /// return a reference to the instance, this can be used in the "builder
    /// pattern" form.
    class Builder
    {
    public:
        HDPRMAN_API
        Builder &SetValue(
            const HdSampledDataSourceHandle &value);
        HDPRMAN_API
        Builder &SetDetailType(
            const HdTokenDataSourceHandle &detailType);
        HDPRMAN_API
        Builder &SetRole(
            const HdTokenDataSourceHandle &role);

        /// Returns a container data source containing the members set thus far.
        HDPRMAN_API
        HdContainerDataSourceHandle Build();

    private:
        HdSampledDataSourceHandle _value;
        HdTokenDataSourceHandle _detailType;
        HdTokenDataSourceHandle _role;

    };

    /// Returns token data source for use as detailType value.
    ///
    /// The following values will be stored statically and reused for future
    /// calls:
    /// - HdPrmanRileyPrimvarSchemaTokens->constant
    /// - HdPrmanRileyPrimvarSchemaTokens->uniform
    /// - HdPrmanRileyPrimvarSchemaTokens->vertex
    /// - HdPrmanRileyPrimvarSchemaTokens->varying
    /// - HdPrmanRileyPrimvarSchemaTokens->facevarying
    /// - HdPrmanRileyPrimvarSchemaTokens->reference
    HDPRMAN_API
    static HdTokenDataSourceHandle BuildDetailTypeDataSource(
        const TfToken &detailType);

    /// @}
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif