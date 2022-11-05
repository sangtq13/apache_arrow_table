#ifndef PARSE_XML_H
#define PARSE_XML_H

#include <string>

class XMLParse {
public:
	struct BookEdition {
		std::string isbn;
		uint32_t year;
		float price;
	};

	struct Book {
		std::string category;
		std::string title;
		std::string author;
		std::vector<BookEdition> editions;
	};

	struct BookStore {
		std::vector<Book> books;
	};

	bool Parse(BookStore& bookstore, std::string xml_file_path);
private:
	std::string TrimWhitespace(std::string input);
};

#endif /* PARSE_XML_H */
