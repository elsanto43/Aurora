// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2021-2023 by Agustin Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "Uri.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Content
{
    // -=(Undocumented)=-
    class Resource
    {
    public:

        // -=(Undocumented)=-
        enum class Condition
        {
            None, Pending, Loaded, Failed, Unloaded
        };

    public:

        // -=(Undocumented)=-
        Resource(Ref<const Uri> Key, UInt Category)
            : mKey       { Key },
              mCategory  { Category },
              mMemory    { 0 },
              mCondition { Condition::None }
        {
        }

        // -=(Undocumented)=-
        virtual ~Resource() = default;

        // -=(Undocumented)=-
        Ref<const Uri> GetKey() const
        {
            return mKey;
        }

        // -=(Undocumented)=-
        UInt GetCategory() const
        {
            return mCategory;
        }

        // -=(Undocumented)=-
        void SetMemory(UInt Memory)
        {
            mMemory = Memory;
        }

        // -=(Undocumented)=-
        UInt GetMemory() const
        {
            return mMemory;
        }

        // -=(Undocumented)=-
        void SetCondition(Condition Condition)
        {
            mCondition = Condition;
        }

        // -=(Undocumented)=-
        Bool HasCreated() const
        {
            return mCondition == Condition::None;
        }

        // -=(Undocumented)=-
        Bool HasUnloaded() const
        {
            return mCondition == Condition::Unloaded;
        }

        // -=(Undocumented)=-
        Bool HasFinished() const
        {
            return HasLoaded() | HasFailed();
        }

        // -=(Undocumented)=-
        Bool HasFailed() const
        {
            return mCondition == Condition::Failed;
        }

        // -=(Undocumented)=-
        Bool HasLoaded() const
        {
            return mCondition == Condition::Loaded;
        }

    public:

        // -=(Undocumented)=-
        virtual Bool OnLoad(Ref<Subsystem::Context> Context)
        {
            return false;
        }

        // -=(Undocumented)=-
        virtual void OnUnload(Ref<Subsystem::Context> Context)
        {
            SetMemory(0);
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        const Uri  mKey;
        const UInt mCategory;
        UInt       mMemory;
        Condition  mCondition;
    };

    // -=(Undocumented)=-
    template<UInt Category>
    class AbstractResource : public Resource
    {
    public:

        // -=(Undocumented)=-
        static constexpr inline UInt RTTI_CATEGORY = Category;

    public:

        // -=(Undocumented)=-
        AbstractResource(Ref<const Uri> Key)
            : Resource(Key, RTTI_CATEGORY)
        {
        }
    };
}