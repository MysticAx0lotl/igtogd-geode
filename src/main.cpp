#include "parser.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <asp/fs/fs.hpp>

using namespace geode::prelude;

// stupid iOS crashes when you open the folder picker
static const file::FilePickOptions& getImportPickOptions()
{
    static const auto OPTIONS = file::FilePickOptions{
        .defaultPath = std::nullopt,
#ifndef GEODE_IS_IOS
        .filters = {{.description = "Impossible Game Levels (.lvl)", .files = {"*.lvl"}}}
#else
        .filters = {{.description = "Impossible Game Levels (.dat)", .files = {"*.dat"}}}
#endif
    };
    return OPTIONS;
}

static Result<std::pair<asp::fs::path, std::string>> handleLevelFile(asp::fs::path path)
{
    asp::fs::path levelFilePath = path;

    GEODE_UNWRAP_INTO(
        bool pathIsDir,
        asp::fs::isDirectory(path).mapErr(
            [](auto const&)
            {
                return std::string("Failed to check if path is a directory");
            }
        )
    );

    if (pathIsDir)
    {
        GEODE_UNWRAP_INTO(
            auto files,
            file::readDirectory(path).mapErr(
                [](const std::string& err)
                {
                    return fmt::format("Failed to read directory: {}", err);
                }
            )
        );

        bool found = false;
        for (const auto& filePath : files)
        {
            GEODE_UNWRAP_INTO(
                bool entryIsDir,
                asp::fs::isDirectory(filePath).mapErr(
                    [](auto const&)
                    {
                        return std::string("Failed to check entry");
                    }
                )
            );
            if (entryIsDir)
            {
                continue;
            }
            auto ext = utils::string::toLower(utils::string::pathToString(filePath.extension()));
            if (!ext.empty() && ext.front() == '.')
            {
                ext.erase(0, 1);
            }

            if (ext == "dat")
            {
                levelFilePath = filePath;
                found = true;
                break;
            }
        }

        if (!found)
        {
            return Err("No .dat file found inside selected .lvl folder.");
        }
    }

    GEODE_UNWRAP_INTO(
        auto igLevel,
        Level::create(levelFilePath)
            .mapErr(
                [](const std::string& err)
                {
                    return err.empty() ? "This is not a valid Impossible Game level!" : err;
                }
            )
    );

    if (igLevel.getBlockCount() == 0 && igLevel.getBackgroundCount() == 0 && igLevel.getEndPos() == 3015)
    {
        return Err("This is most likely not a valid Impossible Game level file");
    }

    GEODE_UNWRAP_INTO(auto innerLevelString, Level::buildObjectString(igLevel));

    return Ok(std::make_pair(std::move(path), ZipUtils::compressString(innerLevelString, false, 0)));
}

class $modify(ImportLayer, LevelBrowserLayer)
{
    struct Fields
    {
        async::TaskHolder<Result<std::pair<asp::fs::path, std::string>>> m_importTask;
    };

    void onImport()
    {
#ifdef GEODE_IS_IOS
        auto mode = file::PickMode::OpenFile;
#else
        auto mode = file::PickMode::OpenFolder;
#endif

        m_fields->m_importTask.spawn(
            "Importing Impossible Game Level",
            [mode]() -> arc::Future<Result<std::pair<asp::fs::path, std::string>>>
            {
                auto pickResult = co_await file::pick(mode, getImportPickOptions());
                if (pickResult.isErr() || !pickResult.unwrap().has_value())
                {
                    co_return Err("");
                }

                co_return handleLevelFile(std::move(*pickResult.unwrap()));
            }(),

            [this](Result<std::pair<asp::fs::path, std::string>> result) -> void
            {
                if (result.isErr())
                {
                    auto err = result.unwrapErr();
                    if (!err.empty())
                    {
                        FLAlertLayer::create("Import Error", err, "OK")->show();
                    }
                    return;
                }

                auto [path, levelString] = std::move(result).unwrap();

                std::string name;
                for (auto it = path.begin(); it != path.end(); ++it)
                {
                    auto part = utils::string::pathToString(*it);
                    if (part.size() >= 4 && utils::string::toLower(part).substr(part.size() - 4) == ".lvl")
                    {
                        name = utils::string::pathToString(asp::fs::path(part).stem());
                        break;
                    }
                }

                if (name.empty())
                {
                    auto ext = utils::string::toLower(utils::string::pathToString(path.extension()));
                    if (ext == ".lvl")
                    {
                        name = utils::string::pathToString(path.stem());
                    }
                }

                if (!name.empty())
                {
                    std::string first = name.substr(0, 1);
                    utils::string::toUpperIP(first);
                    name = first + name.substr(1);
                }
                else
                {
                    name = "Impossible Game Import";
                }

                auto* gdLevel = GJGameLevel::create();
                gdLevel->m_levelType = GJLevelType::Editor;
                gdLevel->m_levelString = std::move(levelString);
                gdLevel->m_levelName = std::move(name);

                LocalLevelManager::get()->m_localLevels->insertObject(gdLevel, 0);

                auto* scene = CCScene::create();
                scene->addChild(LevelBrowserLayer::create(GJSearchObject::create(SearchType::MyLevels)));
                CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(.5f, scene));
            }
        );
    }

    bool init(GJSearchObject* search)
    {
        if (!LevelBrowserLayer::init(search))
        {
            return false;
        }

        if (search->m_searchType == SearchType::MyLevels || search->m_searchType == SearchType::MyLists)
        {
            auto* btnMenu = this->getChildByID("new-level-menu");
            auto* sprite = CircleButtonSprite::createWithSpriteFrameName(
                "file.png"_spr,
                .85f,
                CircleBaseColor::Pink,
                CircleBaseSize::Big
            );
            auto* igImportBtn = CCMenuItemExt::createSpriteExtra(
                sprite,
                [this](auto)
                {
                    onImport();
                }
            );

            igImportBtn->setID("import-ig-level-button"_spr);

            // This one has an ID but no layout which is CRINGE
            if (search->m_searchType == SearchType::MyLists && search->m_searchIsOverlay)
            {
                btnMenu->addChildAtPosition(igImportBtn, Anchor::BottomLeft, {0, 60}, false);
            }
            else
            {
                btnMenu->addChild(igImportBtn);
                btnMenu->updateLayout();
            }
        }
        return true;
    }
};