#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;


class $modify(EditorUI) {

    struct Fields {
        Ref<CCArray> affectedButtons;
        Fields() {
            affectedButtons = CCArray::create();
        }
    };


    static void onModify(auto& self) {
        // after other mods set their buttons
        (void) self.setHookPriorityPost("EditorUI::init", Priority::VeryLate);
    }


    inline bool setting(const char* name) {
        return Mod::get()->getSettingValue<bool>(name);
    }


    // combination of geode CCNode::swapChildIndices and CCNode::insertAfter
    void moveChildToTheEnd(CCNode* parent, CCNode* child) {
        auto lastChild = static_cast<CCNode*>(parent->getChildren()->lastObject());
        child->setZOrder(lastChild->getZOrder());
        child->setOrderOfArrival(lastChild->getOrderOfArrival() + 1);
        parent->m_pChildren->removeObject(child);
        parent->m_pChildren->addObject(child); // add to the end
    }


    void moveChildToTheStart(CCNode* parent, CCNode* child) {
        auto firstChild = static_cast<CCNode*>(parent->getChildren()->firstObject());
        child->setZOrder(firstChild->getZOrder());
        child->setOrderOfArrival(firstChild->getOrderOfArrival() - 1);
        parent->m_pChildren->removeObject(child);
        parent->m_pChildren->insertObject(child, 0); // add first
    }


    void handleMenuChild(CCNode* menu, const char* childId, bool invertedMenu=false) {
        if (auto btn = menu->getChildByID(childId)) {
            m_fields->affectedButtons->addObject(btn);
            if (!invertedMenu) moveChildToTheEnd(menu, btn);
            else moveChildToTheStart(menu, btn);
            btn->setVisible(false);
        } else {
            log::error("child {} not found", childId);
        }
    }


    bool init(LevelEditorLayer* p) {
        if (!EditorUI::init(p)) {
            return false;
        }

        if (!setting("enable")) return true;

        // Since we shouldn't delete RobTop's nodes, we'll just make them invisible and 
        // move them to the end of the menu child list so that they don't affect layouts

        if (setting("no-zoom")) {
            auto menu = getChildByID("zoom-menu");
            auto pos = menu->getPositionX() - menu->getScaledContentWidth() / 2;
            if (menu->getChildrenCount() == 2) {
                if (auto menu2 = getChildByID("link-menu")) {
                    menu2->setPositionX(pos + menu2->getScaledContentWidth() / 2);
                }
            }
            handleMenuChild(menu, "zoom-out-button");
            handleMenuChild(menu, "zoom-in-button");
            menu->updateLayout();
        }

        if (setting("no-link")) {
            if (auto menu = getChildByID("link-menu")) {
                handleMenuChild(menu, "link-button", true);
                handleMenuChild(menu, "unlink-button", true);
                menu->updateLayout();
            }
        }

        // todo:
        // if (setting("no-play-test")) {

        // }

        // if (setting("no-music-test")) {

        // }

        if (setting("no-undo-redo")) {
            auto menu = getChildByID("undo-menu");
            handleMenuChild(menu, "undo-button");
            handleMenuChild(menu, "redo-button");
            menu->updateLayout();
        }

        if (setting("no-trashcan")) {
            auto menu = getChildByID("undo-menu");
            handleMenuChild(menu, "delete-trash-button");
            menu->updateLayout();
        }

        if (setting("no-pause")) {
            auto menu = getChildByID("settings-menu");
            handleMenuChild(menu, "pause-button");
            menu->updateLayout();
        }

        auto bigMenu = getChildByID("editor-buttons-menu");

        if (setting("no-copy")) {
            handleMenuChild(bigMenu, "copy-button");
        }

        if (setting("no-paste")) {
            handleMenuChild(bigMenu, "paste-button");
        }

        if (setting("no-copy-paste")) {
            handleMenuChild(bigMenu, "copy-paste-button");
        }

        if (setting("no-edit-special")) {
            handleMenuChild(bigMenu, "edit-special-button");
        }

        if (setting("no-edit-group")) {
            handleMenuChild(bigMenu, "edit-group-button");
        }

        if (setting("no-edit-object")) {
            handleMenuChild(bigMenu, "edit-object-button");
        }

        if (setting("no-copy-values")) {
            handleMenuChild(bigMenu, "copy-values-button");
        }

        if (setting("no-paste-state")) {
            handleMenuChild(bigMenu, "paste-state-button");
        }

        if (setting("no-paste-color")) {
            handleMenuChild(bigMenu, "paste-color-button");
        }

        if (setting("no-adjust-hsv")) {
            handleMenuChild(bigMenu, "hsv-button");
        }

        if (setting("no-goto-layer")) {
            handleMenuChild(bigMenu, "go-to-layer-button");
        }

        if (setting("no-deselect")) {
            handleMenuChild(bigMenu, "deselect-button");
        }

        bigMenu->updateLayout();

        


        return true;
    }

    void showUI(bool isVisible) {
        EditorUI::showUI(isVisible);
        if (!isVisible) return;
        // don't show useless buttons
        for (auto* btn : CCArrayExt<CCNode*>(m_fields->affectedButtons)) {
            btn->setVisible(false);
        }
    }
};
// made in 2 hours
