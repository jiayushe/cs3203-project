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

        EntityRef();

        ~EntityRef();

        void set_type(EntityRefType entityRefType);

        void set_synonym(std::string synonym);

        void set_name(std::string name);

    private:
        EntityRefType type;
        std::string synonym;
        std::string name;

    };

}

#endif //PQLPARSER_ENTITYREF_H
