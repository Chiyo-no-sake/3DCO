#include <log/Log.h>
#include <fstream>
#include "XmlPropertiesParser.h"

XmlPropertiesParser *XmlPropertiesParser::getInstance() {

    static XmlPropertiesParser parser{};

    return &parser;

}

void XmlPropertiesParser::init() {

    std::ifstream input("../../tests/assets/physics-properties.xml");

    std::string content;

    if (input) {

        CO_LOG_INFO("Properties file opened succesfully");

        std::stringstream os;
        os << input.rdbuf();
        content = os.str();
        m_content = const_cast<char *>(content.c_str());

        m_document.parse<0>(m_content);

    } else
        CO_LOG_ERR("Properties file could not be opened");


}

float XmlPropertiesParser::getProperty(const std::string &nodeName, const std::string &propertyName) {

    rapidxml::xml_node<> *root = m_document.first_node();
    rapidxml::xml_node<> *node = root->first_node(nodeName.substr(nodeName.find('.') + 1).c_str());

    if (node == nullptr) {

        CO_LOG_WARN("Material {} is not a valid type. Default properties will be applied.", nodeName);
        node = root->first_node("default");
    }

    if(node == nullptr){

        CO_LOG_WARN("B R U H");

    }

    rapidxml::xml_node<> *property = node->first_node(propertyName.c_str());

    if (property == nullptr) {

        CO_LOG_ERR("Property {} is not valid within material {}", propertyName, nodeName);
        return 0;

    }

    CO_LOG_WARN("BBruh {}", property->name());

    return std::stof(property->value());

}
