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
    int speedMode;
    int primaryColor;
    int secondaryColor;
};

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

static int g_activeGeneratedLevelIndex = -1;

class ProceduralPlayLayer : public PlayLayer {
public:
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        if (g_activeGeneratedLevelIndex >= 0 && m_player1) {
            float playerX = m_player1->getPositionX();
        }
    }
};

class ProceduralLevelsPopup : public Popup {
protected:
    bool setup() override {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        this->setTitle("Playable Procedural Levels");

        auto gridMenu = CCMenu::create();
        gridMenu->setContentSize({ 340.f, 205.f });
        gridMenu->setPosition({ winSize.width / 2.f, winSize.height / 2.f - 10.f });

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
                [this, levelIndex](CCObject*) {
                    g_activeGeneratedLevelIndex = levelIndex;
                    this->onClose(nullptr);

                    auto level = GJGameLevel::create();
                    level->m_levelName = GENERATED_LEVELS[levelIndex].name;
                    level->m_levelID = 99990 + levelIndex;
                    level->m_levelType = GJLevelType::Editor;
                    level->m_audioTrack = 15;
                    
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
        if (ret && ret->initAnchored(380.f, 270.f, "square001_001.png", { 0.f, 0.f, 0.f, 180.f })) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
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
            [](CCObject*) {
                ProceduralLevelsPopup::create()->show();
            }
        );
        btn->setID("procedural-levels-btn"_spr);

        bottomMenu->addChild(btn);
        bottomMenu->updateLayout();

        return true;
    }
};
