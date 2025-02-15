// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2021-2023 by Agustin Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "Service.hpp"

#ifdef    AE_CONTENT_LOCATOR_DEFAULT
    #include "Locator/SystemLocator.hpp"
#endif // AE_CONTENT_LOCATOR_DEFAULT
#ifdef    AE_CONTENT_LOADER_MP3
    #include "Content/Sound/MP3/Loader.hpp"
#endif // AE_CONTENT_LOADER_MP3
#ifdef    AE_CONTENT_LOADER_WAV
    #include "Content/Sound/WAV/Loader.hpp"
#endif // AE_CONTENT_LOADER_WAV
#ifdef    AE_CONTENT_LOADER_STB
    #include "Content/Texture/STB/Loader.hpp"
#endif // AE_CONTENT_LOADER_STB
#ifdef    AE_CONTENT_LOADER_EFFECT
    #include "Graphic/Service.hpp"
    #include "Content/Pipeline/Loader.hpp"
#endif // AE_CONTENT_LOADER_EFFECT

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Content
{
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Service::Service(Ref<Context> Context)
        : Subsystem { Context }
    {
        RegisterDefaultResources();
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::OnTick()
    {
        // TODO: Async
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::AddLoader(Ref<const SPtr<Loader>> Loader)
    {
        for (const CStr Extension : Loader->GetExtensions())
        {
            mLoaders.try_emplace(Extension, Loader);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::RemoveLoader(CStr Extension)
    {
        mLoaders.erase(Extension);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::AddLocator(CStr Schema, Ref<const SPtr<Locator>> Locator)
    {
        mLocators.try_emplace(Schema, Locator);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::RemoveLocator(CStr Schema)
    {
        mLocators.erase(Schema);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::SetMemoryBudget(UInt Category, UInt Budget)
    {
        GetFactory(Category)->SetMemoryBudget(Budget);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    UInt Service::GetMemoryBudget(UInt Category)
    {
        return GetFactory(Category)->GetMemoryBudget();
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    UInt Service::GetMemoryUsage(UInt Category)
    {
        return GetFactory(Category)->GetMemoryUsage();
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Chunk Service::Find(Ref<const Uri> Key)
    {
        if (const auto It = mLocators.find(Key.GetSchema()); It != mLocators.end())
        {
            return It->second->Open(Key.GetPath());
        }

        for (const auto [Schema, Locator] : mLocators)
        {
            if (Chunk Data = Locator->Open(Key.GetPath()); Data.HasData())
            {
                return eastl::move(Data);
            }
        }
        return Chunk();
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::Register(Ref<const SPtr<Resource>> Asset, Bool Async)
    {
        if (Asset)
        {
            const Bool Inserted = GetFactory(Asset->GetCategory())->Insert(Asset);

            if (Inserted)
            {
                if (Async)
                {
                    // TODO: Async
                }
                else
                {
                    Process(Asset, true);
                }
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::Load(Ref<const SPtr<Resource>> Asset, Bool Async)
    {
        if (Asset && Asset->HasCreated())
        {
            Asset->SetCondition(Resource::Condition::Pending);

            if (Async)
            {
                // TODO: Async
            }
            else
            {
                Process(Asset, Parse(Asset));
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::Reload(Ref<const SPtr<Resource>> Asset, Bool Async)
    {
        if (Asset && Asset->HasFinished())
        {
            if (Async)
            {
                // TODO: Async
            }
            else
            {
                Process(Asset, Parse(Asset));
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::Unload(Ref<const SPtr<Resource>> Asset)
    {
        if (Asset && GetFactory(Asset->GetCategory())->Unload(Asset->GetKey()))
        {
            Process(Asset, false);  // TODO: Async
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::Prune(UInt Category, Bool Force)
    {
        const Vector<SPtr<Resource>> Assets = GetFactory(Category)->Prune(Force);

        for (Ref<const SPtr<Resource>> Asset : Assets)
        {
            Process(Asset, false);  // TODO: Async
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::RegisterDefaultResources()
    {
#ifdef    AE_CONTENT_LOCATOR_DEFAULT
        AddLocator("file://", NewPtr<Content::SystemLocator>());
#endif // AE_CONTENT_LOCATOR_DEFAULT

#ifdef    AE_CONTENT_LOADER_MP3
        AddLoader(NewPtr<Content::MP3Loader>());
#endif // AE_CONTENT_LOADER_MP3

#ifdef    AE_CONTENT_LOADER_WAV
        AddLoader(NewPtr<Content::WAVLoader>());
#endif // AE_CONTENT_LOADER_WAV

#ifdef    AE_CONTENT_LOADER_STB
        AddLoader(NewPtr<Content::STBLoader>());
#endif // AE_CONTENT_LOADER_STB

#ifdef    AE_CONTENT_LOADER_EFFECT
        const auto GraphicDriverCapabilities = GetSubsystem<Graphic::Service>()->GetCapabilities();
        AddLoader(NewPtr<Content::PipelineLoader>(
            GraphicDriverCapabilities.Backend, GraphicDriverCapabilities.Language));
#endif // AE_CONTENT_LOADER_EFFECT
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Ref<const SPtr<Factory>> Service::GetFactory(UInt Category)
    {
        if (auto Iterator = mFactories.find(Category); Iterator != mFactories.end())
        {
            return Iterator->second;
        }
        return mFactories.try_emplace(Category, NewPtr<Factory>()).first->second;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Bool Service::Parse(Ref<const SPtr<Resource>> Asset)
    {
        Ref<const Uri> Key = Asset->GetKey();

        if (const auto Iterator = mLoaders.find(Key.GetExtension()); Iterator != mLoaders.end())
        {
            Ref<const SPtr<Loader>> Loader = Iterator->second;

            if (Chunk File = Find(Key); File.HasData())
            {
                if (Loader->Load(File, Asset))
                {
                    return true;
                }
                else
                {
                    // TODO: Log(ERROR_INVALID_CONTENT)
                }
            }
            else
            {
                // TODO: Log(ERROR_FILE_NOT_FOUND)
            }
        }
        else
        {
            // TODO: Log(ERROR_UNKNOWN_FILE_FORMAT)
        }
        return false;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Service::Process(Ref<const SPtr<Resource>> Asset, Bool Loaded)
    {
        Ref<const SPtr<Factory>> Factory = GetFactory(Asset->GetCategory());
        Ref<Context>             Context = GetContext();

        if (Loaded)
        {
            const UInt64 LastMemoryUsage = Asset->GetMemory();

            // if the asset is being reloaded, then unload previous data from it
            // and load the new data
            if (Asset->HasLoaded())
            {
                Asset->OnUnload(Context);
            }

            if (Asset->OnLoad(Context))
            {
                Asset->SetCondition(Resource::Condition::Loaded);
            }
            else
            {
                Asset->SetCondition(Resource::Condition::Failed);
            }

            Factory->SetMemoryUsage(Factory->GetMemoryUsage() + Asset->GetMemory() - LastMemoryUsage);
        }
        else
        {
            Factory->SetMemoryUsage(Factory->GetMemoryUsage() - Asset->GetMemory());

            Asset->OnUnload(Context);
            Asset->SetCondition(Resource::Condition::Failed);
        }
    }
}