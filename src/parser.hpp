#pragma once

#include "gdstructs.hpp"

#include <Geode/Result.hpp>
#include <Geode/utils/general.hpp>
#include <asp/fs/fs.hpp>
#include <string>
#include <vector>

constexpr int kDefaultEndPos = 3015;

class Level
{
private:
    Level() = default;
    geode::Result<void> load(const asp::fs::path& path);

    std::vector<BlockObject> m_blocks;
    std::vector<BackgroundChange> m_backgrounds;
    std::vector<GravityChange> m_gravity;
    std::vector<BlocksRiseTrigger> m_rising;
    std::vector<BlocksFallTrigger> m_falling;
    int m_endPos = kDefaultEndPos;
    bool m_loaded = false;

    geode::Result<void> parseGravity(const geode::ByteVector& data, size_t& offset);
    geode::Result<void> parseRising(const geode::ByteVector& data, size_t& offset);
    geode::Result<void> parseFalling(const geode::ByteVector& data, size_t& offset);
    geode::Result<void> parseBlocks(const geode::ByteVector& data, size_t& offset);
    geode::Result<void> parseBackgrounds(const geode::ByteVector& data, size_t& offset);

public:
    explicit Level(const asp::fs::path& path);

    static geode::Result<Level> create(const asp::fs::path& path)
    {
        Level level;
        GEODE_UNWRAP(level.load(path));
        return geode::Ok(std::move(level));
    }

    [[nodiscard]] const std::vector<BlockObject>& getBlocks() const
    {
        return m_blocks;
    }
    [[nodiscard]] const std::vector<BackgroundChange>& getBackgrounds() const
    {
        return m_backgrounds;
    }
    [[nodiscard]] const std::vector<GravityChange>& getGravity() const
    {
        return m_gravity;
    }
    [[nodiscard]] const std::vector<BlocksRiseTrigger>& getRising() const
    {
        return m_rising;
    }
    [[nodiscard]] const std::vector<BlocksFallTrigger>& getFalling() const
    {
        return m_falling;
    }

    [[nodiscard]] size_t getBlockCount() const
    {
        return m_blocks.size();
    }
    [[nodiscard]] size_t getBackgroundCount() const
    {
        return m_backgrounds.size();
    }
    [[nodiscard]] size_t getGravityCount() const
    {
        return m_gravity.size();
    }
    [[nodiscard]] size_t getRisingCount() const
    {
        return m_rising.size();
    }
    [[nodiscard]] size_t getFallingCount() const
    {
        return m_falling.size();
    }

    [[nodiscard]] int getEndPos() const
    {
        return m_endPos;
    }
    [[nodiscard]] bool getLoadedSuccessfully() const
    {
        return m_loaded;
    }

    static geode::Result<std::string> buildObjectString(const Level& inLevel);
};