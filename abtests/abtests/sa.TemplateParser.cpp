#include <catch.hpp>

#include <sa/TemplateParser.h>
#include <sa/Assert.h>

TEST_CASE("TemplateParser parse")
{
    std::string s1 = "SELECT * FROM table WHERE item_flags & ${ItemFlag} = ${ItemFlag} ORDER BY type DESC, name ASC";
    sa::templ::Parser parser;
    auto templ = parser.Parse(s1);
    std::string result = templ.ToString([](const sa::templ::Token& token) -> std::string
    {
        if (token.value == "ItemFlag")
            return std::to_string(4);
        return "???";
    });

    REQUIRE(result.compare("SELECT * FROM table WHERE item_flags & 4 = 4 ORDER BY type DESC, name ASC") == 0);
}

TEST_CASE("TemplateParser string escape")
{
    std::string s1 = "SELECT * FROM table WHERE name = ${name} ORDER BY type DESC, name ASC";
    sa::templ::Parser parser;
    auto templ = parser.Parse(s1);
    std::string result = templ.ToString([](const sa::templ::Token& token) -> std::string
    {
        if (token.value == "name")
            return "'a name'";
        return "???";
    });

    REQUIRE(result.compare("SELECT * FROM table WHERE name = 'a name' ORDER BY type DESC, name ASC") == 0);
}

TEST_CASE("TemplateParser invalid param")
{
    std::string s1 = "SELECT * FROM table WHERE name = ${invalid} ORDER BY type DESC, name ASC";
    sa::templ::Parser parser;
    auto templ = parser.Parse(s1);
    std::string result = templ.ToString([](const sa::templ::Token& token) -> std::string
    {
        if (token.value == "name")
            return "'a name'";
        return "???";
    });

    REQUIRE(result.compare("SELECT * FROM table WHERE name = ??? ORDER BY type DESC, name ASC") == 0);
}

TEST_CASE("TemplateParser quotes")
{
    std::string s1 = "SELECT FROM `table` WHERE `name` = ${name} ORDER BY `type` DESC, `name` ASC";
    sa::templ::Parser parser;
    auto templ = parser.Parse(s1);
    std::string result = templ.ToString([](const sa::templ::Token& token) -> std::string
    {
        switch (token.type)
        {
        case sa::templ::Token::Type::Variable:
            if (token.value == "name")
                return "'a name'";
            return "???";
        case sa::templ::Token::Type::Quote:
            return "'";
        default:
            return "";
        }
    });

    REQUIRE(result.compare("SELECT FROM 'table' WHERE 'name' = 'a name' ORDER BY 'type' DESC, 'name' ASC") == 0);
}

TEST_CASE("TemplateParser nested quotes")
{
    std::string s1 = "SELECT FROM `table` WHERE `nam\"e\"` = ${name} ORDER BY `type` DESC, `name` ASC";
    sa::templ::Parser parser;
    auto templ = parser.Parse(s1);
    std::string result = templ.ToString([](const sa::templ::Token& token) -> std::string
    {
        switch (token.type)
        {
        case sa::templ::Token::Type::Variable:
            if (token.value == "name")
                return "'a name'";
            return "???";
        case sa::templ::Token::Type::Quote:
            return "'";
        default:
            return "";
        }
    });

    REQUIRE(result.compare("SELECT FROM 'table' WHERE 'nam\"e\"' = 'a name' ORDER BY 'type' DESC, 'name' ASC") == 0);
}

TEST_CASE("TemplateParser no quotes")
{
    std::string s1 = "SELECT FROM `table` WHERE `nam\"e\"` = ${name} ORDER BY `type` DESC, `name` ASC";
    sa::templ::Parser parser;
    parser.quotesSupport_ = false;
    auto templ = parser.Parse(s1);
    std::string result = templ.ToString([](const sa::templ::Token& token) -> std::string
    {
        switch (token.type)
        {
        case sa::templ::Token::Type::Variable:
            if (token.value == "name")
                return "'a name'";
            return "???";
        case sa::templ::Token::Type::Quote:
            ASSERT_FALSE();
        default:
            return "";
        }
    });

    REQUIRE(result.compare("SELECT FROM `table` WHERE `nam\"e\"` = 'a name' ORDER BY `type` DESC, `name` ASC") == 0);
}

TEST_CASE("TemplateParser append")
{
    sa::templ::Parser parser;
    sa::templ::Tokens tokens = parser.Parse("SELECT game_item_chances.chance AS `chance`, "
        "game_item_chances.map_uuid AS `map_uuid` "
        "FROM game_item_chances LEFT JOIN game_items ON game_items.uuid = game_item_chances.item_uuid "
        "WHERE (`map_uuid` = ${map_uuid}");
    parser.Append(" OR `map_uuid` = ${empty_map_uuid}", tokens);
    parser.Append(")", tokens);
    parser.Append(" AND `type` = ${type}", tokens);
    std::string result = tokens.ToString([](const sa::templ::Token& token) -> std::string
    {
        switch (token.type)
        {
        case sa::templ::Token::Type::Variable:
            if (token.value == "map_uuid")
                return "map_uuid";
            if (token.value == "empty_map_uuid")
                return "empty_map_uuid";
            if (token.value == "type")
                return "type";
            return "???";
        case sa::templ::Token::Type::Quote:
            if (token.value == "`")
                return "\"";
            return token.value;
        default:
            return "";
        }
    });

    REQUIRE(result.compare("SELECT game_item_chances.chance AS \"chance\", game_item_chances.map_uuid AS \"map_uuid\" FROM game_item_chances LEFT JOIN game_items ON game_items.uuid = game_item_chances.item_uuid WHERE (\"map_uuid\" = map_uuid OR \"map_uuid\" = empty_map_uuid) AND \"type\" = type") == 0);
}
