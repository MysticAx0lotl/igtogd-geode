#include <Geode/Loader.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/IDManager.hpp>
#include <Geode/modify/LevelListLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <Geode/utils/base64.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <cmath>
#include "gdstructs.hpp"
#include "compat_defs.hpp"
#include "libImpossibleLevel.hpp"

using namespace geode::prelude;

std::string buildObjectString(Level inLevel)
{
    gdObj tempGD;
    BlockObject* tempIG;
    bool isPit = false;
    std::string result = level_string_base;

    for(int i = 0; i < inLevel.getBlockCount(); i++)
    {
        tempIG = inLevel.getBlockAtIndex(i);
        
        switch(tempIG->objType)
        {
            case 0:
                tempGD.p1_id = "1";
                break;
            case 1:
                tempGD.p1_id = "8";
                break;
            case 2:
                tempGD.p1_id = "9";
                isPit = true;
                break;
        }

        if(!isPit)
        {
            tempGD.p2_x = std::to_string(tempIG->xPos - 135);
            tempGD.p3_y = std::to_string(tempIG->yPos + 15);
            tempGD.p21_colorID = "0";
            tempGD.p24_zLayer = "0";
            result += "1,";
            result += tempGD.p1_id;
            result += ",2,";
            result += tempGD.p2_x;
            result += ",3,";
            result += tempGD.p3_y;
            result += ",21,";
            result += tempGD.p21_colorID;
            result += ",24,";
            result += tempGD.p24_zLayer;
            result += ";";
        }
        else
        {
            int iterations = round((tempIG->yPos - tempIG->xPos)/30) + 1;
            int currentX = (tempIG->xPos - 135);
            for(int j = 0; j < iterations; j++)
            {
                result += "1,";
                result += "9";
                result += ",2,";
                result += std::to_string(currentX);
                result += ",3,";
                result += "0";
                result += ",21,";
                result += tempGD.p21_colorID;
                result += ",24,";
                result += tempGD.p24_zLayer;
                result += ";";

                currentX += 30;
            }
        }
        isPit = false;
    }

    BackgroundChange* tempBC;
    gdColorTrigger tempCT;

    for(int i = 0; i < inLevel.getBackgroundCount(); i++)
    {
        tempBC = inLevel.getBackgroundAtIndex(i);

        switch(tempBC->colorID)
        {
            case 0:
                //blue
                tempCT.p7_red = "63";
                tempCT.p8_green = "184";
                tempCT.p9_blue = "199";
                break;
            case 1:
                //yellow
                tempCT.p7_red = "236";
                tempCT.p8_green = "216";
                tempCT.p9_blue = "50";
                break;
            case 3:
                //violet
                tempCT.p7_red = "178";
                tempCT.p8_green = "38";
                tempCT.p9_blue = "227";
                break;
            case 4:
                //pink
                tempCT.p7_red = "241";
                tempCT.p8_green = "19";
                tempCT.p9_blue = "242";
                break;
            case 2:
                //green
                tempCT.p7_red = "83";
                tempCT.p8_green = "255";
                tempCT.p9_blue = "83";
                break;
            case 5:
                //black
                tempCT.p7_red = "0";
                tempCT.p8_green = "0";
                tempCT.p9_blue = "0";
                break;
        }
        //this is a fucking awful way to do it but the weirdness of GD has forced my hand
        tempCT.p2_x = std::to_string(tempBC->xPos + 165);
        tempCT.p3_y = std::to_string(3000);
        tempCT.p23_channel = std::to_string(1000);
        result += "1,";
        result += tempCT.p1_id;
        result += ",2,";
        result += tempCT.p2_x;
        result += ",3,";
        result += tempCT.p3_y;
        result += ",7,";
        result += tempCT.p7_red;
        result += ",8,";
        result += tempCT.p8_green;
        result += ",9,";
        result += tempCT.p9_blue;
        result += ",10,";
        result += tempCT.p10_duration;  
        result += ",23,";
        result += tempCT.p23_channel;
        result += tempCT.remainder;
        result += ";";

        tempCT.p2_x = std::to_string(tempBC->xPos + 165);
        tempCT.p3_y = std::to_string(3030);
        tempCT.p23_channel = std::to_string(1001);
        result += "1,";
        result += tempCT.p1_id;
        result += ",2,";
        result += tempCT.p2_x;
        result += ",3,";
        result += tempCT.p3_y;
        result += ",7,";
        result += tempCT.p7_red;
        result += ",8,";
        result += tempCT.p8_green;
        result += ",9,";
        result += tempCT.p9_blue;
        result += ",10,";
        result += tempCT.p10_duration;  
        result += ",23,";
        result += tempCT.p23_channel;
        result += tempCT.remainder;
        result += ";";

        tempCT.p2_x = std::to_string(tempBC->xPos + 165);
        tempCT.p3_y = std::to_string(3060);
        tempCT.p23_channel = std::to_string(1009); //wtf
        result += "1,";
        result += tempCT.p1_id;
        result += ",2,";
        result += tempCT.p2_x;
        result += ",3,";
        result += tempCT.p3_y;
        result += ",7,";
        result += tempCT.p7_red;
        result += ",8,";
        result += tempCT.p8_green;
        result += ",9,";
        result += tempCT.p9_blue;
        result += ",10,";
        result += tempCT.p10_duration;  
        result += ",23,";
        result += tempCT.p23_channel;
        result += tempCT.remainder;
        result += ";";
    }


    GravityChange* tempGC;
    gdMirrorPortal tempMP;
    gdCameraObj tempCO;
    bool currentlyInverted = false;

    for(int i = 0; i < inLevel.getGravityCount(); i++)
    {
        tempGC = inLevel.getGravAtIndex(i);

        if(currentlyInverted)
        {
            tempMP.objID = "46";
            tempCO.rotation = "0";
        }
        else
        {
            tempMP.objID = "45";
            tempCO.rotation = "180";
        }

        currentlyInverted = !currentlyInverted;

        tempMP.xpos = std::to_string(tempGC->xPos + 165);
        tempCO.xpos = std::to_string(tempGC->xPos + 165);

        result += tempMP.base;
        result += tempMP.objID;
        result += tempMP.middle;
        result += tempMP.xpos;
        result += tempMP.remainder;
        result += ";";

        result += tempCO.base;
        result += tempCO.xpos;
        result += tempCO.middle;
        result += tempCO.rotation;
        result += ";";
    }

    BlocksRise* tempBR;
    gdBlocksRise tempGBR;

    for(int i = 0; i < inLevel.getRisingCount(); i++)
    {
        tempBR = inLevel.getRisingAtIndex(i);

        tempGBR.xpos = std::to_string(tempBR->startX - 465);

        tempGBR.id = "23";

        result += tempGBR.base;
        result += tempGBR.id;
        result += tempGBR.middle;
        result += tempGBR.xpos;
        result += tempGBR.remainder;
        result += ";";

        if(tempBR->startX == tempBR->endX)
        {
            tempGBR.xpos = std::to_string(inLevel.getEndPos() - 495);
        }
        else
        {
            tempGBR.xpos = std::to_string(tempBR->endX - 495);
        }
        
        tempGBR.id = "1915";

        result += tempGBR.base;
        result += tempGBR.id;
        result += tempGBR.middle;
        result += tempGBR.xpos;
        result += tempGBR.remainder;
        result += ";";
    }

    BlocksFall* tempBF;
    gdBlocksFall tempGBF;

    for(int i = 0; i < inLevel.getFallingCount(); i++)
    {
        tempBF = inLevel.getFallingAtIndex(i);

        tempGBF.xpos = std::to_string(tempBF->startX - 135);
        tempGBF.id = "23";

        result += tempGBF.base;
        result += tempGBF.id;
        result += tempGBF.middle;
        result += tempGBF.xpos;
        result += tempGBF.remainder;
        result += ";";

        if(tempBF->startX == tempBF->endX)
        {
            tempGBF.xpos = std::to_string(inLevel.getEndPos() - 15);
        }
        else
        {
            tempGBF.xpos = std::to_string(tempBF->endX - 15);
        }

        tempGBF.id = "1915";

        result += tempGBF.base;
        result += tempGBF.id;
        result += tempGBF.middle;
        result += tempGBF.xpos;
        result += tempGBF.remainder;
        result += ";";
    }

    return result;
}


static auto IMPORT_PICK_OPTIONS = file::FilePickOptions {
    std::nullopt,
    {
        {
            "Impossible Game Level Files",
            { "*.dat" }
        }
    }
};


struct $modify(ImportLayer, LevelBrowserLayer) {
    struct Fields {
        EventListener<Task<Result<std::filesystem::path>>> pickListener;
    };

    static void importFiles(std::filesystem::path const& path) {
        Level igLevel(path.generic_string(), false);

        if(igLevel.getBlockCount() == 0 && igLevel.getBackgroundCount() == 0 && igLevel.getEndPos() == 3015)
        {
            FLAlertLayer::create("Load Error", "This is most likely not a valid Impossible Game level file", "OK")->show();
        }
        else if (igLevel.getLoadedSuccessfully() == false)
        {
            FLAlertLayer::create("Parse Error", "This is not a valid Impossible Game level!", "OK")->show();
        }
        else
        {
            std::string innerLevelString = buildObjectString(igLevel);
            std::string encodedString = ZipUtils::compressString(innerLevelString, false, 0);
            
            auto gdLevel = GJGameLevel::create();
            
            gdLevel->m_levelType = GJLevelType::Editor;
            gdLevel->m_levelString = encodedString;
            gdLevel->m_levelName = "Impossible Game Import";

            LocalLevelManager::get()->m_localLevels->insertObject(gdLevel, 0);

            auto scene = CCScene::create();
            auto layer = LevelBrowserLayer::create(
                GJSearchObject::create(SearchType::MyLevels)
            );
            scene->addChild(layer);
            CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(.5f, scene));
        }
    }

    void onImport(CCObject*) {
        m_fields->pickListener.bind([](auto* event) {
            if (auto result = event->getValue()) {
                if (result->isOk()) {
                    importFiles(**result);
                }
                else {
                    FLAlertLayer::create("Error Importing", result->unwrapErr(), "OK")->show();
                }
            }
        });
        m_fields->pickListener.setFilter(file::pick(file::PickMode::OpenFile, IMPORT_PICK_OPTIONS));
    }

    $override
    bool init(GJSearchObject* search) {
        if (!LevelBrowserLayer::init(search))
            return false;

        if (search->m_searchType == SearchType::MyLevels || search->m_searchType == SearchType::MyLists) {
            auto btnMenu = this->getChildByID("new-level-menu");

            auto igImportBtn = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSpriteFrameName(
                    "file.png"_spr, .85f,
                    CircleBaseColor::Pink,
                    CircleBaseSize::Big
                ),
                this,
                menu_selector(ImportLayer::onImport)
            );
            igImportBtn->setID("import-ig-level-button"_spr);

            // This one has an ID but no layout which is CRINGE
            if (search->m_searchType == SearchType::MyLists && search->m_searchIsOverlay) {
                btnMenu->addChildAtPosition(igImportBtn, Anchor::BottomLeft, ccp(0, 60), false);
            }
            else {
                btnMenu->addChild(igImportBtn);
                btnMenu->updateLayout();
            }
        }

        return true;
    }
};