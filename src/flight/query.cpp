#include "flight/query.hpp"

#include <nlohmann/json.hpp>

namespace mosaico {

namespace {
using json = nlohmann::json;

std::string opEq(const std::string& value) {
  return json{{"$eq", value}}.dump();
}
std::string opMatch(const std::string& value) {
  return json{{"$match", value}}.dump();
}
std::string opGeq(int64_t ns) {
  return json{{"$geq", ns}}.dump();
}
std::string opLeq(int64_t ns) {
  return json{{"$leq", ns}}.dump();
}

// Assemble {field: <op_obj>, ...} into a JSON object string.
// Every `op_json` was produced by one of the opEq/opMatch/opGeq/opLeq
// helpers below, so a parse/dump failure here would be a programmer bug
// rather than external input — but the builders do accept user strings,
// so we catch defensively and return an empty object on failure rather
// than letting the exception unwind through the caller (e.g. the Qt
// query-bar slot).
std::string assemble(const std::vector<std::pair<std::string, std::string>>& clauses) {
  try {
    json body = json::object();
    for (const auto& [field, op_json] : clauses) {
      body[field] = json::parse(op_json);
    }
    return body.dump();
  } catch (const json::exception&) {
    return "{}";
  }
}

}  // namespace

// ---------------------------------------------------------------------------
// QueryTopicBuilder
// ---------------------------------------------------------------------------

QueryTopicBuilder& QueryTopicBuilder::withName(const std::string& name) {
  // The server's name/identity filter key is "name". It was "locator" until
  // mosaicod #565 renamed it; a stale "locator" key silently matches nothing
  // against current servers. (The streaming ticket descriptor still uses
  // "resource_locator" — only the query filter key changed.)
  clauses_.emplace_back("name", opEq(name));
  return *this;
}
QueryTopicBuilder& QueryTopicBuilder::withNameMatch(const std::string& partial) {
  clauses_.emplace_back("name", opMatch(partial));
  return *this;
}
QueryTopicBuilder& QueryTopicBuilder::withOntologyTag(const std::string& tag) {
  clauses_.emplace_back("ontology_tag", opEq(tag));
  return *this;
}
QueryTopicBuilder& QueryTopicBuilder::withCreatedAfter(int64_t ns) {
  clauses_.emplace_back("created_at_ns", opGeq(ns));
  return *this;
}
QueryTopicBuilder& QueryTopicBuilder::withCreatedBefore(int64_t ns) {
  clauses_.emplace_back("created_at_ns", opLeq(ns));
  return *this;
}
QueryFilter QueryTopicBuilder::build() const {
  return QueryFilter{"topic", assemble(clauses_)};
}

// ---------------------------------------------------------------------------
// QuerySequenceBuilder
// ---------------------------------------------------------------------------

QuerySequenceBuilder& QuerySequenceBuilder::withName(const std::string& name) {
  // See QueryTopicBuilder::withName — server key renamed "locator" -> "name"
  // (mosaicod #565).
  clauses_.emplace_back("name", opEq(name));
  return *this;
}
QuerySequenceBuilder& QuerySequenceBuilder::withNameMatch(const std::string& partial) {
  clauses_.emplace_back("name", opMatch(partial));
  return *this;
}
QuerySequenceBuilder& QuerySequenceBuilder::withCreatedAfter(int64_t ns) {
  clauses_.emplace_back("created_at_ns", opGeq(ns));
  return *this;
}
QuerySequenceBuilder& QuerySequenceBuilder::withCreatedBefore(int64_t ns) {
  clauses_.emplace_back("created_at_ns", opLeq(ns));
  return *this;
}
QueryFilter QuerySequenceBuilder::build() const {
  return QueryFilter{"sequence", assemble(clauses_)};
}

}  // namespace mosaico
