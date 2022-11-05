#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "parse_xml.h"
#include "log.h"

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

std::string XMLParse::TrimWhitespace(std::string input) {
  return boost::trim_left_copy(boost::trim_right_copy(input));
}

bool XMLParse::Parse(BookStore& book_store, std::string xml_file_path)
{
	if (!fs::exists(fs::path(xml_file_path))) 
	{
		std::cout << "xml_file_path file does not exist!" << std::endl;
		return false;
	}

	if (!fs::is_regular_file(fs::path(xml_file_path))) {
    std::cout << "xml_file_path is not a regular file!" << std::endl;
    return false;
  }

  pt::ptree ptree;
  read_xml(xml_file_path, ptree);

  for (pt::ptree::value_type& root : ptree.get_child("bookstore"))
  {
  	Book book;
  	if (root.first == "book") {
  		std::string category = root.second.get<std::string>("<xmlattr>.category");
  		book.category = TrimWhitespace(category);
      // LOG_INFO(book.category);
      book.title = TrimWhitespace(root.second.get<std::string>("title"));
      // LOG_INFO(book.title);
      book.author = TrimWhitespace(root.second.get<std::string>("author"));
      // LOG_INFO(book.author);
      pt::ptree editions_root = root.second.get_child("editions");
      for (pt::ptree::value_type& edition : editions_root)
      {
        // LOG_INFO(edition.first);
        BookEdition book_edition;
        book_edition.isbn = edition.second.get<std::string>("isbn", "");
        // LOG_INFO(book_edition.isbn);
        book_edition.year = edition.second.get<uint32_t>("year");
        // LOG_INFO(book_edition.year);
        book_edition.price = edition.second.get<float>("price");
        // LOG_INFO(book_edition.price);
        book.editions.push_back(book_edition);
      }
  	}
  	else {
  		continue;
  	}
  	
  	book_store.books.push_back(book);
  }
  return true;
}
