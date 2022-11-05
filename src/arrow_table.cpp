#include <iostream>
#include <string>
#include <arrow/api.h>
#include <vector>
#include "parse_xml.h"
#include "log.h"

arrow::Result<std::shared_ptr<arrow::Table>> CreateTable(XMLParse::BookStore& book_store) {
  std::shared_ptr<arrow::Field> field_category, field_title, field_author, field_editions;
  std::shared_ptr<arrow::Field> field_isbn, field_year, field_price;
  std::shared_ptr<arrow::Schema> book_schema;
  arrow::MemoryPool* pool = arrow::default_memory_pool();

  // Every field needs its name and data type.
  field_category = arrow::field("category", arrow::utf8());
  field_title = arrow::field("title", arrow::utf8());
  field_author = arrow::field("author", arrow::utf8());

  std::vector<std::shared_ptr<arrow::Field>> edition_fields;
  field_isbn = arrow::field("isbn", arrow::utf8());
  edition_fields.push_back(field_isbn);
  field_year = arrow::field("year", arrow::uint32());
  edition_fields.push_back(field_year);
  field_price = arrow::field("price", arrow::float32());
  edition_fields.push_back(field_price);
  std::shared_ptr<arrow::DataType> struct_edition = struct_(edition_fields);
  std::shared_ptr<arrow::DataType> list_edition = arrow::list(struct_edition);
  field_editions = arrow::field("editions", list_edition);
  // field_editions = arrow::field("editions", struct_edition);
  book_schema = arrow::schema({field_category, field_title, field_author, field_editions});

  auto string_builder = std::make_shared<arrow::StringBuilder>(pool);
  auto uint32_builder = std::make_shared<arrow::UInt32Builder>(pool);
  auto float_builder = std::make_shared<arrow::FloatBuilder>(pool);
  std::vector<std::shared_ptr<arrow::ArrayBuilder>> builder{string_builder, uint32_builder, float_builder};
  auto struct_builder = std::make_shared<arrow::StructBuilder>(struct_edition, pool, builder);
  auto list_builder = std::make_shared<arrow::ListBuilder>(pool, struct_builder, list_edition);

  std::vector<std::string> category_list;
  std::vector<std::string> title_list;
  std::vector<std::string> author_list;
  std::vector<XMLParse::BookEdition> book_edition_list;

  std::shared_ptr<arrow::Array> categorys;
  std::shared_ptr<arrow::Array> titles;
  std::shared_ptr<arrow::Array> authors;
  std::shared_ptr<arrow::Array> book_editions;
  // std::cout << ">>>>>>>>> " << __LINE__ << std::endl;
  for (int i = 0; i < book_store.books.size(); ++i) {
    category_list.push_back(book_store.books[i].category);
    title_list.push_back(book_store.books[i].title);
    author_list.push_back(book_store.books[i].author);
    int64_t size = book_store.books[i].editions.size();
    ARROW_RETURN_NOT_OK(list_builder->Append());
    for (int j = 0; j < size; ++j) {
      ARROW_RETURN_NOT_OK(struct_builder->Append());
      ARROW_RETURN_NOT_OK(string_builder->Append(book_store.books[i].editions[j].isbn));
      ARROW_RETURN_NOT_OK(uint32_builder->Append(book_store.books[i].editions[j].year));
      ARROW_RETURN_NOT_OK(float_builder->Append(book_store.books[i].editions[j].price));
    }
    // ARROW_RETURN_NOT_OK(list_builder->Append());
    // ARROW_RETURN_NOT_OK(list_builder->AppendValues(size, (uint8_t*) book_store.books[i].editions.data()));
  }

  // ARROW_ASSIGN_OR_RAISE(book_editions, list_builder->Finish());
  ARROW_RETURN_NOT_OK(list_builder->Finish(&book_editions));

  ARROW_RETURN_NOT_OK(string_builder->AppendValues(category_list));
  
  ARROW_ASSIGN_OR_RAISE(categorys, string_builder->Finish());

  ARROW_RETURN_NOT_OK(string_builder->AppendValues(title_list));
  
  ARROW_ASSIGN_OR_RAISE(titles, string_builder->Finish());

  ARROW_RETURN_NOT_OK(string_builder->AppendValues(author_list));
  
  ARROW_ASSIGN_OR_RAISE(authors, string_builder->Finish());


  // std::shared_ptr<arrow::RecordBatch> book_rbatch;

  // book_rbatch = arrow::RecordBatch::Make(book_schema, categorys->length(), {categorys, titles, authors, field_editions});

  // std::cout << book_rbatch->ToString();


  arrow::ArrayVector category_vecs{categorys};

  std::shared_ptr<arrow::ChunkedArray> category_chunks =
      std::make_shared<arrow::ChunkedArray>(category_vecs);

  arrow::ArrayVector title_vecs{titles};

  std::shared_ptr<arrow::ChunkedArray> title_chunks =
      std::make_shared<arrow::ChunkedArray>(title_vecs);

  arrow::ArrayVector author_vecs{authors};

  std::shared_ptr<arrow::ChunkedArray> author_chunks =
      std::make_shared<arrow::ChunkedArray>(author_vecs);

  arrow::ArrayVector editions_vecs{book_editions};

  std::shared_ptr<arrow::ChunkedArray> edition_chunks =
      std::make_shared<arrow::ChunkedArray>(editions_vecs);

  std::shared_ptr<arrow::Table> table;
  table = arrow::Table::Make(book_schema, {category_chunks, title_chunks, author_chunks, edition_chunks}, 10);

  std::cout << table->ToString();

  return table;
  // return arrow::Status::OK();
}

int main() {
  std::string xml_file_path = "../xml_file/book_store.xml";
  XMLParse xml_parse;
  XMLParse::BookStore book_store;
  xml_parse.Parse(book_store, xml_file_path);

  arrow::Result<std::shared_ptr<arrow::Table>> result = CreateTable(book_store);
  if (!result.ok()) {
    return 1;
  }

  return 0;
}