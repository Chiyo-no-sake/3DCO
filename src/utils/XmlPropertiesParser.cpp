#include <log/Log.h>
#include <fstream>
#include "XmlPropertiesParser.h"

XmlPropertiesParser *XmlPropertiesParser::getInstance() {

    static XmlPropertiesParser parser{};

    return &parser;

}

void XmlPropertiesParser::init() {

    std::ifstream input("../../tests/assets/physics-properties.xml");

    if (input) {

        CO_LOG_INFO("Properties file opened successfully");

        std::string str((std::istreambuf_iterator<char>(input)),
                        std::istreambuf_iterator<char>());
        m_content = new char[str.size()+1];
        strcpy(m_content, str.c_str());
        m_document.parse<0>(m_content);

        input.close();

    } else
        CO_LOG_ERR("Properties file could not be opened");


}

float XmlPropertiesParser::getProperty(const std::string &nodeName, const std::string &propertyName) {

    rapidxml::xml_node<> *root = m_document.first_node();
    rapidxml::xml_node<> *node = root->first_node(nodeName.substr(nodeName.find('.') + 1).c_str());

    if (node == nullptr) {

        CO_LOG_WARN("Material {} has no valid type. Default properties will be applied.", nodeName);
        node = root->first_node("default");
    }

    if(node == nullptr){

        CO_LOG_ERR("Cannot find default material properties");
        return 0;
    }

    rapidxml::xml_node<> *property = node->first_node(propertyName.c_str());

    if (property == nullptr) {

        CO_LOG_ERR("Property {} is not valid within material {}", propertyName, nodeName);
        return 0;

    }

    return std::stof(property->value());

}
