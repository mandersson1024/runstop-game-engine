#pragma once


namespace Engine
{

    struct GraphicsSettings
    {
        int antiAliasingSampleCount = 4;
        int antiAliasingQuality = 0;
    };

    struct AntiAliasingCapability
    {
        uint32_t sampleCount;
        uint32_t qualityLevels;
    };

}

