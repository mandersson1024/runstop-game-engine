#include "pch.h"
#include "Asset.h"
#include "..\Logger.h"

namespace Engine 
{

    void Asset::ReloadIfDirty()
    {
        if (IsDirty())
        {
            bool hasBeenLoaded = false;

            {
                const std::lock_guard<std::mutex> lock(mutex_); // Wait if currently loading loading

                if (loadingState_ == AssetLoadingState::Loaded)
                {
                    hasBeenLoaded = true;
                    loadingState_ = AssetLoadingState::NotLoaded;
                }
            }

            if (hasBeenLoaded)
            {
                Load();
            }
        }
    }

    void Asset::Load()
    {
        auto loadFn = [=]()
        {
            if (loadingState_ == AssetLoadingState::NotLoaded)
            {
                loadingState_ = AssetLoadingState::Loading;
                LoadSync();
                loadingState_ = AssetLoadingState::Loaded;
            }
        };

#ifdef ENGINE_SINGLE_THREAD
        loadFn();
#else
        std::packaged_task<void()> task([=] { loadFn(); });
        future_ = task.get_future();
        std::thread thread(std::move(task));

        {
            // Log the thread id
            std::stringstream ss;
            ss << thread.get_id();
            ENGINE_LOG("Asset::Load(), thread id = {}", ss.str());
        }

        thread.detach();
#endif
    }

    void Asset::Await()
    {
#ifndef ENGINE_SINGLE_THREAD
        if (future_.valid())
        {
            ENGINE_LOG("Waiting for asset load thread to finish");
            future_.wait();
        }
#endif
    }

}

