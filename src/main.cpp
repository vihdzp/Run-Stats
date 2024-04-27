//! Thanks to https://github.com/matcool/geode-mods/tree/main/run-info for providing a base for the
//! code.

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

/// Represents a section in a level.
/*struct Section {
	/// The name of the section (including the percentage it spans).
	const char* name;
	/// Starting percentage.
	int start;
};*/

class RunStatsWidget : public CCNode {
public:
	/// Displays the name of the section.
	CCLabelBMFont* m_section_name = nullptr;
	/// Displays the success chance for this section.
	CCLabelBMFont* m_section_percent = nullptr;
	/// Displays the success chance for the entire level.
	//CCLabelBMFont* m_level_percent = nullptr;

	//Section* sections = nullptr;
	int section_count = 0;
	int current_section = 0;

	/// Create widget. 
	static RunStatsWidget* create() {
		auto* ret = new (std::nothrow) RunStatsWidget;
		if (ret && ret->init()) {
			ret->autorelease();
			return ret;
		}
		else {
			CC_SAFE_DELETE(ret);
			return nullptr;
		}
	}

	bool init() {
		if (!CCNode::init()) {
			return false;
		}

		//Section sec1 = { "First click (0-2)", 0 };
		//Section sec2 = { "Everything else (2-100)", 2 };

		// FIXME: where to free this memory?
		//sections = new Section[]{ sec1, sec2 };
		section_count = 2;

		const auto anchor = ccp(0.0, 1.0);

		// Dummy text for now. 
		auto section_name = CCLabelBMFont::create("0% - 1%", "bigFont.fnt");
		section_name->setOpacity(64);
		section_name->setAnchorPoint(anchor);
		section_name->setPosition(0.0, 0.0);

		auto label_size = section_name->getScaledContentSize();

		auto section_percent = CCLabelBMFont::create("dummy", "bigFont.fnt");
		section_percent->setOpacity(64);
		section_percent->setAnchorPoint(anchor);
		section_percent->setPosition(0.0, -label_size.height);

		this->addChild(section_name);
		this->addChild(section_percent);
		this->setContentSize({ label_size.width, 2 * label_size.height });

		return true;
	}
};

class $modify(PlayLayer) {
	/// m_fields
	struct Fields {
		/// Our widget.
		RunStatsWidget* m_widget = nullptr;
	};

	/// Initializes the widget.
	bool init(GJGameLevel * level, bool _arg1, bool _arg2) {
		if (!PlayLayer::init(level, _arg1, _arg2)) {
			return false;
		}

		// Removes the GD testmode label.
		CCArrayExt<CCNode*> children = this->getChildren();
		for (auto* child : children) {
			using namespace std::literals::string_view_literals;
			auto* label = typeinfo_cast<CCLabelBMFont*>(child);
			if (label && label->getString() == "Testmode"sv) {
				label->setVisible(false);
				break;
			}
		}

		const auto win_size = CCDirector::sharedDirector()->getWinSize();

		auto widget = RunStatsWidget::create();
		widget->setAnchorPoint(ccp(0.0, 0.0));
		widget->setPosition(0.0, win_size.height);
		widget->setScale(0.3);
		widget->setZOrder(999);
		this->addChild(widget);

		return true;
	}

	/// Resets the section.
	/*void resetLevel() {
		PlayLayer::resetLevel();
		auto widget = m_fields->m_widget;
		widget->m_section_name->setCString("0% - 1%");
	}*/

	/// Updates the section.
	void updateProgressbar() {
		PlayLayer::updateProgressbar();
		auto widget = m_fields->m_widget;
		widget->m_section_name->setCString("1% - 100%");
	}
};
