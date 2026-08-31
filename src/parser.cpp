#include "parser.hpp"

#include "compat_defs.hpp"
#include "gdstructs.hpp"

#include <cstring>
#include <fmt/format.h>
#include <string>
#include <string_view>

using namespace geode::prelude;

static constexpr GDMirrorPortalData kMPData{};
static constexpr GDCameraObjectData kCOData{};
static constexpr GDRisingBlocksData kGRBData{};
static constexpr GDFallingBlocksData kGBFData{};

static Result<int> readInt(const ByteVector& data, size_t& offset)
{
    if (offset + 4 > data.size())
    {
        return Err("Unexpected end of level data while reading an integer");
    }

    uint32_t value = 0;
    for (size_t i = 0; i < 4; ++i)
    {
        value = (value << 8) | static_cast<uint32_t>(data[offset + i]);
    }

    offset += 4;
    return Ok(static_cast<int>(value));
}

static Result<uint16_t> readShort(const ByteVector& data, size_t& offset)
{
    if (offset + 2 > data.size())
    {
        return Err("Unexpected end of level data while reading a 16-bit integer");
    }

    uint16_t value = (static_cast<uint16_t>(data[offset]) << 8) | static_cast<uint16_t>(data[offset + 1]);

    offset += 2;
    return Ok(value);
}

Result<void> Level::parseBlocks(const ByteVector& data, size_t& offset)
{
    GEODE_UNWRAP_INTO(uint16_t numBlocks, readShort(data, offset));

    m_blocks.reserve(m_blocks.size() + numBlocks);
    for (uint16_t i = 0; i < numBlocks; ++i)
    {
        if (offset >= data.size())
        {
            return Err("Level file ended before all block objects were read");
        }

        uint8_t type = data[offset++];
        GEODE_UNWRAP_INTO(int xPos, readInt(data, offset));
        GEODE_UNWRAP_INTO(int yPos, readInt(data, offset));

        m_blocks.push_back({
            .xPosition = xPos,
            .yPosition = yPos,
            .objectType = static_cast<int>(type),
            .vectorIndex = i,
        });
    }
    return Ok();
}

Result<void> Level::parseBackgrounds(const ByteVector& data, size_t& offset)
{
    GEODE_UNWRAP_INTO(int numBG, readInt(data, offset));

    m_backgrounds.reserve(m_backgrounds.size() + static_cast<size_t>(numBG));
    for (int i = 0; i < numBG; i++)
    {
        GEODE_UNWRAP_INTO(int xPosition, readInt(data, offset));

        if (offset >= data.size())
        {
            return Err("Background section ended unexpectedly");
        }
        uint8_t isCustom = data[offset++];

        if (isCustom == 0)
        {
            GEODE_UNWRAP_INTO(int colorID, readInt(data, offset));
            m_backgrounds.push_back(
                {.xPosition = xPosition, .colorID = colorID, .colorName = "", .hasCustomTexture = false, .filePath = ""}
            );
        }
        else
        {
            if (offset + 2 > data.size())
            {
                return Err("Custom background data ended unexpectedly");
            }

            auto strLen = (static_cast<size_t>(data[offset]) << 8) | static_cast<size_t>(data[offset + 1]);
            offset += 2;

            if (strLen > data.size() - offset)
            {
                return Err("Custom background texture length exceeds the level data");
            }

            std::string texturePath(reinterpret_cast<const char*>(data.data() + offset), strLen);
            offset += strLen;

            m_backgrounds.push_back(
                {.xPosition = xPosition,
                 .colorID = 0,
                 .colorName = "",
                 .hasCustomTexture = true,
                 .filePath = std::move(texturePath)}
            );
        }
    }
    return Ok();
}

Result<void> Level::parseGravity(const ByteVector& data, size_t& offset)
{
    GEODE_UNWRAP_INTO(int numGrav, readInt(data, offset));
    m_gravity.reserve(m_gravity.size() + static_cast<size_t>(numGrav));
    for (int i = 0; i < numGrav; i++)
    {
        GEODE_UNWRAP_INTO(int gravVal, readInt(data, offset));
        m_gravity.push_back({gravVal});
    }
    return Ok();
}

Result<void> Level::parseRising(const ByteVector& data, size_t& offset)
{
    GEODE_UNWRAP_INTO(int numRise, readInt(data, offset));
    m_rising.reserve(m_rising.size() + static_cast<size_t>(numRise));
    for (int i = 0; i < numRise; i++)
    {
        GEODE_UNWRAP_INTO(int startX, readInt(data, offset));
        GEODE_UNWRAP_INTO(int endX, readInt(data, offset));
        m_rising.push_back({.startXPosition = startX, .endXPosition = endX});
    }
    return Ok();
}

Result<void> Level::parseFalling(const ByteVector& data, size_t& offset)
{
    GEODE_UNWRAP_INTO(int numFall, readInt(data, offset));
    m_falling.reserve(m_falling.size() + static_cast<size_t>(numFall));
    for (int i = 0; i < numFall; i++)
    {
        GEODE_UNWRAP_INTO(int startX, readInt(data, offset));
        GEODE_UNWRAP_INTO(int endX, readInt(data, offset));
        m_falling.push_back({.startXPosition = startX, .endXPosition = endX});
    }
    return Ok();
}

Result<void> Level::load(const asp::fs::path& path)
{
    constexpr size_t kMinLevelFileSize = 10;

    auto data = GEODE_UNWRAP(
        file::readBinary(path).mapErr(
            [](auto&& err)
            {
                return fmt::format("Failed to read level file: {}", err);
            }
        )
    );

    if (data.size() < kMinLevelFileSize)
    {
        return Err("Level file is too small to be valid");
    }

    size_t offset = 0;
    GEODE_UNWRAP_INTO(int formatVer, readInt(data, offset));
    static_cast<void>(formatVer);

    if (offset >= data.size())
    {
        return Err("Level file ended before the graphics flag");
    }
    offset++;

    GEODE_UNWRAP(parseBlocks(data, offset));
    GEODE_UNWRAP_INTO(m_endPos, readInt(data, offset));
    GEODE_UNWRAP(parseBackgrounds(data, offset));
    GEODE_UNWRAP(parseGravity(data, offset));
    GEODE_UNWRAP(parseRising(data, offset));
    GEODE_UNWRAP(parseFalling(data, offset));

    m_loaded = true;
    return Ok();
}

Level::Level(const asp::fs::path& path)
{
    if (load(path).isErr())
    {
        m_loaded = false;
    }
}

static Result<void> appendBlocks(const Level& level, std::string& outResult)
{
    constexpr int kOffsetX135 = 135;
    constexpr int kOffsetY15 = 15;

    for (const auto& block : level.getBlocks())
    {
        std::string_view objID;
        switch (block.objectType)
        {
            case static_cast<int>(ShapeType::Block):
                objID = "1";
                break;
            case static_cast<int>(ShapeType::Spike):
                objID = "8";
                break;
            case static_cast<int>(ShapeType::Pit):
                objID = "9";
                break;
            default:
                return Err("Unsupported shape type in the Impossible Game level: {}", block.objectType);
        }

        if (block.objectType != static_cast<int>(ShapeType::Pit))
        {
            fmt::format_to(
                std::back_inserter(outResult),
                "1,{},2,{},3,{},21,0,24,0;",
                objID,
                block.xPosition - kOffsetX135,
                block.yPosition + kOffsetY15
            );
        }
        else
        {
            int delta = block.yPosition - block.xPosition;
            int iterations = (delta + 15) / 30 + 1;
            int currentX = block.xPosition - kOffsetX135;

            for (int j = 0; j < iterations; j++)
            {
                fmt::format_to(std::back_inserter(outResult), "1,9,2,{},3,0,21,0,24,0;", currentX);
                currentX += 30;
            }
        }
    }
    return Ok();
}

static Result<void> appendBackgrounds(const Level& level, std::string& outResult)
{
    constexpr int kOffsetX165 = 165;
    constexpr int kBlackColorID = 5;

    struct RGB
    {
        std::string_view r, g, b;
    };

    for (const auto& background : level.getBackgrounds())
    {
        RGB color{};
        switch (background.colorID)
        {
            case 0:
                color = {.r = "63", .g = "184", .b = "199"};
                break;
            case 1:
                color = {.r = "236", .g = "216", .b = "50"};
                break;
            case 2:
                color = {.r = "83", .g = "255", .b = "83"};
                break;
            case 3:
                color = {.r = "178", .g = "38", .b = "227"};
                break;
            case 4:
                color = {.r = "241", .g = "19", .b = "242"};
                break;
            case kBlackColorID:
                color = {.r = "0", .g = "0", .b = "0"};
                break;
            default:
                return Err("Unsupported background color ID in the Impossible Game level: {}", background.colorID);
        }

        int xPosition = background.xPosition + kOffsetX165;
        fmt::format_to(
            std::back_inserter(outResult),
            "1,899,2,{},3,3000,7,{},8,{},9,{},10,0.25,23,1000,155,1,35,1;"
            "1,899,2,{},3,3030,7,{},8,{},9,{},10,0.25,23,1001,155,1,35,1;"
            "1,899,2,{},3,3060,7,{},8,{},9,{},10,0.25,23,1009,155,1,35,1;",
            xPosition,
            color.r,
            color.g,
            color.b,
            xPosition,
            color.r,
            color.g,
            color.b,
            xPosition,
            color.r,
            color.g,
            color.b
        );
    }
    return Ok();
}

static void appendPortalsAndMovement(const Level& level, std::string& outResult)
{
    constexpr int kOffsetX135 = 135;
    constexpr int kOffsetX165 = 165;
    constexpr int kOffsetX465 = 465;
    constexpr int kOffsetX495 = 495;
    constexpr int kOffsetY15 = 15;

    bool currentlyInverted = false;
    for (const auto& grav : level.getGravity())
    {
        std::string_view objID = currentlyInverted ? "46" : "45";
        std::string_view rotationDegrees = currentlyInverted ? "0" : "180";
        currentlyInverted = !currentlyInverted;

        int xPosition = grav.xPosition + kOffsetX165;
        fmt::format_to(
            std::back_inserter(outResult),
            "{}{}{}{}{};{}{}{}{};",
            kMPData.stringPrefix,
            objID,
            kMPData.stringMiddle,
            xPosition,
            kMPData.stringRemainder,
            kCOData.stringPrefix,
            xPosition,
            kCOData.stringMiddle,
            rotationDegrees
        );
    }

    for (const auto& rise : level.getRising())
    {
        int startXPos = rise.startXPosition - kOffsetX465;
        int endXPos = (rise.startXPosition == rise.endXPosition) ? (level.getEndPos() - kOffsetX495)
                                                                 : (rise.endXPosition - kOffsetX495);

        fmt::format_to(
            std::back_inserter(outResult),
            "{}23{}{}{};{}1915{}{}{};",
            kGRBData.stringPrefix,
            kGRBData.stringMiddle,
            startXPos,
            kGRBData.stringRemainder,
            kGRBData.stringPrefix,
            kGRBData.stringMiddle,
            endXPos,
            kGRBData.stringRemainder
        );
    }

    for (const auto& fall : level.getFalling())
    {
        int startXPos = fall.startXPosition - kOffsetX135;
        int endXPos = (fall.startXPosition == fall.endXPosition) ? (level.getEndPos() - kOffsetY15)
                                                                 : (fall.endXPosition - kOffsetY15);

        fmt::format_to(
            std::back_inserter(outResult),
            "{}23{}{}{};{}1915{}{}{};",
            kGBFData.stringPrefix,
            kGBFData.stringMiddle,
            startXPos,
            kGBFData.stringRemainder,
            kGBFData.stringPrefix,
            kGBFData.stringMiddle,
            endXPos,
            kGBFData.stringRemainder
        );
    }
}

Result<std::string> Level::buildObjectString(const Level& inLevel)
{
    if (!inLevel.getLoadedSuccessfully())
    {
        return Err("Failed to build object string: level was not properly loaded");
    }

    std::string result;
    size_t estimatedSize = kLevelStringBase.size() + (inLevel.getBlockCount() * 32) +
        (inLevel.getBackgroundCount() * 220) + (inLevel.getGravityCount() * 80) + (inLevel.getRisingCount() * 60) +
        (inLevel.getFallingCount() * 60);

    result.reserve(estimatedSize);
    result.append(kLevelStringBase);

    GEODE_UNWRAP(appendBlocks(inLevel, result));
    GEODE_UNWRAP(appendBackgrounds(inLevel, result));

    appendPortalsAndMovement(inLevel, result);

    return Ok(std::move(result));
}