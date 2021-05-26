#pragma once
#include <mutex>
#include <future>
#include "AssetLoadingState.h"

namespace Engine
{

    class Asset
    {
    public:
        virtual bool IsDirty() = 0;
        virtual void LoadSync() = 0;

        void Load();
        void ReloadIfDirty();
        void Await();
        AssetLoadingState GetLoadingState() const { return loadingState_; }
        void SetLoadingState(AssetLoadingState state) { loadingState_ = state; }

    private:
        std::mutex mutex_;
        std::future<void> future_;
        AssetLoadingState loadingState_ = AssetLoadingState::NotLoaded;
    };

}


