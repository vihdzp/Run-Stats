//! Thanks to https://github.com/matcool/geode-mods/tree/main/run-info for providing a base for the
//! code.

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

/// Represents a section in a level.
struct Section {
	/// The name of the section (including the percentage it spans).
	const char* name;
	/// Starting percentage.
	int start;
};

class RunStatsWidget : public CCNode {
public:
	/// Displays the name of the section.
	CCLabelBMFont* m_section_name = nullptr;
	/// Displays the success chance for this section.
	CCLabelBMFont* m_section_percent = nullptr;
	/// Displays the success chance for the entire level.
	//CCLabelBMFont* m_level_percent = nullptr;

	Section* sections = nullptr;
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
		}
	}

	bool init() {
		if (!CCNode::init()) {
			return false;
		}

		Section sec1 = { "First click (0-2)", 0 };
		Section sec2 = { "Everything else (2-100)", 2 };

		// FIXME: where to free this memory?
		sections = new Section[]{ sec1, sec2 };
		section_count = 2;

		const auto anchor = ccp(0.0, 1.0);

		// Dummy text for now. 
		m_section_name = CCLabelBMFont::create(sec1.name, "bigFont.fnt");
		m_section_name->setOpacity(64);
		m_section_name->setAnchorPoint(anchor);
		m_section_name->setPosition(0.0, 0.0);
		this->addChild(m_section_name);

		auto label_height = m_section_name->getScaledContentSize().height;

		m_section_percent = CCLabelBMFont::create("88% / 12%", "bigFont.fnt");m_section_percent->setOpacity(64);
		m_section_percent->setAnchorPoint(anchor);
		m_section_percent->setPosition(0.0, -label_height);
		this->addChild(m_section_percent);

		float height = m_section_name->getScaledContentSize().height +
			m_section_percent->getScaledContentSize().height;
		float width = m_section_name->getScaledContentSize().width;
		this->setContentSize({ width, height });

		return true;
	}


	/*
		void update_labels(PlayLayer* layer, int percent) {
			if (layer->m_isPracticeMode) {
				m_section_label->setString("Practice");
				m_was_practice = true;
			}
			else if (layer->m_isTestMode) {
				m_section_label->setString("Testmode");
				m_was_practice = false;
			}

			m_info_label->setString(fmt::format("From {}%", percent).c_str());
		}
		*/
};

class $modify(PlayLayer) {
	RunStatsWidget* m_widget = nullptr;

	bool init(GJGameLevel * level, bool _arg1, bool _arg2) {
		if (!PlayLayer::init(level, _arg1, _arg2)) {
			return false;
		}

		// Removes the GD testmode label.
		CCArrayExt<CCNode*> children = this->getChildren();
		for (auto* child : children) {
			using namespace std::literals::string_view_literals;
			if (auto* label = typeinfo_cast<CCLabelBMFont*>(child); label && label->getString() == "Testmode"sv) {
				label->setVisible(false);
				break;
			}
		}

		const auto win_size = CCDirector::sharedDirector()->getWinSize();

		m_widget = RunStatsWidget::create();
		m_widget->setAnchorPoint(ccp(0.0, 0.0));
		m_widget->setPosition(0.0, win_size.height);
		m_widget->setScale(0.3);
		m_widget->setZOrder(999);
		this->addChild(m_widget);

		return true;
	}

	/// Resets the section.
	void resetLevel() {
		PlayLayer::resetLevel();
		m_widget->current_section = 0;
		m_widget->m_section_name->setCString(m_widget->sections[0].name);
	}

	/// Updates the section.
	void updateProgressbar() {
		PlayLayer::updateProgressbar();
		int next_section = m_widget->current_section + 1;
		if (next_section < m_widget->section_count) {
			auto sec = m_widget->sections[next_section];
			if (sec.start <= this->getCurrentPercentInt()) {
				m_widget->current_section = next_section;
				m_widget->m_section_name->setCString(sec.name);
			}
		}
	}
};
