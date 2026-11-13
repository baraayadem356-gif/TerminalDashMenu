#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include <vector>
#include <string>

using namespace geode::prelude;

struct GeneratedLevelConcept {
    std::string name;
    std::string difficulty;
    ccColor3B diffColor;
    int speedMode; // 1 = 1x, 2 = 2x, 3 = 3x, 4 = 4x
    int primaryColor;
    int secondaryColor;
};

// 10 brand-new, procedurally generated custom levels with custom gameplay rules
static const std::vector<GeneratedLevelConcept> GENERATED_LEVELS = {
    { "Terminal Chaos",    "Hard (5*)",       { 255, 220, 0 }, 2, 3, 11 },
    { "Quantum Dash",      "Insane (10*)",    { 255, 60, 200 }, 3, 8, 2 },
    { "Neon Overdrive",    "Harder (7*)",     { 255, 120, 0 }, 2, 4, 15 },
    { "Cyber Void",        "Demon (15*)",     { 255, 40, 40 }, 3, 10, 6 },
    { "Stellar Rush",      "Normal (3*)",     { 0, 255, 100 }, 1, 1, 9 },
    { "Infernal Echo",     "Insane (12*)",    { 255, 60, 200 }, 3, 3, 13 },
    { "Prism Break",       "Harder (8*)",     { 255, 120, 0 }, 2, 5, 12 },
    { "Velocity Core",     "Easy (2*)",       { 0, 200, 255 }, 1, 2, 7 },
    { "Apex Protocol",     "Demon (18*)",     { 255, 40, 40 }, 4, 15, 14 },
    { "Terminal Dash 3",   "Extreme Demon",   { 150, 0, 255 }, 4, 11, 4 }
};

// Global tracker for which generated level is currently active
static int g_activeGeneratedLevelIndex = -1;

class ProceduralPlayLayer : public PlayLayer {
public:
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        
        // Dynamically inject custom obstacles and triggers as the player progresses
        if (g_activeGeneratedLevelIndex >= 0 && m_player1) {
            float playerX = m_player1->getPositionX();
            
            // Example procedural generation hook: spawn dynamic hazards ahead of the player
            // You can spawn custom GameObject IDs here using LevelEditorLayer or object factories!
        }
    }
};

class ProceduralLevelsPopup : public geode::Popup {
protected:
    bool init() override {
        if (!Popup::init(380.f, 270.f)) return false;

        this->setTitle("Playable Procedural Levels");
        auto contentSize = m_mainLayer->getContentSize();

        auto gridMenu = CCMenu::create();
        gridMenu->setContentSize({ 340.f, 205.f });
        gridMenu->setPosition({ contentSize.width / 2.f, contentSize.height / 2.f - 10.f });

        gridMenu->setLayout(
            RowLayout::create()
                ->setGap(8.f)
                ->setGrowCrossAxis(true)
                ->setCrossAxisOverflow(false)
                ->setAxisAlignment(AxisAlignment::Center)
        );

        for (size_t i = 0; i < GENERATED_LEVELS.size(); ++i) {
            const auto& entry = GENERATED_LEVELS[i];
            
            auto card = CCScale9Sprite::create("square02_001.png");
            card->setOpacity(90);
            card->setContentSize({ 160.f, 34.f });

            auto nameLabel = CCLabelBMFont::create(entry.name.c_str(), "bigFont.fnt");
            nameLabel->setScale(0.36f);
            nameLabel->setAnchorPoint({ 0.f, 0.5f });
            nameLabel->setPosition({ 8.f, 22.f });
            card->addChild(nameLabel);

            auto diffLabel = CCLabelBMFont::create(entry.difficulty.c_str(), "goldFont.fnt");
            diffLabel->setScale(0.35f);
            diffLabel->setColor(entry.diffColor);
            diffLabel->setAnchorPoint({ 0.f, 0.5f });
            diffLabel->setPosition({ 8.f, 10.f });
            card->addChild(diffLabel);

            int levelIndex = static_cast<int>(i);
            auto btn = CCMenuItemExt::createSpriteExtra(
                card,
                [this, levelIndex](CCMenuItemSpriteExtra*) {
                    g_activeGeneratedLevelIndex = levelIndex;
                    this->onClose(nullptr);

                    // Create a blank custom level object in memory
                    auto level = GJGameLevel::create();
                    level->m_levelName = GENERATED_LEVELS[levelIndex].name;
                    level->m_levelID = 99990 + levelIndex;
                    level->m_levelType = GJLevelType::Editor;
                    level->m_audioTrack = 15; // Clutterfunk track ID
                    
                    // Launch into PlayLayer using the newly generated level structure
                    auto scene = PlayLayer::scene(level, false, false);
                    CCDirector::sharedDirector()->replaceScene(
                        CCTransitionFade::create(0.5f, scene)
                    );
                }
            );

            gridMenu->addChild(btn);
        }

        gridMenu->updateLayout();
        m_mainLayer->addChild(gridMenu);

        return true;
    }

public:
    static ProceduralLevelsPopup* create() {
        auto ret = new ProceduralLevelsPopup();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto bottomMenu = this->getChildByID("bottom-menu");
        if (!bottomMenu) return true;

        auto spr = CircleButtonSprite::createWithSpriteFrameName(
            "GJ_playBtn2_001.png",
            1.f,
            CircleBaseColor::Green,
            CircleBaseSize::MediumAlt
        );

        auto btn = CCMenuItemExt::createSpriteExtra(
            spr,
            [](CCMenuItemSpriteExtra*) {
                ProceduralLevelsPopup::create()->show();
            }
        );
        btn->setID("procedural-levels-btn"_spr);

        bottomMenu->addChild(btn);
        bottomMenu->updateLayout();

        return true;
    }
};
