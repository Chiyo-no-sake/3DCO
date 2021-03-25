#pragma once

#include "rapidxml/rapidxml.hpp"
#include <string>

class XmlPropertiesParser {

private:

    rapidxml::xml_document<char> m_document{};
    char *m_content;

public:

    static XmlPropertiesParser *getInstance();

    void init();

    float getProperty(const std::string &nodeName, const std::string &propertyName);

};

