#pragma once

#include <string>
#include <string_view>

struct BlockObject
{
    int xPosition = 0;
    int yPosition = 0; // serves as endX position for pit objects
    int objectType = 0;
    int vectorIndex = 0;
};

struct BackgroundChange
{
    int xPosition = 0;
    int colorID = 0;
    std::string colorName;
    bool hasCustomTexture = false;
    std::string filePath;
};

struct GravityChange
{
    int xPosition = 0;
};

struct BlocksRiseTrigger
{
    int startXPosition = 0;
    int endXPosition = 0;
};

struct BlocksFallTrigger
{
    int startXPosition = 0;
    int endXPosition = 0;
};

struct GDObjectData
{
    std::string_view objectID = "1"; // default block; altered for spikes/pits
    std::string_view xPosition = "0";
    std::string_view yPosition = "0";
    std::string_view colorChannelID = "0";
    std::string_view zLayer = "0";
};

struct GDColorTriggerData
{
    std::string_view objectID = "899";
    std::string_view xPosition = "0";
    std::string_view yPosition = "0";
    std::string_view redValue = "0";
    std::string_view greenValue = "0";
    std::string_view blueValue = "0";
    std::string_view fadeDuration = "0.25";
    std::string_view targetChannel = "1000";
    std::string_view stringRemainder = ",155,1,35,1";
};

struct GDSplitScreenObjectData
{
    std::string_view stringPrefix = "1,2924,2,";
    std::string_view xPosition = "0";
    std::string_view stringMiddle = ",3,2970,155,1,36,1,175,1,176,1,191,1,179,1,189,";
    std::string_view yFlip = "0";
    std::string_view stringRemainder = ",181,1,182,1,190,1,10,0.2,85,2;";
};

struct GDRisingBlocksData
{
    std::string_view stringPrefix = "1,";
    std::string_view triggerID = "23"; // 23: start rising, 1915: end rising
    std::string_view stringMiddle = ",2,";
    std::string_view xPosition = "15";
    std::string_view stringRemainder = ",3,2940,155,1,36,1,217,1";
};

struct GDFallingBlocksData
{
    std::string_view stringPrefix = "1,";
    std::string_view triggerID = "23"; // 23: Start falling, 1915: end falling
    std::string_view stringMiddle = ",2,";
    std::string_view xPosition = "15";
    std::string_view stringRemainder = ",3,2910,155,1,36,1,217,2";
};
