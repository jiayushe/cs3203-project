//
// Created by Victor Varian on 27/1/21.
//

#ifndef PQLPARSER_ENTITYREF_H
#define PQLPARSER_ENTITYREF_H

namespace PQLParser {
    enum class EntityRefType {
        ANY,
        SYNONYM,
        NAME,
    };

    class EntityRef {
    public:
        EntityRefType get_type();
        std::string get_synonym();
        std::string get_name();

        EntityRef(DesignEntityType type);

    private:
        EntityRefType type;
        std::string synonym;
        std::string name;

    };

}

#endif //PQLPARSER_ENTITYREF_H
