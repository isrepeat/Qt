#include <thread>
#include <string>
#include <vector>
#include <QString>
#include <json_struct/json_struct.h>


namespace JS
{
    template<>
    struct TypeHandler<QString>
    {
        static inline Error to(QString& to_type, ParseContext& context)
        {
            auto error = JS::Error::NoError;

            std::string result;
            error = TypeHandler<std::string>::to(result, context);

            if (error != JS::Error::NoError) {
                return error;
            }

            to_type = QString::fromStdString(result);
            return error;
        }

        static inline void from(const QString& from_type, Token& token, Serializer& serializer)
        {
            auto str = from_type.toStdString();
            return TypeHandler<std::string>::from(str, token, serializer);
        }
    };
}


struct Purchase {
    int id = -1;
    float price;
    QString name = "...";
    //std::string name = "...";

    JS_OBJECT(
        JS_MEMBER_ALIASES(id, "Id"),
        JS_MEMBER_ALIASES(name, "Name"),
        JS_MEMBER_ALIASES(price, "Price")
    );
};


struct Picture {
    std::vector<uint8_t> imageData = { 16, 32 };
};


namespace JS
{
    template<>
    struct TypeHandler<Picture>
    {
        static inline Error to(Picture& to_type, ParseContext& context)
        {
            //There exists a TypeHandler for T[N] already, so all we have to do is unwrap the
            //data and call the other TypeHandler specialisation
            //return TypeHandler<std::vector<uint8_t>>::to(to_type.imageData, context);

            auto error = JS::Error::NoError;

            std::string bytes;
            error = TypeHandler<std::string>::to(bytes, context);

            if (error != JS::Error::NoError) {
                return error;
            }

            to_type.imageData = std::vector<uint8_t>(bytes.begin(), bytes.end());
            return error;
        }

        static inline void from(const Picture& from_type, Token& token, Serializer& serializer)
        {
            //return TypeHandler<std::vector<uint8_t>>::from(from_type.imageData, token, serializer);

            auto bytes = std::string(from_type.imageData.begin(), from_type.imageData.end());
            return TypeHandler<std::string>::from(bytes, token, serializer);
        }
    };
}



struct Person {
    //Purchase purchases[4];
    //std::vector<Purchase> purchases;
    std::vector<Purchase> purchases;
    std::string firstName = "First Name";
    std::string secondName = "Second Name";
    //std::vector<uint8_t> userPicture{ 11, 22, 33, 44 };
    Picture userPicture;

    JS_OBJECT(
        JS_MEMBER_ALIASES(purchases, "Purchases"),
        JS_MEMBER_ALIASES(firstName, "FirstName"),
        JS_MEMBER_ALIASES(secondName, "SecondName"),
        JS_MEMBER_ALIASES(userPicture, "UserPicture")
    );
};




template <typename T>
struct JsonObjectWrapper {
    JsonObjectWrapper(T& valueRef)
        : valueRef{ valueRef }
    {}

    T& valueRef;
};


namespace JS
{
    template <typename T>
    struct TypeHandler<JsonObjectWrapper<T>>
    {
        static inline Error to(JsonObjectWrapper<T>& to_type, ParseContext& context)
        {
            if (context.token.value_type != JS::Type::ObjectStart)
                return Error::ExpectedObjectStart;

            Error error = context.nextToken();
            if (error != JS::Error::NoError)
                return error;

            while (context.token.value_type != JS::Type::ObjectEnd)
            {
                error = TypeHandler<T>::to(to_type.valueRef, context);
                if (error != JS::Error::NoError)
                    break;

                error = context.nextToken();
                if (error != JS::Error::NoError)
                    break;
            }

            return error;
        }

        static inline void from(const JsonObjectWrapper<T>& from_type, Token& token, Serializer& serializer)
        {
            token.value_type = Type::ObjectStart;
            token.value = DataRef("{");
            serializer.write(token);

            token.name = DataRef("");
            TypeHandler<T>::from(from_type.valueRef, token, serializer);
            token.name = DataRef("");

            token.value_type = Type::ObjectEnd;
            token.value = DataRef("}");
            serializer.write(token);
        }
    };
} // namespace JS











void TestParseComplexJson() {
    /* -------------------------------------------------------------------------- */
    /* Json Objects that will converted to custom struct with registered members  */
    /* -------------------------------------------------------------------------- */
    {
        const char jsonPerson[] = R"json(
        {
            "FirstName": "Alexandr",
            "secondName": "Bondarenko"
        }
        )json";

            const char jsonPersonPurchase[] = R"json(
        {
            "firstName": "Alexandr",
            "secondName": "Bondarenko",
            "purchase": { 
                "id": 1011,
                "name": "1 month",
                "price": 6.99
            }
        }
        )json";

            const char jsonPersonPurchases[] = R"json(
        {
            "firstName": "Alexandr",
            "secondName": "Bondarenko",
            "purchases": [
                { "id": 1011, "price": 6.99, "name": "1 month" },
                { "id": 1044, "price": 14.99, "name": "6 month" },
                { "id": 1077, "price": 21.65, "name": "12 month" }
            ],
            "userPicture": "\x10 €ÿ€@ \x10"
        }
        )json";



        //Person person;
        //printf("Person [src]: \n%s", JS::serializeStruct(person).c_str());

        ////JS::ParseContext parseContext(jsonPerson);
        ////JS::ParseContext parseContext(jsonPersonPurchase);
        //JS::ParseContext parseContext(jsonPersonPurchases);

        //if (parseContext.parseTo(person) != JS::Error::NoError) {
        //    std::string errorStr = parseContext.makeErrorString();
        //    fprintf(stderr, "Error parsing struct %s\n", errorStr.c_str());
        //    return;
        //}

        //printf("\n");
        //printf("\n");
        //printf("Person [parsed]: \n%s", JS::serializeStruct(person).c_str());
    }

    /* ----------------------------------------- */
    /* Json Object with single registered field  */
    /* ----------------------------------------- */
    {
        const char jsonToken[] = R"json(
        {
            "token": "asdadasdasdasdasdasd_1488"
        }
        )json";

        //QString promotionToken;
        //JsonObjectWrapper jsonObjectWrapper{ promotionToken };
        //printf("Token [src]: \n%s", JS::serializeStruct(jsonObjectWrapper).c_str());

        //JS::ParseContext parseContext(jsonToken);
        //if (parseContext.parseTo(jsonObjectWrapper) != JS::Error::NoError) {
        //    std::string errorStr = parseContext.makeErrorString();
        //    fprintf(stderr, "Error parsing struct %s\n", errorStr.c_str());
        //    return;
        //}

        //printf("\n");
        //printf("\n");
        //printf("Token [parsed]: \n%s", JS::serializeStruct(jsonObjectWrapper).c_str());
    }

    /* --------------------------------------- */
    /*      Json Object with unnamed array     */
    /* --------------------------------------- */
    {
        const char jsonArrayPurchases[] = R"json(
        {
            [
                { "id": 1011, "price": 6.99, "name": "1 month" },
                { "id": 1044, "price": 14.99, "name": "6 month" },
                { "id": 1077, "price": 21.65, "name": "12 month" }
            ]
        }
        )json";


        std::vector<Purchase> boughtPurchases;
        JsonObjectWrapper jsObjectWrapper{ boughtPurchases };

        //printf("Bought purchases [src]: \n%s", JS::serializeStruct(JsonObjectWrapper(purchases)).c_str());
        //printf("Bought purchases [src]: \n%s", JS::serializeStruct(boughtPurchases).c_str());
        printf("Bought purchases [src]: \n%s", JS::serializeStruct(jsObjectWrapper).c_str());

        JS::ParseContext parseContext(jsonArrayPurchases);
        if (parseContext.parseTo(jsObjectWrapper) != JS::Error::NoError) {
            std::string errorStr = parseContext.makeErrorString();
            fprintf(stderr, "Error parsing struct %s\n", errorStr.c_str());
            return;
        }

        printf("\n");
        printf("\n");
        printf("Bought purchases [parsed]: \n%s", JS::serializeStruct(boughtPurchases).c_str());
        //printf("Bought purchases [parsed]: \n%s", JS::serializeStruct(jsObjectWrapper).c_str());
    }

}



int main(int argc, char **argv) {
    TestParseComplexJson();

    printf("\n");
    std::this_thread::sleep_for(std::chrono::milliseconds{ 8'000 });
    return 0;
}